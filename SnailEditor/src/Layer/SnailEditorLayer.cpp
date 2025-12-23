#include "SnailEditorLayer.h"

namespace Snail {

	SnailEditorLayer::SnailEditorLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled),
		m_Scene(std::make_shared<Scene>()),
		m_EditorContext(std::make_shared<EditorContext>()),
		m_SHpanel(m_Scene, m_EditorContext),
		m_EVpanel(m_Scene, m_EditorContext),
		m_GSpanel(m_Scene, m_EditorContext)
	{

		m_EditorCamera = std::make_shared<EditorCamera>();
		//m_CameraEntity = m_Scene->CreateEntity("Main Camera");
		//m_CameraEntity.AddComponent<CameraComponent>(/*TODO: 这里需要 SceneCamera */);

		FrameBufferSpecification spec(1920, 1080);
		m_FBO = FrameBuffer::Create(spec);

	}

	void SnailEditorLayer::OnAttach() {
		SNL_PROFILE_FUNCTION();


		SceneSerializer serializer(m_Scene, m_EditorCamera);
		serializer.Deserialize("assets/scenes/test.snl");
		//serializer.Serialize("Test Scene", "assets/scenes/SaveScene.snl");

		// ----- 初始化面板的上下文 EditorContext ------
		// 找到一个选中的物体就退出
		int count = 0;
		auto modelview = m_Scene->GetAllofEntitiesWith<TransformComponent, ModelComponent>();
		for (auto [entity, transform, model] : modelview.each()) {
			if (model.edgeEnable) {
				if (count > 1) { // 多个物体被选择，不展示信息
					m_EditorContext->selectedEntity = {};
					break;
				}
				m_EditorContext->selectedEntity = { entity, m_Scene.get() };
				count += 1;
			}
		}
		// ImGuizmo的模式 默认
		m_EditorContext->mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		m_EditorContext->mCurrentGizmoMode = ImGuizmo::MODE::WORLD;

		InitScreenQuad(); // 初始化屏幕vao，用于附加后处理轮廓
	}

	void SnailEditorLayer::OnUpdate(const Timestep& ts) {

		SNL_PROFILE_FUNCTION();


		if (!ImGuizmo::IsUsing() && (m_EVpanel.IsFocused() || m_EVpanel.IsHovered())) {
			m_EditorCamera->OnUpdate(ts);
		}
	}

	void SnailEditorLayer::OnEvent(Event& e) {
		//SNL_TRACE("ExampleLayer 调用: OnEvent() {0}", e.ToString());
		m_EditorCamera->OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MousePressEvent>(BIND_NSTATIC_MEMBER_Fn(SnailEditorLayer::OnMousePressed));
	}

	bool SnailEditorLayer::OnMousePressed(MousePressEvent& e)
	{
		if (Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT) ||
			Input::IsMouseButton(SNL_MOUSE_BUTTON_RIGHT) ||
			Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT) && Input::IsKeyPressed(SNL_KEY_LEFT_CONTROL)) // 鼠标左键 或右键是取消选中 或ctrl+左鍵是复选
		{
			if (!m_EVpanel.IsHovered()) // 没有悬浮就不作反应
				return false;

			// 获取鼠标位置
			// 获取 ImGui 体系下的屏幕绝对鼠标坐标
			auto [mx_global, my_global] = ImGui::GetMousePos();
			// 减去视口左上角的坐标，得到相对于视口图片左上角的坐标
			float mx = mx_global - m_EVpanel.GetBoundMin().x;
			float my = my_global - m_EVpanel.GetBoundMin().y;
			float width = m_EVpanel.GetSize().x;
			float height = m_EVpanel.GetSize().y;

			Entity hit = m_Scene->CastRay(mx, my, width, height, glm::inverse(m_EditorCamera->GetTransform()), m_EditorCamera->GetProjection());
			if (Input::IsMouseButton(SNL_MOUSE_BUTTON_RIGHT)) {
				m_SHpanel.ResetSelectedEntity({}); // 右键无条件取消选中
			}
			else if (hit.IsValid() && Input::IsKeyPressed(SNL_KEY_LEFT_CONTROL)) {
				m_SHpanel.AddSelectedEntity(hit); // 左键判断结果，如果有效就叠加或更换
			}
			else if (hit.IsValid()) {
				m_SHpanel.ResetSelectedEntity(hit);
			} // else 如果无效不做处理  PS: 有效是指castray成功判定到一个物体
		}
		return false;
	}

	void SnailEditorLayer::OnRender() {
		SNL_PROFILE_FUNCTION();


		//SNL_TRACE("ExampleLayer 调用: OnRender()");
		// -------------------临时------------------------------------------
		m_FBO->Bind();
		RendererCommand::Clear();

		// 渲染
		m_Scene->OnRenderEditor(m_EditorCamera, m_EditorCamera->GetTransform());

		// 1. 关闭深度测试 + func:Always
		RendererCommand::DepthTest(false);
		RendererCommand::SetDepthFunc(RendererCommand::DepthFuncType::ALWAYS);
		// 2. 开启混合 (Blending)，因为描边 Shader 大部分区域是透明的(discard)，只显示边缘
		RendererCommand::EnableBlend(true);

		auto outlineShader = ShaderLibrary::Load("PostProcessOutline", "assets/shaders/process_outline.glsl");
		if (outlineShader)
		{
			outlineShader->Bind();
			// 告诉 Shader 去哪个纹理单元读 Mask
			outlineShader->SetInt("u_MaskTexture", 0);
			outlineShader->SetInt("u_DepthTexture", 1); // 对应 GL_TEXTURE1

			Texture2D::BindExternal(0, m_FBO->GetMaskAttachment());     // 激活纹理单元0，并绑定 FBO 里的 Mask 附件
			Texture2D::BindExternal(1, m_FBO->GetDepthAttachment());    // 深度附件

			outlineShader->SetFloat3("u_OutlineColor", m_OutlineColor);
			outlineShader->SetInt("u_OutlineWidth", m_OutlineWidth);
			outlineShader->SetFloat("u_Width", (float)m_FBO->GetSpecification().width);
			outlineShader->SetFloat("u_Height", (float)m_FBO->GetSpecification().height);

			// 【重点来了】为什么要画 Quad？
			// 因为我们需要激活 Shader 对每一个像素进行计算。
			// 只有画了这个覆盖全屏的 Quad，GPU 才会对 FBO 上的每个像素运行 Fragment Shader，
			// 从而通过边缘检测算法算出哪里该亮，哪里该透明，并叠加到 Color Attachment 上。
			m_ScreenQuadVAO->Bind();
			RendererCommand::DrawIndexed(m_ScreenQuadVAO);
		}

		// 恢复状态
		RendererCommand::DepthTest(true);
		RendererCommand::SetDepthFunc(RendererCommand::DepthFuncType::LESS);
		RendererCommand::EnableBlend(false);

		m_FBO->Unbind();
		//----------------------------------------------------------------
	}

	void SnailEditorLayer::OnImGuiRender() {
		SNL_PROFILE_FUNCTION();



		Application::Get().GetImGuiLayer()->BeginDockingSpace();

		// 编辑模式下菜单
		if (ImGui::BeginMenuBar())
		{
			std::vector<MenuNode> mainMenu = {
				{ u8"文件", {
					{ u8"新建...", {
						{ u8"新建文件", []() { /*TODO: 新建场景*/ }}
					}},
					{ u8"打开...", {
						{ u8"打开场景", []() { FileSelecter::Open("OpenSceneDlg", u8"选择场景文件", ".snl"); } }
					}},
					{ u8"保存...", {
						{ u8"保存文件", []() { FileSelecter::Open("SaveSceneDlg", u8"保存场景文件", ".snl"); } }
					}},
					{ u8"退出", []() { Application::Get().quit(); }, "Alt+F4" }
				}},
				{ u8"Themes", {
					{ u8"深色(Dark)", []() { Application::Get().GetImGuiLayer()->SetDarkThemeColors(); } },
					{ u8"浅色(Light)", []() { Application::Get().GetImGuiLayer()->SetLightThemeColors(); } },
					{ u8"赛博朋克(Cyberpunk Neon)", []() { Application::Get().GetImGuiLayer()->SetCyberpunkThemeColors(); } }
				}}
			};
			GenUI::MenuHierarchy(mainMenu);
			ImGui::EndMenuBar();
		}
		// 处理“打开场景”
		// 使用 [&] 捕获外部的 scene 和 ec 变量，使用 [this] 只能捕获类成员
		FileSelecter::Handle("OpenSceneDlg", [&](const std::string& path) {
			SceneSerializer serializer(m_Scene, m_EditorCamera);

			m_Scene->Clear();
			if (serializer.Deserialize(path)) {
				SNL_CORE_INFO("成功加载场景: {0}", path);
			}
			else {
				SNL_CORE_ERROR("加载场景失败: {0}", path);
			}
			});

		// 处理“保存场景”
		FileSelecter::Handle("SaveSceneDlg", [&](const std::string& path) {
			SceneSerializer serializer(m_Scene, m_EditorCamera);

			std::string sceneName = std::filesystem::path(path).stem().string();

			serializer.Serialize(sceneName, path);
			SNL_CORE_INFO("场景已保存至: {0}", path);
			});

		m_SHpanel.Show();
		m_EVpanel.Show(m_FBO, m_EditorCamera);
		m_GSpanel.Show(m_EditorCamera);


		// docking space 的 End
		ImGui::End();
	}

}
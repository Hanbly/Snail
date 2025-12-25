#include "SnailEditorLayer.h"

namespace Snail {

	SnailEditorLayer::SnailEditorLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled),
		m_Scene(std::make_shared<Scene>()),
		m_EditorContext(std::make_shared<EditorContext>()),
		m_SHpanel(m_Scene, m_EditorContext),
		m_Inspector(m_Scene, m_EditorContext), // 属性面板
		m_EVpanel(m_Scene, m_EditorContext),
		m_GSpanel(m_Scene, m_EditorContext)
	{
		m_EditorCamera = std::make_shared<EditorCamera>();
		FrameBufferSpecification spec(1920, 1080);
		m_FBO = FrameBuffer::Create(spec);
	}

	void SnailEditorLayer::OnAttach() {
		SNL_PROFILE_FUNCTION();

		SceneSerializer serializer(m_Scene, m_EditorCamera);
		serializer.Deserialize("assets/scenes/test.snl");

		// ----- 初始化面板的上下文 EditorContext ------
		int count = 0;
		auto modelview = m_Scene->GetAllofEntitiesWith<TransformComponent, ModelComponent>();
		for (auto [entity, transform, model] : modelview.each()) {
			if (model.edgeEnable) {
				if (count > 1) {
					m_EditorContext->selectedEntity = {};
					break;
				}
				m_EditorContext->selectedEntity = { entity, m_Scene.get() };
				count += 1;
			}
		}

		m_EditorContext->mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		m_EditorContext->mCurrentGizmoMode = ImGuizmo::MODE::WORLD;

		InitScreenQuad();
	}

	void SnailEditorLayer::OnUpdate(const Timestep& ts) {
		SNL_PROFILE_FUNCTION();

		if (!ImGuizmo::IsUsing() && (m_EVpanel.IsFocused() || m_EVpanel.IsHovered())) {
			m_EditorCamera->OnUpdate(ts);
		}
	}

	void SnailEditorLayer::OnEvent(Event& e) {
		m_EditorCamera->OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MousePressEvent>(BIND_NSTATIC_MEMBER_Fn(SnailEditorLayer::OnMousePressed));
	}

	bool SnailEditorLayer::OnMousePressed(MousePressEvent& e)
	{
		if (Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT) ||
			Input::IsMouseButton(SNL_MOUSE_BUTTON_RIGHT) ||
			Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT) && Input::IsKeyPressed(SNL_KEY_LEFT_CONTROL))
		{
			if (!m_EVpanel.IsHovered())
				return false;

			auto [mx_global, my_global] = ImGui::GetMousePos();
			float mx = mx_global - m_EVpanel.GetBoundMin().x;
			float my = my_global - m_EVpanel.GetBoundMin().y;
			float width = m_EVpanel.GetSize().x;
			float height = m_EVpanel.GetSize().y;

			Entity hit = m_Scene->CastRay(mx, my, width, height, glm::inverse(m_EditorCamera->GetTransform()), m_EditorCamera->GetProjection());

			if (Input::IsMouseButton(SNL_MOUSE_BUTTON_RIGHT)) {
				m_SHpanel.ResetSelectedEntity({});
			}
			else if (hit.IsValid() && Input::IsKeyPressed(SNL_KEY_LEFT_CONTROL)) {
				m_SHpanel.AddSelectedEntity(hit);
			}
			else if (hit.IsValid()) {
				m_SHpanel.ResetSelectedEntity(hit);
			}
		}
		return false;
	}

	void SnailEditorLayer::OnRender() {
		SNL_PROFILE_FUNCTION();

		m_FBO->Bind();
		RendererCommand::Clear();

		// 渲染
		m_Scene->OnRenderEditor(m_EditorCamera, m_EditorCamera->GetTransform());

		// -------------------- 后处理：轮廓描边 --------------------
		RendererCommand::DepthTest(false);
		RendererCommand::SetDepthFunc(RendererCommand::DepthFuncType::ALWAYS);
		RendererCommand::EnableBlend(true);

		auto outlineShader = ShaderLibrary::Load("PostProcessOutline", "assets/shaders/process_outline.glsl");
		if (outlineShader)
		{
			outlineShader->Bind();
			outlineShader->SetInt("u_MaskTexture", 0);
			outlineShader->SetInt("u_DepthTexture", 1);

			Texture2D::BindExternal(0, m_FBO->GetMaskAttachment());
			Texture2D::BindExternal(1, m_FBO->GetDepthAttachment());

			outlineShader->SetFloat3("u_OutlineColor", m_OutlineColor);
			outlineShader->SetInt("u_OutlineWidth", m_OutlineWidth);
			outlineShader->SetFloat("u_Width", (float)m_FBO->GetSpecification().width);
			outlineShader->SetFloat("u_Height", (float)m_FBO->GetSpecification().height);

			m_ScreenQuadVAO->Bind();
			RendererCommand::DrawIndexed(m_ScreenQuadVAO);
		}

		RendererCommand::DepthTest(true);
		RendererCommand::SetDepthFunc(RendererCommand::DepthFuncType::LESS);
		RendererCommand::EnableBlend(false);

		m_FBO->Unbind();
	}

	void SnailEditorLayer::OnImGuiRender() {
		SNL_PROFILE_FUNCTION();

		Application::Get().GetImGuiLayer()->BeginDockingSpace();

		// -------------------- 顶部菜单栏 --------------------
		if (ImGui::BeginMenuBar())
		{
			std::vector<MenuNode> mainMenu = {
				{ u8"文件", {
					{ u8"新建...", { { u8"新建文件", []() { /*TODO*/ }} }},
					{ u8"打开...", { { u8"打开场景", []() { FileSelecter::Open("OpenSceneDlg", u8"选择场景文件", ".snl"); } } }},
					{ u8"保存...", { { u8"保存文件", []() { FileSelecter::Open("SaveSceneDlg", u8"保存场景文件", ".snl"); } } }},
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

		// -------------------- 文件操作处理 --------------------
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

		FileSelecter::Handle("SaveSceneDlg", [&](const std::string& path) {
			SceneSerializer serializer(m_Scene, m_EditorCamera);
			std::string sceneName = std::filesystem::path(path).stem().string();
			serializer.Serialize(sceneName, path);
			SNL_CORE_INFO("场景已保存至: {0}", path);
			});

		// -------------------- 面板显示 --------------------
		m_SHpanel.Show();
		m_Inspector.Show();
		m_EVpanel.Show(m_FBO, m_EditorCamera);
		m_GSpanel.Show(m_EditorCamera);

		ImGui::End();
	}

}
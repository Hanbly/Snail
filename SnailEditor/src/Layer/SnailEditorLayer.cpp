#include "SnailEditorLayer.h"

namespace Snail {

	SnailEditorLayer::SnailEditorLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled),
		m_Scene(std::make_shared<Scene>()),
		m_EditorCamera(std::make_shared<EditorCamera>()),
		m_EditorContext(std::make_shared<EditorContext>()),
		m_GlobalSettingsPanel(m_EditorContext),
		m_EditorViewportPanel(m_EditorCamera, m_EditorContext),
		m_SceneHierarchyPanel(m_EditorContext),
		m_InspectorPanel(m_EditorContext),
		m_AssetsBrowserPanel()
	{
		m_EditorContext->scene = m_Scene;

		FrameBufferSpecification spec(1920, 1080);

		spec.width = 1920, spec.height = 1080;
		spec.attachments = {
			FrameBufferTextureFormat::RGBA32F,
			FrameBufferTextureFormat::R32I,
			FrameBufferTextureFormat::DEPTH24_STENCIL8
		};
		m_TempFBO = FrameBuffer::Create(spec);

		spec.width = 2048, spec.height = 2048;
		spec.attachments = { FrameBufferTextureFormat::DEPTH_COMPONENT };
		m_DepthMapFBO = FrameBuffer::Create(spec);

		spec.width = 1920, spec.height = 1080;
		spec.attachments = { FrameBufferTextureFormat::RGB8 };
		m_FinalFBO = FrameBuffer::Create(spec);

		m_AssetsBrowserPanel.LoadIcons();
	}

	void SnailEditorLayer::OnAttach() {
		SNL_PROFILE_FUNCTION();

		LoadScene("assets/scenes/InitScene.snl");

		// ----- 初始化面板的上下文 EditorContext 的选中实体 ------
		// 如果选中只有一个实体就设置 selectedEntity ，如果超过一个就置空
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


		// --------------- UI面板的回调函数绑定 -----------------
		// 视口面板：
		// 1.接收拖动.snl文件
		// 2.接收拖动.obj .fbx ... 文件
		m_EditorViewportPanel.SetOnSceneFileOpenCallback([this](const std::string& path) {
			LoadScene(path);
			});
		m_EditorViewportPanel.SetOnEntityFileOpenCallback([this](const std::string& path) {
			m_EditorContext->scene->CreateModelEntity(path);
			});
		// 场景面板：
		// 1.接收拖动.obj .fbx ... 文件
		m_SceneHierarchyPanel.SetOnEntityFileOpenCallback([this](const std::string& path) {
			m_EditorContext->scene->CreateModelEntity(path);
			});
		// 资源浏览器面板：（拖动发起方）
		// 1.双击.snl文件
		// 2.双击.obj .fbx ... 文件
		m_AssetsBrowserPanel.SetOnSceneFileOpenCallback([this](const std::string& path) {
			LoadScene(path);
			});
		m_AssetsBrowserPanel.SetOnEntityFileOpenCallback([this](const std::string& path) {
			m_EditorContext->scene->CreateModelEntity(path);
			});

		// --- 初始化用于fbo后处理的矩形 ---
		InitScreenQuad();
	}

	void SnailEditorLayer::OnUpdate(const Timestep& ts) {
		SNL_PROFILE_FUNCTION();

		if (!ImGuizmo::IsUsing() && (m_EditorViewportPanel.IsFocused() || m_EditorViewportPanel.IsHovered())) {
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
			Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT) && Input::IsKeyPressed(SNL_KEY_SPACE) ||
			Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT) && Input::IsKeyPressed(SNL_KEY_LEFT_CONTROL))
		{
			if (!m_EditorViewportPanel.IsHovered())
				return false;

			if (Input::IsKeyPressed(SNL_KEY_SPACE)) return false; // 空格左键用来操作gizmo，不会触发物体选择逻辑

			auto [mx_global, my_global] = ImGui::GetMousePos();
			float mx = mx_global - m_EditorViewportPanel.GetBoundMin().x;
			float my = my_global - m_EditorViewportPanel.GetBoundMin().y;
			float width = m_EditorViewportPanel.GetSize().x;
			float height = m_EditorViewportPanel.GetSize().y;

			Entity hit = m_Scene->CastRay(mx, my, width, height, glm::inverse(m_EditorCamera->GetTransform()), m_EditorCamera->GetProjection());

			if (Input::IsKeyPressed(SNL_KEY_LEFT_CONTROL)) {
				if (hit.IsValid() && Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT))
					m_EditorContext->AddSelectedEntity(hit);
			}
			else if (Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT)) {
				if (hit.IsValid())
					m_EditorContext->ResetSelectedEntity(hit);
				else
					m_EditorContext->ClearAllEdge();
			}
		}
		return false;
	}

	void SnailEditorLayer::OnRender() {
		SNL_PROFILE_FUNCTION();


		// 依据 final fbo 来resize中间fbo
		if(m_TempFBO->GetSpecification().width != m_FinalFBO->GetSpecification().width || m_TempFBO->GetSpecification().height != m_FinalFBO->GetSpecification().height)
			m_TempFBO->Resize(m_FinalFBO->GetSpecification().width, m_FinalFBO->GetSpecification().height);

		// ------ 渲染光照空间深度附件 ------
		m_DepthMapFBO->Bind();
		RendererCommand::Clear();
		RendererCommand::CullFront();

		glm::vec3 lightDir = m_Scene->GetMainLightDirection();

		float orthoSize = 1000.0f;
		float nearPlane = 1.0f;
		float farPlane = 1000.0f;
		glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, 2 * farPlane);

		glm::vec3 lightPos = glm::vec3(0.0f) - lightDir * farPlane;
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		if (glm::abs(glm::dot(lightDir, up)) > 0.99f) {
			up = glm::vec3(0.0f, 0.0f, 1.0f);
		}

		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), up);

		// 最终矩阵
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		auto shadowShader = ShaderLibrary::Load("LightShadowDepth", "assets/shaders/light_shadow_depth.glsl", {});
		auto instancedShadowShader = ShaderLibrary::Load("LightShadowDepth", "assets/shaders/light_shadow_depth.glsl", { "INSTANCING" });
		if (shadowShader && instancedShadowShader) {
			shadowShader->Bind();
			shadowShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);
			instancedShadowShader->Bind();
			instancedShadowShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);
				
			m_Scene->OnRenderExternalShader(shadowShader);
		}

		RendererCommand::EnableCull(false);
		m_DepthMapFBO->Unbind();

		// ------ 渲染实际画面 ------
		m_TempFBO->Bind();
		RendererCommand::Clear();
		m_Scene->OnRenderEditor(m_EditorCamera, m_EditorCamera->GetTransform(), lightSpaceMatrix, m_DepthMapFBO->GetDepthAttachmentRendererID());
		m_TempFBO->Unbind();

		// --------后处理：伽马矫正 (Tone Mapping + Gamma) -----------
		m_FinalFBO->Bind();
		RendererCommand::Clear();
		RendererCommand::DepthTest(false);

		auto gammaShader = ShaderLibrary::Load("PostProcessGamma", "assets/shaders/post_process_gamma.glsl", {});
		if (gammaShader) {
			gammaShader->Bind();
			gammaShader->SetInt("u_ScreenTexture", 0);
			gammaShader->SetFloat("u_Gamma", m_Scene->GetGamma());
			gammaShader->SetFloat("u_Exposure", m_Scene->GetExposure());

			// 绑定 中间fbo 的结果作为输入
			Texture2D::BindExternal(0, m_TempFBO->GetColorAttachmentRendererID(0));

			m_ScreenQuadVAO->Bind();
			RendererCommand::DrawIndexed(m_ScreenQuadVAO);
		}

		RendererCommand::DepthTest(true);
		m_FinalFBO->Unbind();
		// --------------------------------------------------------

		// -------------------- 后处理：轮廓描边 --------------------
		m_FinalFBO->Bind();

		RendererCommand::DepthTest(false);
		RendererCommand::SetDepthFunc(RendererCommand::DepthFuncType::ALWAYS);
		RendererCommand::EnableBlend(true);

		auto outlineShader = ShaderLibrary::Load("PostProcessOutline", "assets/shaders/post_process_outline.glsl", {});
		if (outlineShader)
		{
			outlineShader->Bind();
			outlineShader->SetInt("u_MaskTexture", 0);
			outlineShader->SetInt("u_DepthTexture", 1);

			Texture2D::BindExternal(0, m_TempFBO->GetColorAttachmentRendererID(1)); // 索引1对应Mask附件
			Texture2D::BindExternal(1, m_TempFBO->GetDepthAttachmentRendererID());

			outlineShader->SetFloat3("u_OutlineColor", m_OutlineColor);
			outlineShader->SetInt("u_OutlineWidth", m_OutlineWidth);
			outlineShader->SetFloat("u_Width", (float)m_TempFBO->GetSpecification().width);
			outlineShader->SetFloat("u_Height", (float)m_TempFBO->GetSpecification().height);

			outlineShader->SetFloat("u_Near", m_EditorCamera->GetNear());
			outlineShader->SetFloat("u_Far", m_EditorCamera->GetFar());

			m_ScreenQuadVAO->Bind();
			RendererCommand::DrawIndexed(m_ScreenQuadVAO);
		}

		RendererCommand::DepthTest(true);
		RendererCommand::SetDepthFunc(RendererCommand::DepthFuncType::LESS);
		RendererCommand::EnableBlend(false);
		
		m_FinalFBO->Unbind();
		// --------------------------------------------------------
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

		// -------------------- 文件选择器操作处理 --------------------
		FileSelecter::Handle("OpenSceneDlg", [&](const std::string& path) {
			LoadScene(path);
			});

		FileSelecter::Handle("SaveSceneDlg", [&](const std::string& path) {
			SaveScene(path);
			});

		// -------------------- 面板显示 --------------------
		m_GlobalSettingsPanel.Show(m_EditorCamera);
		m_EditorViewportPanel.Show(m_FinalFBO);
		m_SceneHierarchyPanel.Show();
		m_InspectorPanel.Show();
		m_AssetsBrowserPanel.show();

		ImGui::End();
	}

	void SnailEditorLayer::LoadScene(const std::string& path)
	{
		m_EditorContext->selectedEntity = {};
		m_Scene->Clear();

		SceneSerializer serializer(m_Scene, m_EditorCamera);
		if (serializer.Deserialize(path)) {
			SNL_CORE_INFO("成功加载场景: {0}", path);
		}
		else {
			SNL_CORE_ERROR("加载场景失败: {0}", path);
		}
	}

	void SnailEditorLayer::SaveScene(const std::string& path) const
	{
		SceneSerializer serializer(m_Scene, m_EditorCamera);
		std::string sceneName = std::filesystem::path(path).stem().u8string();
		serializer.Serialize(sceneName, path);
		SNL_CORE_INFO("场景已保存至: {0}", path);
	}

}
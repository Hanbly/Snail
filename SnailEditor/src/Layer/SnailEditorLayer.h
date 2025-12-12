#pragma once

#include "Snail.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class SnailEditorLayer : public Snail::Layer
{
private:
	// -------------------临时------------------------------------------
	//Snail::Refptr<Snail::VertexArray> m_VertexArray;
	//Snail::Refptr<Snail::VertexBuffer> m_VertexBuffer;
	//Snail::Refptr<Snail::IndexBuffer> m_IndexBuffer;
	//Snail::Refptr<Snail::Shader> m_Shader;
	Snail::ShaderLibrary m_ShaderLibrary;
	//Snail::Refptr<Snail::Texture> m_Texture1; // 纹理 1
	//Snail::Refptr<Snail::Texture> m_Texture2; // 纹理 2
	//Snail::Refptr<Snail::Material> m_CubeMaterial;
	//Snail::Refptr<Snail::Material> m_LightMaterial;
	//Snail::Refptr<Snail::Mesh> m_CubeMesh;
	Snail::Refptr<Snail::Mesh> m_LightMesh;
	std::vector<Snail::Refptr<Snail::ModelInstance>> m_Objs;
	int8_t m_SelectedModelIndex = -1;

    Snail::Refptr<Snail::FrameBuffer> m_FBO;
    glm::vec2 m_ViewportSize;
    glm::vec2 m_ViewportBounds[2]; // 左上角Min, 右下角Max (屏幕绝对坐标)
    bool m_ViewportHovered = false; // 鼠标是否悬停在视口上
    bool m_ViewportFocused = false; // 视口是否处于焦点
	
	Snail::Refptr<Snail::PerspectiveCameraController> m_CameraController;
	glm::vec3 u_LightPosition = glm::vec3(0.0f, 100.0f, 0.0f);
	glm::vec4 u_LightColor = {1.0f, 1.0f, 1.0f, 1.0f};
	//------------------------------------------------------------------
public:
    SnailEditorLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled) {}

	virtual void OnAttach() override {
		SNL_PROFILE_FUNCTION();


		// -------------------临时------------------------------------------
		std::vector<Snail::Vertex> vertices = {
			// 格式需对应 Vertex 结构体定义: { Position, Normal, TexCoord }
			// 1. 前面 (Front Face) - Z = 0.5f
			// Pos                          // Normal           // UV
			{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f} },
			{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {1.0f, 0.0f} },
			{ { 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {1.0f, 1.0f} },
			{ {-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {0.0f, 1.0f} },

			// 2. 右面 (Right Face) - X = 0.5f
			{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 0.0f},  {0.0f, 0.0f} },
			{ { 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f} },
			{ { 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f},  {1.0f, 1.0f} },
			{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 0.0f},  {0.0f, 1.0f} },

			// 3. 后面 (Back Face) - Z = -0.5f
			{ { 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
			{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
			{ {-0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },
			{ { 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },

			// 4. 左面 (Left Face) - X = -0.5f
			{ {-0.5f, -0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
			{ {-0.5f, -0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
			{ {-0.5f,  0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
			{ {-0.5f,  0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },

			// 5. 上面 (Top Face) - Y = 0.5f
			{ {-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f} },
			{ { 0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f},  {1.0f, 0.0f} },
			{ { 0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f},  {1.0f, 1.0f} },
			{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f} },

			// 6. 下面 (Bottom Face) - Y = -0.5f
			{ {-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
			{ { 0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
			{ { 0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
			{ {-0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} }
		};
		std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0,       // 前面
			4, 5, 6, 6, 7, 4,       // 右面
			8, 9, 10, 10, 11, 8,    // 后面
			12, 13, 14, 14, 15, 12, // 左面
			16, 17, 18, 18, 19, 16, // 上面
			20, 21, 22, 22, 23, 20  // 下面
		};

		m_ShaderLibrary.Load("cube", "assets/shaders/cube.glsl");
		m_ShaderLibrary.Load("light_box", "assets/shaders/light_box.glsl");
		m_ShaderLibrary.Load("model", "assets/shaders/Model_Shader.glsl");
		

		// 独属于Cube示例的纹理设置
		std::vector<Snail::TextureData> td;
		td.push_back(Snail::TextureData(Snail::Texture2D::Create("assets/images/kulisu.png"), "texture_diffuse"));
		td.push_back(Snail::TextureData(Snail::Texture2D::Create("assets/images/mayoli.png"), "texture_diffuse"));
		// --- 创建 Cube 实例 ---
		{
			Snail::ModelInstance cubeObj;
			cubeObj.name = "Cube";
			// 假设 Mesh 也可以被包装进 Model，或者你的 ModelInstance 支持 Mesh
			cubeObj.mesh = std::make_shared<Snail::Mesh>(vertices, indices, m_ShaderLibrary.Get("cube"), td);
			cubeObj.pos = { 0.0f, 20.0f, 0.0f };
			cubeObj.scale = { 25.0f, 25.0f, 25.0f };
			cubeObj.rot = { 20.0f, 0.0f, 0.0f }; // 初始旋转
			m_Objs.push_back(std::make_shared<Snail::ModelInstance>(cubeObj));
		}
		// --- 创建 Light 实例 ---
		{
			Snail::ModelInstance lightObj;
			lightObj.name = "Light";
			// 假设 Mesh 也可以被包装进 Model，或者你的 ModelInstance 支持 Mesh
			lightObj.mesh = std::make_shared<Snail::Mesh>(vertices, indices, m_ShaderLibrary.Get("light_box"));
			lightObj.pos = { 0.0f, 60.0f, 0.0f };
			lightObj.scale = { 25.0f, 25.0f, 25.0f };
			lightObj.rot = { 20.0f, 0.0f, 0.0f }; // 初始旋转
			m_Objs.push_back(std::make_shared<Snail::ModelInstance>(lightObj));
		}

		//m_Model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/bugatti/bugatti.obj");
		//m_Model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/dragon/dragon.obj");
		//m_Model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/sportsCar/sportsCar.obj");
		// --- 创建 Sponza 模型实例 ---
		{
			Snail::ModelInstance ml;
            ml.name = "Sponza Palace";
            ml.model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/sponza/sponza.obj");
			//sponzaObj.model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/AmazonLumberyard/Interior/Interior.obj");
			m_Objs.push_back(std::make_shared<Snail::ModelInstance>(ml));
		}
        {
            Snail::ModelInstance ml;
            ml.name = "SportsCar";
            ml.model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/sportsCar/sportsCar.obj");
            ml.pos = { 0.0f, 90.0f, 0.0f };
            ml.scale = { 25.0f, 25.0f, 25.0f };
            m_Objs.push_back(std::make_shared<Snail::ModelInstance>(ml));
        }
        {
            Snail::ModelInstance ml;
            ml.name = "Spider";
            ml.model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/spider-fbx/Spider.fbx");
            ml.pos = { 10.0f, 10.0f, -210.0f };
            m_Objs.push_back(std::make_shared<Snail::ModelInstance>(ml));
        }


		m_CameraController = std::make_shared<Snail::PerspectiveCameraController>(45.0f, 1920.0f/1080.0f, glm::vec3(0.0f, 0.0f, 3.0f));


        Snail::FrameBufferSpecification spec(1920, 1080);
        m_FBO = Snail::FrameBuffer::Create(spec);
		//------------------------------------------------------------------------------
	}
	virtual void OnDetach() override {

	}

	inline virtual void OnUpdate(const Snail::Timestep& ts) override {

		SNL_PROFILE_FUNCTION();


        if (m_ViewportFocused) // 只有聚焦在视口，才调用OnUpdate（目前只控制相机移动，视角控制在 OnEvent）
		    m_CameraController->OnUpdate(ts);
	}

	inline virtual void OnEvent(Snail::Event& e) {
		//SNL_TRACE("ExampleLayer 调用: OnEvent() {0}", e.ToString());
		m_CameraController->OnEvent(e);

		Snail::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Snail::MousePressEvent>(BIND_NSTATIC_MEMBER_Fn(SnailEditorLayer::OnMousePressed));
	}

	inline bool OnMousePressed(Snail::MousePressEvent& e)
	{
        if (Snail::Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT)) // 鼠标左键
        {
            if (!m_ViewportHovered) // 没有悬浮就不作反应
                return false;

            // 获取鼠标位置
            // 获取 ImGui 体系下的屏幕绝对鼠标坐标
            auto [mx_global, my_global] = ImGui::GetMousePos();
            // 减去视口左上角的坐标，得到相对于视口图片左上角的坐标
            float mx = mx_global - m_ViewportBounds[0].x;
            float my = my_global - m_ViewportBounds[0].y;
            float width = m_ViewportSize.x;
            float height = m_ViewportSize.y;

            // 1. 构建射线
            Snail::MouseRay ray(mx, my, width, height, m_CameraController);

            int hitIndex = -1;
            float minDst = std::numeric_limits<float>::max(); // 记录最近的距离

            // 2. 遍历所有物体
            for (int i = 0; i < m_Objs.size(); i++)
            {
                float currentDst = 0.0f;
                // 传入引用获取距离
                if (ray.Is_Cross(m_Objs[i], currentDst))
                {
                    // 3. 寻找最近的物体 (处理遮挡)
                    if (currentDst < minDst)
                    {
                        minDst = currentDst;
                        hitIndex = i;
                    }
                }
            }

            // 4. 更新选中状态
            for (const auto& obj : m_Objs) {
                obj->edgeEnable = false;
            }

            m_SelectedModelIndex = hitIndex;
            if (hitIndex != -1) {
                SNL_CORE_INFO("选中物体: {0}", m_Objs[hitIndex]->name);
                m_Objs[hitIndex]->edgeEnable = true;
            }
        }
        return false;
	}

	inline virtual void OnRender() override {
		SNL_PROFILE_FUNCTION();


		//SNL_TRACE("ExampleLayer 调用: OnRender()");
		// -------------------临时------------------------------------------
        m_FBO->Bind();
        Snail::RendererCommand::Clear();

		Snail::Renderer3D::BeginScene(m_CameraController->GetCamera(), u_LightPosition, u_LightColor);

		// 5. 渲染
		for (const auto& obj : m_Objs) {
			if (obj->model) // 如果是外部加载的模型
			{
				for (const auto& mesh : obj->model->GetMeshs()) {
					mesh->GetMaterial()->SetFloat("u_AmbientStrength", obj->ambient);
					mesh->GetMaterial()->SetFloat("u_DiffuseStrength", obj->diffuse);
					mesh->GetMaterial()->SetFloat("u_SpecularStrength", obj->specular);
					mesh->GetMaterial()->SetFloat("u_Shininess", obj->shininess);
				}
				obj->model->Draw(obj->GetTransform(), obj->edgeEnable);
			}
			else if (obj->mesh) // 如果是手动创建的单独 Mesh (如 Cube)
			{
				obj->mesh->GetMaterial()->SetFloat("u_AmbientStrength", obj->ambient);
				obj->mesh->GetMaterial()->SetFloat("u_DiffuseStrength", obj->diffuse);
				obj->mesh->GetMaterial()->SetFloat("u_SpecularStrength", obj->specular);
				obj->mesh->GetMaterial()->SetFloat("u_Shininess", obj->shininess);
				obj->mesh->Draw(obj->GetTransform(), obj->edgeEnable);
			}
		}

        m_FBO->Unbind();
		//----------------------------------------------------------------
	}

	inline virtual void OnImGuiRender() override {
		SNL_PROFILE_FUNCTION();



        //############################################################################################################
        // TL;DR; this demo is more complicated than what most users you would normally use.
        // If we remove all options we are showcasing, this demo would become a simple call to ImGui::DockSpaceOverViewport() !!
        // In this specific demo, we are not using DockSpaceOverViewport() because:

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        bool p_open = true;
        ImGui::Begin("DockSpace Demo", &p_open, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        // REMINDER: THIS IS A DEMO FOR ADVANCED USAGE OF DockSpace()!
        // MOST REGULAR APPLICATIONS WILL SIMPLY WANT TO CALL DockSpaceOverViewport(). READ COMMENTS ABOVE.
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
                if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
                if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();

                if (ImGui::MenuItem("Close", NULL, false, &p_open != NULL))
                    p_open = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }




        // ==========================================================
		// 1. 场景列表 (Scene Hierarchy)
		// ==========================================================
        ImGui::Begin(u8"场景列表 (Hierarchy)");

        // 遍历所有对象
        for (int i = 0; i < m_Objs.size(); i++)
        {
            auto& obj = m_Objs[i];
            ImGuiTreeNodeFlags flags = ((m_SelectedModelIndex == i) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
            flags |= ImGuiTreeNodeFlags_SpanAvailWidth; // 选中条占满宽度

            // 渲染列表项
            bool opened = ImGui::TreeNodeEx((void*)(uint64_t)i, flags, obj->name.c_str());

            // 处理点击选中
            if (ImGui::IsItemClicked())
            {
                m_SelectedModelIndex = i;
            }

            // 右键菜单 (ContextMenu) - 可以在这里添加"删除"等功能
            bool entityDeleted = false;
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem(u8"删除对象"))
                {
                    entityDeleted = true;
                }
                ImGui::EndPopup();
            }

            // 如果展开了节点 (目前没有子节点，这里只是为了演示结构，可以只是简单的Text)
            if (opened)
            {
                ImGui::TreePop();
            }

            // 执行删除操作
            if (entityDeleted)
            {
                m_Objs.erase(m_Objs.begin() + i);
                if (m_SelectedModelIndex == i) m_SelectedModelIndex = -1;
                i--; // 调整循环索引
            }
        }

        // 点击空白处取消选中
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectedModelIndex = -1;

        ImGui::Separator();
        if (ImGui::Button(u8"添加模型..."))
        {
            // ImportModel(); 
        }
        ImGui::End();


        // ==========================================================
        // 2. 属性面板 (Inspector)
        // ==========================================================
        ImGui::Begin(u8"属性面板 (Inspector)");

        if (m_SelectedModelIndex >= 0 && m_SelectedModelIndex < m_Objs.size())
        {
            auto& selectedObj = m_Objs[m_SelectedModelIndex];

            // 简单的重命名输入框
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), selectedObj->name.c_str());
            if (ImGui::InputText(u8"名称", buffer, sizeof(buffer)))
            {
                selectedObj->name = std::string(buffer);
            }

            ImGui::Separator();

            // --- 变换组件 (Transform) ---
            // 使用 TreeNodeEx 默认展开，看起来更像组件
            if (ImGui::CollapsingHeader(u8"变换 (Transform)", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // 封装一个简单的 lambda 来画带有 Reset 按钮的滑动条
                auto DrawVec3Control = [](const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
                    {
                        ImGui::PushID(label.c_str());

                        ImGui::Columns(2);
                        ImGui::SetColumnWidth(0, columnWidth);
                        ImGui::Text(label.c_str());
                        ImGui::NextColumn();

                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

                        // X
                        if (ImGui::Button("R")) { values = glm::vec3(resetValue); }
                        ImGui::SameLine();
                        ImGui::DragFloat3("##Values", glm::value_ptr(values), 0.1f);

                        ImGui::PopStyleVar();
                        ImGui::Columns(1);
                        ImGui::PopID();
                    };

                DrawVec3Control(u8"位置 (Position)", selectedObj->pos, 0.0f);
                DrawVec3Control(u8"旋转 (Rotation)", selectedObj->rot, 0.0f);
                DrawVec3Control(u8"缩放 (Scale)", selectedObj->scale, 1.0f);
            }

            ImGui::Separator();

            // --- 材质组件 (Material) ---
            if (ImGui::CollapsingHeader(u8"材质 (Material)", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text(u8"光照参数");
                ImGui::SliderFloat(u8"环境光##Amb", &selectedObj->ambient, 0.0f, 1.0f);
                ImGui::SliderFloat(u8"漫反射##Diff", &selectedObj->diffuse, 0.0f, 1.0f);
                ImGui::SliderFloat(u8"镜面光##Spec", &selectedObj->specular, 0.0f, 1.0f);
                ImGui::DragFloat(u8"反光度##Shin", &selectedObj->shininess, 1.0f, 2.0f, 256.0f);

                // 如果你想看用了什么纹理，可以列出来
                if (selectedObj->model) {
                    ImGui::TextDisabled(u8"包含 %d 个 Mesh", selectedObj->model->GetMeshs().size());
                }
            }
        }
        else
        {
            // 提示用户选择物体
            ImGui::TextDisabled(u8"在场景列表中选择一个物体以查看属性。");
        }

        ImGui::End();


        // ==========================================================
        // 3. 全局设置 (Environment & Stats)
        // ==========================================================
        ImGui::Begin(u8"全局设置");

        // --- 性能统计 (Stats) ---
        ImGui::Text(u8"性能监控:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Separator();

        // --- 相机设置 (Camera) ---
        if (ImGui::TreeNodeEx(u8"相机设置", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text(u8"位置: %.2f, %.2f, %.2f",
                m_CameraController->GetCamera()->GetCameraPos().x,
                m_CameraController->GetCamera()->GetCameraPos().y,
				m_CameraController->GetCamera()->GetCameraPos().z);
            ImGui::TreePop();
        }

        // --- 光源设置 (Lighting) ---
        if (ImGui::TreeNodeEx(u8"定向光/点光源", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool posChanged = ImGui::DragFloat3(u8"光源位置", glm::value_ptr(u_LightPosition), 0.5f);
            ImGui::ColorEdit3(u8"光源颜色", glm::value_ptr(u_LightColor));

            if (posChanged)
            {
                for (auto& obj : m_Objs) {
                    if (obj->name == "Light")
                    {
                        obj->pos = u_LightPosition;
                    }
                }
            }
            ImGui::TreePop();
        }

        ImGui::End();


        // ==========================================================
        // 4. 使用 m_FBO 的color attachment，实现离屏渲染
        // ==========================================================
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin(u8"离屏渲染视口");

        // ----------------- 处理鼠标坐标 ------------------
        // 获取视口范围和位置
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        // 将局部坐标转换为屏幕绝对坐标
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        // ---------------- 处理imgui事件 -------------------
        m_ViewportHovered = ImGui::IsWindowHovered();
        m_ViewportFocused = ImGui::IsWindowFocused();
        // !(m_ViewportHovered || m_ViewportFocused) 与 !m_ViewportHovered && !m_ViewportFocused : (鼠标 悬浮在视口 || 聚焦在视口) 都会让imgui忽略鼠标事件，从而让视口内容接收鼠标事件；不知道为什么键盘事件始终无法被imgui捕获，不论视口是什么状态。
        // 目标是：鼠标 一旦悬浮在视口 imgui就忽略鼠标事件；一旦聚焦于视口 imgui就忽略键盘事件；当鼠标悬浮在视口以外其它部分时，imgui应该拦截鼠标事件；当聚焦于视口以外其它部分时，imgui应该拦截键盘事件。
        Snail::Application::Get().GetImGuiLayer()->BlockMouseEvents(!m_ViewportHovered);
        Snail::Application::Get().GetImGuiLayer()->BlockKeyEvents(!m_ViewportFocused);

        // ---------------- 处理resize ------------------
        ImVec2 ImguiViewportSize = ImGui::GetContentRegionAvail();
        ImGui::Text("视口大小: %.0f x %.0f", ImguiViewportSize.x, ImguiViewportSize.y);
        if ((m_ViewportSize != *(glm::vec2*)&ImguiViewportSize) && ImguiViewportSize.x > 0 && ImguiViewportSize.y > 0) {
            m_FBO->Resize((uint32_t)ImguiViewportSize.x, (uint32_t)ImguiViewportSize.y);
            m_ViewportSize.x = (uint32_t)ImguiViewportSize.x;
            m_ViewportSize.y = (uint32_t)ImguiViewportSize.y;

            m_CameraController->UpdateAspect((float)m_ViewportSize.x / m_ViewportSize.y);
        }

        // 获取帧缓冲信息，绘制纹理
        uint32_t textureId = m_FBO->GetColorAttachment();
        ImGui::Image(
            (void*)(intptr_t)textureId, ImGui::GetContentRegionAvail(),
            ImVec2(0, 1), // UV0: 纹理的(0,1) -> 对应 OpenGL 左上
            ImVec2(1, 0)  // UV1: 纹理的(1,0) -> 对应 OpenGL 右下
        );

        ImGui::End();
        ImGui::PopStyleVar();


        // docking space 的 End
        ImGui::End();
	}
};


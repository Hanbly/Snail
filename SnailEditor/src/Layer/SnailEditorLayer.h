#pragma once

#include "Snail.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Snail {

    class SnailEditorLayer : public Layer
    {
    private:
        // -------------------临时------------------------------------------
        //Refptr<VertexArray> m_VertexArray;
        //Refptr<VertexBuffer> m_VertexBuffer;
        //Refptr<IndexBuffer> m_IndexBuffer;
        //Refptr<Shader> m_Shader;
        //Refptr<Texture> m_Texture1; // 纹理 1
        //Refptr<Texture> m_Texture2; // 纹理 2
        //Refptr<Material> m_CubeMaterial;
        //Refptr<Material> m_LightMaterial;
        //Refptr<Mesh> m_CubeMesh;
        Refptr<Mesh> m_LightMesh;

        Refptr<FrameBuffer> m_FBO;
        glm::vec2 m_ViewportSize;
        glm::vec2 m_ViewportBounds[2]; // 左上角Min, 右下角Max (屏幕绝对坐标)
        bool m_ViewportHovered = false; // 鼠标是否悬停在视口上
        bool m_ViewportFocused = false; // 视口是否处于焦点

        // ECS 核心
        Refptr<Scene> m_Scene;
        Entity m_SelectedEntity; // 当前选中的实体 (空实体即为未选中)
        Entity m_CameraEntity;   // 编辑器相机对应的实体

        // 编辑器特有
        Refptr<PerspectiveCameraController> m_CameraController;
        ShaderLibrary m_ShaderLibrary;

        glm::vec3 u_LightPosition = glm::vec3(0.0f, 100.0f, 0.0f);
        glm::vec4 u_LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        //------------------------------------------------------------------
    public:
        SnailEditorLayer(const std::string& layerName, const bool& layerEnabled)
            : Layer(layerName, layerEnabled) {}

        virtual void OnAttach() override {
            SNL_PROFILE_FUNCTION();


            // -------------------临时------------------------------------------
            std::vector<Vertex> vertices = {
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




            m_Scene = std::make_shared<Scene>();

            // 独属于Cube示例的纹理设置
            std::vector<TextureData> td;
            td.push_back(TextureData(Texture2D::Create("assets/images/kulisu.png"), "texture_diffuse"));
            td.push_back(TextureData(Texture2D::Create("assets/images/mayoli.png"), "texture_diffuse"));
            // --- 创建 Cube 实例 ---
            {
                Refptr<Model> singleMesh = std::make_shared<Model>(vertices, indices, m_ShaderLibrary.Get("cube"), td);

                glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 60.0f, 0.0f)) *
                    glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(25.0f, 25.0f, 25.0f)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(25.0f, 25.0f, 25.0f));

                Entity e = m_Scene->CreateEntity("Cube");
                e.AddComponent<ModelComponent>(singleMesh);

                e.GetComponent<TransformComponent>().transform = trans;
            }
            // --- 创建 Light 实例 ---
            {
                Refptr<Model> singleMesh = std::make_shared<Model>(vertices, indices, m_ShaderLibrary.Get("light_box"));
                
                glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 160.0f, 0.0f)) *
                    glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(25.0f, 25.0f, 25.0f)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(25.0f, 25.0f, 25.0f));

                Entity e = m_Scene->CreateEntity("Light");
                e.AddComponent<ModelComponent>(singleMesh);
                e.AddComponent<PointLightComponent>(u_LightColor, 1.0f);

                e.GetComponent<TransformComponent>().transform = trans;
            }

            //m_Model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/bugatti/bugatti.obj");
            //m_Model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/dragon/dragon.obj");
            //m_Model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/sportsCar/sportsCar.obj");
            // --- 创建 Sponza 模型实例 ---
            {
                Refptr<Model> model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/sponza/sponza.obj");
                //sponzaObj.model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/AmazonLumberyard/Interior/Interior.obj");

                Entity e = m_Scene->CreateEntity("Sponza Palace");
                e.AddComponent<ModelComponent>(model);

                e.GetComponent<TransformComponent>().transform = glm::mat4(1.0f);
            }
            {
                Refptr<Model> model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/sportsCar/sportsCar.obj");
                
                glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 90.0f, 0.0f)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(25.0f, 25.0f, 25.0f));

                Entity e = m_Scene->CreateEntity("SportsCar");
                e.AddComponent<ModelComponent>(model);

                e.GetComponent<TransformComponent>().transform = trans;
            }
            {
                Refptr<Model> model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/spider-fbx/Spider.fbx");
                
                glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, -210.0f));

                Entity e = m_Scene->CreateEntity("Spider");
                e.AddComponent<ModelComponent>(model);

                e.GetComponent<TransformComponent>().transform = trans;
            }

            {
                m_CameraController = std::make_shared<PerspectiveCameraController>(45.0f, 1920.0f / 1080.0f, glm::vec3(0.0f, 0.0f, 3.0f));
                m_CameraEntity = m_Scene->CreateEntity("Main Camera");
                m_CameraEntity.AddComponent<CameraComponent>(m_CameraController->GetCamera());
            }

            FrameBufferSpecification spec(1920, 1080);
            m_FBO = FrameBuffer::Create(spec);

            //------------------------------------------------------------------------------
        }
        virtual void OnDetach() override {

        }

        inline virtual void OnUpdate(const Timestep& ts) override {

            SNL_PROFILE_FUNCTION();


            if (m_ViewportFocused) { // 只有聚焦在视口，才调用OnUpdate（目前只控制相机移动，视角控制在 OnEvent）
                m_CameraController->OnUpdate(ts);
            }

            if (m_CameraEntity)
            {
                // 同步 View 矩阵 (通过 Transform)
                auto& trans = m_CameraEntity.GetComponent<TransformComponent>();
                trans.transform = m_CameraController->GetTransform();

                // 同步 Projection (处理 Zoom)
                auto& camComp = m_CameraEntity.GetComponent<CameraComponent>();
            }
        }

        inline virtual void OnEvent(Event& e) {
            //SNL_TRACE("ExampleLayer 调用: OnEvent() {0}", e.ToString());
            m_CameraController->OnEvent(e);

            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<MousePressEvent>(BIND_NSTATIC_MEMBER_Fn(SnailEditorLayer::OnMousePressed));
        }

        inline bool OnMousePressed(MousePressEvent& e)
        {
            if (Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT)) // 鼠标左键
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
                MouseRay ray(mx, my, width, height, glm::inverse(m_CameraController->GetTransform()), m_CameraController->GetCamera().GetProjectionMatrix());

                float minDst = std::numeric_limits<float>::max();
                Entity hitEntity = {}; // 空实体

                // 2. 遍历所有物体
                auto view = m_Scene->GetAllofEntitiesWith<TransformComponent>();
                for (auto [entity, transform] : view.each())
                {
                    float currentDst = 0.0f;
                    Entity e(entity, m_Scene.get());
                    // 传入引用获取距离
                    if (ray.Is_Cross(e, currentDst))
                    {
                        // 3. 寻找最近的物体 (处理遮挡)
                        if (currentDst < minDst)
                        {
                            minDst = currentDst;
                            hitEntity = e;
                        }
                    }
                }

                // 4. 更新选中状态
                // 点击空白取消选中
                if (!hitEntity.IsValid()) {
                    auto modelview = m_Scene->GetRegistry().view<ModelComponent>();

                    for (auto [entity, model] : modelview.each()) {
                        model.edgeEnable = false;
                    }

                    m_SelectedEntity = {};
                    return false;
                }

                // 点击物体表示选中
                m_SelectedEntity = hitEntity;
                SNL_CORE_INFO("选中物体: {0}", hitEntity.GetComponent<TagComponent>().name);
                if (m_SelectedEntity.TryGetComponent<ModelComponent>())
                    m_SelectedEntity.TryGetComponent<ModelComponent>()->edgeEnable = true;
            }
            return false;
        }

        inline virtual void OnRender() override {
            SNL_PROFILE_FUNCTION();


            //SNL_TRACE("ExampleLayer 调用: OnRender()");
            // -------------------临时------------------------------------------
            m_FBO->Bind();
            RendererCommand::Clear();

            // 5. 渲染
            m_Scene->OnRenderEditor(m_CameraController->GetCamera(), m_CameraController->GetTransform());

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
            auto view = m_Scene->GetRegistry().view<TransformComponent>();
            for(auto entityID : view) {
                Entity entity{ entityID, m_Scene.get() };

                // 获取名字
                std::string name = "Unnamed Entity";
                if (entity.HasAllofComponent<TagComponent>())
                    name = entity.GetComponent<TagComponent>().name;

                ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
                flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

                bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entityID, flags, name.c_str());

                if (ImGui::IsItemClicked())
                {
                    // 处理切换选中：旧的取消描边，新的开启描边
                    if (m_SelectedEntity && m_SelectedEntity.HasAllofComponent<ModelComponent>())
                        m_SelectedEntity.GetComponent<ModelComponent>().edgeEnable = false;

                    m_SelectedEntity = entity;

                    if (m_SelectedEntity.HasAllofComponent<ModelComponent>())
                        m_SelectedEntity.GetComponent<ModelComponent>().edgeEnable = true;
                }

                if (opened) ImGui::TreePop();
            }        

            ImGui::End();


            // ==========================================================
            // 2. 属性面板 (Inspector)
            // ==========================================================
            ImGui::Begin(u8"属性面板 (Inspector)");

            if (m_SelectedEntity)
            {
                // --- Tag Component ---
                if (m_SelectedEntity.HasAllofComponent<TagComponent>())
                {
                    auto& tag = m_SelectedEntity.GetComponent<TagComponent>();
                    char buffer[256];
                    memset(buffer, 0, sizeof(buffer));
                    strcpy_s(buffer, sizeof(buffer), tag.name.c_str());
                    if (ImGui::InputText("Name", buffer, sizeof(buffer)))
                        tag.name = std::string(buffer);
                }

                // --- Transform Component ---
                if (m_SelectedEntity.HasAllofComponent<TransformComponent>())
                {
                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        auto& tc = m_SelectedEntity.GetComponent<TransformComponent>();

                        // 需要把 mat4 分解为 T/R/S 才能在 ImGui 编辑 (这需要 Math 工具函数 DecomposeTransform)
                        // 暂时这部分比较复杂，建议先只支持显示矩阵，或者引入 ImGuizmo
                        // 这里写个伪代码占位：
                        // glm::vec3 translation, rotation, scale;
                        // Math::DecomposeTransform(tc.transform, translation, rotation, scale);
                        // ImGui::DragFloat3("Position", value_ptr(translation));
                        // ...
                        // tc.transform = Math::ComposeTransform(translation, rotation, scale);

                        // 简易版：直接修改矩阵的第四列作为位置
                        glm::vec3 pos = tc.transform[3];
                        if (ImGui::DragFloat3("Position", glm::value_ptr(pos))) {
                            tc.transform[3] = glm::vec4(pos, 1.0f);
                        }
                        ImGui::TextDisabled("Rotation/Scale edit needs Matrix Decompose");
                    }
                }

                // --- Point Light Component ---
                if (m_SelectedEntity.HasAllofComponent<PointLightComponent>())
                {
                    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        auto& plc = m_SelectedEntity.GetComponent<PointLightComponent>();
                        ImGui::ColorEdit4("Color", glm::value_ptr(plc.color));
                        ImGui::DragFloat("Intensity", &plc.intensity, 0.1f);
                    }
                }

                // --- Model Component ---
                if (m_SelectedEntity.HasAllofComponent<ModelComponent>())
                {
                    if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        auto& mc = m_SelectedEntity.GetComponent<ModelComponent>();
                        ImGui::Checkbox("显示", &mc.visible);
                        ImGui::Checkbox("物体轮廓", &mc.edgeEnable);
                    }
                }
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
                if (m_CameraEntity && m_CameraEntity.HasAllofComponent<TransformComponent>())
                {
                    auto& tc = m_CameraEntity.GetComponent<TransformComponent>();

                    // 直接从矩阵的第四列获取位置
                    glm::vec3 pos = glm::vec3(tc.transform[3]);

                    ImGui::Text(u8"位置: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);

                    // 如果你想显示旋转角度 (Yaw/Pitch)，需要从矩阵分解，这比较麻烦。
                    // 或者，让你的 EditorCameraController 在更新时把 Yaw/Pitch 存个临时变量供 UI 读取。
                }
                ImGui::TreePop();
            }
            
            // --- 光源设置 (Lighting) ---
            if (ImGui::TreeNodeEx(u8"定向光/点光源", ImGuiTreeNodeFlags_DefaultOpen))
            {
                bool posChanged = ImGui::DragFloat3(u8"光源位置", glm::value_ptr(u_LightPosition), 0.5f);
                ImGui::ColorEdit3(u8"光源颜色", glm::value_ptr(u_LightColor));

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
            Application::Get().GetImGuiLayer()->BlockMouseEvents(!m_ViewportHovered);
            Application::Get().GetImGuiLayer()->BlockKeyEvents(!m_ViewportFocused);

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

}

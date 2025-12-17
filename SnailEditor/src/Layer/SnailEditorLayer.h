#pragma once

#include "Snail.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EditorViewportPanel.h"
#include "Panels/GlobalSettingsPanel.h"

#include "glm/glm.hpp"

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

        // ECS 核心
        Refptr<Scene> m_Scene;
        //Entity m_CameraEntity;   // 编辑器相机对应的实体

        // 编辑器特有
        Refptr<EditorCamera> m_EditorCamera;
        ShaderLibrary m_ShaderLibrary;

        SceneHierarchyPanel m_SHpanel;
        EditorViewportPanel m_EVpanel;
        GlobalSettingsPanel m_GSpanel;

        //------------------------------------------------------------------
    public:
        SnailEditorLayer(const std::string& layerName, const bool& layerEnabled)
            :   Layer(layerName, layerEnabled),
                m_Scene(std::make_shared<Scene>()),
                m_SHpanel(m_Scene),
                m_EVpanel(),
                m_GSpanel(m_Scene)
        {

            m_EditorCamera = std::make_shared<EditorCamera>(glm::vec3(0.0f, 0.0f, 1.0f), EditorCameraMode::Arcball);
            //m_CameraEntity = m_Scene->CreateEntity("Main Camera");
            //m_CameraEntity.AddComponent<CameraComponent>(/*TODO: 这里需要 SceneCamera */);

            FrameBufferSpecification spec(1920, 1080);
            m_FBO = FrameBuffer::Create(spec);
            
        }

        virtual void OnAttach() override {
            SNL_PROFILE_FUNCTION();


            // -------------------临时------------------------------------------
            std::vector<Vertex> skyboxVertices = {
                // 只需要位置 (Position)，Normal 和 UV 在 Skybox shader 中通常用不到，设为 0
                // ---------------------------------------------------------------------
                // Positions          // Normals           // UVs (Unused)
                { {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 0. 左下前
                { { 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 1. 右下前
                { { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 2. 右上前
                { {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 3. 左上前

                { {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 4. 左下后
                { { 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 5. 右下后
                { { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 6. 右上后
                { {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }  // 7. 左上后
            };
            std::vector<uint32_t> skyboxIndices = {
                // Front face
                0, 1, 2,
                2, 3, 0,

                // Right face
                1, 5, 6,
                6, 2, 1,

                // Back face
                5, 4, 7,
                7, 6, 5,

                // Left face
                4, 0, 3,
                3, 7, 4,

                // Top face
                3, 2, 6,
                6, 7, 3,

                // Bottom face
                4, 5, 1,
                1, 0, 4
            };
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

            m_ShaderLibrary.Load("sky", "assets/shaders/TextureCube_Shader.glsl");
            m_ShaderLibrary.Load("cube", "assets/shaders/cube.glsl");
            m_ShaderLibrary.Load("light_box", "assets/shaders/light_box.glsl");
            m_ShaderLibrary.Load("model", "assets/shaders/Model_Shader.glsl");


         
            {
                std::vector<TextureData> td;
                std::array<std::string, 6> skyAssets = {
                    "assets/images/skybox/right.jpg",
                    "assets/images/skybox/left.jpg",
                    "assets/images/skybox/top.jpg",
                    "assets/images/skybox/bottom.jpg",
                    "assets/images/skybox/front.jpg",
                    "assets/images/skybox/back.jpg",
                };
                td.push_back(TextureData(TextureCube::Create(skyAssets), "texture_cubemap"));
                Refptr<Model> sky = std::make_shared<Model>(skyboxVertices, skyboxIndices, m_ShaderLibrary.Get("sky"), td);
                Entity e = m_Scene->CreateEntity("Sky");
                e.AddComponent<ModelComponent>(sky);
                e.AddComponent<SkyboxComponent>();
                e.RemoveComponent<TransformComponent>();
            }            
            // --- 创建 Cube 实例 ---
            {
                // 独属于Cube示例的纹理设置
                std::vector<TextureData> td;
                td.push_back(TextureData(Texture2D::Create("assets/images/kulisu.png"), "texture_diffuse"));
                td.push_back(TextureData(Texture2D::Create("assets/images/mayoli.png"), "texture_diffuse"));
                Refptr<Model> singleMesh = std::make_shared<Model>(vertices, indices, m_ShaderLibrary.Get("cube"), td);

                Entity e = m_Scene->CreateEntity("Cube");
                e.AddComponent<ModelComponent>(singleMesh);

                e.GetComponent<TransformComponent>().position = { 0, 60, 0 };
                e.GetComponent<TransformComponent>().rotation = { 25, 25, 25 };
                e.GetComponent<TransformComponent>().scale =    { 25, 25, 25 };
            }
            // --- 创建 Light 实例 ---
            {
                Refptr<Model> singleMesh = std::make_shared<Model>(vertices, indices, m_ShaderLibrary.Get("light_box"));

                Entity e = m_Scene->CreateEntity("Light");
                e.AddComponent<ModelComponent>(singleMesh);
                e.AddComponent<PointLightComponent>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), m_Scene->GetAmbientStrength());

                e.GetComponent<TransformComponent>().position = { 0, 160, 0 };
                e.GetComponent<TransformComponent>().rotation = { 25, 25, 25 };
                e.GetComponent<TransformComponent>().scale =    { 25, 25, 25 };
            }

            //m_Model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/bugatti/bugatti.obj");
            //m_Model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/dragon/dragon.obj");
            //m_Model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/sportsCar/sportsCar.obj");
            //// --- 创建 Sponza 模型实例 ---
            //{
            //    Refptr<Model> model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/sponza/sponza.obj");
            //    //sponzaObj.model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/AmazonLumberyard/Interior/Interior.obj");

            //    Entity e = m_Scene->CreateEntity("Sponza Palace");
            //    e.AddComponent<ModelComponent>(model);

            //}
            {
                Refptr<Model> model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/sportsCar/sportsCar.obj");

                Entity e = m_Scene->CreateEntity("SportsCar");
                e.AddComponent<ModelComponent>(model);

                e.GetComponent<TransformComponent>().position = { 0, 90, 0 };
                e.GetComponent<TransformComponent>().rotation = { 0, 0, 0 };
                e.GetComponent<TransformComponent>().scale =    { 25, 25, 25 };
            }
            {
                Refptr<Model> model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/spider-fbx/Spider.fbx");

                Entity e = m_Scene->CreateEntity("Spider");
                e.AddComponent<ModelComponent>(model);

                e.GetComponent<TransformComponent>().position = { 10, 10, -210 };
            }

            SceneSerializer serializer(m_Scene, m_EditorCamera);
            serializer.Serialize("Test Scene", "assets/scenes/test.snl");
            //------------------------------------------------------------------------------
        }
        virtual void OnDetach() override {

        }

        inline virtual void OnUpdate(const Timestep& ts) override {

            SNL_PROFILE_FUNCTION();


            if (m_EVpanel.IsFocused() || m_EVpanel.IsHovered()) {
                m_EditorCamera->OnUpdate(ts);
            }

            //if (m_CameraEntity)
            //{
            //    // 同步 View 矩阵 (通过 Transform)
            //    auto& trans = m_CameraEntity.GetComponent<TransformComponent>();
            //    trans.position = m_CameraController->GetPostion();
            //    trans.rotation = m_CameraController->GetRotation();

            //    // 同步 Projection (处理 Zoom)
            //    auto& camComp = m_CameraEntity.GetComponent<CameraComponent>();
            //}
        }

        inline virtual void OnEvent(Event& e) {
            //SNL_TRACE("ExampleLayer 调用: OnEvent() {0}", e.ToString());
            m_EditorCamera->OnEvent(e);

            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<MousePressEvent>(BIND_NSTATIC_MEMBER_Fn(SnailEditorLayer::OnMousePressed));
        }

        inline bool OnMousePressed(MousePressEvent& e)
        {
            if (Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT)) // 鼠标左键
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

               m_SHpanel.SetSelectedEntity(
                   m_Scene->CastRay(mx, my, width, height, glm::inverse(m_EditorCamera->GetTransform()), m_EditorCamera->GetProjection())
               );
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
            m_Scene->OnRenderEditor(m_EditorCamera, m_EditorCamera->GetTransform());

            m_FBO->Unbind();
            //----------------------------------------------------------------
        }

        inline virtual void OnImGuiRender() override {
            SNL_PROFILE_FUNCTION();



            Application::Get().GetImGuiLayer()->BeginDockingSpace();


            m_SHpanel.Show();
            m_EVpanel.Show(m_FBO, m_EditorCamera);
            m_GSpanel.Show(m_EditorCamera);


            // docking space 的 End
            ImGui::End();
        }
    };

}

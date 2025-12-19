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

        Refptr<FrameBuffer> m_FBO;

        // ECS 核心
        Refptr<Scene> m_Scene;

        // 编辑器特有
        Refptr<EditorCamera> m_EditorCamera;
        //ShaderLibrary m_ShaderLibrary;

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
            

			/*ShaderLibrary::Load("sky", "assets/shaders/TextureCube_Shader.glsl");
			ShaderLibrary::Load("sky", "assets/shaders/TextureCube_Shader.glsl");
			ShaderLibrary::Load("sky", "assets/shaders/TextureCube_Shader.glsl");
			ShaderLibrary::Load("cube", "assets/shaders/TextureCube_Shader.glsl");
			ShaderLibrary::Load("cube", "assets/shaders/cube.glsl");
			ShaderLibrary::Load("light_box", "assets/shaders/light_box.glsl");
			ShaderLibrary::Load("model", "assets/shaders/Model_Shader.glsl");

			std::vector<std::string> skyAssets = {
				"assets/images/skybox/right.jpg",
				"assets/images/skybox/left.jpg",
				"assets/images/skybox/top.jpg",
				"assets/images/skybox/bottom.jpg",
				"assets/images/skybox/front.jpg",
				"assets/images/skybox/back.jpg",
			};
			TextureLibrary::Load("sky", skyAssets, TextureUsage::Cubemap);
			TextureLibrary::Load("sky", skyAssets, TextureUsage::Cubemap);
			TextureLibrary::Load("cube", skyAssets, TextureUsage::Cubemap);
			TextureLibrary::Load("cube", { "assets/images/kulisu.png" }, TextureUsage::Diffuse);
			TextureLibrary::Load("light", { "assets/images/mayoli.png" }, TextureUsage::Diffuse);*/

         
            //{
            //    std::vector<TextureData> td;
            //    std::vector<std::string> skyAssets = {
            //        "assets/images/skybox/right.jpg",
            //        "assets/images/skybox/left.jpg",
            //        "assets/images/skybox/top.jpg",
            //        "assets/images/skybox/bottom.jpg",
            //        "assets/images/skybox/front.jpg",
            //        "assets/images/skybox/back.jpg",
            //    };
            //    td.push_back(TextureData(TextureCube::Create(skyAssets), "texture_cubemap"));
            //    Refptr<Model> sky = std::make_shared<Model>(PrimitiveType::Skybox, skyboxVertices, skyboxIndices, m_ShaderLibrary.Get("sky"),td);
            //    Entity e = m_Scene->CreateEntity("Sky");
            //    e.AddComponent<ModelComponent>(sky);
            //    e.AddComponent<SkyboxComponent>();
            //    e.RemoveComponent<TransformComponent>();
            //}            
            //// --- 创建 Cube 实例 ---
            //{                
            //    // 独属于Cube示例的纹理设置
            //    std::vector<TextureData> td;
			//    td.push_back(TextureData(Texture2D::Create({ "assets/images/kulisu.png" }), "texture_diffuse"));
            //    td.push_back(TextureData(Texture2D::Create({ "assets/images/mayoli.png" }), "texture_diffuse"));
            //    Refptr<Model> singleMesh = std::make_shared<Model>(PrimitiveType::Cube, vertices, indices, m_ShaderLibrary.Get("cube"), td);

            //    Entity e = m_Scene->CreateEntity("Cube");
            //    e.AddComponent<ModelComponent>(singleMesh);

            //    e.GetComponent<TransformComponent>().position = { 0, 60, 0 };
            //    e.GetComponent<TransformComponent>().rotation = { 25, 25, 25 };
            //    e.GetComponent<TransformComponent>().scale =    { 25, 25, 25 };
            //}
            //// --- 创建 Light 实例 ---
            //{
            //    Refptr<Model> singleMesh = std::make_shared<Model>(PrimitiveType::Cube, vertices, indices, m_ShaderLibrary.Get("light_box"));

            //    Entity e = m_Scene->CreateEntity("Light");
            //    e.AddComponent<ModelComponent>(singleMesh);
            //    e.AddComponent<PointLightComponent>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), m_Scene->GetAmbientStrength());

            //    e.GetComponent<TransformComponent>().position = { 0, 160, 0 };
            //    e.GetComponent<TransformComponent>().rotation = { 25, 25, 25 };
            //    e.GetComponent<TransformComponent>().scale =    { 25, 25, 25 };
            //}

            ////m_Model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/bugatti/bugatti.obj");
            ////m_Model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/dragon/dragon.obj");
            ////m_Model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/sportsCar/sportsCar.obj");
            ////// --- 创建 Sponza 模型实例 ---
            ////{
            ////    Refptr<Model> model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/sponza/sponza.obj");
            ////    //sponzaObj.model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/AmazonLumberyard/Interior/Interior.obj");

            ////    Entity e = m_Scene->CreateEntity("Sponza Palace");
            ////    e.AddComponent<ModelComponent>(model);

            ////}
            //{
            //    Refptr<Model> model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/sportsCar/sportsCar.obj");

            //    Entity e = m_Scene->CreateEntity("SportsCar");
            //    e.AddComponent<ModelComponent>(model);

            //    e.GetComponent<TransformComponent>().position = { 0, 90, 0 };
            //    e.GetComponent<TransformComponent>().rotation = { 0, 0, 0 };
            //    e.GetComponent<TransformComponent>().scale =    { 25, 25, 25 };
            //}
            //{
            //    Refptr<Model> model = std::make_shared<Model>(m_ShaderLibrary.Get("model"), "assets/models/spider-fbx/Spider.fbx");

            //    Entity e = m_Scene->CreateEntity("Spider");
            //    e.AddComponent<ModelComponent>(model);

            //    e.GetComponent<TransformComponent>().position = { 10, 10, -210 };
            //}

			SceneSerializer serializer(m_Scene, m_EditorCamera);
			serializer.Deserialize("assets/scenes/LoadScene.snl");
            serializer.Serialize("Test Scene", "assets/scenes/SaveScene.snl");
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

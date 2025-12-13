#include "SNLpch.h"

#include "Scene.h"
#include "Entity.h"

#include "Component.h"
#include "Snail/Render/Renderer/Renderer3D.h"

namespace Snail {

	Scene::Scene()
	{
		
	}

	Scene::~Scene()
	{
		
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity(m_Registry.create(), this);
		entity.AddComponent<TagComponent>(name.empty() ? "UnnamedEntity" : name);
		entity.AddComponent<TransformComponent>();

		return entity;
	}

    void Scene::DestroyEntity(Entity entity)
    {
        if (entity.IsValid()) {
            m_Registry.destroy(entity);
        }
    }

	void Scene::OnUpdateRuntime(const Timestep& ts)
	{
        // ------------------------------------------------
        // 寻找主相机 (System: Camera System)
        // ------------------------------------------------
        Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform;

        {
            // 遍历所有拥有 Transform 和 Camera 组件的实体
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto [entity, transform, camera] : view.each())
            {
                if (camera.primary) {
                    mainCamera = &camera.camera;
                    cameraTransform = transform.transform;
                    break; // 找到主相机就退出
                }
            }
        }

        // 如果没有主相机，就不渲染
        if (!mainCamera) return;
	}

	void Scene::OnRenderEditor(const Camera& camera, const glm::mat4& cameraTransform)
	{
        // ------------------------------------------------
        // 寻找光源 (System: Light System)
        // ------------------------------------------------
        // 这里简单处理：找第一个有点光源组件的实体，如果没有则用默认值
        glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
        glm::vec4 lightColor(1.0f);

        {
            auto view = m_Registry.view<TransformComponent, PointLightComponent>();
            for (auto [entity, transform, light] : view.each())
            {
                lightPos = glm::vec3(transform.transform[3]);
                lightColor = light.color;
                break; // 暂时只支持单光源
            }
        }


        // ------------------------------------------------
        // 渲染流程 (System: Render System)
        // ------------------------------------------------
        Renderer3D::BeginScene(camera, cameraTransform, lightPos, lightColor);


        auto modelview = m_Registry.view<TransformComponent, ModelComponent>();
        for (auto [entity, transform, model] : modelview.each())
        {
            if (model.visible && model.model) {
                Renderer3D::DrawModel(*model.model, model.edgeEnable, transform.transform);
            }
        }

        Renderer3D::EndScene();
	}

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
    }

}
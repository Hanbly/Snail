#include "SNLpch.h"

#include "Scene.h"
#include "Entity.h"

#include "Component.h"
#include "SceneUtils.h"
#include "Snail/Render/Renderer/Renderer3D.h"

namespace Snail {

	Scene::Scene()
	{
		
	}

	Scene::~Scene()
	{
        Clear();
	}

    void Scene::Clear()
    {
        m_Registry.clear();
    }

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity(m_Registry.create(), this);

        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        entity.AddComponent<UUIDComponent>(uuid);

		entity.AddComponent<TagComponent>(name.empty() ? "UnnamedEntity" : name);
		entity.AddComponent<TransformComponent>();

		return entity;
	}

    Entity Scene::CreateEntityWithUuid(const boost::uuids::uuid& uuid, const std::string& name)
    {
		Entity entity(m_Registry.create(), this);

		entity.AddComponent<UUIDComponent>(uuid);

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

    void Scene::CreateModelEntity(const std::string& path)
    {
        auto entity = CreateEntity("New Model");
		auto shader = ShaderLibrary::Load("assets/shaders/Standard.glsl", {});
		auto model = std::make_shared<Model>(shader, path);
        entity.AddComponent<ModelComponent>(model);
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
                    cameraTransform = transform.GetTransform();
                    break; // 找到主相机就退出
                }
            }
        }

        // 如果没有主相机，就不渲染
        if (!mainCamera) return;
	}

    void Scene::OnRenderEditor(const Refptr<EditorCamera>& camera, const glm::mat4& cameraTransform)
	{
        // ------------------------------------------------
        // 寻找光源 (System: Light System)
        // ------------------------------------------------
        std::vector<DirectionLight> dirLights;
        std::vector<PointLight> poiLights;
        
		// // --- 平行光源组件 ---
		{
			auto view = m_Registry.view<DirectionalLightComponent>();
			for (auto [entity, light] : view.each())
			{
				glm::vec3 lightDir = light.direction;
				glm::vec4 lightColor = light.color;
				float ambient = light.ambient;
				float diffuse = light.diffuse;
				float specular = light.specular;
				DirectionLight directionLight(lightDir, lightColor, ambient, diffuse, specular);

				dirLights.push_back(directionLight);
			}
		}

        // --- 点光源组件 ---
        {
            auto view = m_Registry.view<TransformComponent, PointLightComponent>();
            for (auto [entity, transform, light] : view.each())
            {
                glm::vec3 lightPos = transform.position;
                glm::vec4 lightColor = light.color;
				float constant = light.constant;
				float linear = light.linear;
				float quadratic = light.quadratic;
				float ambient = light.ambient;
				float diffuse = light.diffuse;
				float specular = light.specular;
                PointLight pointLight(lightPos, lightColor, constant, linear, quadratic, ambient, diffuse, specular);
                
                poiLights.push_back(pointLight);
            }
        }


        // ------------------------------------------------
        // 渲染流程 (System: Render System)
        // ------------------------------------------------
        Renderer3D::BeginScene(camera.get(), cameraTransform, dirLights, poiLights);


        auto group = m_Registry.group<TransformComponent, ModelComponent>();
        for (auto [entity, transform, model] : group.each())
        {
            
            if (model.visible && model.model) {
                Renderer3D::DrawModel(*model.model, model.edgeEnable, transform.GetTransform());
            }
        }

        auto skyboxView = m_Registry.view<ModelComponent>();
        for (auto [entity, model] : skyboxView.each())
        {
            if (!model.model->IsImported() && model.model->GetPrimitiveType() == PrimitiveType::Skybox && model.visible && model.model) {
                // 使用专门的 Skybox 绘制函数，传入相机用于计算“无位移View矩阵”
                Renderer3D::DrawSkybox(*model.model, *camera);
                break;
            }
        }

        Renderer3D::EndScene();
	}

    Entity Scene::CastRay(const float& x, const float& y, const float& width, const float& height, const glm::mat4& viewMat, const glm::mat4& projMat)
    {
        // 构建射线
        MouseRay ray(x, y, width, height, viewMat, projMat);

        float minDst = std::numeric_limits<float>::max();
        Entity hitEntity = {}; // 空实体

        // 遍历所有几何体
        auto view = this->GetAllofEntitiesWith<TransformComponent, ModelComponent>();
        for (auto [entity, transform, model] : view.each())
        {
            float currentDst = 0.0f;
            Entity e(entity, this);
            // 传入引用获取距离
            if (ray.Is_Cross(e, currentDst))
            {
                // 寻找最近的物体 (处理遮挡)
                if (currentDst < minDst)
                {
                    minDst = currentDst;
                    hitEntity = e;
                }
            }
        }
        // 更新选中状态
        // 点击空白取消选中
        if (!hitEntity.IsValid()) {
            hitEntity = {};
            return hitEntity;
        }

        // 选中
        SNL_CORE_INFO("选中物体: {0}", hitEntity.GetComponent<TagComponent>().name);

        return hitEntity;
    }

}
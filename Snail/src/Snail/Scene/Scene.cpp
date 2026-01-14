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

    glm::vec3 Scene::GetMainLightDirection() const
    {
		// --- 目前只考虑平行光源 ---
		{
			auto view = m_Registry.view<DirectionalLightComponent>();
			for (auto [entity, light] : view.each())
			{
				return glm::normalize(light.direction);
			}
		}
        return glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
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

    void Scene::OnRenderRuntime()
    {
    }

	void Scene::OnRenderEditor(const Refptr<EditorCamera>& camera, const glm::mat4& cameraTransform, const glm::mat4& mainLightSpace, const uint32_t& shadowRendererId)
	{
        // ------------------------------------------------
        // 寻找光源
        // ------------------------------------------------
        std::vector<DirectionLight> dirLights;
        std::vector<PointLight> poiLights;
        
		// --- 平行光源组件 ---
		{
			auto view = m_Registry.view<DirectionalLightComponent>();
			for (auto [entity, light] : view.each())
			{
				glm::vec3 lightDir = light.direction;
				glm::vec3 lightColor = light.color;
				float intensity = light.intensity;
				DirectionLight directionLight(lightDir, lightColor, intensity);

				dirLights.push_back(directionLight);
			}
		}

        // --- 点光源组件 ---
        {
            auto view = m_Registry.view<TransformComponent, PointLightComponent>();
            for (auto [entity, transform, light] : view.each())
            {
                glm::vec3 lightPos = transform.position;
                glm::vec3 lightColor = light.color;
				float intensity = light.intensity;
                PointLight pointLight(lightPos, lightColor, intensity);
                
                poiLights.push_back(pointLight);
            }
        }

		Refptr<Texture> irradianceMap = nullptr;
		Refptr<Texture> prefilterMap = nullptr;
        Refptr<Texture> brdfLUT = TextureLibrary::Load({ "assets/PBRMaps/IBL/BRDF_LUT.png" }, TextureUsage::None);
		// 遍历寻找 Skybox 组件
		auto tempSkyboxView = m_Registry.view<ModelComponent>();
		for (auto [entity, model] : tempSkyboxView.each())
		{
			// 找到启用的 Skybox
			if (model.visible && model.model && model.model->GetPrimitiveType() == PrimitiveType::Skybox)
			{
				// 获取 Skybox 的原始 Cubemap 纹理
				if (!model.model->GetMeshes().empty()) {
					auto& textures = model.model->GetMeshes()[0]->GetMaterial()->GetTextures();
					if (!textures.empty()) {
						auto& envMap = textures[0]; // 第一个纹理就是环境Cube

						// 从 TextureLibrary 获取对应的 IBL 纹理
						irradianceMap = TextureLibrary::GetIBLIrradianceofTexture(envMap);
						prefilterMap = TextureLibrary::GetIBLPrefilterofTexture(envMap);
					}
				}
				break; // 只支持一个 Skybox
			}
		}


        // ------------------------------------------------
        // 渲染流程
        // ------------------------------------------------
        Renderer3D::BeginScene(camera.get(), cameraTransform, dirLights, poiLights, mainLightSpace, shadowRendererId, irradianceMap, prefilterMap, brdfLUT);


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

    void Scene::OnRenderExternalShader(Refptr<Shader>& shader)
    {
		auto group = m_Registry.group<TransformComponent, ModelComponent>();
		for (auto [entity, transform, model] : group.each())
		{
			if (model.visible && model.model) {
				Renderer3D::DrawModel(shader, *model.model, transform.GetTransform());
			}
		}

		Renderer3D::EndScene(shader);
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
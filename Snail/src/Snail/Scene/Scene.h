#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Basic/Timestep.h"

#include "Snail/Render/Renderer/Material/Shader.h"
#include "Snail/Render/Renderer/Camera/EditorCamera.h"

#include <glm/glm.hpp>
#include <entt.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace Snail {
	
	class Entity;
	class Camera;

	class Scene {
		friend class Entity;
	private:
		entt::registry m_Registry;

		float m_Gamma = 2.2f;
		float m_Exposure = 1.0f;
	public:
		Scene();
		~Scene();

		void Clear();

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityWithUuid(const boost::uuids::uuid& uuid, const std::string& name = "");
		void DestroyEntity(Entity entity);

		void CreateModelEntity(const std::string& path);

		template<typename... T>
		auto GetAllofEntitiesWith()
		{
			return m_Registry.view<T...>();
		}

		inline entt::registry& GetRegistry() { return m_Registry; }
		inline const entt::registry& GetRegistry() const { return m_Registry; }

		glm::vec3 GetMainLightDirection() const;
		inline float& GetGamma() { return m_Gamma; }
		inline void SetGamma(const float& gamma) { m_Gamma = gamma; }
		inline float& GetExposure() { return m_Exposure; }
		inline void SetExposure(const float& exposure) { m_Exposure = exposure; }

		void OnUpdateRuntime(const Timestep& ts);
		void OnUpdateEditor(const Timestep& ts);

		void OnRenderRuntime();
		void OnRenderEditor(const Refptr<EditorCamera>& camera, const glm::mat4& cameraTransform, const glm::mat4& mainLightSpace, const uint32_t& shadowRendererId);

		void OnRenderExternalShader(Refptr<Shader>& shader);

		Entity CastRay(const float& x, const float& y, const float& width, const float& height, const glm::mat4& viewMat, const glm::mat4& projMat);
	};
}
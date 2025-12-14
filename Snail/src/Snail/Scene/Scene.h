#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Basic/Timestep.h"

#include "glm/glm.hpp"
#include "entt.hpp"

namespace Snail {
	
	class Entity;
	class Camera;

	class Scene {
	private:
		entt::registry m_Registry;

		glm::vec3 m_LightPosition = glm::vec3(0.0f, 100.0f, 0.0f);
		glm::vec4 m_LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity entity);

		template<typename... T>
		auto GetAllofEntitiesWith()
		{
			return m_Registry.view<T...>();
		}
		inline entt::registry& GetRegistry() { return m_Registry; }
		inline const entt::registry& GetRegistry() const { return m_Registry; }
		inline glm::vec3& GetLightPosition() { return m_LightPosition; }
		inline glm::vec4& GetLightColor() { return m_LightColor; }

		void OnUpdateRuntime(const Timestep& ts);
		void OnUpdateEditor(const Timestep& ts);

		void OnRenderRuntime();
		void OnRenderEditor(const Camera& camera, const glm::mat4& cameraTransform);

		Entity CastRay(const float& x, const float& y, const float& width, const float& height, const glm::mat4& viewMat, const glm::mat4& projMat);

		friend class Entity;
	};
}
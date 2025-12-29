#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Basic/Timestep.h"

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
	private:
		entt::registry m_Registry;

		float m_AmbientStrength = 0.1f;
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
		inline float& GetAmbientStrength() { return m_AmbientStrength; }
		inline void SetAmbientStrength(const float& ambient) { m_AmbientStrength = ambient; }

		void OnUpdateRuntime(const Timestep& ts);
		void OnUpdateEditor(const Timestep& ts);

		void OnRenderRuntime();
		void OnRenderEditor(const Refptr<EditorCamera>& camera, const glm::mat4& cameraTransform);

		Entity CastRay(const float& x, const float& y, const float& width, const float& height, const glm::mat4& viewMat, const glm::mat4& projMat);

		friend class Entity;
	};
}
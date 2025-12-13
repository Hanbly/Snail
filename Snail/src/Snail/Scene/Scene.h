#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Basic/Timestep.h"

#include "entt.hpp"

namespace Snail {
	
	class Entity;

	class Scene {
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
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

		void OnUpdate(const Timestep& ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		friend class Entity;
	};
}
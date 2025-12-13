#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Scene.h"

namespace Snail {

	class Scene;

	class Entity {
	private:
		entt::entity m_EntityId{ entt::null }; // 初始化为无效实体ID
		Scene* m_Scene = nullptr;              // 初始化为空指针
	public:
		Entity() = default;
		Entity(const entt::entity& entity, Scene* scene);
		~Entity();

		template<typename... T>
		bool HasAllofComponent() {
			SNL_CORE_ASSERT(IsValid(), "Entity: Scene 不存在或 EntityId 无效!");
			return m_Scene->m_Registry.all_of<T...>(m_EntityId);
		}

		template<typename... T>
		bool HasAnyofComponent() {
			SNL_CORE_ASSERT(IsValid(), "Entity: Scene 不存在或 EntityId 无效!");
			return m_Scene->m_Registry.any_of<T...>(m_EntityId);
		}

		template<typename T>
		T& GetComponent() {
			SNL_CORE_ASSERT(IsValid(), "Entity: Scene 不存在或 EntityId 无效!");
			SNL_CORE_ASSERT(HasAllofComponent<T>(), "Entity: 没有（任何）对应的组件!");
			return m_Scene->m_Registry.get<T>(m_EntityId);
		}

		template<typename T>
		T* TryGetComponent()
		{
			if (!IsValid()) return nullptr;
			return m_Scene->m_Registry.try_get<T>(m_EntityId);
		}

		template<typename... T>
		auto TryGetAllofComponent()
		{
			if (!IsValid()) return nullptr;
			return m_Scene->m_Registry.try_get<T...>(m_EntityId);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			SNL_CORE_ASSERT(IsValid(), "Entity: Scene 不存在或 EntityId 无效!");
			SNL_CORE_ASSERT(!HasAnyofComponent<T>(), "Entity: 已含有（任一）相同的组件!");
			return m_Scene->m_Registry.emplace<T>(m_EntityId, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent() {
			SNL_CORE_ASSERT(IsValid(), "Entity: Scene 不存在或 EntityId 无效!");
			SNL_CORE_ASSERT(HasAllofComponent<T>(), "Entity: 试图移除不存在的组件!");
			m_Scene->m_Registry.remove<T>(m_EntityId);
		}

		bool IsValid() const {
			return m_Scene != nullptr && m_Scene->m_Registry.valid(m_EntityId);
		}

		// 允许隐式转换为 entt::entity (uint32_t)
		operator entt::entity() const { return m_EntityId; }

		// 允许像 if (entity) 这样判断
		operator bool() const { return IsValid(); }

		// 比较运算符
		bool operator==(const Entity& other) const {
			return m_EntityId == other.m_EntityId && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const {
			return !(*this == other);
		}
	};

}
#include "SNLpch.h"

#include "Entity.h"

namespace Snail {

	Entity::Entity(const entt::entity& entity, Scene* scene)
		: m_EntityId(entity), m_Scene(scene)
	{
	}

	Entity::~Entity()
	{

	}

	

}
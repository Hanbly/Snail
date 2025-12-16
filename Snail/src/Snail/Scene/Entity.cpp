#include "SNLpch.h"

#include "Component.h"

#include "Entity.h"

namespace Snail {

	Entity::Entity(const entt::entity& entity, Scene* scene)
		: m_EntityId(entity), m_Scene(scene)
	{
	}

	Entity::~Entity()
	{

	}

	boost::uuids::uuid Entity::GetUUID() const
	{
		UUIDComponent* uuidComp = TryGetComponent<UUIDComponent>();
		return uuidComp ? uuidComp->uuid : boost::uuids::nil_uuid();
	}

}
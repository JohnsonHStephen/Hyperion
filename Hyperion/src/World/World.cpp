#include "World/World.h"

#include <entt/entt.hpp>

#include "World/Components.h"
#include "World/Entity.h"


namespace Hyperion
{
	World::World()
	{
		entt::entity entity = m_registry.create();
		m_registry.emplace<TransformComponent>(entity);
	}

	Entity World::createEntity(const std::string& name)
	{
		return createEntityWithUUID(uuids::uuid_system_generator{}(), name);
	}

	Entity World::createEntityWithUUID(uuids::uuid uuid, const std::string& name)
	{
		Entity entity = { m_registry.create(), this };
		entity.addComponent<IDComponent>(uuid);
		auto& tag = entity.addComponent<TagComponent>();
		tag.Tag = ((name.empty()) ? "Entity" : name);

		m_entityMap[uuid] = entity;

		return entity;
	}

	void World::destroyEntity(Entity entity)
	{
		m_entityMap.erase(entity.getUUID());
		m_registry.destroy(entity);
	}

	Entity World::findEntityByName(std::string_view name)
	{
		auto view = m_registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return Entity{ entity, this };
		}
		return {};
	}

	Entity World::getEntityByUUID(uuids::uuid uuid)
	{
		if (m_entityMap.find(uuid) != m_entityMap.end())
			return { m_entityMap.at(uuid), this };

		return {};
	}

	void World::viewportResize(uint32_t width, uint32_t height)
	{
		m_viewportWidth = width;
		m_viewportHeight = height;

		// recalculate primary camera matricies
	}
}
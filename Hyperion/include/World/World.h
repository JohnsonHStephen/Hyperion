#pragma once

#include <entt/entt.hpp>
#define UUID_SYSTEM_GENERATOR
#include <stduuid/uuid.h>

namespace Hyperion
{
	class Entity;

	class World
	{
	public:
		World();

		Entity createEntity(const std::string& name = std::string());
		Entity createEntityWithUUID(uuids::uuid uuid, const std::string& name = std::string());
		void destroyEntity(Entity entity);

		Entity findEntityByName(std::string_view name);
		Entity getEntityByUUID(uuids::uuid uuid);

		void viewportResize(uint32_t width, uint32_t height);

		template<typename... Components>
		std::vector<Entity> getAllEntitiesWith()
		{
			std::vector<Entity> entities;
			auto view = m_registry.view<Components...>();

			for (auto entity : view)
			{
				entities.emplace_back(entity, this);
			}
			return entities;
		};

	private:
		entt::registry m_registry;
		uint32_t m_viewportWidth = 0;
		uint32_t m_viewportHeight = 0;

		std::unordered_map<uuids::uuid, entt::entity> m_entityMap;

		friend class Entity;
	};
}
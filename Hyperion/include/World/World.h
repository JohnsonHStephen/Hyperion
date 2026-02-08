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
		World(uint32_t width, uint32_t height);

		Entity createEntity(const std::string& name = std::string());
		Entity createEntityWithUUID(uuids::uuid uuid, const std::string& name = std::string());
		void destroyEntity(Entity entity);

		Entity findEntityByName(std::string_view name);
		Entity getEntityByUUID(uuids::uuid uuid);

		Entity getPrimaryCameraEntity();

		void viewportResize(uint32_t width, uint32_t height);

		template<typename... Components>
		auto getAllEntitiesWith()
		{
			return m_registry.view<Components...>();
		}

	private:
		entt::registry m_registry;
		uint32_t m_viewportWidth;
		uint32_t m_viewportHeight;

		std::unordered_map<uuids::uuid, entt::entity> m_entityMap;

		friend class Entity;
	};
}
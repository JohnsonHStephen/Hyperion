#pragma once

#include <entt/entt.hpp>
#include <stduuid/uuid.h>

#include "World/Components.h"


namespace Hyperion
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, World* world)
			: m_entityHandle(handle), m_world(world)
		{
			if (m_world == nullptr)
				throw std::runtime_error("Entity must be part of a world");
		};
		Entity(const Entity& other)
		{
			m_entityHandle = other.m_entityHandle;
			m_world = other.m_world;
		};

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			if (hasComponent<T>())
				throw std::runtime_error("Entity already has component");

			return m_world->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& addOrReplaceComponent(Args&&... args)
		{
			return m_world->m_registry.emplace_or_replace<T>(m_entityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent()
		{
			if (!hasComponent<T>())
				throw std::runtime_error("Entity doesn't have component");

			return m_world->m_registry.get<T>(m_entityHandle);
		}

		template<typename T>
		bool hasComponent()
		{
			if (m_world == nullptr)
				throw std::runtime_error("Entity is not part of a world");
			return m_world->m_registry.all_of<T>(m_entityHandle);
		}

		template<typename T>
		void removeComponent()
		{
			if (!hasComponent<T>())
				throw std::runtime_error("Entity doesn't have component");

			m_world->m_registry.remove<T>(m_entityHandle);
		}

		operator bool() const { return m_entityHandle != entt::null; }
		operator entt::entity() const { return m_entityHandle; }
		operator uint32_t() const { return (uint32_t)m_entityHandle; }

		uuids::uuid getUUID() { return getComponent<IDComponent>().ID; }
		const std::string& getName() { return getComponent<TagComponent>().Tag; }

		bool operator==(const Entity& other) const
		{
			return m_entityHandle == other.m_entityHandle && m_world == other.m_world;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

	private:
		entt::entity m_entityHandle{ entt::null };

		World* m_world = nullptr;
	};
}
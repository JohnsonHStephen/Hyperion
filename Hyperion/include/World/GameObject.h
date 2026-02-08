#pragma once

#include "Graphics/Mesh.h"

#include "World/Components.h"

namespace Hyperion
{
	// Temporary object class before entity component system is implemented
	class GameObject
	{
	public:
		GameObject(const std::string name, VulkanDevice* device, glm::vec3 translate);
		~GameObject() = default;

		Mesh mesh;
		TransformComponent transform;
	};
}
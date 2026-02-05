#include "Scene/GameObject.h"

namespace Hyperion
{
	GameObject::GameObject(const std::string name, VulkanDevice* device, glm::vec3 translate):
		mesh(name, device), transform(translate)
	{}
}
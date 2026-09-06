#include "Graphics/Gui/Adapters/GLFWWindowAdapter.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Hyperion
{
	GLFWWindowAdapter::GLFWWindowAdapter(GLFWwindow* window)
		: m_window(window)
	{
	}

	VkSurfaceKHR GLFWWindowAdapter::createVulkanSurface(VkInstance instance)
	{
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(instance, m_window, nullptr, &surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create Vulkan surface!");
		}
		return surface;
	}
}

#pragma once

#include "Graphics/Gui/Adapters/WindowGraphicsAdapter.h"

// Forward declare GLFWwindow to avoid including GLFW headers here
struct GLFWwindow;

namespace Hyperion
{
	class GLFWWindowAdapter : public WindowGraphicsAdapter
	{
	public:
		explicit GLFWWindowAdapter(GLFWwindow* window);
		~GLFWWindowAdapter() override = default;

		VkSurfaceKHR createVulkanSurface(VkInstance instance) override;

	private:
		GLFWwindow* m_window;
	};
}

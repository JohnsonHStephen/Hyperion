#pragma once

#include <volk.h>
#include <memory>

namespace Hyperion
{
	class WindowGraphicsAdapter
	{
	public:
		virtual ~WindowGraphicsAdapter() = default;

		// Vulkan surface creation - to be implemented by platform-specific adapters
		virtual VkSurfaceKHR createVulkanSurface(VkInstance instance) = 0;
	};

	// Convenience using for unique pointers to adapters
	using WindowGraphicsAdapterPtr = std::unique_ptr<WindowGraphicsAdapter>;
}

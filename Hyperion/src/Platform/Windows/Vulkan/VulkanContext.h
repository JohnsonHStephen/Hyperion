#pragma once

#include <Vulkan/vulkan.h>

namespace Hyperion
{
	class VulkanContext {
	public:
		VulkanContext();
		~VulkanContext();

		void Init();

	private:
		VkInstance m_instance;

	private:
	};
}
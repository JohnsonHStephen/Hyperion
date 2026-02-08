#pragma once

#include <vulkan/vulkan.h>

#include "Graphics/Gui/Window.h"

namespace Hyperion
{
	class ImGuiOverlay
	{
	public:
		struct InitInfo
		{
			Window* Window;
			VkInstance Instance;
			VkPhysicalDevice PhysicalDevice;
			VkDevice Device;
			uint32_t QueueFamily;
			VkQueue Queue;
			VkPipelineCache PipelineCache;
			VkDescriptorPool DescriptorPool;
			uint32_t ImageCount;
			VkAllocationCallbacks* Allocator;
			VkRenderPass RenderPass;
		};

	public:
		void init(InitInfo initInfo);
		void cleanup();

		void begin();
		void end(VkCommandBuffer commandBuffer);
	};
}

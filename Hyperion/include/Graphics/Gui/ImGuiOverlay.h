#pragma once

#include "Graphics/Gui/Window.h"

namespace Hyperion
{
	class ImGuiOverlay
	{
	public:
		struct InitInfo
		{
			Window* Window;
			uint32_t ApiVersion;
			VkInstance Instance;
			VkPhysicalDevice PhysicalDevice;
			VkDevice Device;
			uint32_t QueueFamily;
			VkQueue Queue;
			VkPipelineCache PipelineCache;
			uint32_t ImageCount;
			VkAllocationCallbacks* Allocator;
			VkPipelineRenderingCreateInfo PipelineRenderingCreateInfo;
		};

	public:
		void init(InitInfo initInfo);
		void cleanup();

		void begin();
		void end(VkCommandBuffer commandBuffer);
	};
}

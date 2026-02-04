#pragma once

#include <Vulkan/vulkan.h>

#include "Renderer/VulkanPipeline.h"

namespace Hyperion
{
	class MeshPipeline
	{
	public:
		void init(VulkanDevice* device, VkRenderPass renderPass);
		void cleanup();

		void draw(VkCommandBuffer& commandBuffer, const std::vector<VulkanBuffer>& vertexBuffers);
	private:
		void createGraphicsPipelineLayout();
		void createGraphicsPipeline(VkRenderPass renderPass);

	private:
		VulkanDevice* m_device = nullptr;

		VulkanPipeline m_pipeline{};
		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	};
}
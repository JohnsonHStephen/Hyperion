#pragma once

#include <Vulkan/vulkan.h>

#include "Graphics/Vulkan/VulkanPipeline.h"

#include <Graphics/Mesh.h>

namespace Hyperion
{
	class MeshPipeline
	{
	public:
		void init(VulkanDevice* device, VkRenderPass renderPass);
		void cleanup();

		void draw(VkCommandBuffer& commandBuffer, const std::vector<Mesh>& meshs);
	private:
		void createGraphicsPipelineLayout();
		void createGraphicsPipeline(VkRenderPass renderPass);

	private:
		VulkanDevice* m_device = nullptr;

		VulkanPipeline m_pipeline{};
		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	};
}
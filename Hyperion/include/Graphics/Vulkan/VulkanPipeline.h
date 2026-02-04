#pragma once

#include <Vulkan/vulkan.h>

#include "Core/HyperionPCH.h"

#include "Graphics/Vulkan/VulkanDevice.h"

namespace Hyperion
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		VkViewport viewport{};
		VkRect2D scissor{};
		VkPipelineViewportStateCreateInfo viewportInfo{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		std::vector<VkDynamicState> dynamicStateEnables{};
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class VulkanPipeline
	{
	public:
		VulkanPipeline() {}
		~VulkanPipeline() {}

		VulkanPipeline(const VulkanPipeline&) = delete;
		void operator=(const VulkanPipeline&) = delete;

		void init(
			VulkanDevice* device,
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo,
			const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
			const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions,
			uint32_t binding = 0);
		void cleanup();

		void bind(VkCommandBuffer commandBuffer);

		static void defaultPipelineConfigInfo(
			PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);

	private:
		static std::vector<char> readFile(const std::string& filename);

		void createGraphicsPipeline(
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo,
			const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
			const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions,
			uint32_t binding);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

	private:
		VulkanDevice* m_device = nullptr;
		VkPipelineLayout	m_pipelineLayout = VK_NULL_HANDLE;
		VkPipeline			m_graphicsPipeline = VK_NULL_HANDLE;
		VkShaderModule		m_vertShaderModule = VK_NULL_HANDLE;
		VkShaderModule		m_fragShaderModule = VK_NULL_HANDLE;
	};
}
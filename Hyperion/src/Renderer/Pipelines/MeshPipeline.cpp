#include "Renderer/Pipelines/MeshPipeline.h"

#include "Core/Application.h"

namespace Hyperion
{
	void MeshPipeline::init(VulkanDevice* device, VkRenderPass renderPass)
	{
		m_device = device;

		createGraphicsPipelineLayout();
		createGraphicsPipeline(renderPass);
	}

	void MeshPipeline::cleanup()
	{
		vkDestroyPipelineLayout(m_device->device(), m_pipelineLayout, nullptr);
	}

	void MeshPipeline::draw(VkCommandBuffer& commandBuffer, const std::vector<VulkanBuffer>& vertexBuffers)
	{
		m_pipeline.bind(commandBuffer);

		for (const auto& vertexBuffer : vertexBuffers)
		{
			VkBuffer buffers[] = { vertexBuffer.getInternalBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
			vkCmdDraw(commandBuffer, vertexBuffer.getInstanceCount(), 1, 0, 0);
		}

	}

	void MeshPipeline::createGraphicsPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(m_device->device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void MeshPipeline::createGraphicsPipeline(VkRenderPass renderPass)
	{
		BufferLayout layout = BufferLayout({
			{ ShaderDataType::e_Float2, "inPosition" },
			{ ShaderDataType::e_Float3, "inColour" },
			});

		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig, Application::get()->getWindow()->getWidth(), Application::get()->getWindow()->getHeight());
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		m_pipeline.init(
			m_device,
			"../../../../Hyperion/include/Renderer/shaders/vert.spv",
			"../../../../Hyperion/include/Renderer/shaders/frag.spv",
			pipelineConfig,
			layout);
	}
}
#include "Graphics/Pipelines/MeshPipeline.h"

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

	void MeshPipeline::draw(VkCommandBuffer& commandBuffer, const Camera& camera, const std::vector<GameObject>& objects)
	{
		m_pipeline.bind(commandBuffer);

		for (const auto& object : objects)
		{
			PushConstants pushConstants{};
			pushConstants.transform = object.transform.getTransform();
			pushConstants.normal = object.transform.getNormal();
			pushConstants.projView = camera.getProjection() * camera.getView();

			vkCmdPushConstants(
				commandBuffer,
				m_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PushConstants),
				&pushConstants);

			VkBuffer buffers[] = { object.mesh.getVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, object.mesh.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(commandBuffer, object.mesh.getIndexCount(), 1, 0, 0, 0);
		}

	}

	void MeshPipeline::createGraphicsPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstants);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_device->device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void MeshPipeline::createGraphicsPipeline(VkRenderPass renderPass)
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Mesh::Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Mesh::Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Mesh::Vertex, colour) });

		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig, Application::get()->getWindow()->getWidth(), Application::get()->getWindow()->getHeight());
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		m_pipeline.init(
			m_device,
			"../../../../Hyperion/include/Graphics/shaders/vert.spv",
			"../../../../Hyperion/include/Graphics/shaders/frag.spv",
			pipelineConfig,
			bindingDescriptions,
			attributeDescriptions);
	}
}
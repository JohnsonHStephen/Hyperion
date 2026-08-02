#include "Graphics/Pipelines/MeshPipeline.h"

#include "Core/Application.h"

#include "World/Components.h"
#include "World/Entity.h"

namespace Hyperion
{
	void MeshPipeline::init(VulkanDevice* device, VkRenderPass renderPass, WindowSize frameBufferSize)
	{
		m_device = device;

		createGraphicsPipelineLayout();
		createGraphicsPipeline(renderPass, frameBufferSize);
	}

	void MeshPipeline::cleanup()
	{
		vkDestroyPipelineLayout(m_device->device(), m_pipelineLayout, nullptr);
	}

	void MeshPipeline::draw(VkCommandBuffer& commandBuffer, Entity& camera, World& world, const MeshCache& meshCache)
	{
		m_pipeline.bind(commandBuffer);

		auto meshEntities = world.getAllEntitiesWith<TransformComponent, MeshComponent>();
		for (auto& meshEntity : meshEntities)
		{
			PushConstants pushConstants{};

			auto transform = meshEntity.getComponent<TransformComponent>();
			auto meshId = meshEntity.getComponent<MeshComponent>();
			pushConstants.transform = transform.getTransform();
			pushConstants.normal = transform.getNormal();
			pushConstants.projView = camera.getComponent<CameraComponent>().camera.getProjection() * camera.getComponent<TransformComponent>().getTransform();

			vkCmdPushConstants(
				commandBuffer,
				m_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PushConstants),
				&pushConstants);

			const auto& mesh = meshCache.getMesh(meshId.meshId);
			VkBuffer buffers[] = { mesh.vertexBuffer.getInternalBuffer()};
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer.getInternalBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(commandBuffer, mesh.numIndicies, 1, 0, 0, 0);
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

	void MeshPipeline::createGraphicsPipeline(VkRenderPass renderPass, WindowSize frameBufferSize)
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(CPUMesh::Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(CPUMesh::Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(CPUMesh::Vertex, colour) });

		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig, frameBufferSize.width, frameBufferSize.height);
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
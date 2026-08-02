#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <Vulkan/vulkan.h>

#include "Core/HyperionPCH.h"

#include "Graphics/Gui/Window.h"

#include "Graphics/Vulkan/VulkanDevice.h"
#include "Graphics/Vulkan/VulkanPipeline.h"

#include "Resources/MeshCache.h"

#include "World/Camera.h"
#include "World/World.h"

namespace Hyperion
{
	class MeshPipeline
	{
	public:
		void init(VulkanDevice* device, VkRenderPass renderPass, WindowSize frameBufferSize);
		void cleanup();

		void draw(VkCommandBuffer& commandBuffer, Entity& camera, World& world, const MeshCache& meshCache);

	private:
		struct PushConstants
		{
			glm::mat4 transform;
			glm::mat4 normal;
			glm::mat4 projView;
		};

	private:
		void createGraphicsPipelineLayout();
		void createGraphicsPipeline(VkRenderPass renderPass, WindowSize frameBufferSize);

	private:
		VulkanDevice* m_device = nullptr;

		VulkanPipeline m_pipeline{};
		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	};
}
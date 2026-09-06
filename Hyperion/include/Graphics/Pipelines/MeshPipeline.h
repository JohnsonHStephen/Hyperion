#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Core/HyperionPCH.h"

#include "Graphics/Gui/Window.h"

#include "Graphics/Vulkan/VulkanDevice.h"
#include "Graphics/Vulkan/VulkanPipeline.h"

#include "Resources/MeshCache.h"

#include "World/World.h"

namespace Hyperion
{
	class MeshPipeline
	{
	public:
		void init(VulkanDevice* device, WindowSize frameBufferSize, const VkFormat& imageFormat, const VkFormat& depthFormat);
		void cleanup();

		void draw(VkCommandBuffer& commandBuffer, Entity& camera, World& world, const MeshCache& meshCache);

	public:
		VulkanPipeline pipeline{};

	private:
		struct PushConstants
		{
			glm::mat4 transform;
			glm::mat4 normal;
			glm::mat4 projView;
		};

	private:
		void createGraphicsPipeline(WindowSize frameBufferSize, const VkFormat& imageFormat, const VkFormat& depthFormat);

	private:
		VulkanDevice* m_device = nullptr;
	};
}
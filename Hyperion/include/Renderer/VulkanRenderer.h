#pragma once

#include <optional>
#include <Vulkan/vulkan.h>

#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanSwapChain.h"

namespace Hyperion
{
	class VulkanRenderer
	{
	public:
		VulkanRenderer() {}
		~VulkanRenderer() {}

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;

		void init(VulkanDevice* device);
		void cleanup();

		VkRenderPass getSwapChainRenderPass() const { return m_swapChain->getRenderPass(); }
		bool isFrameInProgress() const { return m_isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const
		{
			if (!m_isFrameStarted)
				std::runtime_error("Cannot get command buffer when frame not in progress");
			return m_commandBuffers[m_currentFrameIndex];
		}

		int getFrameIndex() const
		{
			if (!m_isFrameStarted)
				std::runtime_error("Cannot get command buffer when frame not in progress");
			return m_currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

	private:
		VulkanDevice*						m_device	= nullptr;
		std::unique_ptr<VulkanSwapChain>	m_swapChain	= nullptr;
		std::vector<VkCommandBuffer>		m_commandBuffers{};

		uint32_t							m_currentImageIndex = 0;
		int									m_currentFrameIndex = 0;
		bool								m_isFrameStarted = false;
	};
}
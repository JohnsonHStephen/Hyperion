#pragma once

#include <Vulkan/vulkan.h>

#include "Graphics/Gui/Window.h"

#include "Graphics/Vulkan/VulkanDevice.h"
#include "Graphics/Vulkan/VulkanSwapChain.h"

namespace Hyperion
{
	class VulkanRenderer
	{
	public:
		VulkanRenderer() {}
		~VulkanRenderer() {}

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;

		void init(VulkanDevice* device, WindowSize frameBufferSize);
		void cleanup();

		VkRenderPass getSwapChainRenderPass() const { return m_swapChain->getRenderPass(); }
		bool isFrameInProgress() const { return m_isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const
		{
			return m_commandBuffers[getFrameIndex()];
		}

		int getFrameIndex() const
		{
			if (!m_isFrameStarted)
				throw std::runtime_error("Cannot get frame index when frame not in progress");
			if (!m_swapChain)
				throw std::runtime_error("Cannot get frame index swapchain doesnt exist");
			return m_swapChain->frameIndex();
		}

		uint32_t getImageCount() const { return m_swapChain->imageCount(); }

		VkCommandBuffer beginFrame(Window* window);
		void endFrame(Window* window);
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain(WindowSize frameBufferSize);

	private:
		VulkanDevice*						m_device	= nullptr;
		std::unique_ptr<VulkanSwapChain>	m_swapChain	= nullptr;
		std::vector<VkCommandBuffer>		m_commandBuffers{};

		uint32_t							m_currentImageIndex = 0;
		int									m_currentFrameIndex = 0;
		bool								m_isFrameStarted = false;
	};
}
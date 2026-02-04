#pragma once

#include "Graphics/Vulkan/VulkanDevice.h"

namespace Hyperion
{
	class VulkanSwapChain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		VulkanSwapChain() {}
		~VulkanSwapChain() { cleanup(); }

		VulkanSwapChain(const VulkanSwapChain&) = delete;
		VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

		void init(VulkanDevice* device,
					VkExtent2D windowExtent,
					std::shared_ptr<VulkanSwapChain> previous = nullptr);
		void cleanup();

		VkFramebuffer getFrameBuffer(int index) const { return m_swapChainFramebuffers[index]; }
		VkRenderPass getRenderPass() const { return m_renderPass; }
		VkImageView getImageView(int index) const { return m_swapChainImageViews[index]; }
		size_t imageCount() const { return m_swapChainImages.size(); }
		VkFormat getSwapChainImageFormat() const { return m_swapChainImageFormat; }
		VkExtent2D getSwapChainExtent() const { return m_swapChainExtent; }
		uint32_t width() const { return m_swapChainExtent.width; }
		uint32_t height() const { return m_swapChainExtent.height; }
		uint32_t frameIndex() const { return m_currentFrame; }

		float extentAspectRatio() const
		{
			return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
		}
		VkFormat findDepthFormat();

		VkResult acquireNextImage(uint32_t* imageIndex);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

		bool compareSwapFormats(const VulkanSwapChain& swapChain) const
		{
			return swapChain.m_swapChainDepthFormat == m_swapChainDepthFormat &&
				swapChain.m_swapChainImageFormat == m_swapChainImageFormat;
		}

	private:
		void createSwapChain();
		void createImageViews();
		void createDepthResources();
		void createRenderPass();
		void createFramebuffers();
		void createSyncObjects();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		VkFormat	m_swapChainImageFormat	= VK_FORMAT_UNDEFINED;
		VkFormat	m_swapChainDepthFormat	= VK_FORMAT_UNDEFINED;
		VkExtent2D	m_swapChainExtent{};

		std::vector<VkFramebuffer>	m_swapChainFramebuffers{};
		VkRenderPass				m_renderPass = VK_NULL_HANDLE;

		std::vector<VkImage>		m_depthImages{};
		std::vector<VkDeviceMemory>	m_depthImageMemorys{};
		std::vector<VkImageView>	m_depthImageViews{};
		std::vector<VkImage>		m_swapChainImages{};
		std::vector<VkImageView>	m_swapChainImageViews{};

		VulkanDevice*	m_device = nullptr;
		VkExtent2D		m_windowExtent{};

		VkSwapchainKHR						m_swapChain		= VK_NULL_HANDLE;
		std::shared_ptr<VulkanSwapChain>	m_oldSwapChain	= nullptr;

		std::vector<VkSemaphore>	m_imageAvailableSemaphores{};
		std::vector<VkSemaphore>	m_renderFinishedSemaphores{};
		std::vector<VkFence>		m_inFlightFences{};
		std::vector<VkFence>		m_imagesInFlight{};
		size_t						m_currentFrame = 0;
	};
}
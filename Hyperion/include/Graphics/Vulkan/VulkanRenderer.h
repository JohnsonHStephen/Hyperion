#pragma once

#include "Graphics/Gui/Window.h"

#include "Graphics/Pipelines/MeshPipeline.h"

#include "Graphics/Vulkan/VulkanDevice.h"
#include "Graphics/Vulkan/VulkanSwapChain.h"

namespace Hyperion
{
	struct FrameResources
	{
		uint32_t			lastFrameId = 0;
		VkCommandPool		commandPool = VK_NULL_HANDLE;
		VkCommandBuffer		commandBuffer = VK_NULL_HANDLE;
		VkSemaphore			imageAvailableSemaphore = VK_NULL_HANDLE;
	};

	class VulkanRenderer
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		VulkanRenderer() {}
		~VulkanRenderer() {}

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;

		void init(VulkanDevice* device, WindowSize frameBufferSize);
		void cleanup();

		bool isFrameInProgress() const { return m_isFrameStarted; }

		uint64_t getFrameIndex() const
		{
			if (!m_isFrameStarted)
				throw std::runtime_error("Cannot get frame index when frame not in progress");
			return m_currentFrameIndex;
		}

		uint32_t getImageCount() const { return m_swapChain->imageCount(); }

		VkCommandBuffer beginFrame(Window* window);
		void endFrame(Window* window);
		void beginRendering(VkCommandBuffer commandBuffer);
		void endRendering(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer, Entity& camera, World& world, MeshCache& meshCache);

		VkPipelineRenderingCreateInfo getSwapChainPipelineRenderingCreateInfo() const;

	private:
		void createCommandBuffers();
		void recreateSwapChain(WindowSize frameBufferSize);

		void createSyncObjects();

		uint64_t getCurrentFrameId() const { return m_currentFrameIndex % MAX_FRAMES_IN_FLIGHT; }

	private:
		VulkanDevice*										m_device	= nullptr;
		std::unique_ptr<VulkanSwapChain>					m_swapChain	= nullptr;

		std::array<FrameResources, MAX_FRAMES_IN_FLIGHT>	m_frameResources{};
		std::vector<VkSemaphore>							m_renderFinishedSemaphore;

		MeshPipeline										m_meshPipeline{};

		uint32_t											m_currentImageIndex = 0;
		uint64_t											m_currentFrameIndex = 0;
		uint64_t											m_nextSignalValue = MAX_FRAMES_IN_FLIGHT+1;
		bool												m_isFrameStarted = false;
	};
}
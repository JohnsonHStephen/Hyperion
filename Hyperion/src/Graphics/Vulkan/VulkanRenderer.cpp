#include "Graphics/Vulkan/VulkanRenderer.h"

namespace Hyperion
{
	void VulkanRenderer::init(VulkanDevice* device, WindowSize frameBufferSize)
	{
		m_device = device;

		recreateSwapChain(frameBufferSize);
		createCommandBuffers();
		createSyncObjects();

		m_meshPipeline.init(m_device, frameBufferSize, m_swapChain->getSwapChainImageFormat(), m_swapChain->getSwapChainDepthFormat());
	}

	void VulkanRenderer::cleanup()
	{
		m_meshPipeline.cleanup();

		for (FrameResources& frameResource : m_frameResources)
		{
			if (frameResource.imageAvailableSemaphore != nullptr)
			{
				vkDestroySemaphore(m_device->device(), frameResource.imageAvailableSemaphore, nullptr);
				frameResource.imageAvailableSemaphore = nullptr;
			}

			if (frameResource.commandPool != nullptr)
			{
				vkDestroyCommandPool(m_device->device(), frameResource.commandPool, nullptr);
				frameResource.commandPool = nullptr;
			}
		}

		for (VkSemaphore& semaphore : m_renderFinishedSemaphore)
		{
			if (semaphore != nullptr)
			{
				vkDestroySemaphore(m_device->device(), semaphore, nullptr);
			}
		}
		m_renderFinishedSemaphore.clear();
	}

	VkCommandBuffer VulkanRenderer::beginFrame(Window* window)
	{
		if (m_isFrameStarted)
			throw std::runtime_error("Can't call beginFrame while already in progress");

		if (window->isMinimized())
			return nullptr;

		const uint64_t waitSignalValue = ++m_nextSignalValue - MAX_FRAMES_IN_FLIGHT;

		m_device->waitForTimelineSemaphore(waitSignalValue);

		FrameResources& currentFrameResource = m_frameResources[getCurrentFrameId()];
		vkResetCommandPool(m_device->device(), currentFrameResource.commandPool, 0);

		auto result = m_swapChain->acquireNextImage(&m_currentImageIndex, currentFrameResource.imageAvailableSemaphore);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain(window->getFramebufferPixelSize());
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		m_isFrameStarted = true;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(currentFrameResource.commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return currentFrameResource.commandBuffer;
	}

	void VulkanRenderer::endFrame(Window* window)
	{
		if (!m_isFrameStarted)
			throw std::runtime_error("Can't call endFrame while frame is not in progress");

		FrameResources& currentFrameResource = m_frameResources[getCurrentFrameId()];

		// transition the image from color attachment to presentation so we can show it
		VkImageMemoryBarrier2 presentLayoutBarrier{};
		presentLayoutBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		presentLayoutBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		presentLayoutBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		presentLayoutBarrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE; // nothing is waiting, but the cache is flushed and layout is transition
		presentLayoutBarrier.dstAccessMask = 0;
		presentLayoutBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		presentLayoutBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		presentLayoutBarrier.image = m_swapChain->getImage(m_currentImageIndex);
		presentLayoutBarrier.subresourceRange =
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};

		VkDependencyInfo presentDepInfo{};
		presentDepInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		presentDepInfo.imageMemoryBarrierCount = 1;
		presentDepInfo.pImageMemoryBarriers = &presentLayoutBarrier;

		vkCmdPipelineBarrier2(currentFrameResource.commandBuffer, &presentDepInfo);

		if (vkEndCommandBuffer(currentFrameResource.commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}

		// ensure swapchain image is actually vailable to start color output
		VkSemaphoreSubmitInfo imageAcquireWaitInfo{};
		imageAcquireWaitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		imageAcquireWaitInfo.semaphore = currentFrameResource.imageAvailableSemaphore;
		imageAcquireWaitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | // wait before drawing to image
			VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT; // prevent depth buffer clearing before image is ready

		// signal that the image can be presented
		std::vector<VkSemaphoreSubmitInfo> semaphoreSignals
		{
			{ // render work completion signal
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
				.semaphore = m_renderFinishedSemaphore[m_currentImageIndex],
				.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT
			},
			{ // entire frame is completed (timeline)
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
				.semaphore = m_device->timelineSemaphore(),
				.value = m_nextSignalValue,
				.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
			}
		};
		VkCommandBufferSubmitInfo cmdSubmitInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.commandBuffer = currentFrameResource.commandBuffer,
		};
		VkSubmitInfo2 submitInfo
		{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
			.waitSemaphoreInfoCount = 1,
			.pWaitSemaphoreInfos = &imageAcquireWaitInfo, // ensure the image is ready
			.commandBufferInfoCount = 1,
			.pCommandBufferInfos = &cmdSubmitInfo,
			.signalSemaphoreInfoCount = static_cast<uint32_t>(semaphoreSignals.size()),
			.pSignalSemaphoreInfos = semaphoreSignals.data()
		};
		vkQueueSubmit2(m_device->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

		auto result = m_swapChain->queuePresentCommandBuffers(&m_currentImageIndex, m_renderFinishedSemaphore[m_currentImageIndex]);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			window->isResized())
		{
			window->resetResized();
			recreateSwapChain(window->getFramebufferPixelSize());
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		m_isFrameStarted = false;
		m_currentFrameIndex++;
	}

	void VulkanRenderer::beginRendering(VkCommandBuffer commandBuffer)
	{
		if (!m_isFrameStarted)
			throw std::runtime_error("Can't call beginRendering if frame is not in progress");

		FrameResources& currentFrameResource = m_frameResources[getCurrentFrameId()];

		if (commandBuffer != currentFrameResource.commandBuffer)
			throw std::runtime_error("Can't begin render pass on command buffer from a different frame");

		std::vector<VkImageMemoryBarrier2> layoutBarriers
		{
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
				.srcAccessMask = 0,
				.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.image = m_swapChain->getImage(m_currentImageIndex),
				.subresourceRange
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				}
			},
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT, // both specified to control memory access at both stages (write)
				.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT, // both specified to control memory access at both stages (write)
				.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
				.image = m_swapChain->getDepthImage(m_currentImageIndex),
				.subresourceRange
				{
					.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				}
			}
		};

		VkDependencyInfo depInfo{};
		depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		depInfo.imageMemoryBarrierCount = static_cast<uint32_t>(layoutBarriers.size());
		depInfo.pImageMemoryBarriers = layoutBarriers.data();

		vkCmdPipelineBarrier2(commandBuffer, &depInfo);

		VkRenderingAttachmentInfo colourAttachInfo{};
		colourAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colourAttachInfo.imageView = m_swapChain->getImageView(m_currentImageIndex);
		colourAttachInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colourAttachInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear the image
		colourAttachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // keep data for presentation
		colourAttachInfo.clearValue.color = { 0.01f, 0.01f, 0.01f, 1.0f };

		VkRenderingAttachmentInfo depthAttachInfo{};
		depthAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachInfo.imageView = m_swapChain->getDepthImageView(m_currentImageIndex);
		depthAttachInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		depthAttachInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear the depth data
		depthAttachInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // don't care after rendering
		depthAttachInfo.clearValue.depthStencil = { 1.0f, 0 };

		VkRenderingInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.renderArea.extent = m_swapChain->getSwapChainExtent();
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colourAttachInfo;
		renderingInfo.pDepthAttachment = &depthAttachInfo;

		vkCmdBeginRendering(commandBuffer, &renderingInfo);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_swapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_meshPipeline.pipeline.pipeline());
	}

	void VulkanRenderer::endRendering(VkCommandBuffer commandBuffer)
	{
		if (!m_isFrameStarted)
			throw std::runtime_error("Can't call endRendering if frame is not in progress");

		FrameResources& currentFrameResource = m_frameResources[getCurrentFrameId()];

		if (commandBuffer != currentFrameResource.commandBuffer)
			throw std::runtime_error("Can't end render pass on command buffer from a different frame");

		vkCmdEndRendering(commandBuffer);
	}

	void VulkanRenderer::draw(VkCommandBuffer commandBuffer, Entity& camera, World& world, MeshCache& meshCache)
	{
		m_meshPipeline.draw(commandBuffer, camera, world, meshCache);
	}

	VkPipelineRenderingCreateInfo VulkanRenderer::getSwapChainPipelineRenderingCreateInfo() const
	{
		VkPipelineRenderingCreateInfo pipelineRenderingInfo{};
		pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		pipelineRenderingInfo.colorAttachmentCount = 1;
		pipelineRenderingInfo.pColorAttachmentFormats = &m_swapChain->getSwapChainImageFormatPtr();
		pipelineRenderingInfo.depthAttachmentFormat = m_swapChain->getSwapChainDepthFormat();

		return pipelineRenderingInfo;
	}

	void VulkanRenderer::createCommandBuffers()
	{
		for (FrameResources& frameResource : m_frameResources)
		{
			frameResource.commandPool = m_device->createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = frameResource.commandPool;
			allocInfo.commandBufferCount = 1;
			if (vkAllocateCommandBuffers(m_device->device(), &allocInfo, &frameResource.commandBuffer) !=
				VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate command buffers!");
			}
		}
	}

	void VulkanRenderer::recreateSwapChain(WindowSize frameBufferSize)
	{
		vkDeviceWaitIdle(m_device->device());

		VkExtent2D extent(frameBufferSize.width, frameBufferSize.height);
		if (m_swapChain == nullptr)
		{
			m_swapChain = std::make_unique<VulkanSwapChain>();
			m_swapChain->init(m_device, extent);
		}
		else
		{
			std::shared_ptr<VulkanSwapChain> oldSwapChain = std::move(m_swapChain);
			m_swapChain = std::make_unique<VulkanSwapChain>();
			m_swapChain->init(m_device, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*m_swapChain.get()))
			{
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
	}

	void VulkanRenderer::createSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (FrameResources &resource : m_frameResources)
		{
			if (vkCreateSemaphore(m_device->device(), &semaphoreInfo, nullptr, &resource.imageAvailableSemaphore) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create image-available semaphore");
			}
		}

		m_renderFinishedSemaphore.resize(m_swapChain->imageCount());
		for (VkSemaphore& semaphore : m_renderFinishedSemaphore)
		{
			if (vkCreateSemaphore(m_device->device(), &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create render-complete semaphore");
			}
		}
	}
}
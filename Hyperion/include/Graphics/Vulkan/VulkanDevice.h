#pragma once

#include <volk.h>
#include <vma/vk_mem_alloc.h>

#include "Core/HyperionPCH.h"

#include "Graphics/Gui/Window.h"

namespace Hyperion
{
	class ApplicationInfo; // Forward declaration for Game.h

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct InstanceVersion
	{
		uint32_t Number = 0;

		int Major() const { return VK_API_VERSION_MAJOR(Number); }
		int Minor() const { return VK_API_VERSION_MINOR(Number); }
		int Patch() const { return VK_API_VERSION_PATCH(Number); }
	};

	class VulkanDevice
	{
	public:
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif
		VkPhysicalDeviceProperties properties;

	public:
		VulkanDevice() = default;
		~VulkanDevice() = default;

		// Not copyable or movable
		VulkanDevice(const VulkanDevice&) = delete;
		VulkanDevice& operator=(const VulkanDevice&) = delete;
		VulkanDevice(VulkanDevice&&) = delete;
		VulkanDevice& operator=(VulkanDevice&&) = delete;

		void init(const ApplicationInfo& appInfo, Window* window);
		void cleanup();

		VkInstance getInstance() const { return m_instance; }
		VkDevice device() const { return m_device_; }
		VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
		VkSurfaceKHR surface() const { return m_surface_; }
		uint32_t graphicsQueueFamily() const { return m_graphicsQueueFamilyIndex; }
		VkQueue graphicsQueue() const { return m_graphicsQueue_; }
		VmaAllocator allocator() const { return m_vmaAllocator; }
		VkSemaphore timelineSemaphore() const { return m_timelineSemaphore; }

		SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(m_physicalDevice); }
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		std::optional<uint32_t> findPhysicalQueueFamilies() { return findQueueFamilies(m_physicalDevice); }
		VkFormat findSupportedFormat(
			const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		void createBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& bufferMemory);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset = 0);
		//void copyBufferToImage(
		//	VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

		//void createImageWithInfo(
		//	const VkImageCreateInfo& imageInfo,
		//	VkMemoryPropertyFlags properties,
		//	VkImage& image,
		//	VkDeviceMemory& imageMemory);

		VkCommandPool createCommandPool(VkCommandPoolCreateFlags flags = 0);

		void waitForTimelineSemaphore(const uint64_t value, uint64_t timeout = UINT64_MAX);

		InstanceVersion getInstanceVersion() const;

	private:
		void createInstance(const ApplicationInfo& appInfo);
		void setupDebugMessenger();
		void createSurface(Window* window);
		void pickPhysicalDevice();
		void createLogicalDevice();
		void initializeVMA();
		void createTimelineSemaphore();

		void findInstanceVersion();

		std::vector<VkPhysicalDevice> getPhysicalDevices();
		int rateDeviceSuitability(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();
		bool requiredExtensionsAvailable(const std::vector<const char*>& requiredExtensions);

		bool checkValidationLayerSupport() const;
		std::optional<uint32_t> findQueueFamilies(VkPhysicalDevice device);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	private:
		InstanceVersion m_instanceVersion;

		VkInstance m_instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkCommandPool m_transientCommandPool = VK_NULL_HANDLE;

		VkDevice m_device_ = VK_NULL_HANDLE;
		VkSurfaceKHR m_surface_ = VK_NULL_HANDLE;
		VkQueue m_graphicsQueue_ = VK_NULL_HANDLE;
		uint32_t m_graphicsQueueFamilyIndex = (uint32_t)-1;
		VmaAllocator m_vmaAllocator = nullptr;

		VkSemaphore					m_timelineSemaphore = VK_NULL_HANDLE;

		const std::vector<const char*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
		std::vector<const char*> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};
}
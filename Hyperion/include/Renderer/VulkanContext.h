#pragma once

#include <optional>
#include <Vulkan/vulkan.h>

namespace Hyperion
{
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanContext {
	public:
		VulkanContext();
		~VulkanContext();

		void Init();
		void Cleanup();

	private:
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkDevice m_device = VK_NULL_HANDLE;
		VkSurfaceKHR m_surface = VK_NULL_HANDLE;
		VkQueue m_graphicsQueue = VK_NULL_HANDLE;
		VkQueue m_presentQueue = VK_NULL_HANDLE;
		uint32_t m_graphicsQueueFamilyIndex = (uint32_t)-1;
		VkRenderPass m_renderPass = VK_NULL_HANDLE;
		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;

		VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
		std::vector<VkImage> m_swapChainImages;
		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;
		std::vector<VkImageView> m_swapChainImageViews;

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void DestroySurface();

		void DestroyDebugMessenger();

		std::vector<const char*> GetRequiredExtensions();
		bool RequiredExtensionsAvailable(const std::vector<const char*>& requiredExtensions);

		bool CheckValidationLayerSupport() const;

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		std::vector<VkPhysicalDevice> GetPhysicalDevices();
		int RateDeviceSuitability(VkPhysicalDevice device);

		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		void CreateSwapChain();
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void CreateImageViews();

		void CreateGraphicsPipeline();

		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		void CreateRenderPass();
	};
}
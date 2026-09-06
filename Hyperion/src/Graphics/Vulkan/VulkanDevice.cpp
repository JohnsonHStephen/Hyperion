#include "Graphics/Vulkan/VulkanDevice.h"

#include "Core/Application.h"
#include "Core/EngineInfo.h"

#include <iostream>
#include <map>
#include <set>

#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace Hyperion
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	VkResult createDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance,
			"vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void destroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance,
			"vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	void VulkanDevice::init(const ApplicationInfo& appInfo, Window* window)
	{
		createInstance(appInfo);
		setupDebugMessenger();
		createSurface(window);
		pickPhysicalDevice();
		createLogicalDevice();
		m_transientCommandPool = createCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		initializeVMA();
		createTimelineSemaphore();
	}

	void VulkanDevice::cleanup()
	{
		vmaDestroyAllocator(m_vmaAllocator);

		vkDestroySemaphore(m_device_, m_timelineSemaphore, nullptr);

		vkDestroyDevice(m_device_, nullptr);

		if (enableValidationLayers)
		{
			destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(m_instance, m_surface_, nullptr);
		if (m_instance != VK_NULL_HANDLE)
			vkDestroyInstance(m_instance, nullptr);

		volkFinalize();
	}

	void VulkanDevice::createInstance(const ApplicationInfo& appInfo)
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			throw std::runtime_error("failed to initialize volk!");
		}

		if (enableValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but not available!");
		}

		findInstanceVersion();

		// dynamic rendering is an extension for vulkan 1.2 that needs to be enabled explicitly, but is core in vulkan 1.3+
		if (m_instanceVersion.Major() == 1 && m_instanceVersion.Minor() == 2)
		{
			m_deviceExtensions.emplace_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
		}

		VkApplicationInfo vkAppInfo = {};
		vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		vkAppInfo.pApplicationName = appInfo.appName.c_str();
		vkAppInfo.applicationVersion = VK_MAKE_API_VERSION(
			appInfo.appVersionVariant,
			appInfo.appVersionMajor,
			appInfo.appVersionMinor,
			appInfo.appVersionPatch);
		vkAppInfo.pEngineName = EngineInfo::NAME.c_str();
		vkAppInfo.engineVersion = VK_MAKE_API_VERSION(0,
			EngineInfo::VERSION_MAJOR,
			EngineInfo::VERSION_MINOR,
			EngineInfo::VERSION_PATCH);
		vkAppInfo.apiVersion = m_instanceVersion.Number;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &vkAppInfo;

		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (!requiredExtensionsAvailable(extensions))
			throw std::runtime_error("Missing vulkan extensions");

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}

		volkLoadInstance(m_instance);
	}

	void VulkanDevice::setupDebugMessenger()
	{
		if (!enableValidationLayers) return;
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);
		if (createDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void VulkanDevice::createSurface(Window* window)
	{
		auto adapter = window->getGraphicsAdapter();
		m_surface_ = adapter->createVulkanSurface(m_instance);
	}

	void VulkanDevice::pickPhysicalDevice()
	{
		const auto devices = getPhysicalDevices();
		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			int score = rateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		// Check if the best candidate is suitable at all
		if (candidates.rbegin()->first > 0)
		{
			m_physicalDevice = candidates.rbegin()->second;
		}
		else
		{
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
		std::cout << "physical device: " << properties.deviceName << std::endl;
	}

	void VulkanDevice::createLogicalDevice()
	{
		auto graphicsQueueFamily = findQueueFamilies(m_physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { graphicsQueueFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceVulkan14Features deviceFeatures14 = {};
		deviceFeatures14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
		deviceFeatures14.pNext = nullptr;

		VkPhysicalDeviceVulkan13Features deviceFeatures13 = {};
		deviceFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		deviceFeatures13.pNext = &deviceFeatures14;
		deviceFeatures13.synchronization2 = VK_TRUE;
		deviceFeatures13.dynamicRendering = VK_TRUE;

		VkPhysicalDeviceVulkan12Features deviceFeatures12 = {};
		deviceFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		deviceFeatures12.pNext = &deviceFeatures13;
		deviceFeatures12.timelineSemaphore = VK_TRUE;

		VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
		deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceFeatures2.pNext = &deviceFeatures12;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = &deviceFeatures2;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = nullptr;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		m_graphicsQueueFamilyIndex = graphicsQueueFamily.value();
		vkGetDeviceQueue(m_device_, graphicsQueueFamily.value(), 0, &m_graphicsQueue_);

		volkLoadDevice(m_device_);
	}

	void VulkanDevice::initializeVMA()
	{
		VmaVulkanFunctions vulkanFunctions = {};

		VmaAllocatorCreateInfo vmaAllocatorInfo = {};
		vmaAllocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaAllocatorInfo.physicalDevice = m_physicalDevice;
		vmaAllocatorInfo.device = m_device_;
		vmaAllocatorInfo.pVulkanFunctions = &vulkanFunctions;
		vmaAllocatorInfo.instance = m_instance;
		vmaAllocatorInfo.vulkanApiVersion = m_instanceVersion.Number;

		vmaImportVulkanFunctionsFromVolk(&vmaAllocatorInfo, &vulkanFunctions);

		if (vmaCreateAllocator(&vmaAllocatorInfo, &m_vmaAllocator) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create VMA allocator!");
		}
	}

	VkCommandPool VulkanDevice::createCommandPool(VkCommandPoolCreateFlags flags)
	{
		auto graphicsQueueFamily = findPhysicalQueueFamilies();

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = graphicsQueueFamily.value();
		if (flags != 0)
			poolInfo.flags = flags;

		VkCommandPool commandPool;
		if (vkCreateCommandPool(m_device_, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}

		return commandPool;
	}

	void VulkanDevice::waitForTimelineSemaphore(const uint64_t value, uint64_t timeout)
	{
		VkSemaphoreWaitInfo waitInfo{};
		waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
		waitInfo.semaphoreCount = 1;
		waitInfo.pSemaphores = &m_timelineSemaphore;
		waitInfo.pValues = &value;

		if (vkWaitSemaphores(m_device_, &waitInfo, timeout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to wait for timeline semaphore!");
		}
	}

	InstanceVersion VulkanDevice::getInstanceVersion() const
	{
		if (m_instanceVersion.Number == 0) 
			throw std::runtime_error("Instance version not initialized");
		return m_instanceVersion;
	}

	void VulkanDevice::createTimelineSemaphore()
	{
		VkSemaphoreTypeCreateInfo semaphoreTypeInfo = {};
		semaphoreTypeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
		semaphoreTypeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
		semaphoreTypeInfo.initialValue = VulkanRenderer::MAX_FRAMES_IN_FLIGHT;

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreInfo.pNext = &semaphoreTypeInfo;

		if (vkCreateSemaphore(m_device_, &semaphoreInfo, nullptr, &m_timelineSemaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

	void VulkanDevice::findInstanceVersion()
	{
		VkResult result = vkEnumerateInstanceVersion(&m_instanceVersion.Number);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to get instance version: " + std::to_string(result));
		}

		if (m_instanceVersion.Major() < 1 || (m_instanceVersion.Major() == 1 && m_instanceVersion.Minor() < 2))
		{
			throw std::runtime_error("Vulkan instance version is too low: " +
				std::to_string(m_instanceVersion.Major()) + "." +
				std::to_string(m_instanceVersion.Minor()) + "." +
				std::to_string(m_instanceVersion.Patch()));
		}

		std::cout << "Vulkan instance version: " << m_instanceVersion.Major() << "."
			<< m_instanceVersion.Minor() << "." << m_instanceVersion.Patch() << std::endl;
	}

	std::vector<VkPhysicalDevice> VulkanDevice::getPhysicalDevices()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::cout << "Device count: " << deviceCount << std::endl;
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

		return devices;
	}

	int VulkanDevice::rateDeviceSuitability(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceVulkan14Features supportedFeatures14 = {};
		supportedFeatures14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
		supportedFeatures14.pNext = nullptr;
		VkPhysicalDeviceVulkan13Features supportedFeatures13 = {};
		supportedFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		supportedFeatures13.pNext = &supportedFeatures14;
		VkPhysicalDeviceVulkan12Features supportedFeatures12 = {};
		supportedFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		supportedFeatures12.pNext = &supportedFeatures13;
		VkPhysicalDeviceVulkan11Features supportedFeatures11 = {};
		supportedFeatures11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		supportedFeatures11.pNext = &supportedFeatures12;

		VkPhysicalDeviceFeatures2 deviceFeatures = {};
		deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceFeatures.pNext = &supportedFeatures11;

		vkGetPhysicalDeviceFeatures2(device, &deviceFeatures);

		// Application can't function without geometry shaders
		if (!supportedFeatures12.timelineSemaphore || !supportedFeatures13.dynamicRendering || !supportedFeatures13.synchronization2)
			return 0;

		// Application requires graphics and present queue support for the window surface
		if (!findQueueFamilies(device))
			return 0;

		if (!checkDeviceExtensionSupport(device))
			return 0;

		// Check for swapchain support
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
			return 0;

		int score = 0;

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 1000;

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		std::cout << deviceProperties.deviceName << " has a rating of " << score << '\n';

		return score;
	}

	bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(
			device,
			nullptr,
			&extensionCount,
			availableExtensions.data());

		std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	std::vector<const char*> VulkanDevice::getRequiredExtensions()
	{
		uint32_t requiredExtensionCount = 0;
		const char** requiredInstanceExtensions = Window::requiredInstanceExtensions(requiredExtensionCount);

		std::vector<const char*> requiredExtensions;

		for (uint32_t i = 0; i < requiredExtensionCount; i++)
		{
			requiredExtensions.emplace_back(requiredInstanceExtensions[i]);
		}

		requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

		if (enableValidationLayers)
		{
			requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return requiredExtensions;
	}

	bool VulkanDevice::requiredExtensionsAvailable(const std::vector<const char*>& requiredExtensions)
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::vector<const char*> missingExtensions;
		for (const auto& requiredExtension : requiredExtensions)
		{
			for (const auto& extension : extensions)
			{
				if (strcmp(extension.extensionName, requiredExtension) == 0)
					break;
				if (&extension == &extensions.back())
					missingExtensions.push_back(requiredExtension);
			}
		}

		if (!missingExtensions.empty())
		{
			std::cout << "missing vulkan extensions:\n";

			for (const auto& missingExtension : missingExtensions)
				std::cout << '\t' << missingExtension << '\n';

			return false;
		}

		return true;
	}

	bool VulkanDevice::checkValidationLayerSupport() const
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : m_validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	std::optional<uint32_t> VulkanDevice::findQueueFamilies(VkPhysicalDevice device)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties2(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties2> queueFamilies(queueFamilyCount, {.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
		vkGetPhysicalDeviceQueueFamilyProperties2(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface_, &presentSupport);
			if (queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupport)
			{
				return i;
			}

			i++;
		}

		return std::nullopt;
	}

	void VulkanDevice::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;  // Optional
	}

	SwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface_, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface_, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface_, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface_, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device,
				m_surface_,
				&presentModeCount,
				details.presentModes.data());
		}
		return details;
	}

	VkFormat VulkanDevice::findSupportedFormat(
		const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (
				tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format!");
	}

	uint32_t VulkanDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void VulkanDevice::createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_device_, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(m_device_, buffer, bufferMemory, 0);
	}

	VkCommandBuffer VulkanDevice::beginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_transientCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_device_, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	void VulkanDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_graphicsQueue_);

		vkFreeCommandBuffers(m_device_, m_transientCommandPool, 1, &commandBuffer);
	}

	void VulkanDevice::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = srcOffset;
		copyRegion.dstOffset = 0;  // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}

	//void VulkanDevice::copyBufferToImage(
	//	VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount)
	//{
	//	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	//	VkBufferImageCopy region{};
	//	region.bufferOffset = 0;
	//	region.bufferRowLength = 0;
	//	region.bufferImageHeight = 0;

	//	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//	region.imageSubresource.mipLevel = 0;
	//	region.imageSubresource.baseArrayLayer = 0;
	//	region.imageSubresource.layerCount = layerCount;

	//	region.imageOffset = { 0, 0, 0 };
	//	region.imageExtent = { width, height, 1 };

	//	vkCmdCopyBufferToImage(
	//		commandBuffer,
	//		buffer,
	//		image,
	//		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	//		1,
	//		&region);
	//	endSingleTimeCommands(commandBuffer);
	//}

	//void VulkanDevice::createImageWithInfo(
	//	const VkImageCreateInfo& imageInfo,
	//	VkMemoryPropertyFlags properties,
	//	VkImage& image,
	//	VkDeviceMemory& imageMemory)
	//{
	//	if (vkCreateImage(m_device_, &imageInfo, nullptr, &image) != VK_SUCCESS)
	//	{
	//		throw std::runtime_error("failed to create image!");
	//	}

	//	VkMemoryRequirements memRequirements;
	//	vkGetImageMemoryRequirements(m_device_, image, &memRequirements);

	//	VkMemoryAllocateInfo allocInfo{};
	//	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	//	allocInfo.allocationSize = memRequirements.size;
	//	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	//	if (vkAllocateMemory(m_device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	//	{
	//		throw std::runtime_error("failed to allocate image memory!");
	//	}

	//	if (vkBindImageMemory(m_device_, image, imageMemory, 0) != VK_SUCCESS)
	//	{
	//		throw std::runtime_error("failed to bind image memory!");
	//	}
	//}
}
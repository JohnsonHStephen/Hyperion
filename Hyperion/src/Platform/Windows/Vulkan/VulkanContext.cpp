#include "VulkanContext.h"

#include <iostream>

#include "Core/Application.h"
#include "Core/EngineInfo.h"
#include "Gui/Window.h"
#include <map>


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

namespace Hyperion
{
	VulkanContext::VulkanContext() {
		Init();
	}

	VulkanContext::~VulkanContext() {
		vkDestroyDevice(m_device, nullptr);
		DestroyDebugMessenger();
		vkDestroyInstance(m_instance, nullptr);
	}

	void VulkanContext::Init() {
		CreateInstance();
		SetupDebugMessenger();
		PickPhysicalDevice();
	}

	void VulkanContext::CreateInstance() {
		if (enableValidationLayers && !CheckValidationLayerSupport()) {
			throw std::runtime_error("Validation layers requested, but not available!");
		}

		const Application& app = Application::Get();
		VkApplicationInfo vkAppInfo{};
		vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		const ApplicationInfo& appInfo = app.GetAppInfo();
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
		vkAppInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo vkCreateInfo{};
		vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		vkCreateInfo.pApplicationInfo = &vkAppInfo;

		vkCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

		std::vector requiredExtensions = GetRequiredExtensions();

		vkCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		vkCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

		if (!RequiredExtensionsAvailable(requiredExtensions))
			throw std::runtime_error("Missing vulkan extensions");

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers) {
			vkCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			vkCreateInfo.ppEnabledLayerNames = validationLayers.data();

			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			vkCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		} else {
			vkCreateInfo.enabledLayerCount = 0;
		}

		VkResult result = vkCreateInstance(&vkCreateInfo, nullptr, &m_instance);

		if (result != VK_SUCCESS)
			throw std::runtime_error("failed to create instance! " + result);
	}

	void VulkanContext::SetupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void VulkanContext::PickPhysicalDevice() {
		const auto devices = GetPhysicalDevices();
		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices) {
			int score = RateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		// Check if the best candidate is suitable at all
		if (candidates.rbegin()->first > 0) {
			m_physicalDevice = candidates.rbegin()->second;
		} else {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	void VulkanContext::CreateLogicalDevice() {
		QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;

		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;

		createInfo.pEnabledFeatures = &deviceFeatures;

		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}
	}

	void VulkanContext::DestroyDebugMessenger() {
		if (m_debugMessenger == VK_NULL_HANDLE)
			return;

		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}

	std::vector<const char*> VulkanContext::GetRequiredExtensions() {
		uint32_t requiredExtensionCount = 0;
		const char** requiredInstanceExtensions = Window::RequiredInstanceExtensions(requiredExtensionCount);

		std::vector<const char*> requiredExtensions;

		for (uint32_t i = 0; i < requiredExtensionCount; i++) {
			requiredExtensions.emplace_back(requiredInstanceExtensions[i]);
		}

		requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

		if (enableValidationLayers) {
			requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return requiredExtensions;
	}

	bool VulkanContext::RequiredExtensionsAvailable(const std::vector<const char*>& requiredExtensions) {

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::vector<const char*> missingExtensions;
		for (const auto& requiredExtension : requiredExtensions) {
			for (const auto& extension : extensions) {
				if (strcmp(extension.extensionName, requiredExtension) == 0)
					break;
				if (&extension == &extensions.back())
					missingExtensions.push_back(requiredExtension);
			}
		}

		if (!missingExtensions.empty()) {
			std::cout << "missing vulkan extensions:\n";

			for (const auto& missingExtension : missingExtensions)
				std::cout << '\t' << missingExtension << '\n';

			return false;
		}

		return true;
	}

	bool VulkanContext::CheckValidationLayerSupport() const {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
				return false;
		}

		return true;
	}

	void VulkanContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
	}

	VkResult VulkanContext::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		} else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanContext::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			std::cerr << "[Vulkan Validation]: " << pCallbackData->pMessage << std::endl;
		}
		return VK_FALSE;
	}

	std::vector<VkPhysicalDevice> VulkanContext::GetPhysicalDevices() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

		return devices;
	}

	int VulkanContext::RateDeviceSuitability(VkPhysicalDevice device) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		int score = 0;

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 1000;

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		// Application can't function without geometry shaders
		if (!deviceFeatures.geometryShader)
			return 0;

		// Application requires at least on queue family that supports VK_QUEUE_GRAPHICS_BIT
		if (!FindQueueFamilies(device).isComplete())
			return 0;

		std::cout << deviceProperties.deviceName << " has a rating of " << score << '\n';

		return score;
	}

	QueueFamilyIndices VulkanContext::FindQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;
		
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			i++;
		}

		return indices;
	}
}
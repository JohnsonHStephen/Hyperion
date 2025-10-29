#include "VulkanContext.h"

#include "Core/Application.h"
#include "Core/EngineInfo.h"

namespace Hyperion
{
	VulkanContext::VulkanContext() {
		Init();
	}

	VulkanContext::~VulkanContext() {
		vkDestroyInstance(m_instance, nullptr);
	}

	void VulkanContext::Init() {
		const Application& app = Application::Get();
		VkApplicationInfo vkAppInfo{};
		vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		const ApplicationInfo& appInfo = app.GetAppInfo();
		vkAppInfo.pApplicationName = appInfo.appName.c_str();
		vkAppInfo.applicationVersion = VK_MAKE_API_VERSION(appInfo.appVersionVariant, appInfo.appVersionMajor, appInfo.appVersionMinor, appInfo.appVersionPatch);
		vkAppInfo.pEngineName = EngineInfo::NAME.c_str();
		vkAppInfo.engineVersion = VK_MAKE_API_VERSION(0, EngineInfo::VERSION_MAJOR, EngineInfo::VERSION_MINOR, EngineInfo::VERSION_PATCH);
		vkAppInfo.apiVersion = VK_API_VERSION_1_3;

		uint32_t extensionCount = 0;
		VkInstanceCreateInfo vkCreateInfo{};
		vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		vkCreateInfo.pApplicationInfo = &vkAppInfo;
		vkCreateInfo.ppEnabledExtensionNames = app.GetWindow().RequiredInstanceExtensions(extensionCount);
		vkCreateInfo.enabledExtensionCount = extensionCount;
		vkCreateInfo.enabledLayerCount = 0;

		VkResult result = vkCreateInstance(&vkCreateInfo, nullptr, &m_instance);

		if (result != VK_SUCCESS)
			throw std::runtime_error("failed to create instance! " + result);
	}

}
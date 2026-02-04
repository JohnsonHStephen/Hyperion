#pragma once

#include "Core/HyperionPCH.h"

#include "Events/WindowEvent.h"

#include "Graphics/Gui/ImGuiLayer.h"
#include "Graphics/Gui/Window.h"
#include "Graphics/Pipelines/MeshPipeline.h"
#include "Graphics/Vulkan/VulkanDevice.h"
#include "Graphics/Vulkan/VulkanRenderer.h"

struct ApplicationInfo
{
	unsigned short appVersionVariant;
	unsigned short appVersionMajor;
	unsigned short appVersionMinor;
	unsigned short appVersionPatch;

	std::string appName;
};

namespace Hyperion
{
	class Application
	{
	public:
		Application(ApplicationInfo appInfo);
		virtual ~Application();

		inline static Application* get() { return s_instance; }

		void run();
		void close();

		void pushLayer(Layer* layer);

		void onEvent(Event& e);

		inline const Window* getWindow() const { return m_window.get(); }
		inline Window* getWindow() { return m_window.get(); }

		inline const ApplicationInfo& getAppInfo() const { return m_appInfo; }
		inline ApplicationInfo& getAppInfo() { return m_appInfo; }

		inline const VulkanDevice* getVulkanDevice() const { return &m_device; }
		inline VulkanDevice* getVulkanDevice() { return &m_device; }

		inline bool isMinimized() { return m_window->isMinimized(); }
	private:
		bool onWindowClose(WindowCloseEvent& e);

	private:
		static Application* s_instance;
		std::unique_ptr<Window> m_window = nullptr;
		VulkanDevice m_device{};
		VulkanRenderer m_renderer{};
		std::vector<Layer*> m_layers{};
		MeshPipeline m_meshPipeline{};

		ApplicationInfo m_appInfo;

		bool m_running = false;
	};

	Application* createApplication();
} // namespace Hyperion
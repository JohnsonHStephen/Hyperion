#pragma once

#include "Core/HyperionPCH.h"
#include "Events/WindowEvent.h"
#include "GUI/ImGuiLayer.h"
#include "GUI/Window.h"
#include "Renderer/VulkanContext.h"

struct ApplicationInfo {
	unsigned short appVersionVariant;
	unsigned short appVersionMajor;
	unsigned short appVersionMinor;
	unsigned short appVersionPatch;

	std::string appName;
};

namespace Hyperion
{
	class Application {
	public:
		Application(ApplicationInfo appInfo);
		virtual ~Application();

		inline static Application* Get() { return s_Instance; }

		void Run();
		void Close();

		void PushLayer(Layer* layer);

		void OnEvent(Event& e);

		inline const Window* GetWindow() const { return m_Window.get(); }
		inline Window* GetWindow() { return m_Window.get(); }

		inline const ApplicationInfo& GetAppInfo() const { return m_AppInfo; }
		inline ApplicationInfo& GetAppInfo() { return m_AppInfo; }

		inline const VulkanContext* GetVulkanContext() const { return m_VulkanContext.get(); }
		inline VulkanContext* GetVulkanContext() { return m_VulkanContext.get(); }

		inline bool IsMinimized() { return m_Minimized; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		static Application* s_Instance;
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<VulkanContext> m_VulkanContext;
		//ImGuiLayer* m_OverlayLayer;
		std::vector<Layer*> m_Layers;

		ApplicationInfo m_AppInfo;

		bool m_Running = false;
		bool m_Minimized = false;
	};

	Application* CreateApplication();
} // namespace Hyperion
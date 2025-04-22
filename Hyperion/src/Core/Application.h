#pragma once

#include "GUI/ImGuiLayer.h"
#include "GUI/WindowsWindow.h"

namespace Hyperion
{
	class Application {
	public:
		static Application* Create();
		inline static Application& Get() { return *s_Instance; }

		void Run();
		void Close();

		void PushLayer(Layer* layer);

		inline WindowsWindow& GetWindow() { return *m_Window; }
	private:
		Application();
		virtual ~Application();

		void Event_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	private:
		static Application* s_Instance;
		std::unique_ptr<WindowsWindow> m_Window;
		ImGuiLayer* m_OverlayLayer;
		std::vector<Layer*> m_Layers;

		bool m_Running = false;
		bool m_Minimized = false;
	};
} // namespace Hyperion
#pragma once

#include "GUI/ImGuiLayer.h"
#include "Window.h"
#include <WindowEvent.h>

namespace Hyperion
{
	class Application {
	public:
		static Application* Create();
		inline static Application& Get() { return *s_Instance; }

		void Run();
		void Close();

		void PushLayer(Layer* layer);

		void OnEvent(Event& e);

		inline Window& GetWindow() { return *m_Window; }
	private:
		Application();
		virtual ~Application();

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		static Application* s_Instance;
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_OverlayLayer;
		std::vector<Layer*> m_Layers;

		bool m_Running = false;
		bool m_Minimized = false;
	};
} // namespace Hyperion
#pragma once

#include "Core/HyperionPCH.h"
#include "Events/WindowEvent.h"
#include "GUI/ImGuiLayer.h"
#include "GUI/Window.h"

namespace Hyperion
{
	class Application {
	public:
		Application();
		virtual ~Application();

		inline static Application& Get() { return *s_Instance; }

		void Run();
		void Close();

		void PushLayer(Layer* layer);

		void OnEvent(Event& e);

		inline Window& GetWindow() { return *m_Window; }
	private:
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

	Application* CreateApplication();
} // namespace Hyperion
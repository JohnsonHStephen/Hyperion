#include "Application.h"

#define BIND_EVENT_FN(fn)  [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Hyperion
{
	Application* Application::s_Instance = nullptr;

	void Application::Event_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			m_Running = false;
	}

	Application* Application::Create() {
		if (s_Instance)
			return s_Instance;

		return new Application();
	}

	void Application::Run() {

		while (m_Running) {
			for (auto layer : m_Layers) {
				layer->OnFrameStart(0);
			}

			m_OverlayLayer->Begin();
			for (auto layer : m_Layers) {
				layer->OnOverlay();
			}
			m_OverlayLayer->End();

			m_Window->ResetWindowContext();

			for (auto layer : m_Layers) {
				layer->OnFrameEnd(0);
			}

			m_Window->OnUpdate();
		}

	}

	void Application::Close() {
		for (Layer* layer : m_Layers) {
			layer->OnDetach();
			delete layer;
		}
	}

	void Application::PushLayer(Layer* layer) {
		m_Layers.emplace_back(layer);
		layer->OnAttach();
	}

	Application::Application() {
		s_Instance = this;

		m_Window = std::make_unique<WindowsWindow>(WindowsWindow::WindowProps());

		m_Window->SetEventCallback(BIND_EVENT_FN(Event_Callback));

		m_OverlayLayer = new ImGuiLayer();
		PushLayer((Layer*)m_OverlayLayer);

		m_Running = true;
	}
	Application::~Application() {}
}
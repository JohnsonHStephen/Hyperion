#include "Core/Application.h"

#define BIND_EVENT_FN(fn)  [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Hyperion
{
	Application* Application::s_Instance = nullptr;

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

			for (auto layer : m_Layers) {
				layer->OnFrameEnd(0);
			}

			m_Window->Render();
		}

	}

	void Application::Close() {
		for (Layer* layer : m_Layers) {
			layer->OnDetach();
			delete layer;
		}

		m_Running = false;
	}

	void Application::PushLayer(Layer* layer) {
		m_Layers.emplace_back(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e) {
		if (e.GetEventType() == EventType::WindowClose)
			e.SetHandled(OnWindowClose(*(WindowCloseEvent*)&e));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		Close();
		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		return false;
	}

	Application::Application(ApplicationInfo appInfo)
							: m_AppInfo(appInfo) {
		if (s_Instance != nullptr)
			throw std::runtime_error("Application already created!");

		s_Instance = this;

		m_Window = Window::Create();

		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_OverlayLayer = new ImGuiLayer();
		PushLayer((Layer*)m_OverlayLayer);

		m_Running = true;
	}

	Application::~Application() {}
}
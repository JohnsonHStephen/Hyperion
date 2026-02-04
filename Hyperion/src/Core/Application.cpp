#include "Core/Application.h"

#include "Graphics/Vulkan/VulkanBuffer.h"
#include "Graphics/Vulkan/VulkanPipeline.h"

#define BIND_EVENT_FN(fn)  [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Hyperion
{
	Application* Application::s_instance = nullptr;

	void Application::run()
	{
		std::vector<Mesh::Vertex> vertices{
			{{0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
			{{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		std::vector<Mesh> meshs;
		meshs.emplace_back("Triangle", &m_device).loadCustomData(vertices, {0, 1, 2, 0, 2, 3});

		while (m_running)
		{
			if (auto commandBuffer = m_renderer.beginFrame())
			{
				m_renderer.beginSwapChainRenderPass(commandBuffer);
				for (auto layer : m_layers)
				{
					layer->OnFrameStart(0);
				}

				//m_OverlayLayer->Begin();
				for (auto layer : m_layers)
				{
					layer->OnOverlay();
				}
				//m_OverlayLayer->End();

				for (auto layer : m_layers)
				{
					layer->OnFrameEnd(0);
				}

				m_meshPipeline.draw(commandBuffer, meshs);

				m_renderer.endSwapChainRenderPass(commandBuffer);
				m_renderer.endFrame();
			}

			m_window->pollEvents();
		}
	}

	void Application::close()
	{
		for (Layer* layer : m_layers)
		{
			layer->OnDetach();
			delete layer;
		}

		m_running = false;
	}

	void Application::pushLayer(Layer* layer)
	{
		m_layers.emplace_back(layer);
		layer->OnAttach();
	}

	void Application::onEvent(Event& e)
	{
		if (e.GetEventType() == EventType::WindowClose)
			e.SetHandled(onWindowClose(*(WindowCloseEvent*)&e));
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		close();
		return false;
	}

	Application::Application(ApplicationInfo appInfo)
		: m_appInfo(appInfo), m_device(), m_renderer()
	{
		if (s_instance != nullptr)
			throw std::runtime_error("Application already created!");

		s_instance = this;

		m_window = Window::create();

		m_device.init();
		m_renderer.init(&m_device);

		m_window->setEventCallback(BIND_EVENT_FN(onEvent));

		m_meshPipeline.init(&m_device, m_renderer.getSwapChainRenderPass());

		//m_OverlayLayer = new ImGuiLayer();
		//PushLayer((Layer*)m_OverlayLayer);

		m_running = true;
	}

	Application::~Application()
	{
		m_meshPipeline.cleanup();
		m_renderer.cleanup();
		m_device.cleanup();
	}
}
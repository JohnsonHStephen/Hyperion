#include "Core/Application.h"

#include "Resources/MeshCache.h"
#include "Resources/MeshLoader.h"

#include "World/Camera.h"
#include "World/Components.h"
#include "World/Entity.h"
#include "World/World.h"

#define BIND_EVENT_FN(fn)  [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Hyperion
{
	Application::Application(const ApplicationInfo& appInfo)
		: m_device(), m_renderer()
	{
		m_window = Window::create();

		m_device.init(appInfo, m_window.get());
		m_renderer.init(&m_device, m_window->getFramebufferPixelSize());

		m_window->setEventCallback(BIND_EVENT_FN(onEvent));

		m_meshCache.init(m_device);

		ImGuiOverlay::InitInfo initInfo{};
		initInfo.ApiVersion = m_device.getInstanceVersion().Number;
		initInfo.Window = m_window.get();
		initInfo.Instance = m_device.getInstance();
		initInfo.PhysicalDevice = m_device.physicalDevice();
		initInfo.Device = m_device.device();
		initInfo.QueueFamily = m_device.graphicsQueueFamily();
		initInfo.Queue = m_device.graphicsQueue();
		initInfo.PipelineCache = VK_NULL_HANDLE;
		initInfo.ImageCount = m_renderer.getImageCount();
		initInfo.Allocator = nullptr;
		initInfo.PipelineRenderingCreateInfo = m_renderer.getSwapChainPipelineRenderingCreateInfo();

		m_overlay.init(initInfo);

		m_running = true;
	}

	Application::~Application()
	{
		m_overlay.cleanup();
		m_meshCache.cleanup(m_device);
		m_renderer.cleanup();
		m_device.cleanup();
	}

	void Application::run(Game* game)
	{
		m_game = game;
		m_game->init(GameContext{ m_device, m_meshCache });

		while (m_running)
		{
			m_game->onUpdate(1);

			if (auto commandBuffer = m_renderer.beginFrame(m_window.get()))
			{
				m_renderer.beginRendering(commandBuffer);
				m_game->onRender();

				auto primaryCameraOpt = m_game->getPrimaryCameraEntity();

				if (primaryCameraOpt)
				{
					auto primaryCamera = primaryCameraOpt.value();
					primaryCamera.getComponent<CameraComponent>().camera.setViewportSize(m_window->getWidth(), m_window->getHeight());
					m_renderer.draw(commandBuffer, primaryCamera, m_game->getWorld(), m_meshCache);
				}

				m_overlay.begin();
				m_game->onOverlay();
				m_overlay.end(commandBuffer);

				m_renderer.endRendering(commandBuffer);
				m_renderer.endFrame(m_window.get());
			}

			m_window->pollEvents();
		}

		m_game->cleanup();
	}

	void Application::close()
	{
		m_running = false;
	}

	void Application::onEvent(Event& e)
	{
		if (e.GetEventType() == EventType::WindowClose)
			e.SetHandled(onWindowClose(*(WindowCloseEvent*)&e));

		m_game->onEvent(e);
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		close();
		return false;
	}
}
#include "Core/Application.h"

#include "Scene/Camera.h"
#include "Scene/GameObject.h"

#define BIND_EVENT_FN(fn)  [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Hyperion
{
	Application* Application::s_instance = nullptr;

	void Application::run()
	{
		std::vector<GameObject> objects;
		objects.emplace_back("Square", &m_device, glm::vec3{0.0f, 0.0f, 2.5f});
		glm::vec3 rotation(0.0f, 0.0f, 0.0f);

		std::vector<Mesh::Vertex> vertices{
			{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
			{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
			{{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},
			{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 1.0f}},
			{{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
			{{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 0.0f}}
		};

		objects[0].mesh.loadCustomData(vertices, {
			0, 1, 2,	// front face
			2, 1, 3,

			1, 5, 3,	// right face
			3, 5, 7,

			5, 4, 7,	// back face
			7, 4, 6,
			
			4, 0, 6,	// left face
			6, 0, 2,
			
			4, 5, 0,	// top face
			0, 5, 1,
			
			2, 3, 6,	// bottom face
			6, 3, 7});

		Camera camera{};
		camera.setProjectionType(Camera::ProjectionType::Perspective);
		camera.setPerspective(glm::radians(45.0f), 0.01f, 1000.0f);

		while (m_running)
		{
			if (auto commandBuffer = m_renderer.beginFrame())
			{
				m_renderer.beginSwapChainRenderPass(commandBuffer);
				for (auto frameListener : m_frameListeners)
				{
					frameListener->onFrameStart(0);
				}

				rotation.z += 0.001f;
				rotation.y += 0.002f;
				rotation.x += 0.0005f;
				objects[0].transform.rotation = rotation;
				camera.setViewportSize(m_window->getWidth(), m_window->getHeight());
				m_meshPipeline.draw(commandBuffer, camera, objects);

				m_overlay.begin();
				for (auto frameListener : m_frameListeners)
				{
					frameListener->onOverlay();
				}
				m_overlay.end(commandBuffer);

				for (auto frameListener : m_frameListeners)
				{
					frameListener->onFrameEnd(0);
				}

				m_renderer.endSwapChainRenderPass(commandBuffer);
				m_renderer.endFrame();
			}

			m_window->pollEvents();
		}
	}

	void Application::close()
	{
		for (auto frameListener : m_frameListeners)
		{
			frameListener->onDetach();
			delete frameListener;
		}

		m_running = false;
	}

	void Application::pushListener(FrameListener* frameListener)
	{
		m_frameListeners.emplace_back(frameListener);
		frameListener->onAttach();
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

		ImGuiOverlay::InitInfo initInfo{};
		initInfo.Window = m_window.get();
		initInfo.Instance = m_device.getInstance();
		initInfo.PhysicalDevice = m_device.physicalDevice();
		initInfo.Device = m_device.device();
		initInfo.QueueFamily = m_device.graphicsQueueFamily();
		initInfo.Queue = m_device.graphicsQueue();
		initInfo.PipelineCache = VK_NULL_HANDLE;
		initInfo.DescriptorPool = m_device.descriptorPool();
		initInfo.ImageCount = m_renderer.getImageCount();
		initInfo.Allocator = m_device.allocator();
		initInfo.RenderPass = m_renderer.getSwapChainRenderPass();

		m_overlay.init(initInfo);

		m_running = true;
	}

	Application::~Application()
	{
		m_meshPipeline.cleanup();
		m_renderer.cleanup();
		m_device.cleanup();
	}
}
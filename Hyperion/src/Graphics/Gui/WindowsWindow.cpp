#include "WindowsWindow.h"

#include "Core/Application.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

#include "Graphics/Gui/Adapters/GLFWWindowAdapter.h"

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

namespace Hyperion
{
	static uint8_t s_GLFWWindowCount = 0;

	WindowsWindow::WindowsWindow(const WindowProps& props):
		m_Window(NULL)
	{
		init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		destroy();
	}

	int WindowsWindow::init(const WindowProps& props)
	{
		m_Data.Title = props.Name;
		m_Data.Width = props.WindowSize.width;
		m_Data.Height = props.WindowSize.height;
		m_Data.VSync = props.VSync;

		if (s_GLFWWindowCount == 0)
		{
			if (glfwInit() != GLFW_TRUE)
				return -1;

			glfwSetErrorCallback(error_callback);
		}

		// Tell GLFW not to create an OpenGL context (we're using Vulkan)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_Window = glfwCreateWindow(props.WindowSize.width, props.WindowSize.height, m_Data.Title.c_str(), NULL, NULL);
		if (!m_Window)
		{
			glfwTerminate();
			return -1;
		}
		s_GLFWWindowCount++;

		glfwSetWindowUserPointer(m_Window, &m_Data);
		setVSync(props.VSync);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;
				data.Resized = true;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(static_cast<KeyCode>(key), 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(static_cast<KeyCode>(key));
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(static_cast<KeyCode>(key), true);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int key)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				CharTypedEvent event(static_cast<KeyCode>(key));
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(static_cast<MouseCode>(button));
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
		return 0;
	}

	void WindowsWindow::destroy()
	{

		glfwDestroyWindow(m_Window);

		s_GLFWWindowCount--;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowsWindow::pollEvents()
	{
		glfwPollEvents();
	}

	void WindowsWindow::setVSync(bool enabled)
	{
		// TODO: Set vulkan vsync (present mode setting)
		m_Data.VSync = enabled;
	}

	WindowSize WindowsWindow::getFramebufferPixelSize() const
	{
		WindowSize size{};
		glfwGetFramebufferSize(m_Window, &size.width, &size.height);
		return size;
	}

	const char** WindowsWindow::requiredInstanceExtensions(uint32_t& extensionCount)
	{
		return glfwGetRequiredInstanceExtensions(&extensionCount);
	}

	WindowGraphicsAdapterPtr WindowsWindow::getGraphicsAdapter() const
	{
		return std::make_unique<GLFWWindowAdapter>(m_Window);
	}
}

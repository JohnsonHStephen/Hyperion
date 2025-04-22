#include "WindowsWindow.h"

#include <glad/glad.h>

static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

namespace Hyperion
{
	static uint8_t s_GLFWWindowCount = 0;

	WindowsWindow::WindowsWindow(const WindowProps& props):
		m_Window(NULL) {
		Init(props);
	}

	WindowsWindow::~WindowsWindow() {
		Destroy();
	}

	int WindowsWindow::Init(const WindowProps& props) {
		m_Data.Title = props.Name;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.VSync = props.VSync;

		if (s_GLFWWindowCount == 0) {
			if (glfwInit() != GLFW_TRUE)
				return -1;

			glfwSetErrorCallback(error_callback);
		}

		m_Window = glfwCreateWindow(props.Width, props.Height, m_Data.Title.c_str(), NULL, NULL);
		if (!m_Window) {
			glfwTerminate();
			return -1;
		}
		s_GLFWWindowCount++;

		glfwMakeContextCurrent(m_Window);
		gladLoadGL();
		glfwSwapInterval(1);

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(props.VSync);

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.EventCallback(window, key, scancode, action, mods);
			});

		return 0;
	}

	void WindowsWindow::Destroy() {

		glfwDestroyWindow(m_Window);

		s_GLFWWindowCount--;

		if (s_GLFWWindowCount == 0) {
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled) {
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}
	void WindowsWindow::ResetWindowContext() {
		if (m_Window)
			glfwMakeContextCurrent(m_Window);
	}
}

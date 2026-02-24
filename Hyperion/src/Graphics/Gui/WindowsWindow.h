#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Graphics/Gui/Window.h"

namespace Hyperion
{
	class WindowsWindow: public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		~WindowsWindow();

		void pollEvents();

		inline unsigned int getWidth() const override { return m_Data.Width; }
		inline unsigned int getHeight() const override { return m_Data.Height; }
		inline bool isMinimized() const override { return (m_Data.Width == 0 || m_Data.Height == 0); }

		inline void setEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void setVSync(bool enabled) override;
		inline bool isVSync() const override { return m_Data.VSync; }
		inline bool isResized() const override { return m_Data.Resized; }
		inline void resetResized() override { m_Data.Resized = false; }

		inline void* getNativeWindow() const override { return m_Window; }
		inline NativeWindowInfo getNativeWindowInfo() const override
		{
			NativeWindowInfo info{};
			info.handle = (void*)glfwGetWin32Window(m_Window);
			info.instance = (void*)GetModuleHandle(NULL);
			return info;
		}

		WindowSize getFramebufferPixelSize() const override;

		static const char** requiredInstanceExtensions(uint32_t& extensionCount);

	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			bool Resized;

			EventCallbackFn EventCallback;
		};
		WindowData m_Data;

		int init(const WindowProps& props);
		void destroy();
	};
}
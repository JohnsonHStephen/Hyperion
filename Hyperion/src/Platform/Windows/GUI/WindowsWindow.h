#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Gui/Window.h"

namespace Hyperion
{
	class WindowsWindow: public Window {
	public:
		WindowsWindow(const WindowProps& props);
		~WindowsWindow();

		void Render();

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		inline bool IsVSync() const override { return m_Data.VSync; }

		inline void* GetNativeWindow() const override { return m_Window; }
		inline NativeWindowInfo GetNativeWindowInfo() const override {
			NativeWindowInfo info{};
			info.handle = (void*)glfwGetWin32Window(m_Window);
			info.instance = (void*)GetModuleHandle(NULL);
			return info;
		}

		static const char** RequiredInstanceExtensions(uint32_t& extensionCount);

	private:
		GLFWwindow* m_Window;

		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};
		WindowData m_Data;

		int Init(const WindowProps& props);
		void Destroy();
	};
}
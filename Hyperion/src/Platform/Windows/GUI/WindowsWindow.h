#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Hyperion
{
	class WindowsWindow {
	public:
		using EventCallbackFn = std::function<void(GLFWwindow*, int, int, int, int)>;
		struct WindowProps {
			std::string Name;
			unsigned int Width, Height;
			bool VSync;

			WindowProps(const std::string& name = "Hyperion",
				int width = 640, int height = 480,
				bool vsync = false)
				: Name(name), Width(width), Height(height), VSync(vsync) {}
		};

		WindowsWindow(const WindowProps& props);
		~WindowsWindow();

		void OnUpdate();

		inline unsigned int GetWidth() const { return m_Data.Width; }
		inline unsigned int GetHeight() const { return m_Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled);
		inline bool IsVSync() const { return m_Data.VSync; }

		inline void* GetNativeWindow() const { return m_Window; }

		void ResetWindowContext();
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
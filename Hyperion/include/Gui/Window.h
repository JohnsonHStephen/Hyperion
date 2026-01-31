#pragma once

#include "Core/HyperionPCH.h"
#include "Events/Event.h"

namespace Hyperion
{
	struct WindowProps
	{
		std::string Name;
		unsigned int Width, Height;
		bool VSync;

		WindowProps(const std::string& name = "Hyperion",
			int width = 640, int height = 480,
			bool vsync = false)
			: Name(name), Width(width), Height(height), VSync(vsync)
		{}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		static std::unique_ptr<Window> create(const WindowProps& props = WindowProps());
		static const char** requiredInstanceExtensions(uint32_t& extensionCount);

		virtual void pollEvents() = 0;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;
		virtual bool isMinimized() const = 0;

		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;
		virtual bool isResized() const = 0;
		virtual void resetResized() = 0;

		virtual void* getNativeWindow() const = 0;

		struct NativeWindowInfo
		{
			void* handle = nullptr;		// e.g. HWND on Windows
			void* instance = nullptr;	// e.g. HINSTANCE on Windows
		};
		// Fill out platform-native window info without introducing platform headers here
		virtual NativeWindowInfo getNativeWindowInfo() const = 0;

		virtual void getFramebufferPixelSize(int& width, int& height) const = 0;
	};
}
#pragma once

#include "Core/HyperionPCH.h"
#include "Events/Event.h"

namespace Hyperion
{
	struct WindowProps {
		std::string Name;
		unsigned int Width, Height;
		bool VSync;

		WindowProps(const std::string& name = "Hyperion",
			int width = 640, int height = 480,
			bool vsync = false)
			: Name(name), Width(width), Height(height), VSync(vsync) {}
	};

	class Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
		static const char** RequiredInstanceExtensions(uint32_t& extensionCount);

		virtual void Render() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

        struct NativeWindowInfo {
            void* handle = nullptr;     // e.g. HWND on Windows
            void* instance = nullptr;   // e.g. HINSTANCE on Windows
        };
        // Fill out platform-native window info without introducing platform headers here
        virtual NativeWindowInfo GetNativeWindowInfo() const = 0;

	};
}
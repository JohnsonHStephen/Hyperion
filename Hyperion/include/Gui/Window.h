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

		virtual void Render() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual const char** RequiredInstanceExtensions(uint32_t& extensionCount) const = 0;
	};
}
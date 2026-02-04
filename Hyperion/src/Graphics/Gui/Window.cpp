#include "Graphics/Gui/Window.h"

#ifdef PLATFORM_WINDOWS
#include "Graphics/Gui/WindowsWindow.h"
#endif

namespace Hyperion
{
	std::unique_ptr<Window> Window::create(const WindowProps& props)
	{
#ifdef PLATFORM_WINDOWS
		return std::make_unique<WindowsWindow>(props);
#else
		return nullptr;
#endif
	}
	const char** Window::requiredInstanceExtensions(uint32_t& extensionCount)
	{
#ifdef PLATFORM_WINDOWS
		return WindowsWindow::requiredInstanceExtensions(extensionCount);
#else
		return nullptr;
#endif
	}
}
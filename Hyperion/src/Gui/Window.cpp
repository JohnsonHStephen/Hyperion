#include "Gui/Window.h"

#ifdef PLATFORM_WINDOWS
#include "GUI/WindowsWindow.h"
#endif

namespace Hyperion
{
	std::unique_ptr<Window> Window::Create(const WindowProps& props) {
#ifdef PLATFORM_WINDOWS
		return std::make_unique<WindowsWindow>(props);
#else
		return nullptr;
#endif
	}
    const char** Window::RequiredInstanceExtensions(uint32_t& extensionCount) {
#ifdef PLATFORM_WINDOWS
		return WindowsWindow::RequiredInstanceExtensions(extensionCount);
#else
		return nullptr;
#endif
    }
}
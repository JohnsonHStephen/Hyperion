# source code
target_sources (Hyperion
	PRIVATE
		"ImGuiOverlay.cpp"
		"Window.cpp"
		"WindowsWindow.cpp"
)

target_sources (Hyperion
	PRIVATE
		FILE_SET engineScope
		TYPE HEADERS
		FILES
			"WindowsWindow.h"
)

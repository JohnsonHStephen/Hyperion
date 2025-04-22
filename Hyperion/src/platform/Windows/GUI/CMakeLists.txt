# source code
target_sources (Hyperion
	PRIVATE
		"ImGuiLayer.cpp"
		"WindowsWindow.cpp"
)

# private headers
target_sources (Hyperion
	PRIVATE
		FILE_SET windows
		TYPE HEADERS
		FILES
			"ImGuiLayer.h"
			"WindowsWindow.h"
)

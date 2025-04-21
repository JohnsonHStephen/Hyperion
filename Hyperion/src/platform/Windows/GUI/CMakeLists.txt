# source code
target_sources (Hyperion
	PRIVATE
		"ImGuiLayer.cpp"
)

# private headers
target_sources (Hyperion
	PRIVATE
		FILE_SET gui
		TYPE HEADERS
		FILES
			"ImGuiLayer.h"
)

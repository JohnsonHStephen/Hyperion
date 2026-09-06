# source code
target_sources (Hyperion
	PRIVATE
		"GLFWWindowAdapter.cpp"
)

target_sources (Hyperion
	PRIVATE
		FILE_SET engineScope
		TYPE HEADERS
		FILES
			"GLFWWindowAdapter.h"
)

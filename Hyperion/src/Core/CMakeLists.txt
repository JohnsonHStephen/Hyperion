# CMakeList.txt : CMake project for Hyperion, include sources

# source code
target_sources (Hyperion
	PRIVATE
		"Application.cpp"
		"Window.cpp"
)

# private headers
target_sources (Hyperion
	PRIVATE
		FILE_SET core
		TYPE HEADERS
		FILES
			"Application.h"
			"Window.h"
)
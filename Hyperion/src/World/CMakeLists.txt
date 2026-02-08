# CMakeList.txt : CMake project for Hyperion, include sources
target_sources (Hyperion
	PRIVATE
		"Camera.cpp"
		"GameObject.cpp"
		"World.cpp"
)

target_sources (Hyperion
	PRIVATE
		FILE_SET engineScope
		TYPE HEADERS
		FILES
			"Entity.h"
)

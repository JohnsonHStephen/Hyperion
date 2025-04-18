# CMakeList.txt : CMake project for Hyperion, include sources

# source code
target_sources (Hyperion
	PRIVATE
		Hyperion.cpp
)

# private headers
target_sources (Hyperion
	PRIVATE
		FILE_SET private
		TYPE HEADERS
		FILES
			Hyperion.h
)

add_subdirectory ("imgui")
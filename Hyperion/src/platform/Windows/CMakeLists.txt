# CMakeList.txt : CMake project for Hyperion, include sources

# private headers
target_sources (Hyperion
	PRIVATE
		FILE_SET windows
		TYPE HEADERS
)

add_subdirectory ("GUI")
add_subdirectory ("OpenGL")
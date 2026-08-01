# CMakeList.txt : CMake project for Hyperion

# private headers
target_sources (Hyperion
	PUBLIC
		FILE_SET api
		TYPE HEADERS
		FILES
)


add_subdirectory ("Gui")
add_subdirectory ("Pipelines")
add_subdirectory ("Vulkan")
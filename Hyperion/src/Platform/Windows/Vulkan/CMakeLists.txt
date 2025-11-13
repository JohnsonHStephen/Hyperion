# CMakeList.txt : CMake project for Hyperion, include sources

# source code
target_sources (Hyperion
	PRIVATE
		"VulkanContext.cpp"
)

# private headers
target_sources (Hyperion
	PRIVATE
		FILE_SET windows
		TYPE HEADERS
		FILES
)
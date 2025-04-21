# CMakeList.txt : CMake project for Hyperion, include external sources

target_sources (Hyperion
	PRIVATE
		FILE_SET external
		TYPE HEADERS
)

add_subdirectory ("imgui")
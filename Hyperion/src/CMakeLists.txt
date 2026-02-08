# CMakeList.txt : CMake project for Hyperion, include sources

target_sources (Hyperion
	PRIVATE
		FILE_SET engineScope
		TYPE HEADERS
		FILES
)

add_subdirectory ("Core")
add_subdirectory ("Graphics")
add_subdirectory ("World")
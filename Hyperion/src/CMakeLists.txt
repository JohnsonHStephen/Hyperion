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

# precompiled header
# this is auto included by all source files acording to the CMake documentation
# https://cmake.org/cmake/help/v3.23/command/target_precompile_headers.html
target_precompile_headers (Hyperion
	PRIVATE
		HyperionPCH.h
)

add_subdirectory ("Core")
add_subdirectory ("Platform")
# CMakeList.txt : CMake project for Hyperion

# headers
target_sources (Hyperion
	PUBLIC
		FILE_SET api
		TYPE HEADERS
		FILES
			"Application.h"
			"EngineInfo.h"
			"FrameListener.h"
			"Hyperion.h"
			"Main.h"
)

# precompiled header
# this is auto included by all source files acording to the CMake documentation
# https://cmake.org/cmake/help/v3.23/command/target_precompile_headers.html
target_precompile_headers (Hyperion
	PRIVATE
		HyperionPCH.h
)

# CMakeList.txt : CMake project for Hyperion, include sources

# private headers
target_sources (Hyperion
	PRIVATE
		FILE_SET event
		TYPE HEADERS
		FILES
			"Event.h"
			"MouseEvent.h"
			"WindowEvent.h"
 "KeyEvent.h")
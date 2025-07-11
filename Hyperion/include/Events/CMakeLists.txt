# CMakeList.txt : CMake project for Hyperion

# private headers
target_sources (Hyperion
	PUBLIC
		FILE_SET api
		TYPE HEADERS
		FILES
			"Event.h"
			"MouseEvent.h"
			"WindowEvent.h"
			"KeyEvent.h"
)
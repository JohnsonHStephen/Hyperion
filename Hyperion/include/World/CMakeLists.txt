# This is for all headers necessary for using the Hyperion Engine

target_sources (Hyperion
	PUBLIC
		FILE_SET api
		TYPE HEADERS
		FILES
			"Camera.h"
			"Components.h"
			"Entity.h"
			"World.h"
)
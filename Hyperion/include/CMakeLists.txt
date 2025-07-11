# This is for all headers necessary for using the Hyperion Engine

target_sources (Hyperion
	PUBLIC
		FILE_SET api
		TYPE HEADERS
		FILES
)

add_subdirectory ("Core")
add_subdirectory ("Events")
add_subdirectory ("Gui")
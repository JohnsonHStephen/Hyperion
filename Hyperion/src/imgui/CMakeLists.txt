# CMakeList.txt : CMake project for Hyperion, include sources

# source code
target_sources (Hyperion
	PRIVATE
		imgui_impl_glfw.cpp
		imgui_impl_opengl3.cpp
)

# private headers
target_sources (Hyperion
	PRIVATE
		FILE_SET private
		TYPE HEADERS
		FILES
			imgui_impl_glfw.h
			imgui_impl_opengl3.h
			imgui_impl_opengl3_loader.h
)
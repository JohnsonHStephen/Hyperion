add_subdirectory(../Hyperion Hyperion_build) # Path to Hyperion

find_package(imgui CONFIG REQUIRED)

add_executable(Sandbox)

set_property(TARGET Sandbox PROPERTY CXX_STANDARD 20)

# Include the Hyperion library and includes
target_link_libraries(Sandbox PRIVATE Hyperion)
target_include_directories(Sandbox PRIVATE ../Hyperion/include)


target_link_libraries(Sandbox PRIVATE imgui::imgui)

#add sources
target_sources (Sandbox
	PRIVATE
		"SandboxEntry.cpp"
		"SandboxLayer.cpp")
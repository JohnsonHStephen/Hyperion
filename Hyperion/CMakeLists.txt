# CMakeList.txt : CMake project for Hyperion, include source and define
# project specific logic here.
#

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

# Add source to this project's executable.
add_library (Hyperion)

if (PLATFORM_WINDOWS)
target_compile_definitions(Hyperion PUBLIC PLATFORM_WINDOWS)
elseif(PLATFORM_LINUX)
target_compile_definitions(Hyperion PUBLIC PLATFORM_LINUX)
elseif(PLATFORM_MACOS)
target_compile_definitions(Hyperion PUBLIC PLATFORM_MACOS)
endif()

add_subdirectory ("src")
add_subdirectory ("include")
add_subdirectory ("external")

set_property(TARGET Hyperion PROPERTY CXX_STANDARD 20)

target_link_libraries(Hyperion PRIVATE glfw)
target_link_libraries(Hyperion PRIVATE glad::glad)
target_link_libraries(Hyperion PRIVATE imgui::imgui)
target_link_libraries(Hyperion PRIVATE Vulkan::Vulkan)

# TODO: Add tests and install targets if needed.

# CMakeList.txt : CMake project for Hyperion, include source and define
# project specific logic here.
#

find_package(EnTT CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

# Define a custom command to run the batch file
add_custom_target(CompileShaders
    COMMAND cmd.exe /c ${CMAKE_CURRENT_SOURCE_DIR}/include/Graphics/Shaders/compile.bat
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/Graphics/Shaders/
    COMMENT "Compiling Shaders..."
)

# Add source to this project's executable.
add_library (Hyperion)
add_dependencies(Hyperion CompileShaders)

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

target_link_libraries(Hyperion PRIVATE EnTT::EnTT)
target_link_libraries(Hyperion PRIVATE glfw)
target_link_libraries(Hyperion PRIVATE glm::glm)
target_link_libraries(Hyperion PRIVATE imgui::imgui)
target_link_libraries(Hyperion PRIVATE stduuid)
target_link_libraries(Hyperion PRIVATE Vulkan::Vulkan)

# TODO: Add tests and install targets if needed.

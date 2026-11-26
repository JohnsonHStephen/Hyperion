# CMakeList.txt : CMake project for Hyperion, include source and define
# project specific logic here.
#

find_package(EnTT CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)


find_program(GLSLC_EXECUTABLE
    NAMES glslc
    HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin"
    REQUIRED
)

add_custom_target(CompileShaders
    COMMAND "${GLSLC_EXECUTABLE}" shader.vert -o vert.spv
    COMMAND "${GLSLC_EXECUTABLE}" shader.frag -o frag.spv
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/Graphics/Shaders"
    COMMENT "Compiling Shaders..."
    VERBATIM
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

target_compile_definitions(Hyperion PRIVATE IMGUI_IMPL_VULKAN_USE_VOLK)
target_compile_definitions(Hyperion PRIVATE VK_NO_PROTOTYPES)

add_subdirectory ("src")
add_subdirectory ("include")
add_subdirectory ("external")

set_property(TARGET Hyperion PROPERTY CXX_STANDARD 20)

target_link_libraries(Hyperion PRIVATE EnTT::EnTT)
target_link_libraries(Hyperion PRIVATE glfw)
target_link_libraries(Hyperion PRIVATE glm::glm)
target_link_libraries(Hyperion PRIVATE stduuid)
target_link_libraries(Hyperion PRIVATE Vulkan::Headers GPUOpen::VulkanMemoryAllocator volk::volk volk::volk_headers)
target_link_libraries(Hyperion PRIVATE imgui::imgui)

# TODO: Add tests and install targets if needed.

# CMakeList.txt : CMake project for Hyperion, include source and define
# project specific logic here.
#

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_path(LINMATH_INCLUDE_DIRS "linmath.h/linmath.h")
find_package(imgui CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (Hyperion "Hyperion.cpp" "Hyperion.h" "imgui/imgui_impl_glfw.cpp" "imgui/imgui_impl_glfw.h" "imgui/imgui_impl_opengl3.cpp" "imgui/imgui_impl_opengl3.h" "imgui/imgui_impl_opengl3_loader.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Hyperion PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(Hyperion PRIVATE glfw)
target_link_libraries(Hyperion PRIVATE glad::glad)
target_link_libraries(Hyperion PRIVATE imgui::imgui)

target_include_directories(Hyperion PRIVATE ${LINMATH_INCLUDE_DIRS})

# TODO: Add tests and install targets if needed.

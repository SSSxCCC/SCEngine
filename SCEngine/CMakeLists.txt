# CMakeList.txt : CMake project for SCEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Add source to this project's executable.
add_executable (SCEngine "main.cpp" "imgui_backends/imgui_impl_glfw.cpp" "imgui_backends/imgui_impl_opengl3.cpp")

set_property(TARGET SCEngine PROPERTY CXX_STANDARD 20)

target_link_libraries(SCEngine PUBLIC box2d)
target_link_libraries(SCEngine PUBLIC glfw)
target_link_libraries(SCEngine PUBLIC glad)
target_link_libraries(SCEngine PUBLIC imgui)

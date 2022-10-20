# CMakeList.txt : CMake project for SCEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Add source to this project's executable.
add_executable (SCEngineEditor "main.cpp" "imgui_backends/imgui_impl_glfw.cpp" "imgui_backends/imgui_impl_opengl3.cpp" "editor/GameWorldEditor.cpp" "editor/SubWindow.cpp")
target_include_directories (SCEngineEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_property (TARGET SCEngineEditor PROPERTY CXX_STANDARD 20)

add_subdirectory ("../SCEngineCommon" "SCEngineCommon")
add_subdirectory ("../Dependency/glfw" "glfw")
add_subdirectory ("../Dependency/imgui" "imgui")

target_link_libraries (SCEngineEditor PUBLIC SCEngineCommon)
target_link_libraries (SCEngineEditor PUBLIC glfw)
target_link_libraries (SCEngineEditor PUBLIC imgui)

install (TARGETS SCEngineEditor DESTINATION bin)

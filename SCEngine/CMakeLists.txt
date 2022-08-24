# CMakeList.txt : CMake project for SCEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Add source to this project's executable.
add_executable (SCEngine "main.cpp" "imgui_backends/imgui_impl_glfw.cpp" "imgui_backends/imgui_impl_opengl3.cpp" "common/RectangleCollider.cpp" "common/RectangleRender.cpp" "common/RigidBody.cpp" "core/Camera.cpp" "core/GameObject.cpp" "core/GameWorld.cpp" "editor/DeugDraw.cpp" "editor/EditorCameraController.cpp" "utility/Window.h" "core/Input.cpp" "core/Script.cpp")
target_include_directories(SCEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_property(TARGET SCEngine PROPERTY CXX_STANDARD 20)

target_link_libraries(SCEngine PUBLIC box2d)
target_link_libraries(SCEngine PUBLIC glfw)
target_link_libraries(SCEngine PUBLIC glad)
target_link_libraries(SCEngine PUBLIC glm)
target_link_libraries(SCEngine PUBLIC imgui)
target_link_libraries(SCEngine PUBLIC nlohmann_json)
target_link_libraries(SCEngine PUBLIC static_block)

install(TARGETS SCEngine DESTINATION bin)
install(DIRECTORY "shaders"
  DESTINATION bin
  )

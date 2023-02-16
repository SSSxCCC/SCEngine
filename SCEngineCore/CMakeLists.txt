# CMakeList.txt : CMake project for SCEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Add source to this project's executable.
add_library (SCEngineCore "sc/physics2d/RectangleCollider2D.cpp" "sc/renderer2d/RectangleRenderer2D.cpp" "sc/physics2d/RigidBody2D.cpp" "sc/physics2d/PhysicsWorld2D.cpp" "sc/core/Camera.cpp" "sc/core/GameObject.cpp" "sc/core/Scene.cpp" "sc/editor/DebugDrawPhysics2D.cpp" "sc/editor/EditorCameraController.cpp" "sc/input/Input.cpp" "sc/input/InputManager.cpp" "sc/core/Script.cpp" "sc/utility/Window.cpp" "sc/core/Transform2D.cpp" "sc/core/Engine.cpp")
target_include_directories (SCEngineCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_property (TARGET SCEngineCore PROPERTY CXX_STANDARD 20)

add_subdirectory ("../SCEngineCommon" "SCEngineCommon")
add_subdirectory ("../Dependency/box2d/src" "box2d")

target_link_libraries (SCEngineCore PUBLIC SCEngineCommon)
target_link_libraries (SCEngineCore PUBLIC box2d)

install (DIRECTORY "sc/asset/SCEngineAssets" DESTINATION "Assets")
#install (TARGETS SCEngineCore DESTINATION bin)

# CMakeList.txt : CMake project for SCEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Add source to this project's executable.
add_library (SCEngineCore "physics2d/RectangleCollider2D.cpp" "renderer2d/RectangleRenderer2D.cpp" "physics2d/RigidBody2D.cpp" "physics2d/PhysicsWorld2D.cpp" "core/Camera.cpp" "core/GameObject.cpp" "core/GameWorld.cpp" "editor/DebugDrawPhysics2D.cpp" "editor/EditorCameraController.cpp" "input/Input.cpp" "core/Script.cpp" "utility/Window.cpp" "utility/Asset.cpp" "core/Transform2D.cpp")
target_include_directories (SCEngineCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_property (TARGET SCEngineCore PROPERTY CXX_STANDARD 20)

add_subdirectory ("../SCEngineCommon" "SCEngineCommon")
add_subdirectory ("../Dependency/box2d/src" "box2d")

target_link_libraries (SCEngineCore PUBLIC SCEngineCommon)
target_link_libraries (SCEngineCore PUBLIC box2d)

install (DIRECTORY "asset/SCEngineAsset" DESTINATION "asset")
#install (TARGETS SCEngineCore DESTINATION bin)

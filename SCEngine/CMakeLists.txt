# CMakeList.txt : CMake project for SCEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Add source to this project's executable.
add_library (SCEngine SHARED "SCEngine.cpp" "physics/RectangleCollider.cpp" "graphics/RectangleRender.cpp" "physics/RigidBody.cpp" "physics/PhysicsWorld.cpp" "core/Camera.cpp" "core/GameObject.cpp" "core/GameWorld.cpp" "editor/PhysicsDeugDraw.cpp" "editor/EditorCameraController.cpp" "input/Input.cpp" "core/Script.cpp" "utility/Window.cpp" "core/Transform.cpp")
target_include_directories(SCEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_property(TARGET SCEngine PROPERTY CXX_STANDARD 20)

target_link_libraries(SCEngine PUBLIC SCEngineCommon)
target_link_libraries(SCEngine PUBLIC box2d)
target_link_libraries(SCEngine PUBLIC glm)
target_link_libraries(SCEngine PUBLIC nlohmann_json)

install(DIRECTORY "shaders" DESTINATION bin)
install(TARGETS SCEngine DESTINATION bin)

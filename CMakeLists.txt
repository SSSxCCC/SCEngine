# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.12)

project ("SCEngine" VERSION 0.1)

add_subdirectory ("SCEngine")

add_subdirectory ("Dependency/box2d/src")
add_subdirectory ("Dependency/glad")
add_subdirectory ("Dependency/glfw")
add_subdirectory ("Dependency/glm")
add_subdirectory ("Dependency/imgui")
add_subdirectory("Dependency/nlohmann_json")

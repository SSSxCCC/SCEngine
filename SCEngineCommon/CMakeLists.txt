cmake_minimum_required (VERSION 3.12)

add_library (SCEngineCommon "Common.cpp")

target_include_directories (SCEngineCommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_property (TARGET SCEngineCommon PROPERTY CXX_STANDARD 20)

add_subdirectory ("../Dependency/glad" "glad")
add_subdirectory ("../Dependency/glm" "glm")
add_subdirectory ("../Dependency/nlohmann_json" "nlohmann_json")

target_link_libraries (SCEngineCommon PUBLIC glad)
target_link_libraries (SCEngineCommon PUBLIC glm)
target_link_libraries (SCEngineCommon PUBLIC nlohmann_json)

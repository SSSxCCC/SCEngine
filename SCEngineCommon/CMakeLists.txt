cmake_minimum_required (VERSION 3.12)

add_library (SCEngineCommon "graphics/VulkanManager.cpp" "common/Platform.cpp")
set_property (TARGET SCEngineCommon PROPERTY CXX_STANDARD 20)

target_include_directories (SCEngineCommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (SCEngineCommon PUBLIC "../Dependency/glfw/include")

add_subdirectory ("../Dependency/glm" "glm")
add_subdirectory ("../Dependency/nlohmann_json" "nlohmann_json")
add_subdirectory ("../Dependency/stb_image" "stb_image")
add_subdirectory ("../Dependency/tinyobjloader" "tinyobjloader")

target_link_libraries (SCEngineCommon PUBLIC glm)
target_link_libraries (SCEngineCommon PUBLIC nlohmann_json)
target_link_libraries (SCEngineCommon PUBLIC stb_image)
target_link_libraries (SCEngineCommon PUBLIC tinyobjloader)

find_package(Vulkan REQUIRED)
target_include_directories(SCEngineCommon PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries (SCEngineCommon PUBLIC ${Vulkan_LIBRARIES})

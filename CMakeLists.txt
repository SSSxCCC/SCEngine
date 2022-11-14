# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.12)

project ("SCEngine" VERSION 0.1)

# Currently asan generates more bugs than it fixes
#add_compile_options(-fsanitize=address)
#add_compile_definitions(SANITIZE)

#add_subdirectory ("SCEngineCore")
add_subdirectory ("SCEngineEditor")
#add_subdirectory ("DemoGames/EmptyProject")

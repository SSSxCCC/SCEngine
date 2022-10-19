cmake_minimum_required (VERSION 3.12)

project ("EmptyProject")

add_subdirectory ("../.." "SCEngineRoot")

add_library (SCEngine SHARED "EmptyProject.cpp")

set_property (TARGET SCEngine PROPERTY CXX_STANDARD 20)

target_link_libraries (SCEngine PUBLIC SCEngineCore)

install (TARGETS SCEngine DESTINATION bin)

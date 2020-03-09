# Works with 3.11 and tested through 3.15
cmake_minimum_required(VERSION 3.11...3.16)

# Project name and a few useful settings. Other commands can pick up the results
project(test_plugin
    VERSION 0.1
    DESCRIPTION "An example project with CMake"
    LANGUAGES CXX)

# so example
add_subdirectory(so1)
add_executable(test_so main1.cpp)
target_link_libraries(test_so so1)

# plugin example
include_directories(include)
add_compile_options("-g;-ldl")

add_subdirectory(plugin1)
add_subdirectory(plugin2)

add_executable(test_plugin main2.cpp)
# target_link_libraries(test_plugin plugin1 ${CMAKE_DL_LIBS})
target_link_libraries(test_plugin ${CMAKE_DL_LIBS})

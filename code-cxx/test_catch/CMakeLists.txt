cmake_minimum_required(VERSION 3.0.0)

project(test_catch VERSION 0.1 LANGUAGES CXX)

# add_compile_options("$<$<CONFIG:DEBUG>:-W;-Wall;-O0;-g;-pedantic>")
add_compile_options("-g;-D_GLIBCXX_DEBUG")

# test_catch.cpp
add_executable(${PROJECT_NAME} "test_catch.cpp" "test_x.cpp")
target_link_libraries(${PROJECT_NAME})

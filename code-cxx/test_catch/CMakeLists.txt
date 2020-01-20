cmake_minimum_required(VERSION 3.0.0)

project(test_catch VERSION 0.1 LANGUAGES CXX)

# add_compile_options("$<$<CONFIG:DEBUG>:-W;-Wall;-O0;-g;-pedantic>")
add_compile_options("-g;-D_GLIBCXX_DEBUG")

find_package(Qt5 COMPONENTS Core)

if(Qt5Core_FOUND)
  message(STATUS "Qt5Core found")
else()
  message(STATUS "Qt5Core not found, not building moc")
endif()

# test_catch.cpp
add_executable(${PROJECT_NAME} "test_catch.cpp")

target_link_libraries(${PROJECT_NAME} Qt5::Core)

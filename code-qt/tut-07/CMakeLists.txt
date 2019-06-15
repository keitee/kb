cmake_minimum_required(VERSION 3.0.0)

project(tut-07 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "/home/keitee/Qt/5.12.3/gcc_64/lib/cmake/Qt5Core")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} Qt5::Core)

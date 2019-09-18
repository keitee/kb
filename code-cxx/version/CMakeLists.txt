cmake_minimum_required(VERSION 3.0.0)

project(get-version VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} "get_version.cpp")

target_link_libraries(${PROJECT_NAME})

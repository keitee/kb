cmake_minimum_required(VERSION 3.0.0)

project(io VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(iof "iof.cpp")
target_link_libraries(iof)

add_executable(poll_pipes "poll_pipes.cpp")
target_link_libraries(poll_pipes)

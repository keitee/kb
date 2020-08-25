cmake_minimum_required(VERSION 3.0.0)

project(io VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(iof "iof.cpp")
target_link_libraries(iof)

add_executable(poll_pipes "poll_pipes.cpp")
target_link_libraries(poll_pipes)

add_executable(socket_server "socket_server.c")
target_link_libraries(socket_server)

add_executable(socket_client "socket_client.c")
target_link_libraries(socket_client)

cmake_minimum_required(VERSION 3.0.0)

project(io VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(GMOCK_DIR "$ENV{HOME}/works/googletest/googlemock" CACHE PATH "The path to the GoogleMock")
add_subdirectory(${GMOCK_DIR} ${CMAKE_BINARY_DIR}/gmock)
include_directories(SYSTEM ${GMOCK_DIR}/include)

# to use libs static library
#
# Add directories in which the linker will look for libraries.
#
# link_directories([AFTER|BEFORE] directory1 [directory2 ...])
#
# Adds the paths in which the linker should search for libraries. Relative paths
# given to this command are interpreted as relative to the current source
# directory, see CMP0015.
set(LIBS_DIR "../libs")
set(LIBS libcxx.a)
link_directories(${LIBS_DIR}/build)
include_directories(${LIBS_DIR})

add_executable(io_copy "io_copy.cpp")
target_link_libraries(io_copy ${LIBS})

add_executable(io_bad_exclusive_open "io_bad_exclusive_open.cpp")
target_link_libraries(io_bad_exclusive_open ${LIBS})

add_executable(io_simple_pipe "io_simple_pipe.cpp")
target_link_libraries(io_simple_pipe ${LIBS})

add_executable(io_pipe_filter "io_pipe_filter.cpp")
target_link_libraries(io_pipe_filter ${LIBS})

add_executable(io_pipe_popen "io_pipe_popen.cpp")
target_link_libraries(io_pipe_popen ${LIBS})

add_executable(io_popen_glob "io_popen_glob.cpp")
target_link_libraries(io_popen_glob ${LIBS})

add_executable(io_select "io_select.cpp")
target_link_libraries(io_select ${LIBS})

add_executable(io_poll_pipes "io_poll_pipes.cpp")
target_link_libraries(io_poll_pipes ${LIBS})

add_executable(io_epoll_input "io_epoll_input.cpp")
target_link_libraries(io_epoll_input ${LIBS})

add_executable(test_os_io "test_os_io.cpp")
target_link_libraries(test_os_io gmock_main ${LIBS})

# add_executable(socket_server "socket_server.c")
# target_link_libraries(socket_server)

# add_executable(socket_client "socket_client.c")
# target_link_libraries(socket_client)

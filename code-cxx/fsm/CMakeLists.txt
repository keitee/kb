cmake_minimum_required(VERSION 3.0.0)

project(fsm VERSION 0.1 LANGUAGES CXX)

enable_testing()

# https://stackoverflow.com/questions/690223/how-to-retrieve-a-user-environment-variable-in-cmake-windows
# Getting variables into your CMake script
# You can pass a variable on the line with the cmake invocation:
#
# FOO=1 cmake
# or by exporting a variable in BASH:
#
# export FOO=1
# Then you can pick it up in a cmake script using:
#
# $ENV{FOO}

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# to gnerate signal/slot codes
set(CMAKE_AUTOMOC ON)

set(GMOCK_DIR "$ENV{HOME}/works/googletest/googlemock" CACHE PATH "The path to the GoogleMock")


# Include the directory source_dir in your project. This directory must contain
# a CMakeLists.txt file. Note: We're omitting the optional second parameter.
# This only works with subdirectories of the current directory. We will see how
# to add external directories later.
#
# As checked:
# /home/keitee/works/googletest/googlemock/CMakeLists.txt
#
# add_subdirectory(source_dir [binary_dir])
#
# Add the directory source_dir to the current project with binary_dir as its
# corresponding binary output directory. When adding a directory that is a
# subdirectory of the current directory CMake will automatically determine what
# the binary output directory should be, making the second argument optional.
# However if you add a directory that isn't a subdirectory you need to specify
# the binary output directory. 
#
#
# CMAKE_BINARY_DIR 
#
# This variable holds the path to the top level binary output directory, i.e.
# the directory in which you ran the cmake command

add_subdirectory(${GMOCK_DIR} ${CMAKE_BINARY_DIR}/gmock)


# include_directories([AFTER|BEFORE] [SYSTEM] directory...)
#
# AFTER|BEFORE
# Specify whether or not these include directories should be appended or prepended
# to the list of include directories. If omitted then the default behavior is
# used. By default directories are appended to the list. This behavior can be
# changed by setting CMAKE_INCLUDE_DIRECTORIES_BEFORE to TRUE. 
#
# SYSTEM 
# Specify that these directories are system include directories. This only has an
# affect on compilers that support the distinction. This can change the order in
# which the compiler searches include directories or the handling of warnings from
# headers found in these directories. 
#
# directory... 
# The directories to be added to the list of include directories.

include_directories(SYSTEM ${GMOCK_DIR}/include)

# do not work
# add_compile_options("$<$<CONFIG:DEBUG>:-W;-Wall;-O0;-g;-pedantic>")
add_compile_options("-g")


# to use static library
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


# fsm atm
add_executable(${PROJECT_NAME}_atm fsm_atm.cpp sfsm.cpp dfsm.cpp)
target_link_libraries(${PROJECT_NAME}_atm gmock_main)


# fsm ex
add_executable(${PROJECT_NAME}_ex fsm_ex.cpp)
target_link_libraries(${PROJECT_NAME}_ex gmock_main)


# qtfsm
find_package(Qt5 COMPONENTS Core REQUIRED)
if(Qt5Core_FOUND)
  message(STATUS "Qt5Core found")
else()
  message(STATUS "Qt5Core not found, not building moc")
endif()

find_package(Qt5Test)
add_executable(qtfsm test_qtfsm.cpp qtfsm.cpp)
target_link_libraries(qtfsm gmock_main Qt5::Core Qt5::Test ${LIBS})


# qfsm which mimic qtfsm without qt
add_executable(qfsm test_qfsm.cpp qfsm.cpp)
target_link_libraries(qfsm gmock_main ${LIBS})

# add_test(Gtest.MatcherForPair ${PROJECT_NAME})
add_test(${PROJECT_NAME} ${PROJECT_NAME})

cmake_minimum_required(VERSION 3.0.0)

project(cxx VERSION 0.1 LANGUAGES CXX)

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

# https://stackoverflow.com/questions/6646405/how-do-you-add-boost-libraries-in-cmakelists-txt
#
# For this warning:

# CMake Warning at /usr/share/cmake-3.7/Modules/FindBoost.cmake:761 (message):
#  Imported targets not available for Boost version 106501
#
# see https://stackoverflow.com/questions/42123509/cmake-finds-boost-but-the-imported-targets-not-available-for-boost-version

find_package(Boost 1.65.1 COMPONENTS system thread)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

include_directories(${Boost_INCLUDE_DIRS}) 


# cxx.cpp
add_executable(${PROJECT_NAME} "cxx.cpp")

# cause link errors to Boost::thread
# target_link_libraries(${PROJECT_NAME} gmock_main ${Boost_LIBRARIES})

target_link_libraries(${PROJECT_NAME} gmock_main Boost::thread)


# cxx_ex.cpp
add_executable(${PROJECT_NAME}_ex "cxx_ex.cpp")
target_link_libraries(${PROJECT_NAME}_ex gmock_main Boost::thread)

# cxx_pattern.cpp
add_executable(${PROJECT_NAME}_pattern "cxx_pattern.cpp")
target_link_libraries(${PROJECT_NAME}_pattern gmock_main Boost::thread)

# cxx_case.cpp
add_executable(${PROJECT_NAME}_case "cxx_case.cpp")
target_link_libraries(${PROJECT_NAME}_case gmock_main Boost::thread)

# add_test(Gtest.MatcherForPair ${PROJECT_NAME})
add_test(${PROJECT_NAME} ${PROJECT_NAME})


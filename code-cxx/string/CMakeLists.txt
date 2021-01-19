cmake_minimum_required(VERSION 3.0.0)

project(test_string VERSION 0.1 LANGUAGES CXX)

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

# to gnerate signal/slot codes
# set(CMAKE_AUTOMOC ON)

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
#
# CMake Warning at /usr/share/cmake-3.7/Modules/FindBoost.cmake:761 (message):
#  Imported targets not available for Boost version 106501
#
# see https://stackoverflow.com/questions/42123509/cmake-finds-boost-but-the-imported-targets-not-available-for-boost-version
#
# 1.65.1 emits some error when linking with cmake and 1.70.0 fixes up.
# find_package(Boost 1.65.1 COMPONENTS system thread)
#
# -- Found Boost 1.70.0 at /usr/local/lib/cmake/Boost-1.70.0
# --   Requested configuration: QUIET COMPONENTS system;thread;regex
# -- Found boost_headers 1.70.0 at /usr/local/lib/cmake/boost_headers-1.70.0
# -- Found boost_system 1.70.0 at /usr/local/lib/cmake/boost_system-1.70.0
# --   libboost_system.a
# -- Adding boost_system dependencies: headers
# -- Found boost_thread 1.70.0 at /usr/local/lib/cmake/boost_thread-1.70.0
# --   libboost_thread.a
# -- Adding boost_thread dependencies: headers
# -- Found boost_regex 1.70.0 at /usr/local/lib/cmake/boost_regex-1.70.0
# --   libboost_regex.a
# -- Adding boost_regex dependencies: headers
# -- Boost 1.70.0 found.
# -- Found Boost components:
#    system;thread;regex
#
# /usr/local/lib/cmake/boost_regex-1.70.0/boost_regex-config.cmake
# # Create imported target Boost::regex
# add_library(Boost::regex UNKNOWN IMPORTED)

find_package(Boost 1.70.0 COMPONENTS system thread regex)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
include_directories(${Boost_INCLUDE_DIRS}) 

# https://gcc.gnu.org/projects/cxx-status.html#cxx1z
# C++17 Support in GCC
# GCC has almost full support for the latest revision of the C++ standard, which
# was published in 2017. Some library features are missing or incomplete, as
# described in the library documentation.

# C++17 features are available since GCC 5. This mode is the default in GCC 11;
# it can be explicitly selected with the -std=c++17 command-line flag, or
# -std=gnu++17 to enable GNU extensions as well.

# add_compile_options("-O2;-std=c++17")
# add_compile_options("-g;-std=c++17")
# add_compile_options("-g;-std=c++17;-D_GLIBCXX_DEBUG")
add_compile_options("-g;-std=c++17;-D_GLIBCXX_DEBUG_PEDANTIC")

# string_01
add_executable(${PROJECT_NAME}_1 test_string_1.cpp)
target_link_libraries(${PROJECT_NAME}_1 gmock_main Boost::regex)

# string_02
add_executable(${PROJECT_NAME}_2 test_string_2.cpp)
target_link_libraries(${PROJECT_NAME}_2 gmock_main Boost::regex)

# string_03
add_executable(${PROJECT_NAME}_3 test_string_3.cpp)
target_link_libraries(${PROJECT_NAME}_3 gmock_main)

# string_view standalone app
add_executable(test_string_view test_string_view.cpp)
target_link_libraries(test_string_view)

# string_sso
add_executable(test_string_sso test_string_sso.cpp)
target_link_libraries(test_string_sso)

# string_pad
add_executable(test_string_pad test_string_pad.cpp)
target_link_libraries(test_string_pad gmock_main)

# add_test(Gtest.MatcherForPair ${PROJECT_NAME})
add_test(${PROJECT_NAME} ${PROJECT_NAME})


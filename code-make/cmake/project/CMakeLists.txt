# Works with 3.11 and tested through 3.15
cmake_minimum_required(VERSION 3.11...3.16)

# Project name and a few useful settings. Other commands can pick up the results
project(ModernCMakeExample
    VERSION 0.1
    DESCRIPTION "An example project with CMake"
    LANGUAGES CXX)

# PROJECT_NAME
# 
# Name of the project given to the project command.
# 
# This is the name given to the most recently called project() command in the
# current directory scope or above. To obtain the name of the top level project,
# see the CMAKE_PROJECT_NAME variable.
# 
# https://cmake.org/cmake/help/latest/variable/CMAKE_PROJECT_NAME.html?highlight=cmake_project_name
#
# CMAKE_PROJECT_NAME
# 
# The name of the top level project.
# 
# This variable holds the name of the project as specified in the top level
# CMakeLists.txt file by a project() command. 


# PROPERTY
#
# Many target properties are initialized from a matching variable with CMAKE_ at
# the front. So setting CMAKE_CXX_STANDARD, for example, will mean that all new
# targets created will have CXX_STANDARD set to that when they are created.

# MESSAGE
# --log-level=<ERROR|WARNING|NOTICE|STATUS|VERBOSE|DEBUG|TRACE>
#
# Set the log level.
# 
# The message() command will only output messages of the specified log level or
# higher. The default log level is STATUS.
# 
# For backward compatibility reasons, --loglevel is also accepted as a synonym
# for this option


# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here

  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Let's nicely support folders in IDE's
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  include(CTest)

  # Docs only available if this is the main app
  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    # NOTE: can see while configuring stage
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()


# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14
if(${CMAKE_VERSION} VERSION_LESS 3.14)
  message(STATUS "cmake is less than 3.14 and add FetchContent")
  include(cmake/add_FetchContent_MakeAvailable.cmake)
endif()


# Accumulator library
# This is header only, so could be replaced with git submodules or FetchContent
find_package(Boost REQUIRED)
# Adds Boost::boost

# NOTE: it also runs its test
# # Formatting library
# FetchContent_Declare(
#   fmtlib
#   GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#   GIT_TAG        5.3.0
# )
# FetchContent_MakeAvailable(fmtlib)
# # Adds fmt::fmt

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)


# GENERATOR
#
# Generator-expressions are really powerful, but a bit odd and specialized. Most
# CMake commands happen at configure time, include the if statements seen above.
# But what if you need logic to occur at build time or even install time?
# Generator expressions were added for this purpose.1 They are evaluated in
# target properties.
#
# https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html
#
# saw this and what's $<TARGET_OBJECTS:Service>?
#
# add_executable(
#         asrdkplayer
#         main.cpp
#         include/player.h
#         $<TARGET_OBJECTS:Service>
#         $<TARGET_OBJECTS:AAMP>
#         )
#
# the above statement:
# add_subdirectory(src)
# will run it's cmake file which has:
# add_library(student Student.cxx ${HEADER_LIST})
#
# so can use "$<TARGET_OBJECTS:student>". HOWEVER, that's in it?
# cannot use message() to print out but can write to a file:
# https://stackoverflow.com/questions/51353110/how-do-i-output-the-result-of-a-generator-expression-in-cmake
#
# file(GENERATE OUTPUT xx.out CONTENT $<TARGET_OBJECTS:student>)
# this will create xx.out in `the build directory` and it has:
# /home/keitee/git/kb/code-make/cmake/tutorial/step6/build/src/CMakeFiles/student.dir/Student.cxx.o


# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
    add_subdirectory(tests)
endif()


# from:
# https://gitlab.com/CLIUtils/modern-cmake/tree/master/examples/extended-project
# https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html

# https://cmake.org/cmake/help/latest/manual/cmake.1.html
#
# -S <path-to-source>
# Path to root directory of the CMake project to build.
#
# -B <path-to-build>
# Path to directory which CMake will use as the root of build directory.
# If the directory doesn’t already exist CMake will make it.

# To configure:
# cmake -S . -B build
#
# To build:
# cmake --build build
#
# To test (--target can be written as -t in CMake 3.15+):
# cmake --build build --target test
#
# To build docs (requires Doxygen, output in build/docs/html):
# cmake --build build --target docs

cmake_minimum_required(VERSION 3.0.0)

project(fetch VERSION 0.1 LANGUAGES CXX)

# https://cmake.org/cmake/help/v3.11/module/FetchContent.html
#
# This module enables populating content at configure time via any method
# supported by the ExternalProject module. Whereas ExternalProject_Add()
# downloads at `build time`, the FetchContent module makes content available
# immediately, allowing the configure step to use the content in commands like
# add_subdirectory(), include() or file() operations.

# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14
if(${CMAKE_VERSION} VERSION_LESS 3.14)
  message(STATUS "cmake is less than 3.14 and add FetchContent")
  include(cmake/add_FetchContent_MakeAvailable.cmake)
endif()

FetchContent_Declare(
catch2
GIT_REPOSITORY https://github.com/catchorg/Catch2.git
GIT_TAG        v2.9.1
)
FetchContent_MakeAvailable(catch2)
# Adds Catch2::Catch2

# Tests need to be added as executables first
add_executable(fetch fetch.cpp)

# I'm using C++17 in the test
target_compile_features(fetch PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(fetch PRIVATE Catch2::Catch2)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# add_test(NAME testlibtest COMMAND testlib) # Command can be a target

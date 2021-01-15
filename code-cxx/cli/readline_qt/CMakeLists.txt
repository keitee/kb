cmake_minimum_required(VERSION 3.0.0)

project(readline_qt VERSION 0.1 LANGUAGES CXX)

# both works
# CMAKE_PREFIX_PATH=/home/keitee/Qt/5.12.6/gcc_64 cmake ..
# CMAKE_PREFIX_PATH=/home/keitee/Qt/5.12.6/gcc_64/lib/cmake cmake ..

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
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories( ${CMAKE_CURRENT_LIST_DIR} )
include_directories( ./include  )


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

find_package(Qt5Core)
find_package(Qt5Test)

# find_package(Qt5.12 COMPONENTS Core)
# find_package(Qt5.12 COMPONENTS Test)

set_property(DIRECTORY PROPERTY QT_VERSION_MAJOR "5")
set_property(DIRECTORY PROPERTY QT_VERSION_MINOR "12")

# do not work
# add_compile_options("$<$<CONFIG:DEBUG>:-W;-Wall;-O0;-g;-pedantic>")
add_compile_options("-g;-ldl;")

# CMAKE_DL_LIBS
# The name of the library that has dlopen and dlclose in it, usually -ldl on
# most UNIX machines.

# console abd use readline.so
add_executable(readline_qt readline_qt.cpp 
  console.cpp 
  readline.cpp
  )

target_link_libraries(readline_qt Qt5::Core Qt5::Test ${CMAKE_DL_LIBS})

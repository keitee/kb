cmake_minimum_required (VERSION 2.6)
project(directory_test)

# bring the headers into the project
# 
# include_directories
# Add include directories to the build.
# 
# include_directories([AFTER|BEFORE] [SYSTEM] dir1 [dir2 ...])
# Add the given directories to those the compiler uses to search for include
# files. Relative paths are interpreted as relative to the current source
# directory.
# 
# The include directories are added to the INCLUDE_DIRECTORIES directory
# property for the current CMakeLists file. They are also added to the
# INCLUDE_DIRECTORIES target property for each target in the current CMakeLists
# file. The target property values are the ones used by the generators.
# 
# By default the directories specified are appended onto the current list of
# directories. This default behavior can be changed by setting
# CMAKE_INCLUDE_DIRECTORIES_BEFORE to ON. By using AFTER or BEFORE explicitly,
# you can select between appending and prepending, independent of the default.

# OK, to use here
# include_directories(include)

# can manually add the sources using the set command as:
# set(SOURCES src/main.cxx src/Student.cxx)
# However, the file(GLOB...) allows wildcard additions:

# *cmake-error* when typo cpp which should be cxx
# file(GLOB SOURCES "src/*.cpp")
#
# CMake Error at CMakeLists.txt:12 (add_executable):
#   No SOURCES given to target: student

file(GLOB SOURCES "src/*.cxx")

# make "student" executable
add_executable(student ${SOURCES})

# NOTE: HOWEVER, target_include_directories() should be after add_executable()
# since it has to use target name
#
# target_include_directories
# Add include directories to a target.
# 
# target_include_directories(<target> [SYSTEM] [BEFORE]
#   <INTERFACE|PUBLIC|PRIVATE> [items1...]
#   [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])
#
# Specifies include directories to use when compiling a given target. The named
# <target> must have been created by a command such as add_executable() or
# add_library() and must not be an ALIAS target.

target_include_directories(student PUBLIC include)


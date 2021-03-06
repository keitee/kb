cmake_minimum_required (VERSION 2.6)
project (code1)

# Full directory of the listfile currently being processed.
#
# As CMake processes the listfiles in your project this variable will always be
# set to the directory where the listfile which is currently being processed
# (CMAKE_CURRENT_LIST_FILE) is located. The value has dynamic scope. When CMake
# starts processing commands in a source file it sets this variable to the
# directory where this file is located. When CMake finishes processing commands
# from the file it restores the previous value. Therefore the value of the
# variable inside a macro or function is the directory of the file invoking the
# bottom-most entry on the call stack, not the directory of the file containing
# the macro or function definition.
#
# CMAKE_CURRENT_LIST_DIR: /home/keitee/git/kb/code-make/cmake/tutorial/code1

message("CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")


# find_program (
#           <VAR>
#           name | NAMES name1 [name2 ...] [NAMES_PER_DIR]
# 
# This command is used to find a program. A cache entry named by <VAR> is
# created to store the result of this command. If the program is found the
# result is stored in the variable and the search will not be repeated unless
# the variable is cleared. If nothing is found, the result will be
# <VAR>-NOTFOUND, and the search will be attempted again the next time
# find_program is invoked with the same variable.
# 
# Options include:
# 
# NAMES
#
# Specify one or more possible names for the program.
# 
# When using this to specify names with and without a version suffix, we
# recommend specifying the unversioned name first so that locally-built packages
# can be found before those provided by distributions.

# MAKE_EXE      : /usr/bin/make
find_program(MAKE_EXE NAMES gmake nmake make)
message("MAKE_EXE      : ${MAKE_EXE}")

message("PROJECT_NAME      : ${PROJECT_NAME}")
message("CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")

message("Regular variable (before): ${abc}")
message("Cache variable (before): ${xyz}")

set(abc "123")
set(xyz "321" CACHE STRING "???")

message("Regular variable (after): ${abc}")
message("Cache variable (after): ${xyz}")

# https://cmake.org/cmake/help/v3.7/manual/cmake.1.html
#
# -D <var>:<type>=<value>, -D <var>=<value>
# Create a cmake cache entry.
# 
# When cmake is first run in an empty build tree, it creates a CMakeCache.txt
# file and populates it with customizable settings for the project. This option
# may be used to specify a setting that takes priority over the project’s
# default value. The option may be repeated for as many cache entries as
# desired.
# 
# If the :<type> portion is given it must be one of the types specified by the
# set() command documentation for its CACHE signature. If the :<type> portion is
# omitted the entry will be created with no type if it does not exist with a
# type already. If a command in the project sets the type to PATH or FILEPATH
# then the <value> will be converted to an absolute path.
# 
# This option may also be given as a single argument: -D<var>:<type>=<value> or
# -D<var>=<value>.

#
# $ cmake .. -DTARGET_REGION=X6
#

if(TARGET_REGION STREQUAL "X1" OR TARGET_REGION STREQUAL "X6")
  message("TARGET_REGION : ${TARGET_REGION} ${CMAKE_CURRENT_LIST_DIR}")
else()
  message("TARGET_REGION is not defined")
endif()

# Add our local cmake directory to search for components
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")

# will find cmake/Findglib-2.0.cmake
find_package(glib-2.0 REQUIRED)

if(NOT DEFINED GLIB_FOUND)
  message("GLIB_FOUND is not defined : ${GLIB_FOUND}")
else()
  message("GLIB_FOUND is defined : ${GLIB_FOUND}")
endif()

# when found, the user uses variable like:
#
# include_directories( ${GLIB_INCLUDE_DIRS} )
# target_link_libraries(
#         ${GST_LIBRARIES}
#         )

# CMAKE_CURRENT_LIST_FILE:/home/keitee/git/kb/code-make/cmake/code1/CMakeLists.txt
# CMAKE_DIR: /home/keitee/git/kb/code-make/cmake/code1
get_filename_component(CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
message("CMAKE_CURRENT_LIST_FILE:" ${CMAKE_CURRENT_LIST_FILE})
message("CMAKE_DIR: ${CMAKE_DIR}")


option(ENABLE_OPTION "test option" ON)
# option(ENABLE_OPTION "test option" OFF)

if (ENABLE_OPTION)
  message("ENABLE_OPTION is ON")
else()
  message("ENABLE_OPTION is OFF")
endif()

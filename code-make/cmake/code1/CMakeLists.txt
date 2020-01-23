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

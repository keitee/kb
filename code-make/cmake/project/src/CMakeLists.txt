# to build a library

# The headers will be, for most intents and purposes, ignored; the only reason
# to list them is to get them to show up in IDEs. Targets show up as folders in
# many IDEs. More about the general build system and targets is available at
# buildsystem.
# 
# add_library(<name> [STATIC | SHARED | MODULE]
#             [EXCLUDE_FROM_ALL]
#             [source1] [source2 ...])


# XXX_SOURCE_DIR comes from cmake cache where XXX is project name

# Optionally glob, but only for CMake 3.12 or later:
# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${ModernCMakeExample_SOURCE_DIR}/include/modern/*.hpp")

# NOTE: even we don't need it
# set(HEADER_LIST "${ModernCMakeExample_SOURCE_DIR}/include/modern/Student.h")

# https://cmake.org/cmake/help/latest/command/add_library.html?highlight=add_library
#
# If no type is given explicitly the type is STATIC or SHARED based on whether
# the current value of the variable BUILD_SHARED_LIBS is ON. 

# Make an automatic library - will be static or dynamic based on user setting
# 
# If no type is given explicitly the type is STATIC or SHARED based on whether
# the current value of the variable BUILD_SHARED_LIBS is ON.
# NOTE: static is made and it seems static is default

add_library(student Student.cxx ${HEADER_LIST})

# if we use this:
#
# target_include_directories(student
#         PUBLIC
#         ${CMAKE_CURRENT_LIST_DIR}
#         )
# 
# then all module who want to use Student.h, have to use
# #include "include/modern/Student.h"

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
# so this is where main CMakeLists.txt sits

# also have to have in CMakeLists.txt:
# include_directories( ${CMAKE_CURRENT_LIST_DIR} )


# we need this directory, and users of our library will need it too.
target_include_directories(student PUBLIC ../include)

# NOTE: not use for now
# # depends on (header only) boost
# target_link_libraries(student PRIVATE Boost::boost)

# needs at least C++11
target_compile_features(student PUBLIC cxx_std_11)

# TODO: for IDE??
# # IDEs should put the headers in a nice place
# source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})

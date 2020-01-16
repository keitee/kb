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

set(HEADER_LIST "${ModernCMakeExample_SOURCE_DIR}/include/modern/Student.h")

# https://cmake.org/cmake/help/latest/command/add_library.html?highlight=add_library
#
# If no type is given explicitly the type is STATIC or SHARED based on whether
# the current value of the variable BUILD_SHARED_LIBS is ON. 

# Make an automatic library - will be static or dynamic based on user setting
# TODO: where??
add_library(student Student.cxx ${HEADER_LIST})

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

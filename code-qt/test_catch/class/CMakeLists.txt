set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# https://cmake.org/cmake/help/latest/command/add_library.html?highlight=add_library
#
# If no type is given explicitly the type is STATIC or SHARED based on whether
# the current value of the variable BUILD_SHARED_LIBS is ON.

# Make an automatic library - will be static or dynamic based on user setting
# TODO: where??
add_library(class class.cpp ${HEADER_LIST})

# we need this directory, and users of our library will need it too.
# target_include_directories(student PUBLIC ../include)

target_link_libraries(class PRIVATE Qt5::Core)

# needs at least C++11
target_compile_features(class PUBLIC cxx_std_11)


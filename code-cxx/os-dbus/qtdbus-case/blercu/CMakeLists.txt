# https://cmake.org/cmake/help/latest/command/add_library.html?highlight=add_library
#
# If no type is given explicitly the type is STATIC or SHARED based on whether
# the current value of the variable BUILD_SHARED_LIBS is ON. 

# Make an automatic library - will be static or dynamic based on user setting
# TODO: where??
add_library(blercu 
  blercucontroller.cpp
  adaptors/blercucontroller1_adaptor.cpp
  ${HEADER_LIST})


# to make it available to all sub projects from main cmake file?
target_include_directories(blercu 
  PUBLIC 
  ../dbus
  ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(blercu PRIVATE dbus Qt5::DBus Qt5::Core)

# needs at least C++11
target_compile_features(blercu PUBLIC cxx_std_11)

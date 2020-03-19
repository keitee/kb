# https://cmake.org/cmake/help/latest/command/add_library.html?highlight=add_library
#
# If no type is given explicitly the type is STATIC or SHARED based on whether
# the current value of the variable BUILD_SHARED_LIBS is ON. 

# Make an automatic library - will be static or dynamic based on user setting
# TODO: where??
add_library(dbus 
  dbusabstractinterface.cpp
  dbusabstractadaptor.cpp
  ${HEADER_LIST})

target_include_directories(dbus 
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(dbus PUBLIC Qt5::DBus Qt5::Core)

# needs at least C++11
target_compile_features(dbus PUBLIC cxx_std_11)

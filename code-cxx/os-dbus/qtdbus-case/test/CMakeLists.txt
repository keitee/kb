find_package(Qt5Core)
if(Qt5Core_FOUND)
  message(STATUS "Qt5Core found")
else()
  message(STATUS "Qt5Core not found, not building moc")
endif()

find_package(Qt5DBus)
if(Qt5DBus_FOUND)
  message(STATUS "Qt5DBus found")
else()
  message(STATUS "Qt5DBus not found, not building moc")
endif()

# find_package(Qt5Test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# However, when move to cmake 3.15.5:
#
# CMake Warning (dev) in /home/keitee/works/googletest/googletest/CMakeLists.txt:
#   AUTOGEN: No valid Qt version found for target gtest_main.  AUTOMOC
#   disabled.  Consider adding:
# 
#     find_package(Qt<QTVERSION> COMPONENTS Core)
# 
#   to your CMakeLists.txt file.
# This warning is for project developers.  Use -Wno-dev to suppress it.

# find_package(Qt5 "5.12.6" COMPONENTS Core)
# find_package(Qt5 COMPONENTS Core)

# set_property(DIRECTORY PROPERTY QT_VERSION_MAJOR ${QT_VERSION_MAJOR})
# set_property(DIRECTORY PROPERTY QT_VERSION_MINOR ${QT_VERSION_MINOR})

set(GMOCK_DIR "$ENV{HOME}/works/googletest/googlemock" CACHE PATH "The path to the GoogleMock")


# Include the directory source_dir in your project. This directory must contain
# a CMakeLists.txt file. Note: We're omitting the optional second parameter.
# This only works with subdirectories of the current directory. We will see how
# to add external directories later.
#
# As checked:
# /home/keitee/works/googletest/googlemock/CMakeLists.txt
#
# add_subdirectory(source_dir [binary_dir])
#
# Add the directory source_dir to the current project with binary_dir as its
# corresponding binary output directory. When adding a directory that is a
# subdirectory of the current directory CMake will automatically determine what
# the binary output directory should be, making the second argument optional.
# However if you add a directory that isn't a subdirectory you need to specify
# the binary output directory. 
#
#
# CMAKE_BINARY_DIR 
#
# This variable holds the path to the top level binary output directory, i.e.
# the directory in which you ran the cmake command

add_subdirectory(${GMOCK_DIR} ${CMAKE_BINARY_DIR}/gmock)


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

include_directories(SYSTEM ${GMOCK_DIR}/include)


# add_executable(test_qt "test_qt.cpp" "qclass.cpp")

# # The recommended way to use Qt libraries and headers with CMake is to use the
# # target_link_libraries command. This command automatically adds appropriate
# # include directories, compile definitions, the position-independent-code flag,
# # and links to the qtmain.lib library on Windows.

# # target_link_libraries(${PROJECT_NAME} gmock_main Qt5::Core Qt5::Test)
# target_link_libraries(test_qt gmock_main Qt5::Core)


# NOTE:
# Qt5::Test is needed for QSignalSpy.h

add_executable(bletest 
  tst_main.cpp
  tst_configsettings.cpp
  mck_blercucontroller.h 
  tst_blercucontrolleradaptor.cpp 
  utl_eventloop.cpp
  utl_dbuspropertychangespy.cpp
  utl_dbussignalspy.cpp 
  )

target_link_libraries(bletest gmock_main blercu dbus configsettings Qt5::Core Qt5::DBus)

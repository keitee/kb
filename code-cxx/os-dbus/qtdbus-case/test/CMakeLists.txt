cmake_minimum_required(VERSION 3.0.0)

project(test VERSION 0.1 LANGUAGES CXX)

# TODO:
# To fix find_package issue:

# CMAKE_PREFIX_PATH=/home/keitee/Qt/5.12.3/gcc_64 cmake ..
# cmake version 3.15.5


# https://doc.qt.io/qt-5/cmake-manual.html

# For find_package to be successful, CMake must find the Qt installation in one
# of the following ways: Set your CMAKE_PREFIX_PATH environment variable to the
# Qt 5 installation prefix. "This is the recommended way."

# The CMAKE_AUTOMOC setting runs moc automatically when required. For more on
# this feature see the CMake AUTOMOC documentation

# use this 
# CMAKE_PREFIX_PATH=/home/keitee/Qt/5.12.3/gcc_64 cmake .. 

# set(CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/5.12.3/gcc_64/lib/cmake/Qt5Core")
#
# keitee@keitee-mint:~/Qt/5.12.3/gcc_64/lib/cmake$ ls
# Qt5             Qt53DQuick           Qt53DRender           Qt5DBus           Qt5Location           Qt5OpenGL            Qt5Qml              Qt5QuickWidgets   Qt5Sensors     Qt5TextToSpeech       Qt5WebChannel        Qt5Widgets
# Qt53DAnimation  Qt53DQuickAnimation  Qt5Bluetooth          Qt5Designer       Qt5Multimedia         Qt5OpenGLExtensions  Qt5Quick            Qt5RemoteObjects  Qt5SerialBus   Qt5UiPlugin           Qt5WebEngine         Qt5X11Extras
# Qt53DCore       Qt53DQuickExtras     Qt5Charts             Qt5Gamepad        Qt5MultimediaWidgets  Qt5Positioning       Qt5QuickCompiler    Qt5RepParser      Qt5SerialPort  Qt5UiTools            Qt5WebEngineCore     Qt5Xml
# Qt53DExtras     Qt53DQuickInput      Qt5Concurrent         Qt5Gui            Qt5Network            Qt5PositioningQuick  Qt5QuickControls2   Qt5Script         Qt5Sql         Qt5VirtualKeyboard    Qt5WebEngineWidgets  Qt5XmlPatterns
# Qt53DInput      Qt53DQuickRender     Qt5Core               Qt5Help           Qt5NetworkAuth        Qt5PrintSupport      Qt5QuickTemplates2  Qt5ScriptTools    Qt5Svg         Qt5WaylandClient      Qt5WebSockets
# Qt53DLogic      Qt53DQuickScene2D    Qt5DataVisualization  Qt5LinguistTools  Qt5Nfc                Qt5Purchasing        Qt5QuickTest        Qt5Scxml          Qt5Test        Qt5WaylandCompositor  Qt5WebView
#
# ~/Qt/5.12.3/gcc_64/lib/cmake$ more Qt5DBus/Qt5DBusConfig.cmake 
# set(Qt5DBus_LIBRARIES Qt5::DBus)


find_package(Qt5Core)
if(Qt5Core_FOUND)
  message(STATUS "Qt5Core found")
else()
  message(STATUS "Qt5Core not found, not building moc")
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

# add_executable(test "tst_main.cpp" 
#   "tst_configsettings.cpp"
#   "tst_blercucontrolleradaptor.cpp"
#   )

add_executable(test "tst_main.cpp" 
  "tst_configsettings.cpp"
  )

# NOTE: don't need to do this since uses "target_include_directories" in
# configsettings.
#
# target_include_directories(test PUBLIC 
#   ../configsettings)
# target_include_directories(test PUBLIC 
#    configsettings)

target_link_libraries(test gmock_main configsettings Qt5::Core)

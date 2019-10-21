cmake_minimum_required(VERSION 3.0.0)

project(qtest VERSION 0.1 LANGUAGES CXX)

enable_testing()

# To fix find_package issue:
#
# https://doc.qt.io/qt-5/cmake-manual.html
#

# The first requirement when using CMake is to use find_package to locate the
# libraries and header files shipped with Qt. These libraries and header files
# can then be used to build libraries and applications based on Qt.

# In order for find_package to be successful, Qt 5 must be found below the
# CMAKE_PREFIX_PATH, or the Qt5<Module>_DIR must be set in the CMake cache to
# the location of the Qt5WidgetsConfig.cmake file. The easiest way to use CMake
# is to set the CMAKE_PREFIX_PATH environment variable to the install prefix of
# Qt 5.
#
# The CMAKE_AUTOMOC setting runs moc automatically when required. For more on
# this feature see the CMake AUTOMOC documentation

# use this 
# CMAKE_PREFIX_PATH=/home/keitee/Qt/5.12.3/gcc_64 cmake .. 

# since it do not work when use Widgets
# set(CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/5.12.3/gcc_64/lib/cmake/Qt5Core")
#
# keitee@keitee-mint:~/Qt/5.12.3/gcc_64/lib/cmake$ ls
# Qt5             Qt53DQuick           Qt53DRender           Qt5DBus           Qt5Location           Qt5OpenGL            Qt5Qml              Qt5QuickWidgets   Qt5Sensors     Qt5TextToSpeech       Qt5WebChannel        Qt5Widgets
# Qt53DAnimation  Qt53DQuickAnimation  Qt5Bluetooth          Qt5Designer       Qt5Multimedia         Qt5OpenGLExtensions  Qt5Quick            Qt5RemoteObjects  Qt5SerialBus   Qt5UiPlugin           Qt5WebEngine         Qt5X11Extras
# Qt53DCore       Qt53DQuickExtras     Qt5Charts             Qt5Gamepad        Qt5MultimediaWidgets  Qt5Positioning       Qt5QuickCompiler    Qt5RepParser      Qt5SerialPort  Qt5UiTools            Qt5WebEngineCore     Qt5Xml
# Qt53DExtras     Qt53DQuickInput      Qt5Concurrent         Qt5Gui            Qt5Network            Qt5PositioningQuick  Qt5QuickControls2   Qt5Script         Qt5Sql         Qt5VirtualKeyboard    Qt5WebEngineWidgets  Qt5XmlPatterns
# Qt53DInput      Qt53DQuickRender     Qt5Core               Qt5Help           Qt5NetworkAuth        Qt5PrintSupport      Qt5QuickTemplates2  Qt5ScriptTools    Qt5Svg         Qt5WaylandClient      Qt5WebSockets
# Qt53DLogic      Qt53DQuickScene2D    Qt5DataVisualization  Qt5LinguistTools  Qt5Nfc                Qt5Purchasing        Qt5QuickTest        Qt5Scxml          Qt5Test        Qt5WaylandCompositor  Qt5WebView


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

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

# find_package(Qt5Core)
# find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5Core)
find_package(Qt5Test)

add_executable(${PROJECT_NAME} "qtest.cpp" "qclass.cpp")

# The recommended way to use Qt libraries and headers with CMake is to use the
# target_link_libraries command. This command automatically adds appropriate
# include directories, compile definitions, the position-independent-code flag,
# and links to the qtmain.lib library on Windows.

target_link_libraries(${PROJECT_NAME} gmock_main Qt5::Core Qt5::Test)

add_executable(qstandalone "qstandalone.cpp" "qclass.cpp")
target_link_libraries(qstandalone Qt5::Core Qt5::Test)


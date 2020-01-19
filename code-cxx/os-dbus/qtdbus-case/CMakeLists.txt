cmake_minimum_required(VERSION 3.0.0)

project(qtdbus-case VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
# to autogen ui form files
set(CMAKE_AUTOUIC ON)

# https://doc.qt.io/qt-5/cmake-manual.html
#
# have to say where Qt is by either:
#
# CMAKE_PREFIX_PATH=/home/keitee/Qt/5.12.3/gcc_64 cmake ..
#
# or 
#
# set(CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/5.12.3/gcc_64/lib/cmake/Qt5Core")
#
# Find the QtWidgets library

find_package(Qt5DBus)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# NOTE: CONSIDERATION
#
# take an approach where each sub dicrectory has headers and sources and each
# sub directory will makes static library.
#
# the other approach is to have common directory which has all headers which are
# public to users but has each directory for module sources or single source
# directory which has all sources.
#
# the other approach is to add all sources to the main executable rather than
# make each module a library.

add_subdirectory(cmdlineoptions)
add_subdirectory(configsettings)

add_executable(qtdbus-case main.cpp)

# TODO: have to include each library??
target_include_directories(qtdbus-case PUBLIC cmdlineoptions configsettings)

target_link_libraries(qtdbus-case 
  cmdlineoptions 
  configsettings
  Qt5::DBus Qt5::Core)

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
add_subdirectory(dbus)
add_subdirectory(blercu)

add_executable(qtdbus-case main.cpp blercu)

# NOTE: qt-moc
#
# add_executable(qtdbus-case main.cpp blercu)
# this causes an error in main when use:
# QSharedPointer<BleRcuControllerImpl>::create(config, nullptr);
#
# blercu/libblercu.a(blercucontroller.cpp.o): In function `BleRcuControllerImpl::BleRcuControllerImpl(QSharedPointer<ConfigSettings> const&, QObject*)':
# blercucontroller.cpp:(.text+0x77): undefined reference to `vtable for BleRcuControllerImpl'
# blercu/libblercu.a(blercucontroller.cpp.o): In function `BleRcuControllerImpl::~BleRcuControllerImpl()':
# blercucontroller.cpp:(.text+0x1c5): undefined reference to `vtable for BleRcuControllerImpl'
#
# https://github.com/bincrafters/community/issues/466
# solves it:
# You are welcome. This problem is basically not documented. Most user don't
# have it because the header files are automatically processed by moc if they
# are in the same folder as the associated cpp (and have the same name), as
# explained here
# https://cmake.org/cmake/help/latest/prop_tgt/AUTOMOC.html
#
# add_executable(qtdbus-case main.cpp blercu/blercucontroller_impl.h)
#
# NOTE: the cause is that Qt scans *_p.h automatically so
# `blercucontroller_impl.h` is not not found so have to add it manully. rename
# it to `blercucontroller_p.h` and this works.
#
# https://cmake.org/cmake/help/latest/prop_tgt/AUTOMOC.html
# Header file processing
# 
# At configuration time, a list of header files that should be scanned by
# AUTOMOC is computed from the target’s sources.
# 
# All header files in the target’s sources are added to the scan list.
# 
# For all C++ source files <source_base>.<source_extension> in the target’s
# sources, CMake searches for
# 
# a regular header with the same base name (<source_base>.<header_extention>)
# and
# 
# a private header with the same base name and a _p suffix
# (<source_base>_p.<header_extention>)
# 
# and adds these to the scan list.


# include_directories(SYSTEM dbus)

# TODO: have to include each library??
target_include_directories(qtdbus-case PUBLIC 
  cmdlineoptions 
  configsettings
  dbus
  blercu)

# NOTE: using `Qt5::DBus` makes its headers available
target_link_libraries(qtdbus-case 
  cmdlineoptions 
  configsettings
  dbus
  blercu
  Qt5::DBus Qt5::Core)

# Add test
# if use enable_testing(), "test" target name is reserverd so changed test
# target to "bletest". what other purpose?
enable_testing()
add_subdirectory(test)

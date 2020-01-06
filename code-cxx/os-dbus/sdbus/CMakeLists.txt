cmake_minimum_required(VERSION 3.0.0)

project(sdbus VERSION 0.1 LANGUAGES CXX)
# project(sdbus VERSION 0.1)

enable_testing()

# https://stackoverflow.com/questions/690223/how-to-retrieve-a-user-environment-variable-in-cmake-windows
# Getting variables into your CMake script
# You can pass a variable on the line with the cmake invocation:
#
# FOO=1 cmake
# or by exporting a variable in BASH:
#
# export FOO=1
# Then you can pick it up in a cmake script using:
#
# $ENV{FOO}

set(GMOCK_DIR "$ENV{HOME}/works/googletest/googlemock" CACHE PATH "The path to the GoogleMock")


# Include the directory source_dir in your project. This directory must contain
# a CMakeLists.txt file. Note: We're omitting the optional second parameter.
# This only works with subdirectories of the current directory. We will see how
# to add external directories later.
#
# As checked: /home/keitee/works/googletest/googlemock/CMakeLists.txt
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
# AFTER|BEFORE Specify whether or not these include directories should be
# appended or prepended to the list of include directories. If omitted then the
# default behavior is used. By default directories are appended to the list.
# This behavior can be changed by setting CMAKE_INCLUDE_DIRECTORIES_BEFORE to
# TRUE. 
#
# SYSTEM Specify that these directories are system include directories. This
# only has an affect on compilers that support the distinction. This can change
# the order in which the compiler searches include directories or the handling
# of warnings from headers found in these directories. 
#
# directory...  The directories to be added to the list of include directories.

include_directories(SYSTEM ${GMOCK_DIR}/include)


# do not work
# add_compile_options("$<$<CONFIG:DEBUG>:-W;-Wall;-O0;-g;-pedantic>")
add_compile_options("-g;-D_GLIBCXX_DEBUG;-lsystemd")


# cmake help
#
# cmake --help-command target_compile_options
#
# target_compile_options
# ----------------------
# 
# Add compile options to a target.
# 
# ::
# 
#  target_compile_options(<target> [BEFORE]
#    <INTERFACE|PUBLIC|PRIVATE> [items1...]
#    [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])
# 
# Specify compile options to use when compiling a given target.  The
# named ``<target>`` must have been created by a command such as
# ``add_executable()`` or ``add_library()`` and must not be an
# :ref:`IMPORTED Target <Imported Targets>`.  If ``BEFORE`` is specified,
# the content will be prepended to the property instead of being appended.
# 
# This command can be used to add any options, but
# alternative commands exist to add preprocessor definitions
# (``target_compile_definitions()`` and ``add_definitions()``) or
# include directories (``target_include_directories()`` and
# ``include_directories()``).  See documentation of the
# ``directory`` and
# ``target`` ``COMPILE_OPTIONS`` properties.
# 
# The ``INTERFACE``, ``PUBLIC`` and ``PRIVATE`` keywords are required to
# specify the scope of the following arguments.  ``PRIVATE`` and ``PUBLIC``
# items will populate the ``COMPILE_OPTIONS`` property of
# ``<target>``.  ``PUBLIC`` and ``INTERFACE`` items will populate the
# ``INTERFACE_COMPILE_OPTIONS`` property of ``<target>``.  The
# following arguments specify compile options.  Repeated calls for the same
# ``<target>`` append items in the order called.
# 
# Arguments to ``target_compile_options`` may use "generator expressions"
# with the syntax ``$<...>``. See the ``cmake-generator-expressions(7)``
# manual for available expressions.  See the ``cmake-buildsystem(7)``
# manual for more on defining buildsystem properties.


# https://stackoverflow.com/questions/6646405/how-do-you-add-boost-libraries-in-cmakelists-txt
#
# For this warning:
#
# CMake Warning at /usr/share/cmake-3.7/Modules/FindBoost.cmake:761 (message):
#  Imported targets not available for Boost version 106501
#
# see https://stackoverflow.com/questions/42123509/cmake-finds-boost-but-the-imported-targets-not-available-for-boost-version
#
# 1.65.1 emits some error when linking with cmake and 1.70.0 fixes up.
# find_package(Boost 1.65.1 COMPONENTS system thread)
#
# -- Found Boost 1.70.0 at /usr/local/lib/cmake/Boost-1.70.0
# --   Requested configuration: QUIET COMPONENTS system;thread;regex
# -- Found boost_headers 1.70.0 at /usr/local/lib/cmake/boost_headers-1.70.0
# -- Found boost_system 1.70.0 at /usr/local/lib/cmake/boost_system-1.70.0
# --   libboost_system.a
# -- Adding boost_system dependencies: headers
# -- Found boost_thread 1.70.0 at /usr/local/lib/cmake/boost_thread-1.70.0
# --   libboost_thread.a
# -- Adding boost_thread dependencies: headers
# -- Found boost_regex 1.70.0 at /usr/local/lib/cmake/boost_regex-1.70.0
# --   libboost_regex.a
# -- Adding boost_regex dependencies: headers
# -- Boost 1.70.0 found.
# -- Found Boost components:
#    system;thread;regex
#
# /usr/local/lib/cmake/boost_regex-1.70.0/boost_regex-config.cmake
# # Create imported target Boost::regex
# add_library(Boost::regex UNKNOWN IMPORTED)


find_package(Boost 1.70.0 COMPONENTS system thread)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
include_directories(${Boost_INCLUDE_DIRS}) 


# to find library name:
#
# ~/Qt/5.12.3/gcc_64/lib/cmake/Qt5Core/Qt5CoreConfig.cmake
# # For backwards compatibility only. Use Qt5Core_VERSION instead.
# set(Qt5Core_VERSION_STRING 5.12.3)
# 
# set(Qt5Core_LIBRARIES Qt5::Core)

# used in library name:
# target_link_libraries(${PROJECT_NAME} gmock_main Qt5::Core Qt5::Widgets)


# to use libs static library
#
# Add directories in which the linker will look for libraries.
#
# link_directories([AFTER|BEFORE] directory1 [directory2 ...])
#
# Adds the paths in which the linker should search for libraries. Relative paths
# given to this command are interpreted as relative to the current source
# directory, see CMP0015.

# set(LIBS_DIR "../libs")
set(LIBS_DIR "../../libs")
set(LIBS libcxx.a)
link_directories(${LIBS_DIR}/build)
include_directories(${LIBS_DIR})

# sdbus_client
add_executable(sdbus_client_1 "sdbus_client_1.cpp")
target_link_libraries(sdbus_client_1 ${LIBS} "-lsystemd")


# NOTE: fails to run cmake for c file
#
# CMake Error: Error required internal CMake variable not set, cmake may not be built correctly.
# Missing variable is:
# CMAKE_C_COMPILE_OBJECT
# CMake Error: Error required internal CMake variable not set, cmake may not be built correctly.
# Missing variable is:
# CMAKE_C_LINK_EXECUTABLE
#
# the solution is:
# https://stackoverflow.com/questions/5042932/cmake-missing-environment-variables-errors
# The 2nd and later optional args to the PROJECT command should be known CMake language values.
# 
# It's typical values are:
# 
# leave it off, don't provide a 2nd arg (default to C and CXX)
#
# https://cmake.org/cmake/help/latest/command/project.html
# 
# LANGUAGES <language-name>...
# 
# Optional. Can also be specified without LANGUAGES keyword per the first, short
# signature.
#
# By default C and CXX are enabled if no language options are given. Specify
# language NONE, or use the LANGUAGES keyword and list no languages, to skip
# enabling any languages.


# sdbus_server_1
# add_executable(sdbus_server_1 "sdbus_server_1.c")

# Ok on gcc (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0
add_executable(sdbus_server_1 "sdbus_server_1.cpp")
target_link_libraries(sdbus_server_1 ${LIBS} "-lsystemd")

add_test(${PROJECT_NAME} ${PROJECT_NAME})


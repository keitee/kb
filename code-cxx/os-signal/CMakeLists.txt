cmake_minimum_required(VERSION 3.0.0)

project(os_signal VERSION 0.1 LANGUAGES CXX)

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

# ok, add_compile_options("-g;-D_GLIBCXX_DEBUG;-lsystemd")
add_compile_options(-g -D_GLIBCXX_DEBUG -lsystemd)

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
set(LIBS_DIR "../libs")
set(LIBS libcxx.a)
link_directories(${LIBS_DIR}/build)
include_directories(${LIBS_DIR})

# test_os
# add_executable(${PROJECT_NAME} "test_os.cpp")
# cause link errors to Boost::thread
# target_link_libraries(${PROJECT_NAME} gmock_main ${Boost_LIBRARIES})


# lpi-signal-1
add_executable(lpi_signal_1 "lpi_signal_1.cpp")
target_link_libraries(lpi_signal_1 ${LIBS})

# lpi-signal-2
add_executable(lpi_signal_2 "lpi_signal_2.cpp")
target_link_libraries(lpi_signal_2 ${LIBS})

# lpi-signal-3
add_executable(lpi_signal_3 "lpi_signal_3.cpp")
target_link_libraries(lpi_signal_3 ${LIBS})

# lpi-signal-sender and receiver
add_executable(lpi_signal_sender "lpi_signal_sender.cpp")
target_link_libraries(lpi_signal_sender ${LIBS})

add_executable(lpi_signal_receiver "lpi_signal_receiver.cpp")
target_link_libraries(lpi_signal_receiver ${LIBS})

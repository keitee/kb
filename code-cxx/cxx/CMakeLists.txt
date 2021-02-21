cmake_minimum_required(VERSION 3.0.0)

project(test_cxx VERSION 0.1 LANGUAGES CXX)

# enable_testing()

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

# add_compile_options("-g;-D_GLIBCXX_DEBUG")
# add_compile_options(-g -Wall -Werror)
add_compile_options(-g -Wall)

# Supported values are 98, 11 and 14.
# set( CMAKE_CXX_STANDARD 14 )
add_compile_options("-std=c++17")

# set( CMAKE_CXX_STANDARD_REQUIRED ON )

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


#find_package(Boost 1.70.0 COMPONENTS system thread)
find_package(Boost 1.70.0 COMPONENTS system)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
include_directories(${Boost_INCLUDE_DIRS}) 


# cmake -DCXX_CPP_VALUE=GCC ..
# to use on #if/ifdef test

# add_definitions(-DGCC=1 -DCLANG=2)

# if (CXX_CPP_VALUE STREQUAL "GCC")
#   add_definitions(-DCXX_CPP_VALUE=GCC)
# elseif(CXX_CPP_VALUE STREQUAL "GLANG")
#   add_definitions(-DCXX_CPP_VALUE=CLANG)
# else()
#   message(WARNING "CXX_CPP_VALUE is not provided." )
# endif()

# add_definitions(-DARM=3 -DMIPS=4)

# if (CXX_CPP_VALUE2 STREQUAL "ARM")
#   add_definitions(-DCXX_CPP_VALUE2=ARM)
# elseif(CXX_CPP_VALUE2 STREQUAL "MIPS")
#   add_definitions(-DCXX_CPP_VALUE2=MIPS)
# else()
#   message(WARNING "CXX_CPP_VALUE2 is not provided." )
# endif()


# test_cxx_1.cpp
add_executable(test_cxx_1 "test_cxx_1.cpp")
target_link_libraries(test_cxx_1 gmock_main)

# test_cxx_2.cpp
add_executable(test_cxx_2 "test_cxx_2.cpp")
target_link_libraries(test_cxx_2 gmock_main)

# _pad.cpp
add_executable(${PROJECT_NAME}_pad "test_cxx_pad.cpp")
target_link_libraries(${PROJECT_NAME}_pad gmock_main)

# _pattern.cpp
add_executable(${PROJECT_NAME}_pattern "test_cxx_pattern.cpp")
#target_link_libraries(${PROJECT_NAME}_pattern gmock_main Boost::thread)
target_link_libraries(${PROJECT_NAME}_pattern gmock_main)

# _case.cpp
add_executable(${PROJECT_NAME}_case "test_cxx_case.cpp")
# target_link_libraries(${PROJECT_NAME}_case gmock_main Boost::thread)
target_link_libraries(${PROJECT_NAME}_case gmock_main)

# _boost.cpp
add_executable(test_boost "test_boost.cpp")
# target_link_libraries(test_boost gmock_main Boost::thread)
target_link_libraries(test_boost gmock_main)


# test_gcc.cpp
add_executable(test_gcc "test_gcc.cpp")
target_link_libraries(test_gcc gmock_main)

# test_cpp.cpp
add_executable(test_cpp "test_cpp.cpp")
target_link_libraries(test_cpp gmock_main)

# test bridge pattern.
# bridge client. not needed bridge_p.h since there is no subclassing on impl.
add_executable(bridge_client1 "bridge_client1.cpp" "bridge.cpp")
add_executable(bridge_client2 "bridge_client2.cpp" "bridge.cpp")

# bridge client when has subclassing on impl
# bridge_abs.cpp which has WindowImpl and Window code
# bridge_p.h which has WindowImpl declaration
# briage_con.h bridge_con.cpp
# NOTE: so need to have bridge_p.h for subclassing.

add_executable(bridge_client3 "bridge_client1.cpp" 
  "bridge_abs.cpp"
  "bridge_con.cpp")
add_executable(bridge_client4 "bridge_client2.cpp" 
  "bridge_abs.cpp"
  "bridge_con.cpp")

# aout
add_executable(aout "aout.cpp") 

# test_core_guideline.cpp
add_executable(test_core "test_core_guideline.cpp")

# note: must use "PUBLIC" to have the expected result. why?
target_compile_options(test_core PUBLIC -g 
  -Wswitch-default 
#  -Wimplicit-fallthrough
  -Wextra
)

target_link_libraries(test_core gmock_main)

# add_test(Gtest.MatcherForPair ${PROJECT_NAME})
add_test(${PROJECT_NAME} ${PROJECT_NAME})


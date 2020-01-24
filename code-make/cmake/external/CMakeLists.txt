cmake_minimum_required(VERSION 3.0.0)

project(external VERSION 0.1 LANGUAGES CXX)

## https://cmake.org/cmake/help/v3.11/module/FetchContent.html
##
## This module enables populating content `at configure time` via any method
## supported by the ExternalProject module. Whereas ExternalProject_Add()
## downloads `at build time`, the FetchContent module makes content available
## immediately, allowing the configure step to use the content in commands like
## add_subdirectory(), include() or file() operations.
#
## FetchContent added in CMake 3.11, downloads during the configure step
#include(FetchContent)
## FetchContent_MakeAvailable was not added until CMake 3.14
#if(${CMAKE_VERSION} VERSION_LESS 3.14)
#  message(STATUS "cmake is less than 3.14 and add FetchContent")
#  include(cmake/add_FetchContent_MakeAvailable.cmake)
#endif()
#
#FetchContent_Declare(
#catch2
#GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#GIT_TAG        v2.9.1
#)
#FetchContent_MakeAvailable(catch2)
## Adds Catch2::Catch2


# https://cmake.org/cmake/help/latest/module/ExternalProject.html
# 
# ExternalProject_Add
# 
# The ExternalProject_Add() function creates a custom target to drive download,
# update/patch, configure, build, install and test steps of an external project:
#
# Download Step Options:
#
# NOTE: A download method can be omitted if the SOURCE_DIR option is used to
# point to an existing non-empty directory. 
#
# Otherwise, one of the download methods below must be specified (multiple
# download methods should not be given) or a custom DOWNLOAD_COMMAND provided.
#
# DOWNLOAD_COMMAND <cmd>...
#
# Overrides the command used for the download step (generator expressions are
# supported). If this option is specified, all other download options will be
# ignored. 
#
# NOTE: Providing an empty string for <cmd> effectively disables the download
# step.
#
# URL Download
#
# URL <url1> [<url2>...]
#
# NOTE: List of paths and/or URL(s) of the external project’s source. When more
# than one URL is given, they are tried in turn until one succeeds. 
#
# A URL may be an ordinary path in the `local file system` (in which case it
# must be the only URL provided) or any downloadable URL supported by the
# file(DOWNLOAD) command. A local filesystem path may refer to either an
# existing directory or to an archive file, whereas a URL is expected to point
# to a file which can be treated as an archive. When an archive is used, it will
# be unpacked automatically unless the DOWNLOAD_NO_EXTRACT option is set to
# prevent it. The archive type is determined by inspecting the actual content
# rather than using logic based on the file extension.
# 
# URL_HASH <algo>=<hashValue> 
#
# Hash of the archive file to be downloaded. The argument should be of the form
# <algo>=<hashValue> where algo can be any of the hashing algorithms supported
# by the file() command. Specifying this option is strongly recommended for URL
# downloads, as it ensures the integrity of the downloaded content. It is also
# used as a check for a previously downloaded file, allowing connection to the
# remote location to be avoided altogether if the local directory already has a
# file from an earlier download that matches the specified hash.

# Examples
#
# The following example shows how to download and build a hypothetical project
# called FooBar from github:
#
# include(ExternalProject)
# ExternalProject_Add(foobar
#   GIT_REPOSITORY    git@github.com:FooCo/FooBar.git
#   GIT_TAG           origin/release/1.2.3
# )
#
#
# For the sake of the example, also define a second hypothetical external
# project called SecretSauce, which is downloaded from a web server. Two URLs
# are given to take advantage of a faster internal network if available, with a
# "fallback" to a slower external server. 
#
# The project is a typical Makefile project with no configure step, so some of
# the default commands are overridden. The build is only required to build the
# sauce target:
#
# find_program(MAKE_EXE NAMES gmake nmake make)
#
# ExternalProject_Add(secretsauce
#   URL               http://intranet.somecompany.com/artifacts/sauce-2.7.tgz
#                     https://www.somecompany.com/downloads/sauce-2.7.zip
#   URL_HASH          MD5=d41d8cd98f00b204e9800998ecf8427e
#   CONFIGURE_COMMAND ""
#   BUILD_COMMAND     ${MAKE_EXE} sauce
# )
#
#
# Suppose the build step of secretsauce requires that foobar must already be
# built. This could be enforced like so:
# 
# ExternalProject_Add_StepDependencies(secretsauce build foobar)
#
# Another alternative would be to create a custom target for foobar’s build step
# and make secretsauce depend on that rather than the whole foobar project. This
# would mean foobar only needs to be built, it doesn’t need to run its install
# or test steps before secretsauce can be built. The dependency can also be
# defined along with the secretsauce project:
# 
# ExternalProject_Add_StepTargets(foobar build)
#
# ExternalProject_Add(secretsauce
#   URL               http://intranet.somecompany.com/artifacts/sauce-2.7.tgz
#                     https://www.somecompany.com/downloads/sauce-2.7.zip
#   URL_HASH          MD5=d41d8cd98f00b204e9800998ecf8427e
#   CONFIGURE_COMMAND ""
#   BUILD_COMMAND     ${MAKE_EXE} sauce
#   DEPENDS           foobar-build
# )


# Instead of calling ExternalProject_Add_StepTargets(), the target could be
# defined along with the foobar project itself:
#
# ExternalProject_Add(foobar
#   GIT_REPOSITORY git@github.com:FooCo/FooBar.git
#   GIT_TAG        origin/release/1.2.3
#   STEP_TARGETS   build
# )


# If many external projects should have the same set of step targets, setting a
# directory property may be more convenient. The build step target could be
# created automatically by setting the EP_STEP_TARGETS directory property before
# creating the external projects with ExternalProject_Add():
#
# set_property(DIRECTORY PROPERTY EP_STEP_TARGETS build)

# Lastly, suppose that secretsauce provides a script called makedoc which can be
# used to generate its own documentation. Further suppose that the script
# expects the output directory to be provided as the only parameter and that it
# should be run from the secretsauce source directory. A custom step and a
# custom target to trigger the script can be defined like so:
#
# ExternalProject_Add_Step(secretsauce docs
#   COMMAND           <SOURCE_DIR>/makedoc <BINARY_DIR>
#   WORKING_DIRECTORY <SOURCE_DIR>
#   COMMENT           "Building secretsauce docs"
#   ALWAYS            TRUE
#   EXCLUDE_FROM_MAIN TRUE
# )
#
# ExternalProject_Add_StepTargets(secretsauce docs)
#
# The custom step could then be triggered from the main build like so:
# 
# cmake --build . --target secretsauce-docs

include(ExternalProject)

ExternalProject_Add(
catch2
GIT_REPOSITORY https://github.com/catchorg/Catch2.git
GIT_TAG        v2.9.1
)

# Tests need to be added as executables first
add_executable(${PROJECT_NAME} external.cpp)

# I'm using C++17 in the test
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# add_test(NAME testlibtest COMMAND testlib) # Command can be a target

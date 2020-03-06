# - Try to find the glib 2.0 library.
#
# The following are set after configuration is done:
#  GLIB_FOUND
#  GLIB_INCLUDE_DIRS
#  GLIB_LIBRARY_DIRS
#  GLIB_LIBRARIES

# https://cmake.org/cmake/help/v3.17/command/find_path.html?highlight=find_path
#
# find_path (<VAR> name1 [path1 path2 ...])
#
# This command is used to find a directory containing the named file. A cache
# entry named by <VAR> is created to store the result of this command. If the
# file in a directory is found the result is stored in the variable and the
# search will not be repeated unless the variable is cleared. If nothing is
# found, the result will be <VAR>-NOTFOUND, and the search will be attempted
# again the next time find_path is invoked with the same variable.
#
# PATH_SUFFIXES
# Specify additional subdirectories to check below each directory location
# otherwise considered.
#
# If NO_DEFAULT_PATH is specified, then no additional paths are added to the
# search. If NO_DEFAULT_PATH is not specified, the search process is as follows:
# 
# find_path(GLIB_INCLUDE_DIR NAMES glib.h PATH_SUFFIXES glib-2.0)
# 
# if(NOT DEFINED GLIB_INCLUDE_DIR)
#   message("GLIB_INCLUDE_DIR is not defined : ${GLIB_INCLUDE_DIR}")
# else()
#   message("GLIB_INCLUDE_DIR is defined : ${GLIB_INCLUDE_DIR}")
# endif()

find_path( GLIB_INCLUDE_DIR NAMES glib.h PATH_SUFFIXES glib-2.0 )

# NOTE: changed it to make it on Ubuntu PC distro
# find_path( GLIB_CFG_INCLUDE_DIR NAMES glibconfig.h PATH_SUFFIXES lib/glib-2.0/include )
find_path( GLIB_CFG_INCLUDE_DIR NAMES glibconfig.h PATH_SUFFIXES lib/x86_64-linux-gnu/glib-2.0/include )

find_library( GLIB_LIBRARY NAMES libglib-2.0.so.0 glib-2.0 )

# https://cmake.org/cmake/help/v3.17/module/FindPkgConfig.html?highlight=pkg_check_modules
# pkg_check_modules
# Checks for all the given modules, setting a variety of result variables in the
# calling scope.

# message( "GLIB_INCLUDE_DIR include dir = ${GLIB_INCLUDE_DIR}" )
# message( "GLIB_CFG_INCLUDE_DIR include dir = ${GLIB_CFG_INCLUDE_DIR}" )
# message( "GLIB_LIBRARY lib = ${GLIB_LIBRARY}" )

set( GLIB_LIBRARIES ${GLIB_LIBRARY} )
set( GLIB_INCLUDE_DIRS ${GLIB_INCLUDE_DIR} ${GLIB_CFG_INCLUDE_DIR} )

# https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.html?highlight=findpackagehandlestandardargs
# This module provides a function intended to be used in Find Modules
# implementing find_package(<PackageName>) calls. It handles the REQUIRED, QUIET
# and version-related arguments of find_package. It also sets the
# <PackageName>_FOUND variable. The package is considered found if all variables
# listed contain valid results, e.g. valid filepaths.

# The <PackageName>_FOUND variable will be set to TRUE if all the variables
# <required-var>... are valid and any optional constraints are satisfied, and
# FALSE otherwise. A success or failure message may be displayed based on the
# results and on whether the REQUIRED and/or QUIET option was given to the
# find_package() call.

# NOTE that the users uses
#
# find_package(glib-2.0 REQUIRED)
#
# find_package(RapidJSON ${MIN_RapidJSON_VERSION} REQUIRED MODULE)
# 
# In Module mode, CMake searches for a file called `Find<PackageName>.cmake`. The
# file is first searched in the `CMAKE_MODULE_PATH`, then among the Find Modules
# provided by the CMake installation. If the file is found, it is read and
# processed by CMake. It is responsible for finding the package, checking the
# version, and producing any needed messages. Some find-modules provide limited
# or no support for versioning; check the module documentation.
# 
# but variable is not glib-2.0_FOUND but GLIB_FOUND since real `package name` is
# in this find_package_handle_standard_args()

include(FindPackageHandleStandardArgs)

# Handle the QUIETLY and REQUIRED arguments and set the LM_SENSORS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GLIB DEFAULT_MSG
        GLIB_LIBRARY GLIB_INCLUDE_DIR GLIB_CFG_INCLUDE_DIR)

# https://cmake.org/cmake/help/latest/command/mark_as_advanced.html?highlight=mark_as_advanced
# Mark cmake cached variables as advanced.
mark_as_advanced( GLIB_INCLUDE_DIR GLIB_CFG_INCLUDE_DIR GLIB_LIBRARY )

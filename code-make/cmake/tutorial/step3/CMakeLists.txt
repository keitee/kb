cmake_minimum_required (VERSION 2.6)
project(directory_test)

# added
# to set the build type to be a release build
set(CMAKE_BUILD_TYPE Release)

# bring the headers into the project
include_directories(include)

# can manually add the sources using the set command as:
# set(SOURCES src/main.cxx src/Student.cxx)
# However, the file(GLOB...) allows wildcard additions:

# *cmake-error* when typo cpp which should be cxx
# file(GLOB SOURCES "src/*.cpp")
#
# CMake Error at CMakeLists.txt:12 (add_executable):
#   No SOURCES given to target: student

file(GLOB SOURCES "src/*.cxx")

# add_executable(student ${SOURCES})
# generate the shared library from the sources
add_library(student SHARED ${SOURCES})

# Set the location for library installation -- i.e., /usr/lib in this case
# not really necessary in this example. Use "sudo make install" to apply

# The last line uses the install() function to define an installation location
# for the library (in this case it is /usr/lib). Deployment is invoked using a
# call to sudo make install in this case.

install(TARGETS student DESTINATION /usr/lib)

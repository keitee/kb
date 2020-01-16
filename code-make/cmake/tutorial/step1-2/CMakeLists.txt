cmake_minimum_required (VERSION 3.10)
project (Tutorial)

# builds fine without these
#
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(tutorial tutorial.cxx)

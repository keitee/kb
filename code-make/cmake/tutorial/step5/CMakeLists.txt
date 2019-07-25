# Example 5: Using a Shared or Static Library

cmake_minimum_required (VERSION 2.6)
project(use_shared_library)

# for shared library
set(PROJECT_LINK_LIBS libstudent.so)
link_directories(~/git/kb/code-make/cmake/tutorial/step3/build)

# bring the headers into the project
include_directories(~/git/kb/code-make/cmake/tutorial/step3/include)

# make "student" executable
ADD_executable(libtest main.cxx)

target_link_libraries(libtest ${PROJECT_LINK_LIBS})

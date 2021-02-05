cmake_minimum_required(VERSION 3.0.0)

project(test_thread VERSION 0.1 LANGUAGES C CXX)

# add_compile_options("-Wall;-g;-D_GLIBCXX_DEBUG")
add_compile_options("-Wall;-ggdb")

# to use libs static library
#
# Add directories in which the linker will look for libraries.
#
# link_directories([AFTER|BEFORE] directory1 [directory2 ...])
#
# Adds the paths in which the linker should search for libraries. Relative paths
# given to this command are interpreted as relative to the current source
# directory, see CMP0015.
set(LIBS_DIR "../../libs")
set(LIBS libcxx.a)
link_directories(${LIBS_DIR}/build)
include_directories(${LIBS_DIR})


# test_thread1.cpp
# use Threads::Threads to fix pthread link error
# https://cmake.org/cmake/help/v3.1/module/FindThreads.html
add_executable(test_thread_1 "test_thread_1.cpp")
target_link_libraries(test_thread_1 Threads::Threads ${LIBS})

add_executable(test_thread_2 "test_thread_2.cpp")
target_link_libraries(test_thread_2 Threads::Threads ${LIBS})

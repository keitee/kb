cmake_minimum_required(VERSION 3.0.0)

project(test_pthread VERSION 0.1 LANGUAGES C CXX)

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

# Thread is just the generic lib link for the pthread libraries (on platforms
# that don't have a separate pthread library this is a NOP)
find_package( Threads REQUIRED )

add_executable(test_pthread "test_pthread.cpp")
target_link_libraries(test_pthread ${LIBS} Threads::Threads)

# TODO: have no main or tests
# add_executable(test_thread_specific_data "test_thread_specific_data.cpp")
# target_link_libraries(test_thread_specific_data ${LIBS} Threads::Threads)

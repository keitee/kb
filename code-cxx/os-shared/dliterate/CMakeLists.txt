cmake_minimum_required(VERSION 3.0.0)

project(test_dliterate VERSION 0.1 LANGUAGES C)

# do not work
# add_compile_options("$<$<CONFIG:DEBUG>:-W;-Wall;-O0;-g;-pedantic>")
add_compile_options("-g;-D_GLIBCXX_DEBUG;-ldl")

# to use static library
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


add_executable(${PROJECT_NAME} "ml_main.c" driver.c)

# https://cmake.org/cmake/help/latest/variable/CMAKE_DL_LIBS.html
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

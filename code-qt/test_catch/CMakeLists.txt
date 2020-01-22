cmake_minimum_required(VERSION 3.0.0)

project(test_catch VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# add_compile_options("$<$<CONFIG:DEBUG>:-W;-Wall;-O0;-g;-pedantic>")
add_compile_options("-g;-D_GLIBCXX_DEBUG")

find_package(Qt5 COMPONENTS Core)

if(Qt5Core_FOUND)
  message(STATUS "Qt5Core found")
else()
  message(STATUS "Qt5Core not found, not building moc")
endif()

# NOTE:
# this should be after find_package(Qt5) since `class` uses Qt but no mention of
# it in its CMakeLists.txt. if not, cmake emit errors with class/CMakeLists.txt
# saying not found Qt.
add_subdirectory(class)

add_executable(${PROJECT_NAME} "test_qt.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC class)

target_link_libraries(${PROJECT_NAME} Qt5::Core class)

cmake_minimum_required(VERSION 3.0.0)

project(ctut-06 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# https://doc.qt.io/qt-5/cmake-manual.html
#
# have to say where Qt is by either:
#
# CMAKE_PREFIX_PATH=/home/keitee/Qt/5.12.3/gcc_64 cmake ..
#
# or 
#
# set(CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/5.12.3/gcc_64/lib/cmake/Qt5Core")
#
# Find the QtWidgets library

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets)

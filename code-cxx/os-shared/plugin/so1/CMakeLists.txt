# # bring the headers into the project
# include_directories(include)

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
add_library(so1 SHARED ${SOURCES})

target_include_directories(so1
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include
  )



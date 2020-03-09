# can manually add the sources using the set command as:
# set(SOURCES src/main.cxx src/Student.cxx)
# However, the file(GLOB...) allows wildcard additions:

file(GLOB SOURCES "*.cpp")

# generate the shared library from the sources
add_library(plugin1 SHARED ${SOURCES})

# target_include_directories(plugin1
#   PUBLIC
#   ${CMAKE_CURRENT_LIST_DIR}
#   )


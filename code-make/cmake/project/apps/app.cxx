// case1:
// src/CMakeLists.txt
// target_include_directories(student PUBLIC ../include)
//
// then here "apps/app.cxx" can use
//
// #include <modern/Student.h>
// #include "modern/Student.h"
//
// case2:
// src/CMakeLists.txt
// target_include_directories(student PUBLIC ${CMAKE_CURRENT_LIST_DIR})
//
// then here "apps/app.cxx" can use
//
// #include <../include/modern/Student.h>
// #include "../include/modern/Student.h"
//
// the same applies to "src/Student.cxx"

#include "../include/modern/Student.h"

int main(int argc, char *argv[])
{
  Student s("Joe");
  s.display();
  return 0;
}

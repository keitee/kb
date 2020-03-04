// TODO:
// although app's CMakeLists.txt do not have `include` setting, able to find a
// header
//
// also, have to use "<>" and "<modern/..." since include setting is "include"
// but not "inlcude/modern"
//
// is that because it uses "student" library which has include setting and cmake
// knows where to find header?

#include <modern/Student.h>

int main(int argc, char *argv[])
{
  Student s("Joe");
  s.display();
  return 0;
}

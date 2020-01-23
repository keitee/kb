#include <iostream>
// #include "Student.h"
#include <modern/Student.h>

using namespace std;

Student::Student(string name): name(name) {}

void Student::display()
{
  cout << "A student with name " << name << endl;
}

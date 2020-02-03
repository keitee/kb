#include <iostream>

// causes error
//
// kb/code-cxx/cxx_enum/cxx_use_enum.cpp:9:6: error: 
//  use of enum ‘EnumValues’ without previous declaration
// enum EnumValues;
//
// Before C++11, C++ didn't support forward-declaration of enums at all!
// However, some compilers (like MS Visual Studio) provide language extensions
// for that.
// 
// enum EnumValues;

// Ok. print_enum_values() uses local one which can be different from others
// declared somewhere else.
//
// enum EnumValues
// {
//   Idle,
//   Searching,
//   Pairing,
//   Complete,
//   Failed
// };
//
// enum EnumValues
// {
//   Idle,
//   Pairing,
//   Complete,
//   Failed,
//   Searching
// };
//
// NOTE: cannot use extern on enum and have to have same definition across all
// files.

// void print_enum_values()
// {
//   std::cout << "EnumValues " << EnumValues::Searching << std::endl;
// }

// cxx_use_enum.cpp:49:38: error: ‘Searching’ 
//  is not a member of ‘EnumClassValues’
//    EnumClassValues e{EnumClassValues::Searching};
// 
// enum class EnumClassValues;

// Ok.
enum class EnumClassValues { Idle, Searching, Pairing, Complete, Failed };

void print_enum_class_values()
{
  EnumClassValues e{EnumClassValues::Searching};

  std::cout << "EnumValues " 
    << (e == EnumClassValues::Searching ? "searching" : "not searching") 
    << std::endl;
}

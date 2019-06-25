#include <iostream>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

// enum EnumValues
// {
//   Idle,
//   Searching,
//   Pairing,
//   Complete,
//   Failed
// };

// enum class EnumClassValues
// {
//   Idle,
//   Searching,
//   Pairing,
//   Complete,
//   Failed
// };

enum class EnumClassValues { Idle, Searching, Pairing, Complete, Failed };

void print_enum_values();
void print_enum_class_values();

int main(int argc, char **argv)
{
  // print_enum_values();

  print_enum_class_values();

  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test.  Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}



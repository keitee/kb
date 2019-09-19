#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <bitset>
// #include <boost/algorithm/string.hpp>
// #include <boost/regex.hpp>
// #include <boost/cast.hpp>
// #include <boost/lexical_cast.hpp>
#include <regex>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::StrEq;
using testing::FloatEq;


// ={=========================================================================
// string-ex

TEST(String, Ex)
{
  std::string coll1{};
  std::string coll2{"not empty"};

  // if (coll1)
  //   cout << "string is empty" << endl;
}


// ={=========================================================================
int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <bitset>
#include <boost/algorithm/string.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::FloatEq;
using testing::Ne;
using testing::StrEq;

// ={=========================================================================
// *cxx-string-ctor*

TEST(StringPad, parse_ids)
{
  {
    std::string s{"this is string"};

    auto found = s.find('i');
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

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

namespace cxx_string
{
  void foo1(const std::string &text)
  {
    // do some processing on input text.
    std::string token = text.substr(0, 3);
  }

  void foo4(std::string_view text)
  {
    // do some processing on input text.
    std::string_view token = text.substr(0, 3);
  }
}

TEST(StringPad, parse_ids)
{
  using namespace cxx_string;

  {
    const char *input = "this is input";

    foo1("this is input");
    foo1(input);
  }

  {
    const char *input = "this is input";

    foo4("this is input");
    foo4(input);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

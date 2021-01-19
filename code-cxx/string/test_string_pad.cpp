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
  // if use iterators
  void trim_2(std::string &s, const char c)
  {
    if (s.empty())
      return;

    auto end = s.end();

    // skip `c` chars if there are and note that `end` is decreased and is
    // checked. hence need to ++end at below.
    //
    // also note that ulike trim_1(), works on "ooo" since `end` is begin.

    for (; end != s.begin() && *--end == c;)
      ;

    if (*end != c)
      ++end;

    if (end == s.end())
      std::cout << "ends are equal\n";
    else
      std::cout << "ends are not equal\n";

    // s.erase(end, s.end());
    s.erase(end);
  }
}

TEST(StringPad, parse_ids)
{
  using namespace cxx_string;

  string s1{"zzz"};
  trim_2(s1, 'o');
  EXPECT_EQ(s1, "zzz");
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

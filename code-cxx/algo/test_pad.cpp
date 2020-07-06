#include <algorithm>
#include <bitset>
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

namespace algopad
{
  // ansic, p62
  void reverse_string_1(char *input)
  {
    int str{}, end{};
    char temp{};

    for (end = strlen(input) - 1; str < end; ++str, --end)
    {
      // swap(str, end)
      temp = input[str], input[str] = input[end], input[end] = temp;
    }
  }

  // c style. assumces input is c-string since it's c version.
  void reverse_string_2(char *input)
  {
    // check input
    if (!input || !strlen(input))
      return;

    char *start{&input[0]};
    char *end{&input[strlen(input)-1]};

    for (start, end; start < end; ++start, --end)
      std::swap(*start, *end);
  }

  // if use std::string and still c-version is better
  std::string reverse_string_3(const std::string &input)
  {
    return std::string{input.crbegin(), input.crend()};
  }

  void reverse_string_4(std::string &input)
  {
    std::reverse(input.begin(), input.end());
  }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  {
    const std::vector<std::function<void(char *)>> imps{reverse_string_1,
                                                        reverse_string_2};

    for (const auto &f : imps)
    {
      char input[] = "REVERSE IT";
      f(input);
      EXPECT_THAT(input, StrEq("TI ESREVER"));
    }
  }

  {
    const std::vector<std::function<std::string(const std::string &)>> imps{
      reverse_string_3};

    for (const auto &f : imps)
    {
      std::string input{"REVERSE IT"};
      EXPECT_THAT(f(input), "TI ESREVER");
    }
  }

  {
    auto f = reverse_string_4;

    {
      std::string input{"REVERSE IT"};
      f(input);
      EXPECT_THAT(input, "TI ESREVER");
    }
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

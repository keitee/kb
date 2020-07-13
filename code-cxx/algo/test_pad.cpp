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
  // 1   5   10  50  100   500   1000
  // I   V   X   L   C     D     M
  // from TDD book
  std::string to_roman_1(unsigned int value)
  {
    std::string result{};

    // // OK
    // const std::vector<std::pair<int, std::string>> table = {
    //   std::make_pair(1000, "M"),
    //   std::make_pair(900, "CM"),
    //   std::make_pair(500, "D"),
    //   std::make_pair(400, "CD"),
    //   std::make_pair(100, "C"),
    //   std::make_pair(90, "XC"),
    //   std::make_pair(50, "L"),
    //   std::make_pair(40, "XL"),
    //   std::make_pair(10, "X"),
    //   std::make_pair(9, "IX"),
    //   std::make_pair(5, "V"),
    //   std::make_pair(4, "IV"),
    //   std::make_pair(1, "I")};

    // // OK
    // const auto table = {
    //   std::make_pair(1000, "M"),
    //   std::make_pair(900, "CM"),
    //   std::make_pair(500, "D"),
    //   std::make_pair(400, "CD"),
    //   std::make_pair(100, "C"),
    //   std::make_pair(90, "XC"),
    //   std::make_pair(50, "L"),
    //   std::make_pair(40, "XL"),
    //   std::make_pair(10, "X"),
    //   std::make_pair(9, "IX"),
    //   std::make_pair(5, "V"),
    //   std::make_pair(4, "IV"),
    //   std::make_pair(1, "I")};

    // error: direct-list-initialization of ‘auto’ requires exactly one element [-fpermissive]
    // const auto roman_table{
    //   make_pair(1000u, "M"),
    //     make_pair(900u, "CM"),
    //     make_pair(500u, "D"),
    //     make_pair(400u, "CD"),
    //     make_pair(100u, "C"),
    //     make_pair(90u, "XC"),
    //     make_pair(50u, "L"),
    //     make_pair(40u, "XL"),
    //     make_pair(10u, "X"),
    //     make_pair(9u, "IX"),
    //     make_pair(5u, "V"),
    //     make_pair(4u, "IV"),
    //     make_pair(1u, "I")
    // };

    // // error: unable to deduce ‘const std::initializer_list<const auto>’ from ‘{{1000, "M"},
    // const auto table = {{1000, "M"},
    //                     {900, "CM"},
    //                     {500, "D"},
    //                     {400, "CD"},
    //                     {100, "C"},
    //                     {90, "XC"},
    //                     {50, "L"},
    //                     {40, "XL"},
    //                     {10, "X"},
    //                     {9, "IX"},
    //                     {5, "V"},
    //                     {4, "IV"},
    //                     {1, "I"}};

    // OK
    const std::initializer_list<std::pair<unsigned int, std::string>> table = {
      {1000, "M"},
      {900, "CM"},
      {500, "D"},
      {400, "CD"},
      {100, "C"},
      {90, "XC"},
      {50, "L"},
      {40, "XL"},
      {10, "X"},
      {9, "IX"},
      {5, "V"},
      {4, "IV"},
      {1, "I"}};

    for (const auto &e : table)
    {
      while (value >= e.first)
      {
        value -= e.first;
        result.append(e.second);
      }
    }

    // if comment out "return result", crashes.
    // From standard 6.6.3/2
    // A return statement without an expression can be used only in functions
    // that do not return a value, that is, a function with the return type
    // void, a constructor (12.1), or a destructor (12.4). A return statement
    // with an expression of non-void type can be used only in functions
    // returning a value; the value of the expression is returned to the caller
    // of the function. The expression is implicitly converted to the return
    // type of the function in which it appears. A return statement can involve
    // the construction and copy of a temporary object (12.2). Flowing off the
    // end of a function is equivalent to a return with no value; this results
    // in undefined behavior in a value-returning function.

    return result;
  }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  // {
  //   // pair<input, expected result>
  //   const std::vector<std::pair<std::vector<int>, std::vector<std::string>>>
  //     inputs{{{42, 54, 60, 62, 66, 68, 71, 73, 78, 89, 98},
  //             {"F", "F", "D-", "D-", "D", "D+", "C-", "C", "C+", "B+", "A+"}}};

  //   for (const auto &f : imps)
  //   {
  //     for (const auto &i : inputs)
  //       EXPECT_THAT(f(i.first), i.second);
  //   }
  // }

  const std::vector<std::function<std::string(unsigned int)>> imps{to_roman_1};

  for (const auto &f : imps)
  {
    EXPECT_THAT(f(1), Eq("I"));
    EXPECT_THAT(f(2), Eq("II"));
    EXPECT_THAT(f(3), Eq("III"));
    EXPECT_THAT(f(4), Eq("IV"));
    EXPECT_THAT(f(5), Eq("V"));
    EXPECT_THAT(f(6), Eq("VI"));
    EXPECT_THAT(f(7), Eq("VII"));
    EXPECT_THAT(f(8), Eq("VIII"));
    EXPECT_THAT(f(9), Eq("IX"));
    EXPECT_THAT(f(10), Eq("X"));
    EXPECT_THAT(f(11), Eq("XI"));
    EXPECT_THAT(f(12), Eq("XII"));
    EXPECT_THAT(f(13), Eq("XIII"));
    EXPECT_THAT(f(16), Eq("XVI"));
    EXPECT_THAT(f(17), Eq("XVII"));
    EXPECT_THAT(f(18), Eq("XVIII"));
    EXPECT_THAT(f(20), Eq("XX"));
    EXPECT_THAT(f(23), Eq("XXIII"));
    EXPECT_THAT(f(41), Eq("XLI"));
    EXPECT_THAT(f(45), Eq("XLV"));
    EXPECT_THAT(f(50), Eq("L"));
    EXPECT_THAT(f(80), Eq("LXXX"));
    EXPECT_THAT(f(91), Eq("XCI"));
    EXPECT_THAT(f(95), Eq("XCV"));
    EXPECT_THAT(f(100), Eq("C"));
    EXPECT_THAT(f(122), Eq("CXXII"));
    EXPECT_THAT(f(152), Eq("CLII"));
    EXPECT_THAT(f(196), Eq("CXCVI"));
    EXPECT_THAT(f(247), Eq("CCXLVII"));
    EXPECT_THAT(f(288), Eq("CCLXXXVIII"));
    EXPECT_THAT(f(298), Eq("CCXCVIII"));
    EXPECT_THAT(f(500), Eq("D"));
    EXPECT_THAT(f(1000), Eq("M"));
    EXPECT_THAT(f(1513), Eq("MDXIII"));
    EXPECT_THAT(f(2999), Eq("MMCMXCIX"));
    EXPECT_THAT(f(3447), Eq("MMMCDXLVII"));
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

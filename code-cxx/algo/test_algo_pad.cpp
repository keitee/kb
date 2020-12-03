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
#include <unordered_set>
#include <vector>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

template <typename T>
void PRINT_ELEMENTS(T &coll, const string optstr = "")
{
  size_t count{};
  cout << optstr;

  for (const auto &elem : coll)
  {
    std::cout << std::setw(2) << elem << " ";
    ++count;
  }

  std::cout << "(" << count << ")" << std::endl;
}

/*
={=========================================================================
algo-leetcode-459. Repeated Substring Pattern Easy

Given a non-empty string check if it can be constructed by taking a substring of
it and appending multiple copies of the substring together. You may assume the
given string consists of lowercase English letters only and its length will not
exceed 10000.

Example 1:

Input: "abab"
Output: True

Explanation: It's the substring "ab" twice.

Example 2:

Input: "aba"
Output: False

Example 3:

Input: "abcabcabcabc"
Output: True

Explanation: It's the substring "abc" four times. (And the substring "abcabc"
twice.)

*/

namespace leetcode_easy_459
{
  int hamming_distance_1(int x, int y)
  {
    unsigned int ux{x};
    unsigned int uy{y};
    int count{};

    while (ux || uy)
    {
      if ((ux & 0x1) != (uy & 0x1))
        ++count;

      ux >>= 1;
      uy >>= 1;
    }

    return count;
  }
}

TEST(AlgoPalindrome, longest_can_be_built)
{
  using namespace leetcode_easy_459;

  {
    auto f = hamming_distance_1;

    EXPECT_THAT(f(1, 2), 2);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

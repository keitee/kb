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

namespace algopad
{
  // old and fails. not get the idea now.
  //
  // should be "aa"
  // EXPECT_THAT(func(std::vector<std::string>{"aa", "aa"}), "");
  //
  // should be ""
  // EXPECT_THAT(func(std::vector<std::string>{"aa", "bb"}), "aa");

  std::string longest_common_prefix_1(const std::vector<std::string> &input)
  {
    // since all are lower cases
    std::vector<unsigned int> table{26, 0};

    auto num_input = input.size();
    unsigned int num_common{};

    if (num_input == 0)
      return "";

    // build occurance table from inputs. scan all input strings
    for (const auto &s : input)
    {
      for (const auto &c : s)
        table[c - 'a'] += 1;
    }

    // take one input. WHY ANY input?
    for (const auto &c : input[0])
    {
      if (table[c - 'a'] != num_input)
        break;

      ++num_common;
    }

    return input[0].substr(0, num_common);
  }

  // 2020.08
  // 1. gets the size of smallest string since that's minimum to compare
  // 2. while doing loop on this size, each chars fron all input string should
  // be the smae. if not, stop the loop.
  std::string longest_common_prefix_2(const std::vector<std::string> &input)
  {
    size_t min_size{std::numeric_limits<size_t>::max()};
    std::string lcp{};

    for (const auto &s : input)
    {
      if (s.size() < min_size)
        min_size = s.size();
    }

    // num of chars to compare
    for (size_t i = 0; i < min_size; ++i)
    {
      // num of strings
      for (size_t run = 0; run < input.size(); ++run)
      {
        // compare [i] char of each string to see all are same.
        //
        // use xor?
        // cxx-xor
        // X XOR  X  = 0
        // X XOR  0  = X
        // X XOR  1  = ~X    // X XOR (~0) = ~X
        // X XOR ~X  = 1
      }
    }

    return lcp;
  }

  // Approach 1: Horizontal scanning
  //
  // For a start we will describe a simple way of finding the longest common
  // prefix shared by a set of strings LCP(S1 ... Sn)
  //
  // We will use the *observation* that :
  //
  // LCP(S1 ... Sn) = LCP(LCP(LCP(S1, S2),S3), ... Sn)
  //
  // Algorithm
  //
  // To employ this idea, the algorithm iterates through the strings [S1 ...
  // Sn], finding at each iteration i the longest common prefix of strings
  // LCP(S1 ...  Si)
  //
  // When LCP(S1 ... Si) is an empty string, the algorithm ends. Otherwise after
  // nn iterations, the algorithm returns LCP(S1 ... Sn)
  //
  // Complexity Analysis
  //
  // Time complexity : O(S), where S is the sum of all characters in all strings.
  //
  // In the *worst case* all n strings are the same. The algorithm compares the
  // string S1 with the other strings [S2 ... Sn]. There are S character
  // comparisons, where S is the sum of all characters in the input array.
  //
  // Space complexity : O(1) We only used constant extra space.

  std::string longest_common_prefix_3(const std::vector<std::string> &input)
  {
    auto num_input = input.size();

    // check on input
    if (!num_input)
      return "";

    // set the first string as lcp
    std::string lcp{input[0]};

    for (size_t i = 1; i < num_input; ++i)
    {
      // get min size to loop
      size_t min_size = std::min(lcp.size(), input[i].size());
      size_t run{};

      for (run = 0; run < min_size; ++run)
      {
        if (lcp[run] != input[i][run])
          break;
      }

      // update lcp
      // note that substr(start, length)
      lcp = lcp.substr(0, run);
    }

    return lcp;
  }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  {
    auto imps = {longest_common_prefix_3};

    for (auto f : imps)
    {
      EXPECT_THAT(f(std::vector<std::string>{"flower", "flow", "flight"}),
                  "fl");
      EXPECT_THAT(f(std::vector<std::string>{"dog", "racecar", "car"}), "");

      EXPECT_THAT(f(std::vector<std::string>{"aa", "aa"}), "aa");
      EXPECT_THAT(f(std::vector<std::string>{"aa", "bb"}), "");
    }
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

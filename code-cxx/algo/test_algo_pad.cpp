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
algo-leetcode-326. Power of Three. Easy

Given an integer, write a function to determine if it is a power of three.

Example 1:
Input: 27
Output: true

Example 2:
Input: 0
Output: false

Example 3:
Input: 9
Output: true

Example 4:
Input: 45
Output: false

Follow up:
Could you do it without using any loop / recursion?

*/

namespace leetcode_easy_326
{
  // makes a better result.
  vector<int> intersection_1(vector<int> &nums1, vector<int> &nums2)
  {
    std::unordered_set<int> table;
    std::unordered_set<int> check;
    std::vector<int> result{};

    if (nums1.size() < nums2.size())
    {
      table.insert(nums2.cbegin(), nums2.cend());
      check.insert(nums1.cbegin(), nums1.cend());

      for (const auto e : check)
      {
        if (table.end() != table.find(e))
          result.push_back(e);
      }
    }
    else
    {
      table.insert(nums1.cbegin(), nums1.cend());
      check.insert(nums2.cbegin(), nums2.cend());

      for (const auto e : check)
      {
        if (table.end() != table.find(e))
          result.push_back(e);
      }
    }

    return result;
  }

  vector<int> intersection_2(vector<int> &nums1, vector<int> &nums2)
  {
    std::set<int> input1;
    std::set<int> input2;
    std::vector<int> result{};

    input1.insert(nums1.cbegin(), nums1.cend());
    input2.insert(nums2.cbegin(), nums2.cend());

    // Processing the Union of Two Sorted Sets
    // OutputIterator
    // set_intersection (InputIterator source1Beg, InputIterator source1End,
    // InputIterator source2Beg, InputIterator source2End,
    // OutputIterator destBeg)

    std::set_intersection(input1.cbegin(), input1.cend(),
        input2.cbegin(), input2.cend(),
        std::back_inserter(result));

    return result;
  }
}

TEST(AlgoLeetCode, power_of_three)
{
  using namespace leetcode_easy_326;

  {
    auto f = intersection_1;

    vector<int> coll1{1,2,2,1};
    vector<int> coll2{2,2};

    EXPECT_THAT(f(coll1, coll2), std::vector<int>{2});
  }

  {
    auto f = intersection_1;

    vector<int> coll1{4,9,5};
    vector<int> coll2{9,4,9,8,4};
    vector<int> result{9, 4};
    // vector<int> result{4, 9};

    EXPECT_THAT(f(coll1, coll2), result);
  }

  {
    auto f = intersection_2;

    vector<int> coll1{1,2,2,1};
    vector<int> coll2{2,2};

    EXPECT_THAT(f(coll1, coll2), std::vector<int>{2});
  }

  {
    auto f = intersection_2;

    vector<int> coll1{4,9,5};
    vector<int> coll2{9,4,9,8,4};
    // vector<int> result{9, 4};
    vector<int> result{4, 9};

    EXPECT_THAT(f(coll1, coll2), result);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

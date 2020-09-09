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

219. Contains Duplicate II

Given an array of integers and an integer k, find out whether there are two
distinct indices i and j in the array such that nums[i] = nums[j] and the
absolute difference between i and j is at most k.

Example 1:

Input: nums = [1,2,3,1], k = 3
Output: true

Example 2:

Input: nums = [1,0,1,1], k = 1
Output: true

Example 3:

Input: nums = [1,2,3,1,2,3], k = 2
Output: false

*/

namespace leetcode_easy_219
{
  // fails on EXPECT_THAT(f({99, 99}, 2), true); since "at most"
  bool contain_duplicates_1(const std::vector<int> &nums, int k)
  {
    int first = 0;
    int last  = k;

    while (last < (int)nums.size())
    {
      if (nums[first] == nums[last])
        return true;

      ++first;
      last = first + k;
    }

    return false;
  }

  // Time Limit Exceeded when the input is huge
  bool contain_duplicates_2(const std::vector<int> &nums, int k)
  {
    auto loop = nums.size();

    for (int run = 0; run < (int)loop; ++run)
    {
      for (int scan = run + 1; (scan < (int)loop) && ((scan - run) <= k);
           ++scan)
      {
        if (nums[run] == nums[scan])
          return true;
      }
    }

    return false;
  }

  // from the discussion.
  // [Java] Linear Time Solution using HashMap
  // https://leetcode.com/problems/contains-duplicate-ii/discuss/824527/Java-Linear-Time-Solution-using-HashMap
  //
  // class Solution {
  //     public boolean containsNearbyDuplicate(int[] nums, int k) {
  //         Map<Integer, Integer> map = new HashMap();
  //         for(int i = 0; i < nums.length; i++) {
  //             if(map.containsKey(nums[i]) && Math.abs(i - map.get(nums[i])) <= k)
  //                 return true;
  //             map.put(nums[i], i);
  //         }
  //         return false;
  //     }
  // }
  //
  // tried it in c++
  //
  // Runtime: 52 ms, faster than 75.15% of C++ online submissions for Contains Duplicate II.
  // Memory Usage: 16.4 MB, less than 73.48% of C++ online submissions for Contains Duplicate II.

  bool contain_duplicates_3(const std::vector<int> &nums, int k)
  {
    auto loop = nums.size();

    // pair<value, index>
    std::unordered_multimap<int, int> coll{};

    for (int i = 0; i < (int)loop; ++i)
    {
      auto size  = coll.count(nums[i]);
      auto first = coll.find(nums[i]);

      // found and scan "equal_range"
      for (int scan = 0; scan < (int)size; ++scan)
      {
        if (std::abs(first->second - i) <= k)
          return true;

        ++first;
      }

      // if not found, insert it
      coll.insert(std::make_pair(nums[i], i));
    }

    return false;
  }
} // namespace leetcode_easy_219

TEST(AlgoLeetCode, check_triangle)
{
  using namespace leetcode_easy_219;

  // auto f = contain_duplicates_1;
  auto f = contain_duplicates_3;

  EXPECT_THAT(f({1, 2, 3, 1}, 3), true);
  EXPECT_THAT(f({1, 0, 1, 1}, 1), true);
  EXPECT_THAT(f({1, 2, 3, 1, 2, 3}, 2), false);

  EXPECT_THAT(f({99, 99}, 2), true);
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

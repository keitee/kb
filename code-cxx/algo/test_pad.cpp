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
  // old
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Remove
  // Element.
  //
  // Memory Usage: 9.3 MB, less than 56.68% of C++ online submissions for Remove
  // Element.

  int my_remove_1(std::vector<int> &nums, int val)
  {
    if (nums.empty())
      return 0;

    size_t end{};

    for (size_t i = 0; i < nums.size(); ++i)
    {
      if (nums[i] != val)
      {
        if (end != i)
          swap(nums[end], nums[i]);

        ++end;
      }
    }

    return end;
  }

  int my_remove_2(std::vector<int> &nums, int val)
  {
    size_t start{};
    size_t end{nums.size()};

    // find pos where the matched is found and which is pos to start "remove"
    while (nums[start] != val)
      ++start;

    for (size_t scan = start + 1; scan < end; ++scan)
    {
      // found the unmatched
      if (nums[scan] != val)
      {
        nums[start] = nums[scan];
        ++start;
      }
    }

    return start;
  }

  int my_remove_3(std::vector<int> &nums, int val)
  {
    size_t start{};
    size_t end{nums.size()};

    // find pos where the matched is found and which is pos to start "remove"
    // note: if all elements of nums are "unmatched", there should way to stop
    // loop as my_remove_1 of iterator version
    while (nums[start] != val)
      if (++start == end)
        return start;

    for (size_t scan = start + 1; scan < end; ++scan)
    {
      // found the unmatched
      if (nums[scan] != val)
      {
        nums[start] = nums[scan];
        ++start;
      }
    }

    return start;
  }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  {
    auto imps = {my_remove_1, my_remove_2, my_remove_3};

    for (auto f : imps)
    {
      std::vector<int> coll{0, 1, 2, 2, 3, 0, 4, 2};

      auto len = f(coll, 2);

      EXPECT_THAT(len, 5);

      std::vector<int> result{};

      for (int i = 0; i < len; ++i)
        result.push_back(coll[i]);

      EXPECT_THAT(result, ElementsAre(0, 1, 3, 0, 4));
    }
  }

  {
    auto imps = {my_remove_1, my_remove_2, my_remove_3};

    for (const auto &f : imps)
    {
      std::vector<int> coll{1, 2, 3, 4, 5, 6, 2, 7, 2, 8, 2, 9};

      auto size = f(coll, 2);

      EXPECT_THAT(size, 8);
      // EXPECT_THAT(coll, ElementsAre(1, 3, 4, 5, 6, 7, 8, 9));
    }
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

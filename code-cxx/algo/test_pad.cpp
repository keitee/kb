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
  // old
  // good on using sum on X and input. can use single loop
  // but space O(A+1) but not O(X)
  int check_frog_river_1(int X, const std::vector<int> &A)
  {
    // input check
    if (A.empty())
      return -1;

    std::vector<bool> lookup(A.size() + 1);

    int target_sum = (X * (X + 1)) / 2;
    int input_sum{};

    for (size_t i = 0; i < A.size(); ++i)
    {
      // see if check
      if (A[i] <= X && lookup[A[i]] == false)
      {
        lookup[A[i]] = true;
        input_sum += A[i];

        if (target_sum == input_sum)
          return i;
      }
    }

    return -1;
  }

  // NOTE: BEST
  // make space O(X+1) due to "if check" and can make O(X)
  int check_frog_river_2(int X, const std::vector<int> &A)
  {
    // input check
    if (A.empty())
      return -1;

    std::vector<bool> lookup(X + 1);

    int target_sum = (X * (X + 1)) / 2;
    int input_sum{};

    for (size_t i = 0; i < A.size(); ++i)
    {
      if (A[i] <= X && lookup[A[i]] == false)
      {
        lookup[A[i]] = true;
        input_sum += A[i];

        if (target_sum == input_sum)
          return i;
      }
    }

    return -1;
  }

  // NOTE: BEST
  // Detected time complexity: O(N)
  // The key idea is that it is to use counting and to use counter to check if
  // receives all inputs, stones, to jump
  int check_frog_river_3(int X, const std::vector<int> &A)
  {
    // write your code in C++11
    if (A.empty() || !X)
      return -1;

    std::vector<bool> flags(X);
    int count = 0;

    for (unsigned int i = 0; i < A.size(); i++)
    {
      int value = A[i] - 1;

      if (value < X && flags[value] == false)
      {
        flags[value] = true;
        count++;
      }

      if (count == X)
        return i;
    }

    return -1;
  }

  // 2020.07
  //
  // each element of array A is an integer within the range [1..X].
  //
  // to pass: EXPECT_THAT(f(1, {1}), 0);
  // xidx = 0 is valid so have xidx with init value -1.
  //
  // space O(X+1) and can make O(X)

  int check_frog_river_4(int X, const std::vector<int> &A)
  {
    // when input is empty
    if (!A.size())
      return -1;

    int xidx{-1};
    bool table[X + 1]{false};

    // get the index of X and set up the lookup table
    for (size_t i = 0; i < A.size(); ++i)
    {
      if (A[i] == X)
        xidx = i;

      table[A[i]] = true;
    }

    // not found so not able to jump
    if (xidx == -1)
      return -1;

    // check if there are missing up to xidx; index of X
    for (size_t i = 1; i <= X; ++i)
      if (table[i] = false)
        return -1;

    return xidx;
  }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  {
    auto imps = {check_frog_river_1,
                 check_frog_river_2,
                 check_frog_river_3,
                 check_frog_river_4};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(5, {1, 3, 1, 4, 2, 3, 5, 4}), 6);
      EXPECT_THAT(f(1, {2, 3, 4, 5, 1, 3, 5, 4}), 4);
      EXPECT_THAT(f(5, {}), -1);
      EXPECT_THAT(f(5, {1}), -1);
      EXPECT_THAT(f(1, {2}), -1);
      EXPECT_THAT(f(1, {1}), 0);
    }
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

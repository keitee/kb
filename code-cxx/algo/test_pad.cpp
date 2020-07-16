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
  // model answer
  int equi_tape_1(std::vector<int> &A)
  {
    // write your code in C++98
    if (!A.size())
      return -1;

    long long sum = 0, rsum = 0, lsum = 0;
    int cmin = INT_MAX;

    for (size_t i = 0; i < A.size(); i++)
      sum += A[i];

    lsum = A[0];

    // it is okay to use (n-1)th? WHY?
    // because lsum is used after getting abs value so the last is not used in
    // that calcuation.

    for (size_t i = 1; i < A.size(); i++)
    {
      rsum = sum - lsum;

      // cmin = abs cause warning of possible loss since assign from long long to int.
      if (abs(lsum - rsum) < cmin)
        cmin = abs(lsum - rsum);

      lsum += A[i];
    }

    return cmin;
  }

  // 2020.07
  int equi_tape_2(std::vector<int> &A)
  {
    long long rsum{}, lsum{};
    int current_min{std::numeric_limits<int>::max()};

    // gets the rsum
    for (int i = 0; i < A.size(); ++i)
      rsum += A[i];

    // NOTE: [0, N-2] since no need to process the last element
    // for (int i = 0; i < A.size(); ++i)

    for (int i = 0; i < A.size() - 1; ++i)
    {
      lsum += A[i];
      rsum -= A[i];

      auto min = std::abs(lsum - rsum);

      if (current_min > min)
        current_min = min;
    }

    return current_min;
  }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  {
    auto imps = {equi_tape_1, equi_tape_2};

    std::vector<int> coll{3, 2, 1, 4, 3};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(coll), 1);
    }
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

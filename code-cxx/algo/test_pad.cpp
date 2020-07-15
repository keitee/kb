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
  int climb_stairs(int start, int end)
  {
    // stopping condition
    if (start == end)
      return 1;
    else if (start > end)
      return 0;

    return climb_stairs(start + 1, end) + climb_stairs(start + 2, end);
  }

  int climb_stairs_1(int n) { return climb_stairs(0, n); }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  {
    auto func = climb_stairs_1;

    EXPECT_THAT(func(2), 2);
    EXPECT_THAT(func(3), 3);
    EXPECT_THAT(func(4), 5);
    EXPECT_THAT(func(30), 1346269);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

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
  void sort_insertion_1(std::vector<int> &coll)
  {
  }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  {
    auto imps = {find_max_counters_1,
                 find_max_counters_2,
                 find_max_counters_3,
                 find_max_counters_4};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(5, {3, 4, 4, 6, 1, 4, 4}), ElementsAre(3, 2, 2, 4, 2));
      EXPECT_THAT(f(5, {3, 4, 4, 6, 1, 4, 6}), ElementsAre(3, 3, 3, 3, 3));
      EXPECT_THAT(f(5, {3, 6, 6, 6, 6, 6, 6}), ElementsAre(1, 1, 1, 1, 1));
    }
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

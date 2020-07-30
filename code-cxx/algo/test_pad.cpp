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
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  {
    std::vector<int> coll{3,2,2,3};
    auto pos = std::remove_if(coll.begin(), coll.end(),
  }

  {
    auto imps = {unique_1, unique_2};

    for (auto f : imps)
    {
      {
        std::vector<int> coll{1, 4, 4, 6};
        auto ret = f(coll);
        EXPECT_THAT(ret, 3);
      }

      {
        std::vector<int> coll{1, 4, 4, 4, 6};
        auto ret = f(coll);
        EXPECT_THAT(ret, 3);
      }

      {
        std::vector<int> coll{0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
        auto ret = f(coll);
        EXPECT_THAT(ret, 5);
      }

      {
        std::vector<int> coll{1, 1, 1, 2, 2, 3, 4, 4, 4, 4, 5, 5, 6, 6, 6, 6, 7};
        auto ret = f(coll);
        EXPECT_THAT(ret, 7);
      }
    }
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

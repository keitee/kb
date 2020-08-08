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
  int my_strstr_1(std::string haystack, std::string needle)
  {
    int ret{-1};

    for (const auto c : haystack)
    {
      if (c == needle[count])
      {
      }
    }
  }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  {
    std::vector<int> coll{1, 4, 4, 6};

    auto length = my_unique_1(coll.begin(), coll.end());

    EXPECT_THAT(length, 3);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

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

namespace prefix_sum
{
  // 2020.08
  int length_last_word_1(std::string s)
  {
    int count{};

    for (auto &e : s)
    {
      if (isspace(e))
      {
        count = 0;
      }
      else
        ++count;
    }

    return count;
  }

  // 2020.08
  int length_last_word_2(std::string s)
  {
    // pos is size_t
    auto pos = s.find_last_of(' ');
    return s.size() - 1 - pos;
  }

  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Length of
  // Last Word.
  //
  // Memory Usage: 8.9 MB, less than 90.45% of C++ online submissions for Length
  // of Last Word.

  int length_last_word_0(std::string s)
  {
    int count{};

    int i = s.size() - 1;

    // skip spaces from the end
    for (; i >= 0; --i)
      if (!isspace(s[i]))
        break;

    // then counts word
    for (; i >= 0; --i)
    {
      if (isspace(s[i]))
        return count;

      ++count;
    }

    return count;
  }
} // namespace prefix_sum

TEST(AlgoPrefixSum, check_prefix_sum)
{
  using namespace prefix_sum;

  // both fails on the last case
  {
    auto imps = {length_last_word_1, length_last_word_2};

    for (const auto &f : imps)
    {

      EXPECT_THAT(f("Hello World"), 5);
      EXPECT_THAT(f("Hello Wor"), 3);
      EXPECT_THAT(f("HelloWorld"), 10);
      EXPECT_THAT(f(""), 0);

      EXPECT_THAT(f("a"), 1);

      // fails. why 1? "a" is last word?
      // EXPECT_THAT(f("a "), 1);
    }
  }

  // both fails on the last case
  {
    auto f = length_last_word_0;

    EXPECT_THAT(f("Hello World"), 5);
    EXPECT_THAT(f("Hello Wor"), 3);
    EXPECT_THAT(f("HelloWorld"), 10);
    EXPECT_THAT(f(""), 0);

    EXPECT_THAT(f("a"), 1);
    EXPECT_THAT(f("a "), 1);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

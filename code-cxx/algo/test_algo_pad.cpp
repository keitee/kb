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
258. Add Digits, Easy

Given a non-negative integer num, repeatedly add all its digits until the result
has only one digit.

Example:

Input: 38
Output: 2 

Explanation: The process is like: 3 + 8 = 11, 1 + 1 = 2. 
             Since 2 has only one digit, return it.

Follow up:
Could you do it without any loop/recursion in O(1) runtime?


https://leetcode.com/problems/add-digits/solution/

The value we're asked to compute is the so-called Digital Root. Logarithmic time
solution is easy to write, although the main question here is how to fit into a
constant time.

Approach 1: Mathematical: Digital Root

Formula for the Digital Root

There is a known formula to compute a digital root in a decimal numeral system

dr(n)= 0, if n=0

dr(n)= 9, if n=9k

dr(n)= n mod 9, if n = 9k

How to derive it? Probably, you already know the following proof from school,
where it was used for a divisibility by 9: "The original number is divisible by
9 if and only if the sum of its digits is divisible by 9". Let's revise it
briefly.

9 x 2 = 18 -> 9       17 (6), 18, 19(1)
9 x 3 = 27 -> 9       26 (8), 27, 28(1)
9 x 4 = 36 -> 9       35 (8), 36, 37(1)
9 x 5 = 45 -> 9
9 x 6 = 54 -> 9
...

*/

namespace leetcode_easy_258
{
  // 2020.09
  // use itoa() and recursion
  // Runtime: 0 ms, faster than 100.00% of C++ online submissions for Add Digits.
  // Memory Usage: 6 MB, less than 41.85% of C++ online submissions for Add Digits.

  int add_digits_1(int num)
  {
    // if num is < 10 then stop recursion.
    if ((num / 10) == 0)
      return num;

    int result{};

    while (num)
    {
      result += (num % 10);
      num /= 10;
    }

    return add_digits_1(result);
  }

  // code from solution
  int add_digits_2(int num)
  {
    if (num == 0)
      return 0;

    if ((num % 9) == 0)
      return 9;

    // else cases
    return num % 9;
  }

} // namespace leetcode_easy_258

TEST(AlgoLeetCode, add_digits)
{
  using namespace leetcode_easy_258;

  {
    auto f = add_digits_1;

    EXPECT_THAT(f(38), 2);
  }

  {
    auto f = add_digits_2;

    EXPECT_THAT(f(38), 2);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

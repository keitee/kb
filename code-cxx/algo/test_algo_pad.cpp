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
={=========================================================================
algo-leetcode-409. Longest Palindrome Easy

989. Add to Array-Form of Integer Easy

For a non-negative integer X, the array-form of X is an array of its digits in
left to right order.  For example, if X = 1231, then the array form is
[1,2,3,1].

Given the array-form A of a non-negative integer X, return the array-form of the
integer X+K.

Example 1:

Input: A = [1,2,0,0], K = 34
Output: [1,2,3,4]
Explanation: 1200 + 34 = 1234

Example 2:

Input: A = [2,7,4], K = 181
Output: [4,5,5]
Explanation: 274 + 181 = 455

Example 3:

Input: A = [2,1,5], K = 806
Output: [1,0,2,1]
Explanation: 215 + 806 = 1021

Example 4:

Input: A = [9,9,9,9,9,9,9,9,9,9], K = 1
Output: [1,0,0,0,0,0,0,0,0,0,0]
Explanation: 9999999999 + 1 = 10000000000
 
Note：

1 <= A.length <= 10000
0 <= A[i] <= 9
0 <= K <= 10000
If A.length > 1, then A[0] != 0

vector<int> addToArrayForm(vector<int>& A, int K);

*/

namespace algo_palindrome
{
  int countSegments(string s)
  {
    int count{};

    for (auto e : s)
    {
    }
  }
} // namespace algo_palindrome

TEST(AlgoPalindrome, longest_can_be_built)
{
  using namespace algo_palindrome;

  {
    auto f = add_strings_1;

    // EXPECT_THAT(f("123", "35"), "158");
    EXPECT_THAT(f("1", "9"), "10");
  }

  {
    auto f = add_array_form_1;

    vector<int> coll{1, 2, 0, 0};

    EXPECT_THAT(f(coll, 34), ElementsAre(1, 2, 3, 4));
  }

  {
    auto f = add_array_form_1;

    vector<int> coll{2, 7, 4};

    EXPECT_THAT(f(coll, 181), ElementsAre(4, 5, 5));
  }

  {
    auto f = add_array_form_1;

    vector<int> coll{2, 1, 5};

    EXPECT_THAT(f(coll, 806), ElementsAre(1, 0, 2, 1));
  }

  {
    auto f = add_array_form_1;

    vector<int> coll{9, 9, 9, 9, 9, 9, 9, 9, 9, 9};

    EXPECT_THAT(f(coll, 1), ElementsAre(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

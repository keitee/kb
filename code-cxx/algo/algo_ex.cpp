#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <bitset>
#include <stack>
#include <list>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

// ={=========================================================================
template <typename T>
void PRINT_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << elem << " ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}

template <typename T>
void PRINT_M_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << "(" << elem.first << ", " << elem.second << ") ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}


// ={=========================================================================
// algo-leetcode-1

/*
1. two sum, easy

given an array of integers, return *indices* of the two numbers such that they
add up to a specific target.

you may assume that each input would have *exactly* one solution, and you may
not use the same element twice.

example:

given nums = [2, 7, 11, 15], target = 9,

because nums[0] + nums[1] = 2 + 7 = 9,
return [0, 1].

*/

namespace leetcode_easy_001
{
  vector<int> two_sum_failed(vector<int> &nums, int target)
  {
    size_t size = nums.size();

    for (int first = 0; first < (int)size; ++first)
    {
      if ((abs(nums[first]) < abs(target)) || (nums[first] == 0))
      {
        // search second
        for (int second = first + 1; second < (int)size; ++second)
        {
          if (abs(nums[second]) == abs(target - nums[first]))
              return vector<int>{first, second};
        }
      }
    }

    // not necessary since assumes that there is always a solution
    return vector<int>{-1, -1};
  }

  vector<int> two_sum_okay(vector<int> &nums, int target)
  {
    size_t size = nums.size();

    for (int first = 0; first < (int)size; ++first)
    {
      // search second
      for (int second = first + 1; second < (int)size; ++second)
      {
        if (target == nums[first] + nums[second])
          return vector<int>{first, second};
      }
    }

    // not necessary since assumes that there is always a solution
    return vector<int>{-1, -1};
  }

  // turns out that "(target - nums[first])" is valid and covers plus and minus
  // input. handling of equal to 0 and abs() are due to "nums[first] < targer"
  // and which is not necessary and assumes input are all positive.

  vector<int> two_sum(vector<int> &nums, int target)
  {
    size_t size = nums.size();

    for (int first = 0; first < (int)size; ++first)
    {
      // search second
      for (int second = first + 1; second < (int)size; ++second)
      {
        if (nums[second] == (target - nums[first]))
          return vector<int>{first, second};
      }
    }

    // not necessary since assumes that there is always a solution. as some
    // solution showes, can use exception.

    return vector<int>{-1, -1};
  }

  // Approach 2: Two-pass Hash Table
  // To improve our run time complexity, we need a more efficient way to check
  // if the complement exists in the array. If the complement exists, we need to
  // look up its index. What is the best way to maintain a mapping of each
  // element in the array to its index? A hash table.
  //
  // A: don't think it works since there may be duplicated values in the input.
  
  // Approach 3: One-pass Hash Table 
  // turns out we can do it in one-pass.  While we iterate and inserting
  // elements into the table, we also look back to check if current element's
  // complement already exists in the table. If it exists, we have found a
  // solution and return immediately.
  //
  // Runtime: 12 ms, faster than 97.81% of C++ online submissions for Two Sum.
  //
  // Memory Usage: 10.4 MB, less than 30.73% of C++ online submissions for Two
  // Sum.

  vector<int> two_sum_single_pass_map(vector<int> &nums, int target)
  {
    std::map<int, int> table{};

    size_t size = nums.size();

    for (int first = 0; first < (int)size; ++first)
    {
      // search second
      auto found = table.find(target - nums[first]);
      
      // should be
      // return vector<int>{found->second, first};
      //
      // but not
      // return vector<int>{first, found->second};
      //
      // since looks the first back into the map

      if (found != table.end())
          return vector<int>{found->second, first};

      table[nums[first]] = first;
    }

    // not necessary since assumes that there is always a solution. as some
    // solution showes, can use exception.

    return vector<int>{-1, -1};
  }

  // py code
  //
  // def two_sum(nums, target):
  // 
  //     myHash = {}
  // 
  //     for index, value in enumerate(nums):
  //         if target - value in myHash:
  //             return [myHash[target-value], index]
  //         myHash[value] = index

  // Runtime: 12 ms, faster than 97.81% of C++ online submissions for Two Sum.
  //
  // Memory Usage: 10.2 MB, less than 49.95% of C++ online submissions for Two
  // Sum.

  vector<int> two_sum_single_pass_unordered_map(vector<int> &nums, int target)
  {
    std::unordered_map<int, int> table{};

    size_t size = nums.size();

    for (int first = 0; first < (int)size; ++first)
    {
      // search second
      auto found = table.find(target - nums[first]);
      
      // should be
      // return vector<int>{found->second, first};
      //
      // but not
      // return vector<int>{first, found->second};
      //
      // since looks the first back into the map

      if (found != table.end())
          return vector<int>{found->second, first};

      table[nums[first]] = first;
    }

    // not necessary since assumes that there is always a solution. as some
    // solution showes, can use exception.

    return vector<int>{-1, -1};
  }
} // namespace

TEST(LeetCode, Easy_001_TwoSum_01)
{
  using namespace leetcode_easy_001;

  {
    vector<int> nums{2,7,11,15};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(0,1));
  }

  {
    vector<int> nums{7,2,11,15};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(0,1));
  }

  {
    vector<int> nums{11,15,2,7};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(2,3));
  }
  {
    vector<int> nums{11,15,2,7,1,8};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(2,3));
  }
  {
    vector<int> nums{11,15,2,6,1,8};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(4,5));
  }

  // failed and by leetcode
  // not the same element but same value. it's special case so add "equal to 0"
  // case.

  {
    vector<int> nums{0,4,3,0};
    auto coll = two_sum(nums, 0);
    EXPECT_THAT(coll, ElementsAre(0,3));
  }

  // failed and by leetcode
  // Err, minus? there's no mention of input range but element type is int so
  // add abs() on values.

  {
    vector<int> nums{-1,-2,-3,-4,-5};
    auto coll = two_sum(nums, -8);
    EXPECT_THAT(coll, ElementsAre(2,4));
  }

  // failed and by leetcode

  {
    vector<int> nums{-3,4,3,90};
    auto coll = two_sum(nums, 0);
    EXPECT_THAT(coll, ElementsAre(0,2));
  }
}


TEST(LeetCode, Easy_001_TwoSum_02)
{
  using namespace leetcode_easy_001;
  const auto two_sum = two_sum_single_pass_map;

  {
    vector<int> nums{2,7,11,15};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(0,1));
  }

  {
    vector<int> nums{7,2,11,15};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(0,1));
  }

  {
    vector<int> nums{11,15,2,7};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(2,3));
  }
  {
    vector<int> nums{11,15,2,7,1,8};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(2,3));
  }
  {
    vector<int> nums{11,15,2,6,1,8};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(4,5));
  }

  // failed and by leetcode
  // not the same element but same value. it's special case so add "equal to 0"
  // case.

  {
    vector<int> nums{0,4,3,0};
    auto coll = two_sum(nums, 0);
    EXPECT_THAT(coll, ElementsAre(0,3));
  }

  // failed and by leetcode
  // Err, minus? there's no mention of input range but element type is int so
  // add abs() on values.

  {
    vector<int> nums{-1,-2,-3,-4,-5};
    auto coll = two_sum(nums, -8);
    EXPECT_THAT(coll, ElementsAre(2,4));
  }

  // failed and by leetcode

  {
    vector<int> nums{-3,4,3,90};
    auto coll = two_sum(nums, 0);
    EXPECT_THAT(coll, ElementsAre(0,2));
  }
}

TEST(LeetCode, Easy_001_TwoSum_03)
{
  using namespace leetcode_easy_001;
  const auto two_sum = two_sum_single_pass_unordered_map;

  {
    vector<int> nums{2,7,11,15};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(0,1));
  }

  {
    vector<int> nums{7,2,11,15};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(0,1));
  }

  {
    vector<int> nums{11,15,2,7};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(2,3));
  }
  {
    vector<int> nums{11,15,2,7,1,8};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(2,3));
  }
  {
    vector<int> nums{11,15,2,6,1,8};
    auto coll = two_sum(nums, 9);
    EXPECT_THAT(coll, ElementsAre(4,5));
  }

  // failed and by leetcode
  // not the same element but same value. it's special case so add "equal to 0"
  // case.

  {
    vector<int> nums{0,4,3,0};
    auto coll = two_sum(nums, 0);
    EXPECT_THAT(coll, ElementsAre(0,3));
  }

  // failed and by leetcode
  // Err, minus? there's no mention of input range but element type is int so
  // add abs() on values.

  {
    vector<int> nums{-1,-2,-3,-4,-5};
    auto coll = two_sum(nums, -8);
    EXPECT_THAT(coll, ElementsAre(2,4));
  }

  // failed and by leetcode

  {
    vector<int> nums{-3,4,3,90};
    auto coll = two_sum(nums, 0);
    EXPECT_THAT(coll, ElementsAre(0,2));
  }
}


// ={=========================================================================
// algo-leetcode-2
/*
7. Reverse Integer, Easy

Given a 32-bit signed integer, reverse digits of an integer.

Example 1:
Input: 123
Output: 321

Example 2:
Input: -123
Output: -321

Example 3:
Input: 120
Output: 21

Note:
Assume we are dealing with an environment which could only store integers within
the 32-bit signed integer range: [−2**31,  2**31 − 1]. For the purpose of this
problem, assume that your function returns 0 when the reversed integer
overflows.

*/

namespace leetcode_easy_002
{
//  void simple_itoa(int value)
//  {
//    while (value)
//    {
//      remains = value % 10;
//      result += '0' + remains;
//      value /= 10;
//    }
//
//    // reverse result string
//  }
//
//  void simple_atoi(std::string input)
//  {
//    int result{};
//
//    for (int i = 0; i < input.size(); ++i)
//    {
//      result = input[i] - '0' + 10*result;
//    }
//  }

  // Unlike atoi(), there is no need to handle:
  //
  // 1. isspace() on every single char of input
  // 2. check and remove sign char, '-', of input
  //
  // However, gets some wrong answers and see test cases below. The final result
  // is:
  //
  // Runtime: 16 ms, faster than 96.53% of C++ online submissions for Reverse
  // Integer.
  //
  // Memory Usage: 13.9 MB, less than 85.02% of C++ online submissions for
  // Reverse Integer.

  int reverse_integer(int value)
  {
    long long result{};
    // int result{};
    int remains{};

    // int sign{};
    // sign = (x < 0) ? -1 : 1;
    // int value = abs(x);
    // cout << "x: " << x << ", value: " << value << endl;

    while (value)
    {
      // as itoa(), get right-most value
      remains = value % 10;

      // as atoi(), reverse it
      result = remains + 10 * result;

      // as itoa(), reduce value
      value /= 10;
      // cout << "value: " << value << ", result: " << result << ", remains: " << remains << endl;
    }

    // return result;
    // return result > std::numeric_limits<int>::max() ? 0 : result;
    return (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min()) ? 0 : result;
  }
} // namespace


// signed int and unsigned int
//
//  // okay
//  {
//    // >>> 2**31-1
//    // 2,147,483,647
//
//    EXPECT_EQ(2147483647, numeric_limits<int>::max());
//
//    // >>> 2**32-1
//    // 4,294,967,295    UNIT_MAX
//
//    EXPECT_EQ(4294967295, numeric_limits<unsigned int>::max());
//  }

TEST(LeetCode, Easy_002_ReverseInteger)
{
  using namespace leetcode_easy_002;

  EXPECT_THAT(reverse_integer(123), 321);
  EXPECT_THAT(reverse_integer(-123), -321);
  EXPECT_THAT(reverse_integer(120), 21);

  // runtime error: signed integer overflow: 964632435 * 10 cannot be represented in type 'int' (solution.cpp)
  // causes overflow while reversing.

  EXPECT_THAT(reverse_integer(534236469), 964632435);

  // wrong answer when input is 1534236469. should be 0
  EXPECT_THAT(reverse_integer(1534236469), 0);

  // as can see, mod(%) and div(/) do not impact on sign of integer.
  //
  // wrong answer, 126087180, but should be 0
  //
  // value: -214748364, result: -8, remains: -8
  // value: -21474836, result: -84, remains: -4
  // value: -2147483, result: -846, remains: -6
  // value: -214748, result: -8463, remains: -3
  // value: -21474, result: -84638, remains: -8
  // value: -2147, result: -846384, remains: -4
  // value: -214, result: -8463847, remains: -7
  // value: -21, result: -84638474, remains: -4
  // value: -2, result: -846384741, remains: -1
  // value: 0, result: -8463847412, remains: -2
  //
  // Actual: 126087180 (of type int)

  EXPECT_THAT(reverse_integer(-2147483648), 0);
}


// ={=========================================================================
// algo-leetcode-3
/*
9. Palindrome Number, Easy

Determine whether an integer is a palindrome. An integer is a palindrome when it
reads the same backward as forward.

Example 1:

Input: 121
Output: true

Example 2:

Input: -121
Output: false

Explanation: From left to right, it reads -121. From right to left, it becomes
121-. Therefore it is not a palindrome.

Example 3:

Input: 10
Output: false

Explanation: Reads 01 from right to left. Therefore it is not a palindrome.

Follow up:
Coud you solve it without converting the integer to a string?

*/

namespace leetcode_easy_003
{
  // int reverse_integer(int value)
  //
  // Runtime: 108 ms, faster than 97.64% of C++ online submissions for
  // Palindrome Number.
  //
  // Memory Usage: 73 MB, less than 63.19% of C++ online submissions for
  // Palindrome Number.

  bool check_palindrome(int input)
  {
    long long result{};
    int remains{};

    int value = input;

    int sign{};
    sign = (input < 0) ? -1 : 1;
    if (sign < 0)
      return false;

    while (value)
    {
      // as itoa(), get right-most value
      remains = value % 10;

      // as atoi(), reverse it
      result = remains + 10 * result;

      // as itoa(), reduce value
      value /= 10;
      // cout << "value: " << value << ", result: " << result << ", remains: " << remains << endl;
    }

    // return result;
    // return result > std::numeric_limits<int>::max() ? 0 : result;
    // return (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min()) ? 0 : result;
    return result == input ? true : false;
  }
} // namespace


TEST(LeetCode, Easy_003_LongestPalindromicSubstring_0)
{
  using namespace leetcode_easy_003;

  EXPECT_THAT(check_palindrome(121), true);
  EXPECT_THAT(check_palindrome(-121), false);
  EXPECT_THAT(check_palindrome(10), false);
}

/*
The Modern C++ Challenge
28. Longest palindromic substring

Write a function that, given an input string, locates and returns the longest
sequence in the string that is a palindrome. If multiple palindromes of the same
length exist, the first one should be returned.

// from wikipedia

In computer science, the longest palindromic substring or longest *symmetric*
factor problem is the problem of finding a maximum-length contiguous substring
of a given string that is also a palindrome. 

For example, the longest palindromic substring of "bananas" is "anana". The
longest palindromic substring is not guaranteed to be unique; for example, in
the string "abracadabra", there is no palindromic substring with length greater
than three, but there are two palindromic substrings with length three, namely,
"aca" and "ada". In some applications it may be necessary to return all maximal
palindromic substrings (that is, all substrings that are themselves palindromes
and cannot be extended to larger palindromic substrings) rather than returning
only one substring or returning the maximum length of a palindromic substring.

{
   using namespace std::string_literals;
   assert(longest_palindrome("sahararahnide") == "hararah");
   assert(longest_palindrome("level") == "level");
   assert(longest_palindrome("s") == "s");
}

Unlike number version, the difference is "string" and "largest"

*/

namespace U28_2018_12_03
{
  // palindrome is *symmetric* substr while moving i from input text.
  //
  // start from 0th and for every char of input:
  //  scan to right and left from that
  //  keep update current symetric substr and save it if that's longer
  //  return the input when there's no finding such as single input

  std::string longest_palindrome(const std::string text)
  {
    auto length = text.size();
    int lidx{}, ridx{};
    char current_char{};
    string current{}, saved{};

    // moving i
    for (decltype(length) i = 0; i < length; ++i)
    {
      // whenever moves i, start search again 
      current.clear();
      current_char = text[i];

      current.push_back(current_char);

      for (lidx = i - 1, ridx = i + 1; 
          lidx >= 0 && ridx < (int)length; 
          --lidx, ++ridx)
      {
        if (text[lidx] == text[ridx])
        {
          current.insert(0, 1, text[lidx]);
          current.push_back(text[lidx]);
          if (saved.length() < current.length())
            saved = current;
        } 
        else
        {
          // exit as soon as break symmetric condition
          break;
        }
      }
    }

    // to pass the case when single input char
    if (saved.length() < current.length())
      saved = current;

    return saved;
  }

} // namespace

// TEST(U28, 2018_12_03)
TEST(LeetCode, Easy_003_LongestPalindromicSubstring_1)
{
  using namespace U28_2018_12_03;

  EXPECT_THAT(longest_palindrome("sahararahnide"), "hararah");
  EXPECT_THAT(longest_palindrome("level"), "level");

  // this fails
  // EXPECT_THAT(longest_palindrome("ss"), "ss");

  EXPECT_THAT(longest_palindrome("s"), "s");
}

/*

The simplest solution to this problem is to try a brute-force approach, checking
if each substring is a palindrome. However, this means we need to check C(N, 2)
substrings (where N is the number of characters in the string), and the time
complexity would be O(N^3). 

The complexity could be reduced to O(N^2) by storing results of sub problems. To
do so we need a table of Boolean values, of size, where 

  the key idea:
  the element at [i, j] indicates whether the substring from position i to j is
  a palindrome. 

We start by initializing all elements [i,i] with true (one-character
palindromes) and all the elements [i,i+i] with true for all consecutive two
identical characters (for two-character palindromes). We then go on to inspect
substrings greater than two characters, setting the element at [i,j] to true if
the element at [i+i,j-1] is true and the characters on the positions i and j in
the string are also equal. Along the way, we retain the start position and
length of the longest palindromic substring in order to extract it after
finishing computing the table.
 
*/

namespace U28_Text
{
  std::string longest_palindrome(std::string str)
  {
    size_t const len = str.size();
    size_t longestBegin = 0;
    size_t maxLen = 1;

    std::vector<bool> table(len * len, false);

    // diagonal elements
    // We start by initializing all elements [i,i] with true (one-character
    // palindromes)
 
    for (size_t i = 0; i < len; i++)
    {
      table[i*len + i] = true;
    }

    // and all the elements [i,i+i] with true for all consecutive two identical
    // characters (for two-character palindromes). We then go on to inspect
    //
    // why "len -1"? since needs two chars to inspect
    // 0 1 2 3 4, len is 5 
    // l e v e 1

    for (size_t i = 0; i < len - 1; ++i)
    {
      if (str[i] == str[i + 1])
      {
        table[i*len + i + 1] = true;
        if (maxLen < 2)
        {
          longestBegin = i;
          maxLen = 2;
        }
      }
    }

    // k = 3 which means see three elements range
    //
    // We then go on to inspect substrings greater than two characters, setting
    // the element at [i,j] to true if the element at [i+1,j-1] is true and the
    // characters on the positions i and j in the string are also equal. 
    //
    // Along the way, we retain the start position and length of the longest
    // palindromic substring in order to extract it after finishing computing
    // the table.

    for (size_t k = 3; k <= len; k++)
    {
      for (size_t i = 0; i < len - k + 1; i++)
      {
        size_t j = i + k - 1;

        cout << "k: " << k << ", i: " << i << ", j: " << j << ", table[" << ((i + 1)*len + j - 1) << "]:" 
          << table[(i + 1)*len + j - 1] << endl;

        if (str[i] == str[j] && table[(i + 1)*len + j - 1])
        {
          table[i*len +j] = true;

          cout << "k: " << k << ", i: " << i << ", j: " << j << ", table[" << ((i + 1)*len + j - 1) << "]:" 
            << table[(i + 1)*len + j - 1] << ", table[" << i*len +j << "] = true" << endl;

          if (maxLen < k)
          {
            longestBegin = i;
            maxLen = k;
          }
        }
      }
    }

    return std::string(str.substr(longestBegin, maxLen));
  }

} // namespace

//       0 1 2 3 4
//       l e v e l
// 0, l  o       *
// 1, e    o   *
// 2, v      o x
// 3, e        o
// 4, l          o
// 
// i = 0, maxLen = 5
//
// why chekc "table[(i + 1)*len + j - 1]"? To see substr is symetric. ex, to see
// [0, 4] is symetic, check substr(1,3) is symetric, that is [1, 3] 


TEST(LeetCode, Easy_003_LongestPalindromicSubstring_2)
{
  using namespace U28_Text;

  // EXPECT_THAT(longest_palindrome("sahararahnide"), "hararah");
 
  // k: 3, i: 0, j: 2, table[6]:1
  // k: 3, i: 1, j: 3, table[12]:1
  // k: 3, i: 1, j: 3, table[12]:1, table[8] = true
  // k: 3, i: 2, j: 4, table[18]:1
  // k: 4, i: 0, j: 3, table[7]:0
  // k: 4, i: 1, j: 4, table[13]:0
  // k: 5, i: 0, j: 4, table[8]:1
  // k: 5, i: 0, j: 4, table[8]:1, table[4] = true

  EXPECT_THAT(longest_palindrome("level"), "level");

  // EXPECT_THAT(longest_palindrome("ss"), "ss");
  // EXPECT_THAT(longest_palindrome("s"), "s");
}


// ={=========================================================================
// algo-leetcode-4
/*
13. Roman to Integer, Easy

Roman numerals are represented by seven different symbols: I, V, X, L, C, D and M.

Symbol       Value
I             1
V             5
X             10
L             50
C             100
D             500
M             1000

For example, two is written as II in Roman numeral, just two one's added
together. Twelve is written as, XII, which is simply X + II. The number twenty
seven is written as XXVII, which is XX + V + II.

Roman numerals are usually written largest to smallest from left to right.
However, the numeral for four is not IIII. Instead, the number four is written
as IV. Because the one is before the five we subtract it making four. The same
principle applies to the number nine, which is written as IX. There are six
instances where subtraction is used:

I can be placed before V (5) and X (10) to make 4 and 9. 
X can be placed before L (50) and C (100) to make 40 and 90. 
C can be placed before D (500) and M (1000) to make 400 and 900.

Given a roman numeral, convert it to an integer. Input is guaranteed to be
within the range from 1 to 3999.

see algo-roman for integer to roman conversion which is rated as:

12. Integer to Roman, Medium

*/

namespace leetcode_easy_004
{
  // Runtime: 52 ms, faster than 97.00% of C++ online submissions for Roman to
  // Integer.
  //
  // Memory Usage: 30.7 MB, less than 96.08% of C++ online submissions for Roman
  // to Integer.

  int convert_roman_to_integer(std::string input)
  {
    // do not use this from since "M" is deduced to "const char*" but want to
    // use it as string so that cah use size() on it. Or can add size member in
    // the table.
    //
    // const auto table = {
    //   make_pair("M", 1000),
    //   make_pair("CM", 900)
    // };

    // for each char of the table, see if there is matched substr using running
    // start position and string size. contine doing so until there is no match
    // and if there is no match, move on to the next char.

    const initializer_list<pair<std::string, int>> table = {
      {"M", 1000},
      {"CM", 900},
      {"D", 500},
      {"CD", 400},
      {"C", 100},
      {"XC", 90},
      {"L", 50},
      {"XL", 40},
      {"X", 10},
      {"IX", 9},
      {"V", 5},
      {"IV", 4},
      {"I", 1}
    };

    size_t run{};
    int result{};

    for (const auto& e : table)
    {
      while ((run < input.size()) && (input.substr(run, e.first.size()) == e.first))
      {
        // cout << "first: " << e.first << endl;
        result += e.second;
        run += e.first.size();
      }
    }
    // cout << "result: " << result << endl;

    return result;
  }
} // namespace


TEST(LeetCode, Easy_004_RomanToInteger)
{
  using namespace leetcode_easy_004;
  const auto func = convert_roman_to_integer;

  EXPECT_THAT(func("I"), 1);
  EXPECT_THAT(func("II"), 2);
  EXPECT_THAT(func("III"), 3);
  EXPECT_THAT(func("IV"), 4);
  EXPECT_THAT(func("V"), 5);
  EXPECT_THAT(func("VI"), 6);
  EXPECT_THAT(func("VII"), 7);
  EXPECT_THAT(func("VIII"), 8);
  EXPECT_THAT(func("IX"), 9);
  EXPECT_THAT(func("X"), 10);
  EXPECT_THAT(func("XI"), 11);
  EXPECT_THAT(func("XII"), 12);
  EXPECT_THAT(func("XIII"), 13);
  EXPECT_THAT(func("XVI"), 16);
  EXPECT_THAT(func("XVII"), 17);
  EXPECT_THAT(func("XVIII"), 18);
  EXPECT_THAT(func("XX"), 20);
  EXPECT_THAT(func("XXIII"), 23);
  EXPECT_THAT(func("XLI"), 41);
  EXPECT_THAT(func("XLV"), 45);
  EXPECT_THAT(func("L"), 50);
  EXPECT_THAT(func("LXXX"), 80);
  EXPECT_THAT(func("XCI"), 91);
  EXPECT_THAT(func("XCV"), 95);
  EXPECT_THAT(func("C"), 100);
  EXPECT_THAT(func("CXXII"), 122);
  EXPECT_THAT(func("CLII"), 152);
  EXPECT_THAT(func("CXCVI"), 196);
  EXPECT_THAT(func("CCXLVII"), 247);
  EXPECT_THAT(func("CCLXXXVIII"), 288);
  EXPECT_THAT(func("CCXCVIII"), 298);
  EXPECT_THAT(func("D"), 500);
  EXPECT_THAT(func("M"), 1000);
  EXPECT_THAT(func("MDXIII"), 1513);
  EXPECT_THAT(func("MMCMXCIX"), 2999);
  EXPECT_THAT(func("MMMCDXLVII"), 3447);
}


// ={=========================================================================
// algo-leetcode-5
/*
14. Longest Common Prefix, Easy

Write a function to find the longest common prefix string amongst an array of
strings.

If there is no common prefix, return an empty string "".

Example 1:

Input: ["flower","flow","flight"]
Output: "fl"
Example 2:

Input: ["dog","racecar","car"]
Output: ""
Explanation: There is no common prefix among the input strings.

Note:
All given inputs are in lowercase letters a-z.

*/

namespace leetcode_easy_005
{
  // own approach which fails on:
  //
  // fail 0: 
  // input []
  // 
  // fail 1:
  // input ["aa","aa"]
  // input ["aa","bb"]

  string longestCommonPrefix_01(const vector<string>& strs) 
  {
    vector<unsigned int> table(26, 0);

    auto num_of_input = strs.size();
    unsigned int common{};

    // to handle fail 0
    if (!num_of_input)
      return "";

    // build table from input strings
    for (const auto& e : strs)
    {
      for (const auto& c : e)
        table[c - 'a'] += 1;
    }

    // take one input
    for (const auto& c : strs[0])
    {
      if (table[c - 'a'] != num_of_input)
        break;

      ++common;
    }

    return strs[0].substr(0, common);
  }


  // Approach 1: Horizontal scanning
  // 
  // For a start we will describe a simple way of finding the longest prefix
  // shared by a set of strings LCP(S1 ... Sn)
  // 
  // We will use the observation that :
  // 
  // LCP(S1 ... Sn) = LCP(LCP(LCP(S1, S2),S3), ... Sn)
  // 
  // Algorithm
  // 
  // To employ this idea, the algorithm iterates through the strings [S1 ...
  // Sn], finding at each iteration i the longest common prefix of strings
  // LCP(S1 ...  Si)
  // 
  // When LCP(S1 ... Si) is an empty string, the algorithm ends. Otherwise after
  // nn iterations, the algorithm returns LCP(S1 ... Sn)
  //
  // Complexity Analysis
  //
  // Time complexity : O(S), where S is the sum of all characters in all strings.
  //
  // In the *worst case* all n strings are the same. The algorithm compares the
  // string S1 with the other strings [S2 ... Sn]. There are S character
  // comparisons, where S is the sum of all characters in the input array.
  // 
  // Space complexity : O(1)O(1). We only used constant extra space. 

  string longestCommonPrefix_02(const vector<string>& strs) 
  {
    auto num_of_input = strs.size();

    if (!num_of_input)
      return "";

    std::string lcp = strs[0];

    for (size_t i = 1; i < num_of_input; ++i)
    {
      int idx{};
      int len = min(lcp.size(), strs[i].size());

      for (idx = 0; idx < len; ++idx)
      {
        // cout << "idx: " << idx << ", lcp[idx]:" << lcp[idx] 
        //   << ", strs[i][idx]:" << strs[i][idx] << endl;

        // ok, found uncommon char between two
        // however, do not call substr on this if and if do, will not cover when
        // all chars matches.
        //
        // so this loop is to get idx but not do substr.

        if (lcp[idx] != strs[i][idx])
          break;
      }

      lcp = lcp.substr(0, idx);
    }

    return lcp;
  }
} // namespace

TEST(LeetCode, Easy_005_LongestCommonPrefix)
{
  using namespace leetcode_easy_005;

  {
    const auto func = longestCommonPrefix_01;

    EXPECT_THAT(func(std::vector<std::string>{"flower","flow","flight"}), "fl");
    EXPECT_THAT(func(std::vector<std::string>{"dog","racecar","car"}), "");

    // should be "aa"
    EXPECT_THAT(func(std::vector<std::string>{"aa","aa"}), "");

    // should be ""
    EXPECT_THAT(func(std::vector<std::string>{"aa","bb"}), "aa");
  }

  {
    const auto func = longestCommonPrefix_02;

    EXPECT_THAT(func(std::vector<std::string>{"flower","flow","flight"}), "fl");
    EXPECT_THAT(func(std::vector<std::string>{"dog","racecar","car"}), "");
    
    EXPECT_THAT(func(std::vector<std::string>{"aa","aa"}), "aa");
    EXPECT_THAT(func(std::vector<std::string>{"aa","bb"}), "");
  }
}


// ={=========================================================================
// algo-leetcode-6
/*
20. Valid Parentheses, Easy

Given a string containing just the characters '(', ')', '{', '}', '[' and ']',
determine if the input string is valid.

An input string is valid if:

Open brackets must be closed by the same type of brackets.
Open brackets must be closed in the correct order.
Note that an empty string is also considered valid.

Example 1:
Input: "()"
Output: true

Example 2:
Input: "()[]{}"
Output: true

Example 3:
Input: "(]"
Output: false

Example 4:
Input: "([)]"
Output: false

Example 5:
Input: "{[]}"
Output: true
*/

namespace leetcode_easy_006
{
  bool ValidParentheses_01(string input)
  {
    std::stack<char> coll;

    for (const auto e : input)
    {
      switch (e)
      {
        case '(':
        case '[':
        case '{':
          // cout << "push: " << e << endl;
          coll.push(e);
          break;

        case ']':

          // cout << "]: " << e << endl;

          if (coll.empty())
            return false;
          else if (!coll.empty() && ('[' != coll.top()))
            return false;
          else
            coll.pop();

          break;

        case ')':

          // cout << "): " << e << endl;

          if (coll.empty())
            return false;
          else if (!coll.empty() && ('(' != coll.top()))
            return false;
          else
            coll.pop();

          break;

        case '}':

          // cout << "}: " << e << endl;

          if (coll.empty())
            return false;
          else if (!coll.empty() && ('{' != coll.top()))
            return false;
          else
            coll.pop();

          break;

          // for other chars
        default:
          break;
      } // switch
    } // for

    // should be empty if all matches up
    return coll.empty() ? true : false;
  }

  // algo-stack
  //
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Valid
  // Parentheses.  
  //
  // Memory Usage: 8.9 MB, less than 81.58% of C++ online submissions for Valid
  // Parentheses.

  bool ValidParentheses_02(string input)
  {
    std::stack<char> coll;

    for (const auto e : input)
    {
      if (e == '(' || e == '[' || e == '{')
        coll.push(e);

      if (e == ')' || e == ']' || e == '}')
      {
        if (coll.empty())
          return false;
        else
        {
          // do not need to keep item since will return as soon as see
          // not-match.

          auto prev = coll.top();
          coll.pop();

          auto match = (prev == '(' && e == ')') 
            || (prev == '[' && e == ']')
            || (prev == '{' && e == '}');

          if (!match)
            return false;
        }
      }

      // do not handle other chars

    } // for

    // should be empty if all matches up
    return coll.empty() ? true : false;
  }
} // namespace

TEST(LeetCode, Easy_006_ValidParentheses)
{
  using namespace leetcode_easy_006;

  {
    const auto func = ValidParentheses_01;

    EXPECT_THAT(func("()"), true);
    EXPECT_THAT(func("()[]{}"), true);
    EXPECT_THAT(func("(]"), false);
    EXPECT_THAT(func("([)]"), false);
    EXPECT_THAT(func("{[]}"), true);
    EXPECT_THAT(func(""), true);
    EXPECT_THAT(func("([{}"), false);

    EXPECT_THAT(func("abc(defg{hijk}lmn)opq"), true);
  }

  {
    const auto func = ValidParentheses_02;

    EXPECT_THAT(func("()"), true);
    EXPECT_THAT(func("()[]{}"), true);
    EXPECT_THAT(func("(]"), false);
    EXPECT_THAT(func("([)]"), false);
    EXPECT_THAT(func("{[]}"), true);
    EXPECT_THAT(func(""), true);
    EXPECT_THAT(func("([{}"), false);

    EXPECT_THAT(func("abc(defg{hijk}lmn)opq"), true);

    EXPECT_THAT(func("{a = (1 + v(b[3 + c[4]]))"), false);
    EXPECT_THAT(func("{ a = (b[0) + 1]; }"), false);
  }
}


// ={=========================================================================
// algo-leetcode-7
/*
21. Merge Two Sorted Lists, Easy

Merge two sorted linked lists and return it as a new list. The new list should
be made by splicing together the nodes of the first two lists.

Example:

Input: 1->2->4, 1->3->4
Output: 1->1->2->3->4->4

//
// Definition for singly-linked list.
// struct ListNode {
//     int val;
//     ListNode *next;
//     ListNode(int x) : val(x), next(NULL) {}
// };

class Solution {
public:
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    }
};

*/

namespace leetcode_easy_007
{
  struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
  };

  ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) 
  {
    if (l1 == nullptr && l2 != nullptr)
      return l1;
    else if (l1 != nullptr && l2 == nullptr)
      return l2;
    else if (l1 == nullptr && l2 == nullptr)
      return nullptr;

    auto first = l1;
    auto second = l2;

    ListNode *result_head{};
    ListNode *result_end{};

    for (; first != nullptr && (second != nullptr);)
    {
      // ascending order
      if (first->val <= second->val)
      {
        if(!result_head)
        {
          result_head = result_end = first;
        }

        result_end->next = first;
        result_end = first;

        first = first->next;
      }
      else
      {
        if(!result_head)
        {
          result_head = result_end = second;
        }

        result_end->next = second;
        result_end = second;

        second = second->next;
      }
    }

    // second has some left
    if (first == nullptr && second != nullptr)
      result_end->next = second;
    // first has some left
    else if (first != nullptr && second == nullptr)
      result_end->next = first;
    else
    {
      // no left from the both
    }

    return result_head;
  }

  void print_list(ListNode* l1) 
  {
    ListNode *run = l1;

    for(; run; run = run->next)
    {
      cout << "val : " << run->val << endl;
    }
  }
} // namespace

// Input: 1->2->4, 1->3->4
// Output: 1->1->2->3->4->4
TEST(LeetCode, Easy_007_MergeSortedList)
{
  using namespace leetcode_easy_007;

  ListNode e1(1);
  ListNode e2(2);
  ListNode e3(4);

  e1.next = &e2;
  e2.next = &e3;

  print_list(&e1);

  ListNode s1(1);
  ListNode s2(3);
  ListNode s3(4);

  s1.next = &s2;
  s2.next = &s3;

  print_list(&s1);

  cout << "====" << endl;
  auto x = mergeTwoLists(&e1, &s1); 
  print_list(x);
}


// ={=========================================================================
// algo-leetcode-8
/*
26. Remove Duplicates from Sorted Array, Easy

Given a sorted array nums, remove the duplicates in-place such that each element
appear only once and return *the new length.*

Do not allocate extra space for another array, you must do this by modifying the
input array in-place with O(1) extra memory.

Example 1:

Given nums = [1,1,2],

Your function should return length = 2, with the first two elements of nums
being 1 and 2 respectively.

It doesn't matter what you leave beyond the returned length.

Example 2:

Given nums = [0,0,1,1,1,2,2,3,3,4],

Your function should return length = 5, with the first five elements of nums
being modified to 0, 1, 2, 3, and 4 respectively.

It doesn't matter what values are set beyond the returned length.

Clarification:

Confused why the returned value is an integer but your answer is an array?

Note that the input array is passed in by reference, which means modification to
the input array will be known to the caller as well.

Internally you can think of this:

// nums is passed in by reference. (i.e., without making a copy)
int len = removeDuplicates(nums);

// any modification to nums in your function would be known by the caller.
// using the length returned by your function, it prints the first len elements.

for (int i = 0; i < len; i++) {
    print(nums[i]);
}

*/

namespace leetcode_easy_008
{
  // num is sorted (ascending)
  // the key idea is to swap to the right
  int RemoveDuplicates_01(vector<int> &nums)
  {
    if (nums.empty())
      return 0;

    int value = nums[0];
    size_t i{};

    for (i = 1; i < nums.size(); ++i)
    {
      int run = nums[i];

      // cout << "for i: " << i << ", run: " << run 
      //   << ", value: " << value << endl;

      // update current max when current value is bigger
      if (run > value)
        value = run;
      // ends when see smaller and means reaches the the new end
      else if(run < value)
      {
        // cout << "break i: " << i << endl;
        break;
      }
      // when run == value, swap it to tne end.
      else
      {
        for (size_t s = i; s < nums.size()-1; ++s)
          swap(nums[s], nums[s+1]);

        if (nums[i] > value)
          value = nums[i];
      }
    }

    // cout << "return i: " << i << endl;

    return i;
  }

  // o the key idea is to swap to the left
  // o no repeated swap until see the new end. single swap is enough 
  // o swap() should be done after updating current_max
  //
  // o end is index but shold return len so +1 -> revised. As algo-partition,
  // end represet start of not-interested, that is end of interested group. so
  // no need to +1. have to think about swap on the same index

  // Runtime: 24 ms, faster than 100.00% of C++ online submissions for Remove
  // Duplicates from Sorted Array.
  //
  // Memory Usage: 11 MB, less than 24.81% of C++ online submissions for Remove
  // Duplicates from Sorted Array.

  int RemoveDuplicates_02(vector<int> &nums)
  {
    if (nums.empty())
      return 0;

    int current_max = nums[0];
    size_t end{1};

    for (size_t i = 1; i < nums.size(); ++i)
    {
      if (nums[i] > current_max)
      {
        current_max = nums[i];
        if (end != i)
          swap(nums[end], nums[i]);
        ++end;
      }
    }

    return end;
  }
  
  using ITERATOR = vector<int>::iterator;

  ITERATOR adjacent_find(ITERATOR first, ITERATOR last)
  {
    if (first == last)
      return last;

    auto next = first;
    while (++next != last)
    {
      // found two consecutive items
      if (*first == *next)
        return first;

      first = next;
    }

    // no two consecutive items found.
    return last;
  }

  // algo-unique, same as unique_1()
  // `first` is end of the interested group

  int RemoveDuplicates_03(vector<int> &nums)
  {
    auto first = nums.begin();
    auto last = nums.end();

    auto end = adjacent_find(first, last);

    // means empty or no duplicates
    if (end == last)
      return 0;

    auto run = end;

    while (++run != last)
    {
      // see different item
      if (*end != *run)
        *++end = *run;
    }

    return distance(first, end) + 1;
  }

} // namespace

TEST(LeetCode, Easy_008_RemoveDuplicates)
{
  using namespace leetcode_easy_008;

  // okay
  {
    const auto func = RemoveDuplicates_01;

    vector<int> coll{1,1,2};
    auto len = func(coll);

    EXPECT_THAT(len, 2);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1,2));
  }

  // fails
  {
    const auto func = RemoveDuplicates_01;

    vector<int> coll{0,0,1,1,1,2,2,3,3,4};
    auto len = func(coll);

    EXPECT_THAT(len, Not(5));

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, Not(ElementsAre(0,1,2,3,4)));
  }

  {
    const auto func = RemoveDuplicates_02;

    vector<int> coll{1,1,2};
    auto len = func(coll);

    EXPECT_THAT(len, 2);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1,2));
  }

  {
    const auto func = RemoveDuplicates_02;

    vector<int> coll{0,0,1,1,1,2,2,3,3,4};
    auto len = func(coll);

    EXPECT_THAT(len, 5);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(0,1,2,3,4));
  }

  // same ex as AlgoUnique
  {
    const auto func = RemoveDuplicates_03;

    vector<int> coll{1, 4, 4, 6};
    auto len = func(coll);

    EXPECT_THAT(len, 3);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1,4,6));
  }
  {
    const auto func = RemoveDuplicates_03;

    vector<int> coll{1, 4, 4, 4, 6};
    auto len = func(coll);

    EXPECT_THAT(len, 3);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1,4,6));
  }
  {
    const auto func = RemoveDuplicates_03;

    vector<int> coll{1, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 5, 7, 5, 4, 4};

    auto len = func(coll);

    EXPECT_THAT(coll, 
        ElementsAreArray({1,4,6,1,2,3,1,6,5,7,5,4,5,7,5,4,4}));
    EXPECT_THAT(len, 12);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, 
        ElementsAreArray({1,4,6,1,2,3,1,6,5,7,5,4}));
  }
}


// ={=========================================================================
// algo-leetcode-9
/*
27. Remove Element, Easy

Given an array nums and a value val, remove all instances of that value in-place
and return the new length.

Do not allocate extra space for another array, you must do this by modifying the
input array in-place with O(1) extra memory.

The order of elements can be changed. It doesn't matter what you leave beyond
the new length.

Example 1:

Given nums = [3,2,2,3], val = 3,

Your function should return length = 2, with the first two elements of nums
being 2.

It doesn't matter what you leave beyond the returned length.

Example 2:

Given nums = [0,1,2,2,3,0,4,2], val = 2,

Your function should return length = 5, with the first five elements of nums
containing 0, 1, 3, 0, and 4.

Note that the order of those five elements can be arbitrary.

It doesn't matter what values are set beyond the returned length.

Clarification:

Confused why the returned value is an integer but your answer is an array?

Note that the input array is passed in by reference, which means modification to
the input array will be known to the caller as well.

Internally you can think of this:

// nums is passed in by reference. (i.e., without making a copy)
int len = removeElement(nums, val);

// any modification to nums in your function would be known by the caller.
// using the length returned by your function, it prints the first len elements.
for (int i = 0; i < len; i++) {
    print(nums[i]);
}

*/

namespace leetcode_easy_009
{
  // o Unlike RemoveDuplicates_02, end is not index but index+1

  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Remove
  // Element.
  //
  // Memory Usage: 9.3 MB, less than 56.68% of C++ online submissions for Remove
  // Element.

  int RemoveIf_01(vector<int> &nums, int val)
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

} // namespace

TEST(LeetCode, Easy_009_RemoveIf)
{
  using namespace leetcode_easy_009;

  {
    const auto func = RemoveIf_01;

    vector<int> coll{3,2,2,3};
    auto len = func(coll, 3);

    EXPECT_THAT(len, 2);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(2,2));
  }

  {
    const auto func = RemoveIf_01;

    vector<int> coll{0,1,2,2,3,0,4,2};
    auto len = func(coll, 2);

    EXPECT_THAT(len, 5);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(0,1,3,0,4));
  }
}


// ={=========================================================================
// algo-leetcode-10
/*
28. Implement strStr(), Easy

Implement strStr().

Return the index of the first occurrence of needle in haystack, or -1 if needle
is not part of haystack.

Example 1:
Input: haystack = "hello", needle = "ll"
Output: 2

Example 2:
Input: haystack = "aaaaa", needle = "bba"
Output: -1

Clarification:

What should we return when needle is an empty string? This is a great question
to ask during an interview.

For the purpose of this problem, we will return 0 when needle is an empty
string. This is consistent to C's strstr() and Java's indexOf().

STRSTR(3)

Linux Programmer's Manual

NAME
       strstr, strcasestr - locate a substring

DESCRIPTION
       The strstr() function finds the first occurrence of the substring needle
       in the string haystack.  The terminating null bytes ('\0') are not
       compared.

       The strcasestr() function is like strstr(), but ignores the case of both
       arguments.

*/

namespace leetcode_easy_010
{
  // Runtime: 8 ms, faster than 99.35% of C++ online submissions for Implement
  // strStr().
  //
  // Memory Usage: 9.3 MB, less than 96.70% of C++ online submissions for
  // Implement strStr().

  int strStr_1(string haystack, string needle) 
  {
    int result{-1};
    size_t count{};

    if (haystack == needle || needle.empty())
      return 0;

    if (haystack.size() < needle.size())
      return result;

    for (size_t hay = 0; hay < haystack.size(); ++hay)
    {
      if (haystack[hay] == needle[0] 
          && ((hay + needle.size() -1) < haystack.size()))
      {
        for (size_t ni = 0, hi = hay; ni < needle.size(); ++ni, ++hi)
          if (haystack[hi] == needle[ni])
            ++count;

        // cout << "needle: " << needle << ", count: " << count << ", result: " << result << endl;

        if(count == needle.size())
        {
          result = hay;
          // cout << "needle: " << needle << ", count: " << count << ", result: " << result << endl;
          return result;
        }

        // do not hit return above and means not found match. so reset it
        count = 0;
      }
    }

    return result;
  }

  // int strStr_2(string haystack, string needle) 
  // {
  //   hay_size = haystack.size();
  //   needle_size = needle.size();
  //   size_t pos{};

  //   if (needle_size <= hay_size)
  //   {
  //     for (; pos <= hay_size - needle_size; ++pos)

  //   }
  // }

} // namespace

TEST(LeetCode, Easy_010_StrStr)
{
  using namespace leetcode_easy_010;

  EXPECT_THAT(strStr_1("hello", "ll"), 2);
  EXPECT_THAT(strStr_1("aaaaa", "bba"), -1);

  // as to problem description
  EXPECT_THAT(strStr_1("", ""), 0);

  EXPECT_THAT(strStr_1("", "a"), -1);

  // mississippi"
  //  issipi"
  //     issipi"

  EXPECT_THAT(strStr_1("mississippi", "issipi"), -1);


  EXPECT_THAT(string("hello").find("ll"), 2);
  EXPECT_THAT(string("aaaaa").find("bba"), string::npos);
  EXPECT_THAT(string("aaaaa").find("bba"), -1);
}


// ={=========================================================================
// algo-leetcode-11
/*
35. Search Insert Position, Easy

Given a sorted array and a target value, return the index if the target is
found. If not, return the index where it would be if it were inserted in order.

You may assume no duplicates in the array.

Example 1:
Input: [1,3,5,6], 5
Output: 2

Example 2:
Input: [1,3,5,6], 2
Output: 1

Example 3:
Input: [1,3,5,6], 7
Output: 4

Example 4:
Input: [1,3,5,6], 0
Output: 0

*/

// see algo-binary-search


// ={=========================================================================
// algo-leetcode-12
/*
38. Count and Say, Easy

The count-and-say sequence is the sequence of integers with the first five terms
as following:

1.     1
2.     11
3.     21
4.     1211
5.     111221

1 is read off as "one 1" or 11.

11 is read off as "two 1s" or 21.

21 is read off as "one 2, then one 1" or 1211.

Given an integer n where 1 ≤ n ≤ 30, generate the nth term of the count-and-say
sequence.

Note: Each term of the sequence of integers will be represented as a string.

Example 1:

Input: 1
Output: "1"

Example 2:
Input: 4
Output: "1211"

*/


// ={=========================================================================
// algo-list

// single
//
// class List
// {
//   public:
//     void clear();
//     bool empty();
//     int size();
//
//     // as push_back();
//     void push(ListEntry const& entry);
//
//     // as traverse()
//     std::vector<ListEntry> snap();
// };

namespace algo_list_linked
{
  // when node and entry are in a single structure and these can be different
  // structure such as ListEntry and ListNode

  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept 
      : row_(row), col_(col), next_(nullptr) 
    {}

    int row_{};
    int col_{};

    ListEntry* next_;
  };

  // cxx-operator-overload
  bool operator==(ListEntry const& lhs, ListEntry const& rhs)
  {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_) ? true : false;
  }

  bool operator!=(ListEntry const& lhs, ListEntry const& rhs)
  {
    return !(lhs == rhs);
  }

  class List
  {
    public:
      explicit List() noexcept
        : head_(nullptr)
        {}

      bool emptry()
      { return count_ == 0 ? true : false; }

      int size()
      { return count_; }

      // push_back()
      void push_old(ListEntry const& entry)
      {
        if (!head_)
          head_ = new ListEntry(entry);
        else
        {
          ListEntry* run = head_;

          // unlike clear(), snap(), run shall be before end() so that can
          // insert new one. Hence check run->next

          while (run->next_)
            run = run->next_;

          run->next_ = new ListEntry(entry);
        }
        
        ++count_;
      }

      // push_back()
      void push(ListEntry const& entry)
      {
        ListEntry* run{};

        // find node for insertion *algo-list-find-end*
        // works both when head_ is null and is not null

        for (run = head_; run && run->next_; run = run->next_)
          ;

        // first item
        if (!run)
          head_ = new ListEntry(entry);
        else
          run->next_ = new ListEntry(entry);

        ++count_;
      }

      void clear()
      {
        ListEntry* run = head_;
        ListEntry* prev{};

        while (run)
        {
          prev = run;
          run = run->next_;
          free(prev);
          --count_;
        }

        head_ = run;
      }

      std::vector<ListEntry> snap()
      {
        ListEntry* run = head_;
        std::vector<ListEntry> coll;

        while (run)
        {
          // ok as well
          // coll.push_back(ListEntry(*run));
          coll.push_back(*run);
          run = run->next_;
        }

        return coll;
      }

    private:
      int count_{};

      // can use ListEntry head_; which changes member implementation

      ListEntry* head_;
  };

} // namespace


TEST(AlgoList, LinkedSimple)
{
  using namespace algo_list_linked;

  std::vector<ListEntry> values{
    ListEntry(1,2), 
    ListEntry(2,3), 
    ListEntry(3,4), 
    ListEntry(4,5), 
    ListEntry(5,6)
  };

  List coll;

  for (auto &e : values)
    coll.push(e);

  EXPECT_THAT(coll.size(), 5);

  coll.push(ListEntry(6,7));
  EXPECT_THAT(coll.size(), 6);

  // requires cxx-operator-overload
  std::vector<ListEntry> expected{
    ListEntry(1,2), 
    ListEntry(2,3),
    ListEntry(3,4),
    ListEntry(4,5),
    ListEntry(5,6),
    ListEntry(6,7)
  };

  EXPECT_THAT(coll.snap(), expected);

  coll.clear();
  EXPECT_THAT(coll.size(), 0);
}


// ={=========================================================================
int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

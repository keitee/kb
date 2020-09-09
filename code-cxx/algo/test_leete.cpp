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

// ={=========================================================================
template <typename T>
void PRINT_ELEMENTS(T &coll, const string optstr = "")
{
  size_t count{};
  cout << optstr;

  for (const auto &elem : coll)
  {
    cout << elem << " ";
    ++count;
  }

  cout << "(" << count << ")" << endl;
}

template <typename T>
void PRINT_M_ELEMENTS(T &coll, const string optstr = "")
{
  size_t count{};
  cout << optstr;

  for (const auto &elem : coll)
  {
    cout << "(" << elem.first << ", " << elem.second << ") ";
    ++count;
  }

  cout << "(" << count << ")" << endl;
}

/*
={=========================================================================
algo-leetcode-1

1. two sum, easy

given an array of integers, return *indices* of the two numbers such that they
add up to a specific target.

you may assume that each input would have *exactly* one solution, and you may
not use the same element twice.

example:

given nums = [2, 7, 11, 15], target = 9,

because nums[0] + nums[1] = 2 + 7 = 9,
return [0, 1].

Q: what if there are duplicates in the input? will be fine since the first seen
will be used.

*/

namespace leetcode_easy_001
{
  std::vector<int> two_sum_1(std::vector<int> &nums, int target)
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
  // input. in the above, handling of equal to 0 and abs() are due to
  // "nums[first] < target" and which is not necessary.

  vector<int> two_sum_2(vector<int> &nums, int target)
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

    // not necessary since assumes that there is always a solution but need to
    // avoid warning.
    return vector<int>{-1, -1};
  }

  // All aboves are O(n^2)

  // Approach 2: Two-pass Hash Table
  // To improve our run time complexity, we need a more efficient way to check
  // if the complement exists in the array. If the complement exists, we need to
  // look up its index. What is the best way to maintain a mapping of each
  // element in the array to its index? A hash table.
  //
  // one pass to build table, map<value, index>, and one pass to find a match

  // Approach 3: One-pass Hash Table
  // turns out we can do it in one-pass. While we iterate and inserting
  // elements into the table, we also *look back* to check if current element's
  // complement already exists in the table. If it exists, we have found a
  // solution and return immediately.
  //
  // Runtime: 12 ms, faster than 97.81% of C++ online submissions for Two Sum.
  //
  // Memory Usage: 10.4 MB, less than 30.73% of C++ online submissions for Two
  // Sum.

  // old
  vector<int> two_sum_3(vector<int> &nums, int target)
  {
    // map<value, index>
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
      // since looks *the first* back into the map and `first` var is actually
      // the index of the second

      if (found != table.end())
        return vector<int>{found->second, first};

      table[nums[first]] = first;
    }

    return vector<int>{-1, -1};
  }

  // 2020.08.
  // this is not a solution since we introduce a table to remove double loop.
  std::vector<int> two_sum_4(std::vector<int> &nums, int target)
  {
    std::map<int, int> table{};

    auto size = nums.size();

    for (int first = 0; first < (int)size; ++first)
    {
      for (int second = first + 1; second < (int)size; ++second)
      {
        // insert it to table
        if (first == 0)
          table[nums[second]] = second;

        // found
        auto it = table.find(target - nums[first]);
        if (it != table.end())
        {
          return std::vector<int>{first, it->second};
        }
      }
    }

    return vector<int>{-1, -1};
  }

  // old but see use less memory than map version.

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
  vector<int> two_sum_5(vector<int> &nums, int target)
  {
    // map<value, index>
    std::unordered_map<int, int> table{};

    size_t size = nums.size();

    for (int first = 0; first < (int)size; ++first)
    {
      // search second
      auto found = table.find(target - nums[first]);

      if (found != table.end())
        return vector<int>{found->second, first};

      table[nums[first]] = first;
    }

    return vector<int>{-1, -1};
  }
} // namespace leetcode_easy_001

TEST(AlgoLeetCode, check_two_sum)
{
  using namespace leetcode_easy_001;

  {
    auto imps = {two_sum_1, two_sum_2, two_sum_3, two_sum_4, two_sum_5};

    for (auto f : imps)
    {
      {
        vector<int> nums{2, 7, 11, 15};
        auto coll = f(nums, 9);
        EXPECT_THAT(coll, ElementsAre(0, 1));
      }

      {
        vector<int> nums{7, 2, 11, 15};
        auto coll = f(nums, 9);
        EXPECT_THAT(coll, ElementsAre(0, 1));
      }

      {
        vector<int> nums{11, 15, 2, 7};
        auto coll = f(nums, 9);
        EXPECT_THAT(coll, ElementsAre(2, 3));
      }
      {
        vector<int> nums{11, 15, 2, 7, 1, 8};
        auto coll = f(nums, 9);
        EXPECT_THAT(coll, ElementsAre(2, 3));
      }

      {
        vector<int> nums{11, 15, 2, 6, 1, 8};
        auto coll = f(nums, 9);
        EXPECT_THAT(coll, ElementsAre(4, 5));
      }

      {
        vector<int> nums{0, 4, 3, 0};
        auto coll = f(nums, 0);
        EXPECT_THAT(coll, ElementsAre(0, 3));
      }

      {
        vector<int> nums{-1, -2, -3, -4, -5};
        auto coll = f(nums, -8);
        EXPECT_THAT(coll, ElementsAre(2, 4));
      }

      {
        vector<int> nums{-3, 4, 3, 90};
        auto coll = f(nums, 0);
        EXPECT_THAT(coll, ElementsAre(0, 2));
      }
    }
  }
}

/*
={=========================================================================
algo-leetcode-167

167. Two Sum II - Input array is sorted, Easy

Given an array of integers that is already sorted in ascending order, find two
numbers such that they add up to a specific target number.

The function twoSum should return indices of the two numbers such that they add
up to the target, where index1 must be less than index2.

Note:

Your returned answers (both index1 and index2) are not zero-based.  You may
assume that each input would have exactly one solution and you may not use the
same element twice.

Example:
Input: numbers = [2,7,11,15], target = 9
Output: [1,2]
Explanation: The sum of 2 and 7 is 9. Therefore index1 = 1, index2 = 2.
 

SO how "sorted" makes difference than "Two Sum I"?


From discussion:

C++ solution simple and sweet, bloomer

since input is sorted, a[0], a[1], ... a[n-1] can be on axis

 --------------------------------------------
 |<-- a -->|
 |<-------- b -------->|
 |<------------- a + b --------->|

a[0] + a[n-1] is max distance (from value 0) in this pair and

if sum > target, reduce right since need to reduce sum
if sum < target, increase left since need to increase sum

since there must be a solution in the input, there is sum == target case during
moving.

this works regardless of sign of input

*/

namespace leetcode_easy_167
{
  std::vector<int> two_sum_1(std::vector<int> &nums, int target)
  {
    int right{(int)nums.size() - 1};
    int left{0};
    int sum{};

    // since there should always be a pair to make sum
    while (left < right)
    {
      sum = nums[left] + nums[right];

      if (sum == target)
      {
        // *cxx-return*
        // return {left + 1, right + 1}; is okay but use `explicit` form.
        //
        // warning: narrowing conversion of ‘(left + 1)’ from ‘size_t {aka long unsigned int}’ to ‘int’ inside { } [-Wnarrowing]
        return std::vector<int>{left + 1, right + 1};
      }
      else if (sum > target)
        --right;
      else
        ++left;
    }

    // to avoid warning
    return std::vector<int>{0, 0};
  }
} // namespace leetcode_easy_167

TEST(AlgoLeetCode, check_two_sum_sorted)
{
  using namespace leetcode_easy_167;

  {
    auto imps = {two_sum_1};

    for (auto f : imps)
    {
      {
        std::vector<int> nums{2, 7, 11, 15};
        auto coll = f(nums, 9);
        EXPECT_THAT(coll, ElementsAre(1, 2));
      }
    }
  }
}

/*
={=========================================================================
algo-leetcode-2

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
  /*
   void simple_itoa(int value) 
   {
     while (value)
     {
       remains = value % 10;
       result += '0' + remains;
       value /= 10;
     }
  
     // reverse result string
   }
  
   void simple_atoi(std::string input)
   {
     int result{};
  
     for (int i = 0; i < input.size(); ++i)
     {
       result = input[i] - '0' + 10*result;
     }
   }

  Unlike atoi(), there is no need to handle:
  
  1. isspace() on every single char of input
  2. check and remove sign char, '-', of input
  
  However, gets some wrong answers and see test cases below. The final result
  is:
  
  Runtime: 16 ms, faster than 96.53% of C++ online submissions for Reverse
  Integer.
  
  Memory Usage: 13.9 MB, less than 85.02% of C++ online submissions for
  Reverse Integer.


  signed int and unsigned int
  
   // okay
   {
     // >>> 2**31-1
     // 2,147,483,647
  
     EXPECT_EQ(2147483647, numeric_limits<int>::max());
  
     // >>> 2**32-1
     // 4,294,967,295    UNIT_MAX
  
     EXPECT_EQ(4294967295, numeric_limits<unsigned int>::max());
   }

  */

  // old.
  int reverse_integer_1(int value)
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
    return (result > std::numeric_limits<int>::max() ||
            result < std::numeric_limits<int>::min())
             ? 0
             : result;
  }

  // assumes base 10. see the check on overflow
  int reverse_integer_2(int value)
  {
    int input{std::abs(value)};

    // NOTE:int result{};
    long long result{};

    int sign{1};

    if (value < 0)
      sign = -1;

    while (input)
    {
      result = result * 10 + input % 10;
      input  = input / 10;
    }

    // return sign * result;

    // to check overflows
    // EXPECT_THAT(f(1534236469), 0);
    return (result > std::numeric_limits<int>::max() ||
            result < std::numeric_limits<int>::min())
             ? 0
             : sign * result;
  }
} // namespace leetcode_easy_002

TEST(AlgoLeetCode, check_reverse_integer)
{
  using namespace leetcode_easy_002;

  {
    auto imps = {reverse_integer_1, reverse_integer_2};

    for (auto f : imps)
    {
      EXPECT_THAT(f(123), 321);
      EXPECT_THAT(f(-123), -321);
      EXPECT_THAT(f(120), 21);
      EXPECT_THAT(f(534236469), 964632435);

      // wrong answer when input is 1534236469. should be 0
      EXPECT_THAT(f(1534236469), 0);

      // NOTE mod(%) and div(/) do not impact on sign of integer.
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
      EXPECT_THAT(f(-2147483648), 0);
    }
  }
}

/*
={=========================================================================
algo-leetcode-5

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
  // old and fails. not get the idea now.
  //
  // should be "aa"
  // EXPECT_THAT(func(std::vector<std::string>{"aa", "aa"}), "");
  //
  // should be ""
  // EXPECT_THAT(func(std::vector<std::string>{"aa", "bb"}), "aa");

  std::string longest_common_prefix_1(const std::vector<std::string> &input)
  {
    // since all are lower cases
    std::vector<unsigned int> table{26, 0};

    auto num_input = input.size();
    unsigned int num_common{};

    if (num_input == 0)
      return "";

    // build occurance table from inputs. scan all input strings
    for (const auto &s : input)
    {
      for (const auto &c : s)
        table[c - 'a'] += 1;
    }

    // take one input. WHY ANY input?
    for (const auto &c : input[0])
    {
      if (table[c - 'a'] != num_input)
        break;

      ++num_common;
    }

    return input[0].substr(0, num_common);
  }

  // 2020.08. not implemented
  // 1. gets the size of smallest string since that's minimum to compare
  // 2. while doing loop on this size, each chars fron all input string should
  // be the smae. if not, stop the loop.
  std::string longest_common_prefix_2(const std::vector<std::string> &input)
  {
    size_t min_size{std::numeric_limits<size_t>::max()};
    std::string lcp{};

    for (const auto &s : input)
    {
      if (s.size() < min_size)
        min_size = s.size();
    }

    // num of chars to compare
    for (size_t i = 0; i < min_size; ++i)
    {
      // num of strings
      for (size_t run = 0; run < input.size(); ++run)
      {
        // compare [i] char of each string to see all are same.
        //
        // use xor?
        // cxx-xor
        // X XOR  X  = 0
        // X XOR  0  = X
        // X XOR  1  = ~X    // X XOR (~0) = ~X
        // X XOR ~X  = 1
      }
    }

    return lcp;
  }

  // Approach 1: Horizontal scanning
  //
  // For a start we will describe a simple way of finding the longest common
  // prefix shared by a set of strings LCP(S1 ... Sn)
  //
  // We will use the *observation* that :
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
  // Space complexity : O(1) We only used constant extra space.

  std::string longest_common_prefix_3(const std::vector<std::string> &input)
  {
    auto num_input = input.size();

    // check on input
    if (!num_input)
      return "";

    // set the first string as lcp
    std::string lcp{input[0]};

    for (size_t i = 1; i < num_input; ++i)
    {
      // get min size to loop
      size_t min_size = std::min(lcp.size(), input[i].size());
      size_t run{};

      for (run = 0; run < min_size; ++run)
      {
        if (lcp[run] != input[i][run])
          break;
      }

      // update lcp
      // note that substr(start, length)
      lcp = lcp.substr(0, run);
    }

    return lcp;
  }
} // namespace leetcode_easy_005

TEST(AlgoLeetCode, check_longest_common_prefix)
{
  using namespace leetcode_easy_005;

  {
    auto imps = {longest_common_prefix_3};

    for (auto f : imps)
    {
      EXPECT_THAT(f(std::vector<std::string>{"flower", "flow", "flight"}),
                  "fl");
      EXPECT_THAT(f(std::vector<std::string>{"dog", "racecar", "car"}), "");

      EXPECT_THAT(f(std::vector<std::string>{"aa", "aa"}), "aa");
      EXPECT_THAT(f(std::vector<std::string>{"aa", "bb"}), "");
    }
  }
}

/*
={=========================================================================
algo-leetcode-6

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
  // old.
  bool validate_parentheses_1(const std::string &input)
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
    }   // for

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

  bool validate_parentheses_2(const std::string &input)
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

          auto match = (prev == '(' && e == ')') || (prev == '[' && e == ']') ||
                       (prev == '{' && e == '}');

          if (!match)
            return false;
        }
      }

      // do not handle other chars

    } // for

    // should be empty if all matches up
    return coll.empty() ? true : false;
  }

  // 2020.08
  // no check on input chars. as above, need to have checks on empty before
  // doing pop/back() since
  // "Calling pop_back on an empty container results in undefined behavior."
  // actually, causes "Segmentation fault (core dumped)"
  bool validate_parentheses_3(const std::string &input)
  {
    std::vector<char> coll;

    auto return_match = [](const char c) {
      if (c == ')')
        return '(';
      if (c == '}')
        return '{';
      if (c == ']')
        return '[';

      return '0';
    };

    for (const auto e : input)
    {
      if (e == '(' || e == '{' || e == '[')
        coll.push_back(e);
      else if (e == ')' || e == '}' || e == ']')
      {
        // added from the above solution.
        if (coll.empty())
          return false;

        auto poped = coll.back();
        coll.pop_back();

        if (poped != return_match(e))
          return false;
      }
    }

    return coll.size() == 0 ? true : false;
  }
} // namespace leetcode_easy_006

TEST(AlgoLeetCode, check_valid_parentheses)
{
  using namespace leetcode_easy_006;

  {
    auto imps = {validate_parentheses_1,
                 validate_parentheses_2,
                 validate_parentheses_3};

    for (auto f : imps)
    {
      EXPECT_THAT(f("()"), true);
      EXPECT_THAT(f("()[]{}"), true);
      EXPECT_THAT(f("(]"), false);
      EXPECT_THAT(f("([)]"), false);
      EXPECT_THAT(f("{[]}"), true);
      EXPECT_THAT(f(""), true);
      EXPECT_THAT(f("([{}"), false);
      EXPECT_THAT(f("abc(defg{hijk}lmn)opq"), true);
      EXPECT_THAT(f("{a = (1 + v(b[3 + c[4]]))"), false);
      EXPECT_THAT(f("{ a = (b[0) + 1]; }"), false);

      // to see if it cause "undefined"
      EXPECT_THAT(f(")}]"), false);
    }
  }
}

/*
={=========================================================================
algo-leetcode-7

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
  struct node
  {
    int value_;
    node *next_;

    node(int value)
        : value_(value)
        , next_(nullptr)
    {}
  };

  std::vector<int> print_list(node *head)
  {
    std::vector<int> coll{};

    while (head)
    {
      // std::cout << "value: " << head->value_ << std::endl;
      coll.emplace_back(head->value_);
      head = head->next_;
    }

    return coll;
  }

  // old
  node *merge_list_1(node *l1, node *l2)
  {
    if (l1 == nullptr && l2 != nullptr)
      return l1;
    else if (l1 != nullptr && l2 == nullptr)
      return l2;
    else if (l1 == nullptr && l2 == nullptr)
      return nullptr;

    auto first  = l1;
    auto second = l2;

    node *result_head{};
    node *result_end{};

    for (; first != nullptr && (second != nullptr);)
    {
      // ascending order
      if (first->value_ <= second->value_)
      {
        if (!result_head)
        {
          result_head = result_end = first;
        }

        result_end->next_ = first;
        result_end        = first;

        first = first->next_;
      }
      else
      {
        if (!result_head)
        {
          result_head = result_end = second;
        }

        result_end->next_ = second;
        result_end        = second;

        second = second->next_;
      }
    }

    // second has some left
    if (first == nullptr && second != nullptr)
      result_end->next_ = second;
    // first has some left
    else if (first != nullptr && second == nullptr)
      result_end->next_ = first;
    else
    {
      // no left from the both
    }

    return result_head;
  }

  // 2020.08
  node *merge_list_2(node *first, node *second)
  {
    node *head{nullptr};
    node *run{nullptr};

    if (!first)
      return second;

    if (!second)
      return first;

    while (first && second)
    {
      if (first->value_ <= second->value_)
      {
        // to distinguish when it's first and is the next
        if (!run)
          run = first;
        else
          run->next_ = first;

        run   = first;
        first = first->next_;
      }
      else
      {
        if (!run)
          run = second;
        else
          run->next_ = second;

        run    = second;
        second = second->next_;
      }

      // to set head
      if (!head)
        head = run;
    }

    // now one of first or second is end; that is null

    if (!first)
      run->next_ = second;
    else if (!second)
      run->next_ = first;

    return head;
  }
} // namespace leetcode_easy_007

TEST(AlgoLeetCode, check_merge_list)
{
  using namespace leetcode_easy_007;

  {
    node n1(1);
    node n2(2);
    node n3(4);

    n1.next_ = &n2;
    n2.next_ = &n3;

    EXPECT_THAT(print_list(&n1), ElementsAre(1, 2, 4));

    node s1(1);
    node s2(3);
    node s3(4);

    s1.next_ = &s2;
    s2.next_ = &s3;

    EXPECT_THAT(print_list(&s1), ElementsAre(1, 3, 4));

    auto x = merge_list_2(&n1, &s1);

    EXPECT_THAT(print_list(x), ElementsAre(1, 1, 2, 3, 4, 4));
  }
}

/*
={=========================================================================
algo-leetcode-10

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

RETURN VALUE
       These functions return a pointer to the beginning of the located
       substring, or NULL if the substring is not found.

*/

namespace leetcode_easy_010
{
  // Runtime: 8 ms, faster than 99.35% of C++ online submissions for Implement
  // strStr().
  //
  // Memory Usage: 9.3 MB, less than 96.70% of C++ online submissions for
  // Implement strStr().

  int my_strstr_1(string haystack, string needle)
  {
    int result{-1};
    size_t count{};

    if (haystack == needle || needle.empty())
      return 0;

    if (haystack.size() < needle.size())
      return result;

    for (size_t hay = 0; hay < haystack.size(); ++hay)
    {
      if (haystack[hay] == needle[0] &&
          ((hay + needle.size() - 1) < haystack.size()))
      {
        for (size_t ni = 0, hi = hay; ni < needle.size(); ++ni, ++hi)
          if (haystack[hi] == needle[ni])
            ++count;

        // cout << "needle: " << needle << ", count: " << count << ", result: " << result << endl;

        if (count == needle.size())
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

  // 2020.08
  int my_strstr_2(std::string haystack, std::string needle)
  {
    int ret{-1};
    size_t size{haystack.size()};
    size_t run{}, start{};

    // error condition from the old although that's not the same as strstr().

    if (haystack == needle || needle.empty())
      return 0;

    if (haystack.size() < needle.size())
      return ret;

    for (; run < size;)
    {
      // find where the first char matches
      for (; run < size; ++run)
        if (haystack[run] == needle[0])
        {
          start = run;
          break;
        }

      // only when the first is found.
      if (run < size)
      {
        ++run;
        size_t match = 1;

        for (; run < size; ++run)
          if (haystack[run] != needle[match])
            break;

        if (match == needle.size() - 1)
          return start;
      }
    }

    // when not found
    return ret;
  }

  // from string_find::
  // std::string::size_type my_find(const std::string &str,
  //    const std::string &sub,
  //    std::string::size_type pos = std::string::npos);

  int my_strstr_3(std::string haystack, std::string needle)
  {
    const auto _size_hay = haystack.size();
    const auto _size_sub = needle.size();

    if (_size_sub <= _size_hay)
    {
      const char *_hay = haystack.c_str();
      const char *_sub = needle.c_str();

      auto _limit = _size_hay - _size_sub;
      std::string::size_type _pos{};

      do
      {
        if (std::memcmp(_hay + _pos, _sub, _size_sub) == 0)
          return _pos;
      } while (_pos++ < _limit);
    }

    // when not found
    return -1; // std::string::npos;

    // return std::string::npos;
    // warning: overflow in implicit constant conversion [-Woverflow]
  }

  // NOTE:
  // as my_strstr_2(), can reduce comparisons but my_strstr_3() is simple and if
  // perforamce is not critical, perfer simpler way.

} // namespace leetcode_easy_010

TEST(AlgoLeetCode, check_strstr)
{
  using namespace leetcode_easy_010;

  {
    EXPECT_THAT(std::string("hello").find("ll"), 2);
    EXPECT_THAT(std::string("aaaaa").find("bba"), std::string::npos);
    EXPECT_THAT(std::string("aaaaa").find("bba"), -1);
  }

  {
    auto imps = {my_strstr_1, my_strstr_2, my_strstr_3};

    for (auto f : imps)
    {
      EXPECT_THAT(f("hello", "ll"), 2);
      EXPECT_THAT(f("aaaaa", "bba"), -1);
      // error conditions
      EXPECT_THAT(f("", ""), 0);
      EXPECT_THAT(f("", "a"), -1);

      // mississippi"
      //  issipi"
      //     issipi"
      EXPECT_THAT(f("mississippi", "issipi"), -1);
    }
  }
}

/*
={=========================================================================
algo-leetcode-11

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

see algo-search-binary-search

*/

/*
={=========================================================================
algo-leetcode-122

122. Best Time to Buy and Sell Stock II, Easy

Say you have an array for which the ith element is the price of a given stock on
day i.

Design an algorithm to find the maximum profit. You may complete as many
transactions as you like (i.e., buy one and sell one share of the stock multiple
times).

Note: You may not engage in multiple transactions at the same time (i.e., you
must sell the stock before you buy again).

Example 1:

Input: [7,1,5,3,6,4]
Output: 7

Explanation: 
Buy on day 2 (price = 1) and sell on day 3 (price = 5), profit = 5-1 = 4.  Then
buy on day 4 (price = 3) and sell on day 5 (price = 6), profit = 6-3 = 3.
so 4 + 3 = 7

"single transaction but not engage in multiple" means

[7, [1,5,3,6], 4]
    one transaction

[7, [1,5], [3,6], 4]
    two transaction


Example 2:

Input: [1,2,3,4,5]
Output: 4

Explanation: 
Buy on day 1 (price = 1) and sell on day 5 (price = 5), profit = 5-1 = 4.  Note
that you cannot buy on day 1, buy on day 2 and sell them later, as you are
engaging multiple transactions at the same time. You must sell before buying
again.

Example 3:
Input: [7,6,4,3,1]
Output: 0
Explanation: In this case, no transaction is done, i.e. max profit = 0.


note:
so this is about multiple transaction.

2020.08
do not understand solutions and perhaps, the question as well. for example, 

{7, 1, 5, 3, 6, 12};
    {  }  {  }{ }
      4     3    6  = 13.   6 used twice. possible?

    {            }          12-1 = 11. how about this?

so not sure about the question.
 
*/

namespace leetcode_easy_122
{
  // Runtime: 8 ms, faster than 99.82% of C++ online submissions for Best Time
  // to Buy and Sell Stock II.
  //
  // Memory Usage: 9.6 MB, less than 11.03% of C++ online submissions for Best
  // Time to Buy and Sell Stock II.

  int max_profit_1(vector<int> &prices)
  {
    int prev_profit{};
    int current_profit{};
    int accumulated_diff{};

    for (size_t i = 1; i < prices.size(); ++i)
    {
      current_profit = current_profit + (prices[i] - prices[i - 1]);
      current_profit = max(0, current_profit);

      if (current_profit >= prev_profit)
      {
        accumulated_diff += current_profit - prev_profit;
        prev_profit = current_profit;
      }
      else
      {
        // accumulated_diff += current_diff;
        prev_profit = current_profit;
      }
    }

    return accumulated_diff;
  }

  // 2020.08. changed a bit from the above and do not get the above code
  int max_profit_1_1(std::vector<int> &coll)
  {
    int current_profit{};
    int accumulated{};

    for (size_t i = 1; i < coll.size(); ++i)
    {
      current_profit = (coll[i] - coll[i - 1]);

      // do not allow negative
      current_profit = std::max(0, current_profit);

      accumulated += current_profit;
    }

    return accumulated;
  }

  // from solution:
  //
  // Say the given array is:
  //
  // [7, 1, 5, 3, 6, 4].
  //
  // If we plot the numbers of the given array on a graph, we get:
  //
  // 7                _           _
  //      _           B     6     C
  //      A     5
  //                              4
  //                  3
  //      1
  // ------------------------------
  //
  // If we analyze the graph, we notice that the points of interest are the
  // consecutive valleys and peaks.
  //
  // The key point is we need to consider every peak immediately following a
  // valley to maximize the profit. In case we skip one of the peaks (trying to
  // obtain more profit), we will end up losing the profit over one of the
  // transactions leading to an overall lesser profit.
  //
  // For example, in the above case, if we skip peak_i and valley_j trying to
  // obtain more profit by considering points with more difference in heights,
  // the net profit obtained will always be lesser than the one obtained by
  // including them, since C will always be *lesser than A+B*
  //
  // note:
  // [3,6] where 3, buy and 6, sell makes profits

  int max_profit_2(vector<int> &prices)
  {
    int i      = 0;
    int valley = prices[0];
    int peak   = prices[0];
    int max_profit{};

    // since see i and i+1 in th loop
    int length = prices.size() - 1;

    while (i < length)
    {
      // like [5,3]
      while (i < length && prices[i] >= prices[i + 1])
        ++i;
      valley = prices[i];

      // like [3,6]
      while (i < length && prices[i] <= prices[i + 1])
        ++i;
      peak = prices[i];

      max_profit += peak - valley;
    }

    return max_profit;
  }

  // Approach 3: Simple One Pass
  //
  // This solution follows the logic used in Approach 2 itself, but with only a
  // slight variation. In this case, instead of looking for every peak following
  // a valley, we can simply go on crawling over the slope and keep on adding
  // the profit obtained from every *consecutive transaction* In the end, we will
  // be using the peaks and valleys effectively, but we need not track the costs
  // corresponding to the peaks and valleys along with the maximum profit, but
  // we can directly keep on adding the difference between the consecutive
  // numbers of the array if the second number is larger than the first one, and
  // at the total sum we obtain will be the maximum profit. This approach will
  // simplify the solution. This can be made clearer by taking this example:
  //
  // [1, 7, 2, 3, 6, 7, 6, 7]
  //        A  B  C  D
  //
  // From the above graph, we can observe that the sum A+B+C is equal to the
  // difference D corresponding to the difference between the heights of the
  // consecutive peak and valley.

  int max_profit_3(vector<int> &prices)
  {
    int max_profit{};

    for (size_t i = 1; i < prices.size(); ++i)
    {
      if (prices[i] > prices[i - 1])
        max_profit += prices[i] - prices[i - 1];
    }

    return max_profit;
  }

  // Approach 1: Brute Force
  //
  // In this case, we simply calculate the profit corresponding to all the
  // possible sets of transactions and find out the maximum profit out of them.
  //
  // [7, 1, 5, 3, 6, 4]
  //
  // calculate(0, 6)
  // calculate(start:1, i:2)
  // calculate(3, 6)
  // calculate(start:3, i:4)
  // calculate(5, 6)
  // calculate(start:3, i:5)
  // return calculate(6)
  // calculate(start:1, i:3)
  // calculate(4, 6)
  // calculate(start:1, i:4)
  // calculate(5, 6)
  // calculate(start:1, i:5)
  // return calculate(6)
  // calculate(start:2, i:4)
  // calculate(5, 6)
  // calculate(start:3, i:4)
  // calculate(5, 6)
  // calculate(start:3, i:5)
  // return calculate(6)

  int calculate(vector<int> &prices, size_t s)
  {
    if (s >= prices.size())
    {
      // cout << "return calculate(" << s << ")" << endl;
      return 0;
    }

    int max = 0;

    // cout << "calculate(" << s << ", " << prices.size() << ")" << endl;

    for (size_t start = s; start < prices.size(); ++start)
    {
      int max_profit = 0;

      for (size_t i = start + 1; i < prices.size(); ++i)
      {
        if (prices[start] < prices[i])
        {
          // cout << "calculate(start:" << start << ", i:" << i << ")" << endl;

          int profit = calculate(prices, i + 1) + prices[i] - prices[start];
          if (profit > max_profit)
            max_profit = profit;
        }
      } // for

      if (max_profit > max)
        max = max_profit;

    } // for

    return max;
  }

  int max_profit_4(vector<int> &prices) { return calculate(prices, 0); }

} // namespace leetcode_easy_122

TEST(AlgoLeetCode, check_max_profit)
{
  using namespace leetcode_easy_122;

  auto imps = {max_profit_1,
               max_profit_1_1,
               max_profit_2,
               max_profit_3,
               max_profit_4};

  for (const auto &f : imps)
  {
    {
      std::vector<int> coll{7, 1, 5, 3, 6, 4};
      EXPECT_THAT(f(coll), 7);
    }

    {
      std::vector<int> coll{1, 2, 3, 4, 5};
      EXPECT_THAT(f(coll), 4);
    }

    {
      std::vector<int> coll{7, 1, 5, 3, 6, 12};
      EXPECT_THAT(f(coll), 13);
    }
  }
}

/*
={=========================================================================
algo-leetcode-14

58. Length of Last Word, Easy

Given a string s consists of upper/lower-case alphabets and empty space
characters ' ', return the length of last word in the string.

If the last word does not exist, return 0.

Note: A word is defined as a character sequence consists of non-space characters
only.

Example:
Input: "Hello World"
Output: 5

*/

namespace leetcode_easy_014
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
} // namespace leetcode_easy_014

TEST(AlgoLeetCode, check_last_word_length)
{
  using namespace leetcode_easy_014;

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

/*
={=========================================================================
algo-leetcode-15

66. Plus One, Easy

Given a non-empty array of digits representing a non-negative integer, plus one
to the integer.

The digits are stored such that the most significant digit is at the head of the
list, and each element in the array contain a single digit.

You may assume the integer does not contain any leading zero, except the number
0 itself.

Example 1:
Input: [1,2,3]
Output: [1,2,4]
Explanation: The array represents the integer 123.

Example 2:
Input: [4,3,2,1]
Output: [4,3,2,2]
Explanation: The array represents the integer 4321.

*/

namespace leetcode_easy_015
{
  // simple approach:
  //
  // o like atoi, convert input to nmumber
  // o add +1
  // o like itoa, conver it back to array
  // o reverse it
  //
  // solution from lonelydream
  //
  // the key idea is that:
  //
  // +1 addition can cause a carry to the next and which can carry on to msb.
  //
  // when done, if [0] == 0, means there was a carry so insert 1 at the
  // beginning.
  //
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Plus One.
  //
  // Memory Usage: 8.7 MB, less than 15.95% of C++ online submissions for Plus
  // One.

  std::vector<int> plus_one_1(std::vector<int> &coll)
  {
    int i{};

    for (i = coll.size() - 1; i >= 0; --i)
    {
      // add +1; +1 at first and means a carry after that.
      coll[i] = coll[i] + 1;

      // check if addition(+1) causes a carry. since we add +1, a carry only
      // happens when it gets from 9 to 10.
      //
      // do "continue" to carry on the next digit
      if (coll[i] > 9)
      {
        coll[i] = 0;
        continue;
      }

      // if not cause a carry, stops.
      break;
    }

    // ok, run through all digits and if the last is 0, it means there was a
    // carry made from the loop.
    if (coll[0] == 0)
      coll.insert(coll.begin(), 1);

    // input arg is modified but return a copy here
    return coll;
  }
} // namespace leetcode_easy_015

TEST(AlgoLeetCode, check_plus_one)
{
  using namespace leetcode_easy_015;

  {
    auto imps = {plus_one_1};

    for (const auto &f : imps)
    {
      {
        std::vector<int> coll{1, 2, 3};
        EXPECT_THAT(f(coll), ElementsAre(1, 2, 4));
      }
      {
        std::vector<int> coll{4, 3, 2, 1};
        EXPECT_THAT(f(coll), ElementsAre(4, 3, 2, 2));
      }
      {
        std::vector<int> coll{9, 9, 9};
        EXPECT_THAT(f(coll), ElementsAre(1, 0, 0, 0));
      }
      {
        std::vector<int> coll{9, 1, 9};
        EXPECT_THAT(f(coll), ElementsAre(9, 2, 0));
      }
    }
  }
}

/*
={=========================================================================
algo-leetcode-16

67. Add Binary, Easy

Given two binary strings, return their sum (also a binary string).

The input strings are both non-empty and contains only characters 1 or 0.

Example 1:
Input: a = "11", b = "1"
Output: "100"

Example 2:
Input: a = "1010", b = "1011"
Output: "10101"

*/

namespace leetcode_easy_016
{
  std::string add_binary_1(std::string a, std::string b)
  {
    std::string ret{};
    int sum{};
    int first{}, second{};
    int carry{};

    // num of loop
    int asize = a.size() - 1;
    int bsize = b.size() - 1;
    int i     = std::max(asize, bsize);

    for (; i >= 0; --i, --asize, --bsize)
    {
      first  = (asize >= 0) ? (a[asize] - '0') : 0;
      second = (bsize >= 0) ? (b[bsize] - '0') : 0;

      sum = first + second + carry;

      // std::cout << "sum: " << sum << endl;

      if (sum == 2)
      {
        ret.push_back('0');
        carry = 1;
      }
      else
      {
        ret.push_back(sum + '0');
        carry = 0;
      }
    }

    // ok, run through all digits and if the last is 0, it means there was a
    // carry made from the loop.
    if (carry == 1)
      ret.push_back('1');

    // std::cout << "ret: " << ret << endl;

    // ok, return the reversed string
    return std::string(ret.rbegin(), ret.rend());
  }

  // 1. num of loop should be max between a.size and b.size
  // 2. while doing a loop, index of a is different from one of b.
  //    0 1 2 3     0 1
  //    1 1 1 1     1 1

  std::string add_binary_2(std::string a, std::string b)
  {
    int aindex = a.size() - 1;
    int bindex = b.size() - 1;
    int loop   = std::max(aindex, bindex);

    int sum{}, carry{};
    std::string ret{};

    // while doing a loop, decrease index of a and b since we read them up.
    for (; loop >= 0; --loop, --aindex, --bindex)
    {
      // get sum
      sum = ((aindex >= 0) ? a[aindex] - '0' : 0) +
            ((bindex >= 0) ? b[bindex] - '0' : 0) + carry;

      // set carry
      sum == 2 ? carry = 1 : carry = 0;

      // get result
      ret.push_back((sum % 2) + '0');
    }

    if (carry)
      ret.push_back('1');

    return std::string(ret.rbegin(), ret.rend());
  }

  // do not "reverse" and use "ch"
  std::string add_binary_3(std::string a, std::string b)
  {
    int aindex = a.size() - 1;
    int bindex = b.size() - 1;
    int loop   = std::max(aindex, bindex);

    int sum{}, carry{};
    std::string ret{};

    // while doing a loop, decrease index of a and b since we read them up.
    for (; loop >= 0; --loop, --aindex, --bindex)
    {
      // get sum
      sum = ((aindex >= 0) ? a[aindex] - '0' : 0) +
            ((bindex >= 0) ? b[bindex] - '0' : 0) + carry;

      // set carry
      sum == 2 ? carry = 1 : carry = 0;

      // get result
      ret.insert(ret.begin(), ((sum % 2) + '0'));
    }

    if (carry)
      ret.insert(ret.begin(), '1');

    return ret;
  }

  // do not "reverse"
  std::string add_binary_4(std::string a, std::string b)
  {
    int aindex = a.size() - 1;
    int bindex = b.size() - 1;
    int loop   = std::max(aindex, bindex);

    int sum{}, carry{};
    std::string ret{};

    // while doing a loop, decrease index of a and b since we read them up.
    for (; loop >= 0; --loop, --aindex, --bindex)
    {
      // get sum
      //
      // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Add
      // Binary.

      sum = ((aindex >= 0) ? a[aindex] - '0' : 0) +
            ((bindex >= 0) ? b[bindex] - '0' : 0) + carry;

      // set carry
      sum == 2 ? carry = 1 : carry = 0;

      // get result
      ret.insert(0, std::to_string(sum % 2));
    }

    if (carry)
      ret.insert(0, std::to_string(1));

    return ret;
  }
} // namespace leetcode_easy_016

TEST(AlgoLeetCode, check_add_binary)
{
  using namespace leetcode_easy_016;

  {
    auto imps = {add_binary_1, add_binary_2, add_binary_3, add_binary_4};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f("10", "11"), "101");
      EXPECT_THAT(f("1", "11"), "100");
      EXPECT_THAT(f("1010", "1011"), "10101");
      EXPECT_THAT(f("", ""), "");
      EXPECT_THAT(f("0", "0"), "0");
    }
  }
}

/*
={=========================================================================
algo-leetcode-198

198. House Robber, Easy

You are a professional robber planning to rob houses along a street. Each house
has a certain amount of money stashed, the only constraint stopping you from
robbing each of them is that adjacent houses have security system connected and
it will automatically contact the police if two adjacent houses were broken into
on the same night.

Given a list of non-negative integers representing the amount of money of each
house, determine the maximum amount of money you can rob tonight without
alerting the police.

Example 1:

Input: [1,2,3,1]
Output: 4

Explanation: 
Rob house 1 (money = 1) and then rob house 3 (money = 3).  Total amount you can
rob = 1 + 3 = 4.

Example 2:

Input: [2,7,9,3,1]
Output: 12

Explanation: 
Rob house 1 (money = 2), rob house 3 (money = 9) and rob house 5 (money = 1).
Total amount you can rob = 2 + 9 + 1 = 12.

*/

namespace leetcode_easy_198
{
  /*
  rob1 falis since return 3 when expects 4
  vector<int> coll{2, 1, 1, 2};

  rob2 falis since return 5 when expects 4
  vector<int> coll{2, 1, 1, 2};

  rob2 falis since return 6 when expects 4
  vector<int> coll{1, 2, 3, 1};
  */

  int rob_1(vector<int> &nums)
  {
    int sum{};
    int local_sum{std::numeric_limits<int>::min()};

    for (size_t i = 0; i < nums.size(); ++i)
    {
      local_sum = 0;

      for (size_t j = i; j < nums.size(); j += 2)
      {
        local_sum += nums[j];
      }

      sum = max(sum, local_sum);
    }

    return sum;
  }

  int rob_2(vector<int> &nums)
  {
    int sum{};
    int local_sum{std::numeric_limits<int>::min()};

    for (size_t i = 0; i < nums.size(); ++i)
    {
      local_sum = 0;

      for (size_t j = i; j < nums.size();)
      {
        local_sum += nums[j];

        if (j == 0)
          j += 2;
        else
          j += 1;
      }

      sum = max(sum, local_sum);
    }

    return sum;
  }

  // C 1ms, O(1)space,  very simple solution, Jasonly

  // 1. due to constraint which is "no two adjacent houses", min step is to move
  // +2. and then +x move to get max.
  //
  // 2. the other sequence is to start from 1 and move +2. and then +x move to
  // get max.
  //
  // 3. "possibe max" you can get. imagine that two robbers runs and check each
  // others max. even if moves +x, should be within possible(current) max.

  int rob_3(std::vector<int> &nums)
  {
    auto loop{nums.size()};

    size_t runner1_sum{};
    size_t runner2_sum{};

    for (size_t i = 0; i < loop; ++i)
    {
      // run on even indexes
      if (i % 2 == 0)
      {
        runner1_sum += nums[i];
        runner1_sum = std::max(runner1_sum, runner2_sum);
      }
      else
      {
        runner2_sum += nums[i];
        runner2_sum = std::max(runner1_sum, runner2_sum);
      }
    }

    return std::max(runner1_sum, runner2_sum);
  }
} // namespace leetcode_easy_198

TEST(AlgoLeetCode, check_robber)
{
  using namespace leetcode_easy_198;

  auto imps = {rob_3};

  for (const auto &f : imps)
  {
    {
      vector<int> coll{1, 2, 3, 1};
      EXPECT_THAT(f(coll), 4);
    }
    {
      vector<int> coll{2, 7, 9, 3, 1};
      EXPECT_THAT(f(coll), 12);
    }
    {
      vector<int> coll{2, 1, 1, 2};
      EXPECT_THAT(f(coll), 4);
    }
    {
      vector<int> coll{1, 2, 3, 1};
      EXPECT_THAT(f(coll), 4);
    }
    {
      vector<int> coll{2, 7, 9, 3, 1, 4};
      EXPECT_THAT(f(coll), 15);
    }
  }
}

/*
={=========================================================================
algo-leetcode-118

Pascal's Triangle
 
NOTE: it's is DP example?

Approach 1: Dynamic Programming

Intuition

If we have the a row of Pascal's triangle, we can easily compute the next row by
each pair of adjacent values.

Algorithm

Although the algorithm is very simple, the iterative approach to constructing
Pascal's triangle can be classified as dynamic programming because we construct
each row based on the previous row.

First, we generate the overall triangle list, which will store each row as a
sublist. Then, we check for the special case of 00, as we would otherwise return
[1][1]. If numRows > 0numRows>0, then we initialize triangle with [1][1] as its
first row, and proceed to fill the rows as follows:


from solution.

class Solution {
    public List<List<Integer>> generate(int numRows) {
        List<List<Integer>> triangle = new ArrayList<List<Integer>>();

        // First base case; if user requests zero rows, they get zero rows.
        if (numRows == 0) {
            return triangle;
        }

        // Second base case; first row is always [1].
        triangle.add(new ArrayList<>());
        triangle.get(0).add(1);

        for (int rowNum = 1; rowNum < numRows; rowNum++) {
            List<Integer> row = new ArrayList<>();
            List<Integer> prevRow = triangle.get(rowNum-1);

            // The first row element is always 1.
            row.add(1);

            // Each triangle element (other than the first and last of each row)
            // is equal to the sum of the elements above-and-to-the-left and
            // above-and-to-the-right.
            for (int j = 1; j < rowNum; j++) {
                row.add(prevRow.get(j-1) + prevRow.get(j));
            }

            // The last row element is always 1.
            row.add(1);

            triangle.add(row);
        }

        return triangle;
    }
 
*/

namespace leetcode_easy_118
{
  std::vector<std::vector<int>> triangle_1(int rows)
  {
    std::vector<std::vector<int>> tri{};

    tri.emplace_back(std::vector<int>({1}));
    tri.emplace_back(std::vector<int>({1, 1}));
    tri.emplace_back(std::vector<int>({1, 2, 1}));
    tri.emplace_back(std::vector<int>({1, 3, 3, 1}));

    for (const auto &row : tri)
    {
      std::cout << "{";

      for (const auto &e : row)
      {
        std::cout << e << ", ";
      }

      std::cout << "}" << std::endl;
    }

    return tri;
  }

  // draw picture of triangle and implements the above java solution.
  // space and time of O(rows^2)

  std::vector<std::vector<int>> triangle_2(int rows)
  {
    std::vector<std::vector<int>> tri{};
    std::vector<int> row{};

    // if row is 0
    if (rows == 0)
      return tri;

    // if row is 1
    // row.emplace_back(1);
    tri.emplace_back(std::vector<int>({1}));

    // for row is >= 2
    for (int i = 2; i <= rows; ++i)
    {
      std::vector<int> row;

      // first
      row.emplace_back(1);

      // note that to access the previous use -1 since vector index starts from
      // 0.
      for (int run = 1; run <= (i - 2); ++run)
      {
        auto sum = tri[i - 2][run - 1] + tri[i - 2][run];
        row.emplace_back(sum);
      }

      // last
      row.emplace_back(1);

      // for (const auto &e : row)
      // {
      //   std::cout << e << ", ";
      // }
      // std::cout << "\n";

      tri.emplace_back(row);
    }

    return tri;
  }

  // row number starts from 0
  // https://leetcode.com/problems/pascals-triangle/discuss/825125/C%2B%2B-100-faster
  // C++ 100% faster. really?

  std::vector<std::vector<int>> triangle_3(int rows)
  {
    std::vector<std::vector<int>> tri{};

    // fill tri with the required num of elements and init them as 1.
    //
    // note that
    // tri.emplace_back(std::vector<int>(i + 0, 1));
    // makes wrong result since it affects `size` in the next loop
    for (int i = 0; i < rows; ++i)
    {
      tri.emplace_back(std::vector<int>(i + 1, 1));
    }

    // execlude the first row
    for (int i = 1; i < rows; ++i)
    {
      // execlude the first and the last element
      for (int j = 1; j < (int)tri[i].size() - 1; j++)
        tri[i][j] = tri[i - 1][j - 1] + tri[i - 1][j];
    }

    return tri;
  }

  // Runtime: 0 ms, faster than 100.00% of C++ online submissions for Pascal's Triangle.
  // Memory Usage: 6.4 MB, less than 84.98% of C++ online submissions for Pascal's Triangle.

  std::vector<std::vector<int>> triangle_4(int rows)
  {
    std::vector<std::vector<int>> tri{};

    // fill tri with the required num of elements and init them as 1.
    //
    // note that
    // tri.emplace_back(std::vector<int>(i + 0, 1));
    // makes wrong result since it affects `size` in the next loop
    for (int i = 0; i < rows; ++i)
    {
      tri.emplace_back(std::vector<int>(i + 1, 1));
    }

    // execlude the first row
    for (int i = 1; i < rows; ++i)
    {
      // execlude the first and the last element
      for (int j = 1; j < i; j++)
        tri[i][j] = tri[i - 1][j - 1] + tri[i - 1][j];
    }

    return tri;
  }
} // namespace leetcode_easy_118

TEST(AlgoLeetCode, check_triangle)
{
  using namespace leetcode_easy_118;

  std::vector<std::vector<int>> expected{{1},
                                         {1, 1},
                                         {1, 2, 1},
                                         {1, 3, 3, 1},
                                         {1, 4, 6, 4, 1}};

  {
    auto f   = triangle_2;
    auto ret = f(5);
    EXPECT_THAT(ret, expected);
  }

  {
    auto f   = triangle_3;
    auto ret = f(5);
    EXPECT_THAT(ret, expected);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

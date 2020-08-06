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

  // 2020.08
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
} // namespace leetcode_easy_006

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
  struct ListNode
  {
    int val;
    ListNode *next;
    ListNode(int x)
        : val(x)
        , next(NULL)
    {}
  };

  ListNode *mergeTwoLists(ListNode *l1, ListNode *l2)
  {
    if (l1 == nullptr && l2 != nullptr)
      return l1;
    else if (l1 != nullptr && l2 == nullptr)
      return l2;
    else if (l1 == nullptr && l2 == nullptr)
      return nullptr;

    auto first  = l1;
    auto second = l2;

    ListNode *result_head{};
    ListNode *result_end{};

    for (; first != nullptr && (second != nullptr);)
    {
      // ascending order
      if (first->val <= second->val)
      {
        if (!result_head)
        {
          result_head = result_end = first;
        }

        result_end->next = first;
        result_end       = first;

        first = first->next;
      }
      else
      {
        if (!result_head)
        {
          result_head = result_end = second;
        }

        result_end->next = second;
        result_end       = second;

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

  void print_list(ListNode *l1)
  {
    ListNode *run = l1;

    for (; run; run = run->next)
    {
      cout << "val : " << run->val << endl;
    }
  }
} // namespace leetcode_easy_007

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
  // input num is sorted (ascending)
  // the idea is to move the duplicates to the end of right using swap
  // don't need to check if run see smaller value since it's sorted input

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
      else if (run < value)
      {
        // cout << "break i: " << i << endl;
        break;
      }
      // when run == value, swap it to tne end.
      else
      {
        for (size_t s = i; s < nums.size() - 1; ++s)
          swap(nums[s], nums[s + 1]);

        // since nums[i] is swapped, it may be bigger

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
  // end represet start of not-interested, that is end of interested group + 1.
  // so no need to +1. have to think about swap on the same index
  //
  // | ... |end ...|

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

        // to avoid swap on the same index
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

  // algo-unique, same as unique_1(), works on not-sorted input.
  // `end` fro adjacent_find() is end of the interested group
  // | ... end| ...|

  int RemoveDuplicates_03(vector<int> &nums)
  {
    auto first = nums.begin();
    auto last  = nums.end();

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

  int RemoveDuplicates_04(vector<int> &nums)
  {
    if (nums.empty())
      return 0;

    size_t unique_end{0};
    size_t current{1};
    size_t size = nums.size() - 1;

    while (current <= size)
    {
      if (nums[unique_end] == nums[current])
      {
        ++current;
      }
      else
      {
        ++unique_end;

        // to avoid assign on the same index
        if (unique_end != current)
          nums[unique_end] = nums[current];

        ++current;
      }
    }

    // +1 since unique_end is index but return length
    return unique_end + 1;
  }

} // namespace leetcode_easy_008

TEST(LeetCode, Easy_008_RemoveDuplicates)
{
  using namespace leetcode_easy_008;

  // okay
  {
    const auto func = RemoveDuplicates_01;

    vector<int> coll{1, 1, 2};
    auto len = func(coll);

    EXPECT_THAT(len, 2);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1, 2));
  }

  // fails
  {
    const auto func = RemoveDuplicates_01;

    vector<int> coll{0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
    auto len = func(coll);

    EXPECT_THAT(len, Not(5));

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, Not(ElementsAre(0, 1, 2, 3, 4)));
  }

  {
    const auto func = RemoveDuplicates_02;

    vector<int> coll{1, 1, 2};
    auto len = func(coll);

    EXPECT_THAT(len, 2);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1, 2));
  }

  {
    const auto func = RemoveDuplicates_02;

    vector<int> coll{0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
    auto len = func(coll);

    EXPECT_THAT(len, 5);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(0, 1, 2, 3, 4));
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

    EXPECT_THAT(result, ElementsAre(1, 4, 6));
  }
  {
    const auto func = RemoveDuplicates_03;

    vector<int> coll{1, 4, 4, 4, 6};
    auto len = func(coll);

    EXPECT_THAT(len, 3);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1, 4, 6));
  }
  {
    const auto func = RemoveDuplicates_03;

    vector<int> coll{1, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 5, 7, 5, 4, 4};

    auto len = func(coll);

    EXPECT_THAT(
      coll,
      ElementsAreArray({1, 4, 6, 1, 2, 3, 1, 6, 5, 7, 5, 4, 5, 7, 5, 4, 4}));
    EXPECT_THAT(len, 12);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAreArray({1, 4, 6, 1, 2, 3, 1, 6, 5, 7, 5, 4}));
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

} // namespace leetcode_easy_009

TEST(LeetCode, Easy_009_RemoveIf)
{
  using namespace leetcode_easy_009;

  {
    const auto func = RemoveIf_01;

    vector<int> coll{3, 2, 2, 3};
    auto len = func(coll, 3);

    EXPECT_THAT(len, 2);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(2, 2));
  }

  {
    const auto func = RemoveIf_01;

    vector<int> coll{0, 1, 2, 2, 3, 0, 4, 2};
    auto len = func(coll, 2);

    EXPECT_THAT(len, 5);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(0, 1, 3, 0, 4));
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

} // namespace leetcode_easy_010

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
// algo-leetcode-53
/*
53. Maximum Subarray, Easy, algo-max-sub-array

Given an integer array nums, find the contiguous subarray (containing at least
one number) which has the largest sum and return its sum.

Example:

Input: [-2,1,-3,4,-1,2,1,-5,4],
Output: 6
Explanation: [4,-1,2,1] has the largest sum = 6.

Follow up:
If you have figured out the O(n) solution, try coding another solution using the
divide and conquer approach, which is more subtle.

int maxSubArray(vector<int>& nums) {}


In discussion, by _LeetCode,  Last Edit: October 24, 2018 8:10 PM
for python

for i in range(1, len(nums)):
        if nums[i-1] > 0:
            nums[i] += nums[i-1]
    return max(nums)

The key observation is:

[i-1]   [i]

if the previous, [i-1], is positive, then "sum" gets bigger whether or not the
current element is positive or negative.


if make sum regardless of singness of the previous, then it makes *prefix-sum*

-2  1   -3  4   -1  2   1   -5  4
    -1  -4  0   -1  1   2   -3  1

if runs the above code, then 6 is max sum

-2  1   -3 [4   -1  2   1]  -5  4
        -2      3   5   6   1   5 

if runs the prefix-sum but do not allow negative value , then 6 is max sum

-2  1   -3 [4   -1  2   1]  -5  4
0   1   -0  4    3  5   6   1   5 


so, the both are basically the same method to get max sub-array


if make the previous value bigger? affect on next sum and will be covered

-2  100   -3    4    -1     2     1   -5    4
          97  101   100   102   103   98  102 


This is about "sum" but not "sub array" How about returnning "sub array"?

-2  1   -3 [4   -1  2   1]  -5  4
        -2      3   5   6   1   5 
        *       *
        *
"*" starts and max_element() is ends.

*/

namespace leetcode_easy_053
{
  int maxSubArray_1(vector<int> &nums)
  {
    int current_max{};

    // do not solve all
    // if (nums.size() == 1)
    // {
    //   return nums[0];
    // }

    for (size_t i = 1; i < nums.size(); ++i)
    {
      // only for positive previous item
      // why [i-1]? since updates `nums` and refer back to the previous

      if (nums[i - 1] > 0)
      {
        // update input to keep *prefix-sum* since do not care changes to the
        // input

        nums[i] += nums[i - 1];

        // to find max sum
        if (nums[i] > current_max)
          current_max = nums[i];
      }
    }

    return current_max;
  }

  // Runtime: 12 ms, faster than 98.45% of C++ online submissions for Maximum
  // Subarray.
  //
  // Memory Usage: 10.5 MB, less than 15.81% of C++ online submissions for
  // Maximum Subarray.

  int maxSubArray_2(vector<int> &nums)
  {
    for (size_t i = 1; i < nums.size(); ++i)
    {
      // only for positive previous item
      if (nums[i - 1] > 0)
      {
        // update input to keep *prefix-sum* since do not care changes to the
        // input

        nums[i] += nums[i - 1];
      }
    }

    return *std::max_element(nums.begin(), nums.end());
  }

  // https://www.geeksforgeeks.org/largest-sum-contiguous-subarray/
  //
  // Simple idea of the Kadane’s algorithm is to look for all positive
  // contiguous segments of the array (max_ending_here is used for this). And
  // keep track of *maximum sum* contiguous segment among all positive segments
  // (max_so_far is used for this). Each time we get a positive sum compare it
  // with max_so_far and update max_so_far if it is greater than max_so_far

  int maxSubArray_3(vector<int> &nums)
  {
    int max_so_far{std::numeric_limits<int>::min()};
    int max_current{};

    for (auto e : nums)
    {
      // works okay
      //
      // in sum, that's to make prefix-sum but do not allow negative value.
      //
      // max_current = e + max_current;
      // if (max_current < 0)
      //   max_current = 0;

      // same as
      max_current = e + max_current;
      max_current = max(0, max_current);

      if (max_current > max_so_far)
        max_so_far = max_current;
    }

    return max_so_far;
  }

} // namespace leetcode_easy_053

TEST(LeetCode, Easy_053_MaxSubArray_1)
{
  using namespace leetcode_easy_053;

  {
    auto func = maxSubArray_1;

    // fails on :
    // input "1" and expected "1"
    {
      vector<int> coll{1};
      EXPECT_THAT(func(coll), Not(1));
    }

    // fails on
    // input "-2, 1" and expected 1
    {
      vector<int> coll{-2, 1};
      EXPECT_THAT(func(coll), Not(1));
    }

    {
      vector<int> coll{-2, 1, -3, 4, -1, 2, 1, -5, 4};
      EXPECT_THAT(func(coll), 6);
    }

    {
      // >>> sum([100,-3,4,-1,2,1,-5,4])
      // 102
      // >>> sum([100,-3,4,-1,2,1])
      // 103
      vector<int> coll{-2, 100, -3, 4, -1, 2, 1, -5, 4};
      EXPECT_THAT(func(coll), 103);
    }
  }

  {
    auto func = maxSubArray_2;

    {
      vector<int> coll{1};
      EXPECT_THAT(func(coll), 1);
    }

    {
      vector<int> coll{-2, 1};
      EXPECT_THAT(func(coll), 1);
    }

    {
      vector<int> coll{-2, 1, -3, 4, -1, 2, 1, -5, 4};
      EXPECT_THAT(func(coll), 6);
    }

    {
      vector<int> coll{-2, 100, -3, 4, -1, 2, 1, -5, 4};
      EXPECT_THAT(func(coll), 103);
    }
  }

  {
    auto func = maxSubArray_3;

    {
      vector<int> coll{1};
      EXPECT_THAT(func(coll), 1);
    }

    {
      vector<int> coll{-2, 1};
      EXPECT_THAT(func(coll), 1);
    }

    {
      vector<int> coll{-2, 1, -3, 4, -1, 2, 1, -5, 4};
      EXPECT_THAT(func(coll), 6);
    }

    {
      vector<int> coll{-2, 100, -3, 4, -1, 2, 1, -5, 4};
      EXPECT_THAT(func(coll), 103);
    }
  }
}

// ={=========================================================================
// algo-leetcode-122
/*
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

*/

namespace leetcode_easy_122
{
  // Runtime: 8 ms, faster than 99.82% of C++ online submissions for Best Time
  // to Buy and Sell Stock II.
  //
  // Memory Usage: 9.6 MB, less than 11.03% of C++ online submissions for Best
  // Time to Buy and Sell Stock II.

  int maxProfit_1(vector<int> &prices)
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

  int maxProfit_2(vector<int> &prices)
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

  int maxProfit_3(vector<int> &prices)
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
      cout << "return calculate(" << s << ")" << endl;
      return 0;
    }

    int max = 0;

    cout << "calculate(" << s << ", " << prices.size() << ")" << endl;

    for (size_t start = s; start < prices.size(); ++start)
    {
      int max_profit = 0;

      for (size_t i = start + 1; i < prices.size(); ++i)
      {
        if (prices[start] < prices[i])
        {
          cout << "calculate(start:" << start << ", i:" << i << ")" << endl;

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

  int maxProfit_4(vector<int> &prices) { return calculate(prices, 0); }

} // namespace leetcode_easy_122

TEST(LeetCode, Easy_122_MaxProfit_1)
{
  using namespace leetcode_easy_122;

  {
    auto func = maxProfit_1;

    {
      vector<int> coll{7, 1, 5, 3, 6, 4};
      EXPECT_THAT(func(coll), 7);
    }
    {
      vector<int> coll{1, 2, 3, 4, 5};
      EXPECT_THAT(func(coll), 4);
    }
  }

  {
    auto func = maxProfit_2;

    {
      vector<int> coll{7, 1, 5, 3, 6, 4};
      EXPECT_THAT(func(coll), 7);
    }
    {
      vector<int> coll{1, 2, 3, 4, 5};
      EXPECT_THAT(func(coll), 4);
    }
  }

  {
    auto func = maxProfit_3;

    {
      vector<int> coll{7, 1, 5, 3, 6, 4};
      EXPECT_THAT(func(coll), 7);
    }
    {
      vector<int> coll{1, 2, 3, 4, 5};
      EXPECT_THAT(func(coll), 4);
    }
  }

  {
    auto func = maxProfit_4;

    {
      vector<int> coll{7, 1, 5, 3, 6, 4};
      EXPECT_THAT(func(coll), 7);
    }
    // {
    //   vector<int> coll{1,2,3,4,5};
    //   EXPECT_THAT(func(coll), 4);
    // }
  }
}

// ={=========================================================================
// algo-leetcode-14
/*
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
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Length of
  // Last Word.
  //
  // Memory Usage: 8.9 MB, less than 90.45% of C++ online submissions for Length
  // of Last Word.

  int lengthOfLastWord(string s)
  {
    int count{};

    int i = s.size() - 1;

    for (; i >= 0; --i)
      if (!isspace(s[i]))
        break;

    for (; i >= 0; --i)
    {
      if (s[i] == ' ')
        return count;

      ++count;
    }

    return count;
  }
} // namespace leetcode_easy_014

TEST(LeetCode, Easy_014_LengthOfLastWord_1)
{
  using namespace leetcode_easy_014;

  EXPECT_THAT(lengthOfLastWord("Hello World"), 5);
  EXPECT_THAT(lengthOfLastWord("HelloWorld"), 10);
  EXPECT_THAT(lengthOfLastWord(""), 0);

  // failed case and change code accordingly
  EXPECT_THAT(lengthOfLastWord("a"), 1);
  EXPECT_THAT(lengthOfLastWord("a "), 1);
}

// ={=========================================================================
// algo-leetcode-15
/*
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
  // initially, thought
  //
  // o like atoi, convert input to nmumber
  // o add +1
  // o like itoa, conver it back to array
  // o reverse it
  //
  // but looks it's not right approach
  //
  // vector<int> plusOne_1(vector<int>& digits)
  // {
  // }
  //
  //  python solution:
  //
  //  def answer(self, digits):
  //      return [int(c) for c in str(int(''.join(str(d) for d in digits)) + 1)]

  // solution from lonelydream
  //
  // the key idea is that:
  //
  // if addition cause a carry made then continue doing so until it do not
  // create a carry.
  //
  // when done, if [0] == 0, means there was a carry so insert 1 at the
  // beginning.
  //
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Plus One.
  //
  // Memory Usage: 8.7 MB, less than 15.95% of C++ online submissions for Plus
  // One.

  vector<int> plusOne_2(vector<int> &digits)
  {
    for (int i = digits.size() - 1; i >= 0; --i)
    {
      digits[i] += 1;

      if (digits[i] > 9)
      {
        digits[i] = 0;
        continue;
      }
      // else
      //   break;

      // also works without else clause
      break;
    }

    if (digits[0] == 0)
      digits.insert(digits.begin(), 1);

    return digits;
  }
} // namespace leetcode_easy_015

TEST(LeetCode, Easy_015_PlusOne)
{
  using namespace leetcode_easy_015;

  auto func = plusOne_2;

  {
    vector<int> coll{1, 2, 3};
    EXPECT_THAT(func(coll), ElementsAre(1, 2, 4));
  }
  {
    vector<int> coll{4, 3, 2, 1};
    EXPECT_THAT(func(coll), ElementsAre(4, 3, 2, 2));
  }
  {
    vector<int> coll{9, 9, 9};
    EXPECT_THAT(func(coll), ElementsAre(1, 0, 0, 0));
  }
}

// ={=========================================================================
// algo-leetcode-16
/*
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

// Like, Plus One, loop on input strings while caring about `carry`
//
// o since the length of inputs can be different, loops on max between them,
// decrease them and note that then can go negative.
//
// o convert char to int

namespace leetcode_easy_016
{
  // Runtime: 8 ms, faster than 95.80% of C++ online submissions for Add Binary.
  //
  // Memory Usage: 8.6 MB, less than 55.73% of C++ online submissions for Add
  // Binary.

  string addBinary_1(string a, string b)
  {
    int i    = a.size() - 1;
    int j    = b.size() - 1;
    auto len = max(i, j);

    int carry{};
    int sum{};
    string result{};

    for (; len >= 0; --len, --i, --j)
    {
      // sum = (i >= 0 ? stoi(a.substr(i, 1)) : 0) + (j >= 0 ? stoi(b.substr(j, 1)) : 0) + carry;

      // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Add
      // Binary.

      sum = (i >= 0 ? a[i] - '0' : 0) + (j >= 0 ? b[j] - '0' : 0) + carry;

      sum >= 2 ? carry = sum / 2 : carry = 0;

      // cout << "insert: " << (sum % 2) << endl;

      result.insert(0, to_string(sum % 2));
    }

    // cout << "sum: " << sum << ", carry: " << carry << endl;

    if (carry)
    {
      // cout << "insert: " << (1) << endl;
      result.insert(0, to_string(1));
    }

    return result;
  }
} // namespace leetcode_easy_016

TEST(LeetCode, Easy_016_AddBinary)
{
  using namespace leetcode_easy_016;

  auto func = addBinary_1;

  EXPECT_THAT(func("10", "11"), "101");
  EXPECT_THAT(func("1", "11"), "100");
  EXPECT_THAT(func("1010", "1011"), "10101");
  EXPECT_THAT(func("", ""), "");

  // fails at run and fixed
  EXPECT_THAT(func("0", "0"), "0");
}

// ={=========================================================================
// algo-leetcode-17
/*
69. Sqrt(x), Easy

Implement int sqrt(int x).

Compute and return the square root of x, where x is guaranteed to be a
non-negative integer.

Since the return type is an integer, the decimal digits are truncated and only
the integer part of the result is returned.

Example 1:
Input: 4
Output: 2

Example 2:
Input: 8
Output: 2
Explanation: The square root of 8 is 2.82842..., and since 
             the decimal part is truncated, 2 is returned.
*/

namespace leetcode_easy_017
{

  // https://www.geeksforgeeks.org/square-root-of-an-integer/
  // binary search version
  //
  // O(Log x)
  //
  //  0   1       mid   sqrt(x)                 x = sqrt(x)*sqrt(x)
  //  |---|--------|-------|--------------------|
  //
  //  actually, trying to find sqrt(x) from [1, x] and x is ^2 domain and big
  //  value but used

  int floor_sqrt_1(int x)
  {
    // base cases
    if (x == 0 || x == 1)
      return x;

    // starts from 1 since it's covered in base cases
    int start{1};
    int end{x - 1};
    int ans{};

    while (start <= end)
    {
      int mid = (start + end) / 2;

      // equality; perfect square
      if (mid * mid == x)
        return mid;
      else if (mid * mid < x)
      {
        // so discard [1, mid], update start and move closer to sqrt(x)
        start = mid + 1;

        // we need floor answer so update ans
        ans = mid;
      }
      // discard [mid, x]
      else
        end = mid - 1;
    }

    // return floor value rather than `not found`
    return ans;
  }

  // Note: The Binary Search can be further optimized to start with ‘start’ = 0
  // and ‘end’ = x/2. Floor of square root of x cannot be more than x/2 when x >
  // 1.
  //
  // cxx-error-overflow
  // Line 18: Char 15: runtime error: signed integer overflow: 536848899 *
  // 536848899 cannot be represented in type 'int' (solution.cpp)

  int floor_sqrt_2(int x)
  {
    // // base cases
    // if (x == 0 || x == 1)
    //   return x;

    // starts from 1 since it's covered in base cases
    int start{0};
    int end{x / 2};
    int ans{};

    while (start <= end)
    {
      int mid = (start + end) / 2;

      // equality; perfect square
      if (mid * mid == x)
        return mid;
      else if (mid * mid < x)
      {
        // so discard [1, mid], update start and move closer to sqrt(x)
        start = mid + 1;

        // we need floor answer so update ans
        ans = mid;
      }
      // discard [mid, x]
      else
        end = mid - 1;
    }

    // return floor value rather than `not found`
    return ans;
  }

  // Runtime: 24 ms, faster than 21.88% of C++ online submissions for Sqrt(x).
  //
  // Memory Usage: 13.9 MB, less than 49.57% of C++ online submissions for
  // Sqrt(x).

  int floor_sqrt_3(int x)
  {
    // // base cases
    if (x == 0 || x == 1)
      return x;

    // starts from 1 since it's covered in base cases
    int start{0};
    int end{x / 2};
    int ans{};

    while (start <= end)
    {
      long long mid   = (start + end) / 2;
      long long sqare = mid * mid;

      // equality; perfect square
      if (sqare == x)
        return mid;
      else if (sqare < x)
      {
        // so discard [1, mid], update start and move closer to sqrt(x)
        start = mid + 1;

        // we need floor answer so update ans
        ans = mid;
      }
      // discard [mid, x]
      else
        end = mid - 1;
    }

    // return floor value rather than `not found`
    return ans;
  }

  // having square variable for mid * mid causes performance penalty?
  //
  // Runtime: 12 ms, faster than 99.18% of C++ online submissions for Sqrt(x).
  //
  // Memory Usage: 13.8 MB, less than 84.46% of C++ online submissions for
  // Sqrt(x).

  int floor_sqrt_4(int x)
  {
    // // base cases
    if (x == 0 || x == 1)
      return x;

    // starts from 1 since it's covered in base cases
    int start{0};
    int end{x / 2};
    int ans{};

    long long mid{};

    while (start <= end)
    {
      mid = (start + end) / 2;

      // equality; perfect square
      if (mid * mid == x)
        return mid;
      else if (mid * mid < x)
      {
        // so discard [1, mid], update start and move closer to sqrt(x)
        start = mid + 1;

        // we need floor answer so update ans
        ans = mid;
      }
      // discard [mid, x]
      else
        end = mid - 1;
    }

    // return floor value rather than `not found`
    return ans;
  }

  // code discussion forum
  //
  // Runtime: 12 ms, faster than 99.18% of C++ online submissions for Sqrt(x).
  //
  // Memory Usage: 13.8 MB, less than 73.75% of C++ online submissions for
  // Sqrt(x).

  int floor_sqrt_5(int x)
  {
    long long l = 1, r = x, mid;

    if (x == 0)
      return 0;

    while (l <= r)
    {
      mid = l + (r - l) / 2;

      if (mid * mid == x)
        return mid;
      else if (mid * mid > x)
        r = mid - 1;
      else
      {
        l = mid + 1;
        if (l * l > x)
          return mid;
      }
    }

    // just to avoid warning
    return mid;
  }
} // namespace leetcode_easy_017

TEST(LeetCode, Easy_017_Sqrt)
{
  using namespace leetcode_easy_017;

  {
    // #include <math.h>
    // double sqrt(double x);
    // float sqrtf(float x);
    // long double sqrtl(long double x);

    // 2
    // 2.82843
    // 3.16228
    // 4

    EXPECT_DOUBLE_EQ(sqrt(4), 2);
    EXPECT_NEAR(sqrt(8), 2.82843, 0.00001);

    // Expected equality of these values:
    //   sqrt(10)
    //     Which is: 3.1622776601683795
    //   3.16228
    // EXPECT_DOUBLE_EQ(sqrt(10), 3.16228);

    EXPECT_NEAR(sqrt(10), 3.16228, 0.00001);
    EXPECT_DOUBLE_EQ(sqrt(16), 4);
  }

  {
    auto func = floor_sqrt_1;

    EXPECT_THAT(func(4), 2);
    EXPECT_THAT(func(8), 2);
    EXPECT_THAT(func(10), 3);
    EXPECT_THAT(func(16), 4);
  }
  {
    auto func = floor_sqrt_2;

    EXPECT_THAT(func(4), 2);
    EXPECT_THAT(func(8), 2);
    EXPECT_THAT(func(10), 3);
    EXPECT_THAT(func(16), 4);
  }
  {
    // >>> 46339*46339, floor
    // 2,147,302,921
    //
    // 2,147,395,599
    //
    // >>> 46340*46340, ceiling
    // 2,147,395,600

    auto func = floor_sqrt_3;
    EXPECT_THAT(func(2147395599), 46339);
    EXPECT_NEAR(sqrt(2147395599), 46340, 0.1);
  }

  {
    // >>> 46339*46339, floor
    // 2,147,302,921
    //
    // 2,147,395,599
    //
    // >>> 46340*46340, ceiling
    // 2,147,395,600

    auto func = floor_sqrt_4;
    EXPECT_THAT(func(1), 1);
    EXPECT_THAT(func(2147395599), 46339);
    EXPECT_NEAR(sqrt(2147395599), 46340, 0.1);
  }
}

TEST(LeetCode, Easy_017_Sqrt_Performance_1)
{
  using namespace leetcode_easy_017;
  auto func = floor_sqrt_3;

  for (int i = 0; i < 1000; ++i)
  {
    EXPECT_THAT(func(1), 1);
    EXPECT_THAT(func(2147395599), 46339);
  }
}

TEST(LeetCode, Easy_017_Sqrt_Performance_2)
{
  using namespace leetcode_easy_017;
  auto func = floor_sqrt_4;

  for (int i = 0; i < 1000; ++i)
  {
    EXPECT_THAT(func(1), 1);
    EXPECT_THAT(func(2147395599), 46339);
  }
}

/*
Approach 3: Dynamic Programming

As we can see this problem can be broken into subproblems, and it contains the
optimal substructure property i.e. its optimal solution can be constructed
efficiently from optimal solutions of its subproblems, we can use dynamic
programming to solve this problem.

One can reach ith step in one of the two ways:

Taking a single step from (i-1) th step.

Taking two step from (i−2) th step.

(since it is about way to reach to n but not number of steps)

So, the total number of ways to reach i th is equal to sum of ways of reaching
(i−1)th step and ways of reaching (i-2)th step.  

Let dp[i] denotes the number of ways to reach on i th step:

dp[i]=dp[i-1]+dp[i-2]


Approach 4: Fibonacci Number

In the above approach we have used dpdp array where dp[i]=dp[i-1]+dp[i-2]. It
can be easily analysed that dp[i] is nothing but ith fibonacci number.

means the dp value sequence. this is fibonacci sequence:

0, 1, 1, 2, 3, 5, 8, 13, 21, 34, ...

dp value sequence:

0, 1, 2, 3, 5, 8, 13, 21, 34, ...

Now we just have to find n th number of the fibonacci series having 1 and 2
their first and second term respectively, i.e. Fib(1)=1 and Fib(2)=2.


  int fibonacci_2(int n)
  {
    int twoback{};  // f(n-2)
    int oneback{};  // f(n-1)
    int current{};

    if (n <= 0)
      return 0;
    else if (n == 1)
      return 1;
    else
    {
      // back from current
      twoback = 0;
      oneback = 1;

      for (int i = 2; i <= n; ++i)
      {
        current = twoback + oneback;

        // for next f
        twoback = oneback;
        oneback = current;
      }
    }

    return current;
  }
*/

namespace leetcode_easy_018
{
  int climbStairs_3(int n)
  {
    // base cases
    if (n <= 0)
      return 0;
    if (n == 1)
      return 1;
    if (n == 2)
      return 2;

    int one_step_before  = 2; // when n == 2
    int two_steps_before = 1; // when n == 1
    int all_ways         = 0;

    // starts from n == 3
    for (int i = 3; i <= n; i++)
    {
      all_ways         = one_step_before + two_steps_before;
      two_steps_before = one_step_before;
      one_step_before  = all_ways;
    }

    return all_ways;
  };
} // namespace leetcode_easy_018

TEST(LeetCode, Easy_018_ClimbStairs_3)
{
  using namespace leetcode_easy_018;
  auto func = climbStairs_3;

  EXPECT_THAT(func(2), 2);
  EXPECT_THAT(func(3), 3);
  EXPECT_THAT(func(4), 5);
  EXPECT_THAT(func(30), 1346269);
}

// ={=========================================================================
// algo-leetcode-19
/*
100. Same Tree, Easy

Given two binary trees, write a function to check if they are the same or not.

Two binary trees are considered the same if they are structurally identical and
the nodes have the same value.

Example 1:

Input:     1         1
          / \       / \
         2   3     2   3

        [1,2,3],   [1,2,3]

Output: true

Example 2:

Input:     1         1
          /           \
         2             2

        [1,2],     [1,null,2]

Output: false

Example 3:

Input:     1         1
          / \       / \
         2   1     1   2

        [1,2,1],   [1,1,2]

Output: false
 
*/

// ={=========================================================================
// algo-leetcode-155
/* 155. Min Stack, Easy

Design a stack that supports push, pop, top, and retrieving the minimum element
in constant time.

push(x) -- Push element x onto stack.
pop() -- Removes the element on top of the stack.
top() -- Get the top element.
getMin() -- Retrieve the minimum element in the stack.
Example:
MinStack minStack = new MinStack();
minStack.push(-2);
minStack.push(0);
minStack.push(-3);
minStack.getMin();   --> Returns -3.
minStack.pop();
minStack.top();      --> Returns 0.
minStack.getMin();   --> Returns -2.

*/

namespace leetcode_easy_155
{
  // o changed from using fixed contiguous arrary to using vector
  // o passes but slow and do not meet condition, "retrieving the minimum
  //   element in constant time"

  class MinStack_1
  {
  public:
    MinStack_1() {}

    bool empty() { return data_.empty(); }

    void push(int x)
    {
      data_.push_back(x);
      min_ = min(min_, x);
    }

    void pop()
    {
      if (empty())
        throw runtime_error("stack is empty");

      data_.pop_back();

      auto it = std::min_element(data_.begin(), data_.end());
      if (it == data_.end())
        min_ = std::numeric_limits<int>::max();
      else
        min_ = *it;
    }

    int top()
    {
      if (empty())
        throw runtime_error("stack is empty");

      return data_.back();
    }

    int getMin() { return min_; }

  private:
    std::vector<int> data_;
    int min_{std::numeric_limits<int>::max()};
  };

  // o as in the dicusstion, can use priority_queue<> but still do not meet
  //   condition, "retrieving the minimum element in constant time"

  // o can keep the min value as last min value and pop() without looping until
  // see that than updating min every time do pop().
  //
  // improvement but when see the last min and poped it, search min and update
  // min value. still not constant.
  //
  // Looked at:
  //
  // Python single stack O(1) all operations, by destinynitsed
  //
  // both uses extra space to keep last min value when that item waw pushed and
  // use the same idea.
  //
  // Python code is clearer
  //
  // class MinStack:
  //
  //     def __init__(self):
  //         self.stack = []
  //
  //     def push(self, x):
  //         if self.stack:
  // 			      self.stack.append(min(self.stack[-2], x))
  //         else:
  //             self.stack.append(x)
  //         self.stack.append(x)
  //
  //
  //     def pop(self):
  //         if self.stack:
  //             self.stack.pop()
  //             self.stack.pop()
  //
  //     def top(self):
  //         if self.stack:
  //             return self.stack[-1]
  //
  //     def getMin(self):
  //         if self.stack:
  //             return self.stack[-2]
  //
  // For example, input -2, 0, 3 makes
  // [-2, -2, -2, 0, -3, -3]

  class MinStack_2
  {
  public:
    MinStack_2() {}

    bool empty() { return data_.empty(); }

    void push(int x)
    {
      if (empty())
      {
        data_.push_back(x);
      }
      else
      {
        data_.push_back(min(*(data_.end() - 2), x));
      }

      data_.push_back(x);
    }

    void pop()
    {
      if (empty())
        throw runtime_error("stack is empty");

      data_.pop_back();
      data_.pop_back();
    }

    int top()
    {
      if (empty())
        throw runtime_error("stack is empty");

      return data_.back();
    }

    int getMin()
    {
      if (empty())
        throw runtime_error("stack is empty");

      return *(data_.end() - 2);
    }

  private:
    std::vector<int> data_;
  };

  // Java accepted solution using one stack, by sometimescrazy
  // saves space by adding previous min only when min changes.
  //
  // For example, input -2, 0, 3 makes
  //
  // # [-2, -2, -2, 0, -3, -3]
  // [max, -2, 0, -2, -3]

  class MinStack_3
  {
  public:
    MinStack_3() {}

    bool empty() { return data_.empty(); }

    // only push the old minimum value when the current
    // minimum value changes after pushing the new value x

    void push(int x)
    {
      if (x <= min_)
      {
        data_.push_back(min_);
        min_ = x;
      }

      data_.push_back(x);
    }

    // if pop operation could result in the changing of the current minimum
    // value, pop twice and change the current minimum value to the last
    // minimum value.

    void pop()
    {
      if (empty())
        throw runtime_error("stack is empty");

      if (data_.back() == min_)
      {
        data_.pop_back();
        min_ = data_.back();
      }

      data_.pop_back();
    }

    int top()
    {
      if (empty())
        throw runtime_error("stack is empty");

      return data_.back();
    }

    int getMin()
    {
      if (empty())
        throw runtime_error("stack is empty");

      return min_;
    }

  private:
    std::vector<int> data_;
    int min_{std::numeric_limits<int>::max()};
  };
} // namespace leetcode_easy_155

TEST(LeetCode, Easy_155_MinStack_1)
{
  using namespace leetcode_easy_155;

  {
    MinStack_1 ms;

    ms.push(-2);
    ms.push(0);
    ms.push(-3);

    EXPECT_THAT(ms.getMin(), -3);

    ms.pop();
    EXPECT_THAT(ms.top(), 0);

    EXPECT_THAT(ms.getMin(), -2);
  }

  {
    MinStack_1 ms;

    ms.push(2147483646);
    ms.push(2147483646);
    ms.push(2147483646);
    ms.top();
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483646);
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483646);
    ms.pop();

    ms.push(2147483647);
    ms.top();
    EXPECT_THAT(ms.getMin(), 2147483647);
    ms.push(-2147483648);
    ms.top();
    EXPECT_THAT(ms.getMin(), -2147483648);
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483647);
  }

  {
    MinStack_2 ms;

    ms.push(-2);
    ms.push(0);
    ms.push(-3);

    EXPECT_THAT(ms.getMin(), -3);

    ms.pop();
    EXPECT_THAT(ms.top(), 0);

    EXPECT_THAT(ms.getMin(), -2);
  }

  {
    MinStack_2 ms;

    ms.push(2147483646);
    ms.push(2147483646);
    ms.push(2147483646);
    ms.top();
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483646);
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483646);
    ms.pop();

    ms.push(2147483647);
    ms.top();
    EXPECT_THAT(ms.getMin(), 2147483647);
    ms.push(-2147483648);
    ms.top();
    EXPECT_THAT(ms.getMin(), -2147483648);
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483647);
  }

  {
    MinStack_3 ms;

    ms.push(-2);
    ms.push(0);
    ms.push(-3);

    EXPECT_THAT(ms.getMin(), -3);

    ms.pop();
    EXPECT_THAT(ms.top(), 0);

    EXPECT_THAT(ms.getMin(), -2);
  }
  {
    MinStack_3 ms;

    ms.push(2147483646);
    ms.push(2147483646);
    ms.push(2147483646);
    ms.top();
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483646);
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483646);
    ms.pop();

    ms.push(2147483647);
    ms.top();
    EXPECT_THAT(ms.getMin(), 2147483647);
    ms.push(-2147483648);
    ms.top();
    EXPECT_THAT(ms.getMin(), -2147483648);
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483647);
  }
}

// ={=========================================================================
// algo-leetcode-191
/*
171. Excel Sheet Column Number, Easy

Given a column title as appear in an Excel sheet, return its corresponding
column number.

For example:

    A -> 1
    B -> 2
    C -> 3
    ...
    Z -> 26
    AA -> 27
    AB -> 28 
    ...

Example 1:
Input: "A"
Output: 1

Example 2:
Input: "AB"
Output: 28

Example 3:
Input: "ZY"
Output: 701


168. Excel Sheet Column Title, Easy

Given a positive integer, return its corresponding column title as appear in an
Excel sheet.

For example:

    1 -> A
    2 -> B
    3 -> C
    ...
    26 -> Z
    27 -> AA
    28 -> AB 
    ...

*/

namespace leetcode_easy_171
{
  // uses *algo-atoi* when base 26
  //
  // A  B  C  ... Z
  // 1  2  3      26
  //
  // AA AB AC ... AZ
  // 27 28 29 ... 52 (26 + 26)
  //
  // BA
  // 53
  //
  // assumes that input are uppercase
  //
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Excel
  // Sheet Column Number.
  //
  // Memory Usage: 8.2 MB, less than 27.21% of C++ online submissions for Excel
  // Sheet Column Number.

  int titleToNumber_1(string s)
  {
    int result{};

    for (auto const e : s)
    {
      if (!isalpha(e))
        return 0;

      result = result * 26 + (e - 'A' + 1);
    }

    return result;
  }

  // base 10:
  //
  // 0  1   2   3   4   5   6   7   8   9
  // 10 11  12  13  14  15  16  17  18  19
  // ...
  //
  // 1  2   3   4   5   6   ... 25  26
  // A  B   C   D   E   F       Y   Z
  // % 26
  // 1  1   3   4   5   6   ... 25  0
  //
  // 27   28    29                  52
  // AA   AB    AC                  AZ
  // 1/1  1/2   1/3                 2/0
  //
  // so special handing when % is 0.
  //
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Excel
  // Sheet Column Title.
  //
  // Memory Usage: 8 MB, less than 84.11% of C++ online submissions for Excel
  // Sheet Column Title.

  string convertToTitle_1(int n)
  {
    string result{};
    char ch{};
    int value{};

    while (n)
    {
      value = (n % 26);
      n     = value == 0 ? (n / 26) - 1 : n / 26;
      ch    = value == 0 ? 'Z' : 'A' + value - 1;

      // not use algo-reverse
      // result += ch;
      result.insert(result.begin(), 1, ch);
    }

    return result;
  }

  // remove special handing by using "--n" and make it same as algo-itoa
  // from discussion:
  // Efficient C++ solution (easily transposable) by daedric

  string convertToTitle_2(int n)
  {
    string result{};
    char ch{};

    while (n)
    {
      --n;
      ch = 'A' + n % 26;
      n /= 26;

      // not use algo-reverse
      // result += ch;
      result.insert(result.begin(), 1, ch);
    }

    return result;
  }

  // algo-recursive
  // My 1 lines code in Java, C++, and Python, xcv58

  string convertToTitle_3(int n)
  {
    // update: because the behavior of different compilers, the safe version should be:
    return n == 0 ? ""
                  : convertToTitle_3((n - 1) / 26) + (char)((n - 1) % 26 + 'A');
  }

} // namespace leetcode_easy_171

TEST(LeetCode, Easy_171_ExcelSheetColumnNumber)
{
  using namespace leetcode_easy_171;

  {
    auto func = titleToNumber_1;

    EXPECT_THAT(func("A"), 1);
    EXPECT_THAT(func("AB"), 28);
    EXPECT_THAT(func("ZY"), 701);
  }
  {
    auto func = convertToTitle_1;

    EXPECT_THAT(func(1), "A");
    EXPECT_THAT(func(28), "AB");
    EXPECT_THAT(func(701), "ZY");
  }
  {
    auto func = convertToTitle_2;

    EXPECT_THAT(func(1), "A");
    EXPECT_THAT(func(28), "AB");
    EXPECT_THAT(func(701), "ZY");
  }
  {
    auto func = convertToTitle_3;

    EXPECT_THAT(func(1), "A");
    EXPECT_THAT(func(28), "AB");
    EXPECT_THAT(func(701), "ZY");
  }
}

// ={=========================================================================
// algo-leetcode-198
/*
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

  int rob_3(vector<int> &nums)
  {
    int a = 0;
    int b = 0;

    for (size_t i = 0; i < nums.size(); i++)
    {
      if (i % 2 == 0)
      {
        a = max(a + nums[i], b);
      }
      else
      {
        b = max(a, b + nums[i]);
      }
    }

    return max(a, b);
  }

} // namespace leetcode_easy_198

TEST(LeetCode, Easy_198_Rob)
{
  using namespace leetcode_easy_198;

  {
    auto func = rob_1;

    {
      vector<int> coll{1, 2, 3, 1};
      EXPECT_THAT(func(coll), 4);
    }
    {
      vector<int> coll{2, 7, 9, 3, 1};
      EXPECT_THAT(func(coll), 12);
    }

    // falis as expects 4
    {
      vector<int> coll{2, 1, 1, 2};
      EXPECT_THAT(func(coll), 3);
    }
  }
  {
    auto func = rob_2;

    // fails as expects 4
    {
      vector<int> coll{2, 1, 1, 2};
      EXPECT_THAT(func(coll), 5);
    }

    // fails as expects 4
    {
      vector<int> coll{1, 2, 3, 1};
      EXPECT_THAT(func(coll), 6);
    }
  }

  {
    auto func = rob_3;

    {
      vector<int> coll{1, 2, 3, 1};
      EXPECT_THAT(func(coll), 4);
    }
    {
      vector<int> coll{2, 7, 9, 3, 1};
      EXPECT_THAT(func(coll), 12);
    }
    {
      vector<int> coll{2, 1, 1, 2};
      EXPECT_THAT(func(coll), 4);
    }
    {
      vector<int> coll{1, 2, 3, 1};
      EXPECT_THAT(func(coll), 4);
    }
    {
      vector<int> coll{2, 7, 9, 3, 1, 4};
      EXPECT_THAT(func(coll), 15);
    }
  }
}

// ={=========================================================================
// algo-xxx

namespace algo_xxx
{
  int water_volume(vector<int> const &A)
  {
    int lindex{};
    int rindex{A.size() - 1};
    int lmax{};
    int rmax{};
    int volume{};

    while (lindex < rindex)
    {
      if (A[lindex] < A[rindex])
      {
        lmax = max(lmax, A[lindex]);
        volume += (lmax - A[lindex]);
        ++lindex;
      }
      else
      {
        rmax = max(rmax, A[rindex]);
        volume += (rmax - A[rindex]);
        --rindex;
      }
    }

    return volume;
  }
} // namespace algo_xxx

TEST(X, XX)
{
  using namespace algo_xxx;

  auto func = water_volume;

  {
    vector<int> coll{2, 5, 1, 2, 3, 4, 7, 7, 6};
    EXPECT_THAT(func(coll), 10);
  }
  {
    vector<int> coll{2, 5, 1, 3, 1, 2, 1, 7, 7, 6};
    EXPECT_THAT(func(coll), 17);
  }
  {
    vector<int> coll{2, 5, 4, 3, 4, 7, 6, 5, 4, 5, 7, 9, 5, 4, 3, 4, 5, 6};
    EXPECT_THAT(func(coll), 21);
  }
}

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
        : row_(row)
        , col_(col)
        , next_(nullptr)
    {}

    int row_{};
    int col_{};

    ListEntry *next_;
  };

  // cxx-operator-overload
  bool operator==(ListEntry const &lhs, ListEntry const &rhs)
  {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_) ? true : false;
  }

  bool operator!=(ListEntry const &lhs, ListEntry const &rhs)
  {
    return !(lhs == rhs);
  }

  class List
  {
  public:
    explicit List() noexcept
        : head_(nullptr)
    {}

    bool emptry() { return count_ == 0 ? true : false; }

    int size() { return count_; }

    // push_back()
    void push_old(ListEntry const &entry)
    {
      if (!head_)
        head_ = new ListEntry(entry);
      else
      {
        ListEntry *run = head_;

        // unlike clear(), snap(), run shall be before end() so that can
        // insert new one. Hence check run->next

        while (run->next_)
          run = run->next_;

        run->next_ = new ListEntry(entry);
      }

      ++count_;
    }

    // push_back()
    void push(ListEntry const &entry)
    {
      ListEntry *run{};

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
      ListEntry *run = head_;
      ListEntry *prev{};

      while (run)
      {
        prev = run;
        run  = run->next_;
        free(prev);
        --count_;
      }

      head_ = run;
    }

    std::vector<ListEntry> snap()
    {
      ListEntry *run = head_;
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

    ListEntry *head_;
  };

  struct ListNode
  {
    explicit ListNode(int row = 0, int col = 0) noexcept
        : row_(row)
        , col_(col)
        , next(nullptr)
    {}

    int row_{};
    int col_{};

    ListNode *next;
  };

  bool hasCycle(ListNode *head)
  {
    ListNode *slow;
    ListNode *fast;

    for (slow = head, fast = slow;
         slow && (fast = fast->next) && (fast = fast->next);)
    {
      if (slow == fast)
        return true;

      slow = slow->next;
    }

    return false;
  }

} // namespace algo_list_linked

TEST(AlgoList, LinkedSimple)
{
  using namespace algo_list_linked;

  std::vector<ListEntry> values{ListEntry(1, 2),
                                ListEntry(2, 3),
                                ListEntry(3, 4),
                                ListEntry(4, 5),
                                ListEntry(5, 6)};

  List coll;

  for (auto &e : values)
    coll.push(e);

  EXPECT_THAT(coll.size(), 5);

  coll.push(ListEntry(6, 7));
  EXPECT_THAT(coll.size(), 6);

  // requires cxx-operator-overload
  std::vector<ListEntry> expected{ListEntry(1, 2),
                                  ListEntry(2, 3),
                                  ListEntry(3, 4),
                                  ListEntry(4, 5),
                                  ListEntry(5, 6),
                                  ListEntry(6, 7)};

  EXPECT_THAT(coll.snap(), expected);

  coll.clear();
  EXPECT_THAT(coll.size(), 0);
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

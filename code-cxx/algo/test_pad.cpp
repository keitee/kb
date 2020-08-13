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
  // build prefix sum, O(n)
  //   (1, 2, 3, 4, 5, 6)
  // (0,1, 3, 6,10,15,21)
  //
  //         a0            a1              a2                ...   an−1
  // p0 = 0  p1 = a0       p2 = a0 + a1    p3 = a0 + a1 + a2 ...   pn = a0 + a1 + ... + an−1
  // p0 = 0  p1 = p0 + a0  p2 = p1 + a1    p3 = p2 + a2      ...   pn = pn-1 + an−1

  std::vector<int> build_prefix_sum(const vector<int> &coll)
  {
    int size = coll.size() + 1;

    std::vector<int> sum(size);

    for (int i = 1; i < size; ++i)
    {
      sum[i] = sum[i - 1] + coll[i - 1];
    }

    return sum;
  }

  // int find_max_sub_array_1(std::vector<int> &nums) {}
} // namespace prefix_sum

TEST(AlgoPrefixSum, check_prefix_sum)
{
  using namespace prefix_sum;

  EXPECT_THAT(build_prefix_sum({1, 2, 3, 4, 5, 6}),
              ElementsAre(0, 1, 3, 6, 10, 15, 21));

  EXPECT_THAT(build_prefix_sum({2, 3, 7, 5, 1, 3, 9}),
              ElementsAre(0, 2, 5, 12, 17, 18, 21, 30));
}

namespace prefix_sum
{
  //    0  1  2  3  4  5
  // p0 1  2  3  4  5  6
  //   {1, 2, 3, 4, 5, 6}
  // (0,1, 3,[6,10,15],21)
  //
  // 15-3 = 12

  int get_slice_sum(const vector<int> &sum, int first, int last)
  {
    // cout << "prefix sum [" << last + 1 << "] - [" << first << "]\n";
    return sum[last + 1] - sum[first];
  }
} // namespace prefix_sum

TEST(AlgoPrefixSum, check_sum_slice)
{
  using namespace prefix_sum;

  EXPECT_THAT(get_slice_sum(build_prefix_sum({1, 2, 3, 4, 5, 6}), 2, 4), 12);

  // (0,2,5,[12,17,18],21,30)
  // 18-5 = 13

  EXPECT_THAT(get_slice_sum(build_prefix_sum({2, 3, 7, 5, 1, 3, 9}), 2, 4), 13);
}

namespace prefix_sum
{
  // old
  int mushroom_picker_1(const vector<int> &A, int start, int moves)
  {
    int max_input_index = A.size() - 1;
    int result{};
    auto prefix_sum = build_prefix_sum(A);
    int loop_max{};

    // cout << "start: " << start << ", moves: " << moves << endl;
    loop_max = min(start, moves) + 1;
    for (int prefix_index = 0; prefix_index < loop_max; ++prefix_index)
    {
      int left_pos           = start - prefix_index;
      int shift_value        = (moves - 2 * prefix_index);
      int possible_right_pos = start + shift_value;

      // max? since right end cannot be less than start
      int max_on_possible_right_pos = max(start, possible_right_pos);

      // min? since right end cannot be greater than max index of input.
      int right_pos = min(max_input_index, max_on_possible_right_pos);

      // collect mushroon only once? count_total use range and counts only once.
      auto xresult = get_slice_sum(prefix_sum, left_pos, right_pos);
      result       = max(result, xresult);

      // cout << "loop(" << prefix_index << ", " << loop_max << ")"
      //   << ", left: " << setw(2) << left_pos
      //   << ", shift: " << setw(2) << shift_value
      //   << ", right: " << setw(2) << possible_right_pos << " -> "
      //   << setw(2) << max_on_possible_right_pos << " -> " << setw(2) << right_pos
      //   << ", xresult: " << xresult << ", result: " << result << endl;
    }

    // cout << "start: " << start << ", moves: " << moves << endl;

    // from codility code but looks wrong.
    // loop_max = min(moves+1, max_input_index-start);
    loop_max = min(moves, max_input_index - start) + 1;
    for (int prefix_index = 0; prefix_index < loop_max; ++prefix_index)
    {
      int right_pos = start + prefix_index;
      // left_pos = max(0, min(K, K-(M-2*prefix_index)));
      int shift_value       = (moves - 2 * prefix_index);
      int possible_left_pos = start - shift_value;

      // min? left end should be less than the start
      int min_on_possible_left_pos = min(start, possible_left_pos);

      // max? cannot not less than 0
      int left_pos = max(0, min_on_possible_left_pos);

      auto xresult = get_slice_sum(prefix_sum, left_pos, right_pos);
      result       = max(result, xresult);

      // cout << "loop(" << prefix_index << ", " << loop_max << ")"
      //   << ", shift: " << setw(2) << shift_value
      //   << ", left: " << setw(2) << possible_left_pos << " -> "
      //   << setw(2) << min_on_possible_left_pos << " -> " << setw(2) << left_pos
      //   << ", right: " << setw(2) << right_pos
      //   << ", xresult: " << xresult << ", result: " << result << endl;
    }

    return result;
  }

  // old
  int mushroom_picker_2(const vector<int> &A, int start, int moves)
  {
    int max_input_index = A.size() - 1;
    int num_loop{};
    int result{};

    auto prefix_sum = build_prefix_sum(A);

    // moves a window to the left
    // since `start` is actually index, it's sure to have elements in [0,start] so
    // use it directly
    num_loop = min(start, moves) + 1;
    for (int i = 0; i < num_loop; ++i)
    {
      int left_end    = start - i;
      int shift_value = (moves - 2 * i);
      int right_end   = start + shift_value;

      // right_end should be in [start, max input index]
      int right_contained      = min(max_input_index, right_end);
      int right_end_calculated = max(start, right_contained);

      int prefix_sum_result =
        get_slice_sum(prefix_sum, left_end, right_end_calculated);
      result = max(prefix_sum_result, result);
    }

    // moves a windows to the right
    // unlike `to the left` case, cannot use start and have to use max input
    // index.
    num_loop = min(moves, max_input_index - start) + 1;
    for (int i = 0; i < num_loop; ++i)
    {
      int right_end   = start + i;
      int shift_value = (moves - 2 * i);
      int left_end    = start - shift_value;

      // left_end should be in [0, start]
      int left_end_contained  = max(0, left_end);
      int left_end_calculated = min(start, left_end_contained);

      int prefix_sum_result =
        get_slice_sum(prefix_sum, left_end_calculated, right_end);
      result = max(prefix_sum_result, result);
    }

    return result;
  }

  // last is a last index of array.
  std::pair<int, int>
    get_slice_index(int last, int start, int max_move, int current_move)
    {
      auto slice_last = std::min(start + current_move, last);

      // since moves right and moves left back so current_move uses twice.
      auto slice_first = std::max(start - (max_move - current_move * 2), 0);

      return {slice_first, slice_last};
    }

  // 2020.08
  int mushroom_picker_3(const std::vector<int> &coll, int start, int moves)
  {
    int result{std::numeric_limits<int>::min()};
    int last = coll.size() - 1;

    // get prefix sum
    auto sum = build_prefix_sum(coll);

    for (int i = start, count = 0; count < moves && i < (int)coll.size();
        ++i, ++count)
    {
      // `start` is fixed
      auto slice = get_slice_index(last, start, moves, count);

      // cout << "slice {" << slice.first << ", " << slice.second << "}\n";

      auto slice_sum = get_slice_sum(sum, slice.first, slice.second);
      if (slice_sum > result)
        result = slice_sum;
    }

    return result;
  }
} // namespace prefix_sum

TEST(AlgoPrefixSum, check_mushroom_picker)
{
  using namespace prefix_sum;

  auto imps = {mushroom_picker_1,mushroom_picker_2,mushroom_picker_3};

  for (const auto &f: imps)
  {
    EXPECT_THAT(f({2, 3, 7, 5, 1, 3, 9}, 4, 6), 25);

    EXPECT_THAT(f({13, 12, 11, 2, 4, 6, 8, 10, 2, 3, 7, 5, 1, 3, 9}, 8, 5), 32);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

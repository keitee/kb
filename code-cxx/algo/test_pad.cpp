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
  // To sum:
  // 1. GT version end loop when "first == end"
  // 2. GT and iterator version work for both returning bool and returing index
  // to insert.
  // 3. GT and index version works differently for both returning bool and
  // returning index.
  // 4. GT version check if it's found when ends the loop
  //
  // Q:should have empty input check? YES, if not, do access to first like
  // "*first"

  // GT(Greater) version, use iterator, return bool.
  //
  // NOTE: access to end, "*end" is okay?
  //
  // NOTE: since distance(first, last) exclude last, "last = mid" is missing out
  // last elements? `first` gets updated and will be the right pos.
  //
  // if (value <= *mid)
  // {
  //   last = mid;
  // }

  template <typename _Iterator, typename _T>
  bool my_binary_search_1_1(_Iterator first, _Iterator last, const _T value)
  {
    while (first < last)
    {
      _Iterator mid = first + std::distance(first, last) / 2;

      if (value <= *mid)
      {
        last = mid;
      }
      else if (value > *mid)
      {
        first = std::next(mid);
      }
    }

    return value == *first ? true : false;
  }

  // NOTE: has "equal check" in the loop.
  template <typename _Iterator, typename _T>
  bool my_binary_search_1_1_1(_Iterator first, _Iterator last, const _T value)
  {
    while (first < last)
    {
      _Iterator mid = first + std::distance(first, last) / 2;

      if (value == *mid)
      {
        return true;
      }
      else if (value <= *mid)
      {
        last = mid;
      }
      else if (value > *mid)
      {
        first = std::next(mid);
      }
    }

    return value == *first ? true : false;
  }

  // GT(Greater) version, use iterator, return index.
  // return index which it is found or is to insert if not found.
  template <typename _Iterator, typename _T>
  size_t my_binary_search_1_2(_Iterator first, _Iterator last, const _T value)
  {
    _Iterator start{first};

    while (first < last)
    {
      _Iterator mid = first + std::distance(first, last) / 2;

      // std::cout << "middle : " << std::distance(first, last) / 2 << std::endl;

      if (value <= *mid)
      {
        last = mid;
      }
      else if (value > *mid)
      {
        first = std::next(mid);
      }
    }

    return std::distance(start, first);
  }

  // GT(Greater) version, use index, return index.
  // return index which it is found or is to insert if not found.
  template <typename _T>
  size_t my_binary_search_1_3(vector<_T> &coll, const _T value)
  {
    size_t first = 0;
    size_t last  = coll.size() - 1;

    while (first < last)
    {
      size_t mid = (first + last) / 2;

      // std::cout << "middle : " << mid << std::endl;

      if (value <= coll[mid])
      {
        last = mid;
      }
      else if (value > coll[mid])
      {
        first = ++mid;
      }
    }

    return first;
  }

  // GT(Greater) version, use index, return *bool*.
  template <typename _T>
  bool my_binary_search_1_4(vector<_T> &coll, const _T value)
  {
    size_t first = 0;
    size_t last  = coll.size() - 1;

    while (first < last)
    {
      size_t mid = (first + last) / 2;

      // std::cout << "middle : " << mid << std::endl;

      if (value <= coll[mid])
      {
        last = mid;
      }
      else if (value > coll[mid])
      {
        first = ++mid;
      }
    }

    return value == coll[first] ? true : false;
  }

  // To sum:
  // 1. EQ ends loop when first > last.
  // 2. EQ check if it's found while in the loop
  //
  // ansic, p58
  // cracking the coding interview, p120
  // Programming Pearl, p46
  //
  // note:
  //
  // *cxx-undefined* since can be negative
  // size_t low{}; size_t high{}; size_t mid{};
  // it can cause `overflow` when mid gets minus value
  // mid = (high-low)/2 + low;
  //
  // it has the same as distance() in iterator version or can use
  // length approach as stl version.

  // EQ(Equality) version, use index, return index
  int my_binary_search_2_1(vector<int> &coll, int key)
  {
    int low{};
    int high{};
    int mid{};

    low  = 0;
    high = coll.size() - 1;

    while (low <= high)
    {
      mid = (low + high) / 2;

      if (key == coll[mid])
        return mid;
      else if (key < coll[mid])
        high = mid - 1;
      else
        low = mid + 1;
    }

    // to return index
    return low;
  }

  int my_binary_search_2_2(vector<int> &coll, int key)
  {
    int low{};
    int high{};
    int mid{};

    low  = 0;
    high = coll.size() - 1;

    while (low <= high)
    {
      mid = (low + high) / 2;

      if (key == coll[mid])
        return true;
      else if (key < coll[mid])
        high = mid - 1;
      else
        low = mid + 1;
    }

    return false;
  }

  // EQ(Equality) version, use iterator, return bool
  template <typename _Iterator, typename _T>
  bool my_binary_search_2_3(_Iterator begin, _Iterator end, _T const key)
  {
    while (begin <= end)
    {
      auto middle = std::distance(begin, end) / 2;

      // cout << "while: middle : " << middle << endl;

      if (key == *(begin + middle))
        return true;
      else if (key > *(begin + middle))
        begin = begin + middle + 1;
      else
        end = begin + middle - 1;
    }

    // when not found only
    return false;
  }

  // EQ(Equality) version, use iterator, return index
  //
  // NOTE:
  // 1. access end() value.
  //
  // it sometimes fail or sometimes succeed because "*input_end" can be either
  // bigger or smaller then key value. if that's bigger then key, "begin" stays
  // the same which is "input_end" so pass. if not, begin advances and fails.
  //
  // EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 7), 4);
  //
  // 2. "<=" on iterator which assumes random access so limited.

  template <typename _Iterator, typename _T>
  _T my_binary_search_2_4(_Iterator begin, _Iterator end, _T const key)
  {
    _Iterator saved_begin = begin;

    while (begin <= end)
    {
      auto middle = std::distance(begin, end) / 2;

      // cout << "while: middle : " << middle << endl;

      if (key == *(begin + middle))
        return std::distance(saved_begin, begin + middle);
      else if (key > *(begin + middle))
        begin = begin + middle + 1;
      else
        end = begin + middle - 1;
    }

    // when not found only
    return std::distance(saved_begin, begin);
  }

  // NOTE:
  // To sum, use EQ and index version.
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  // cxx-binary-search, stl version
  //
  // 11.10 Sorted-Range Algorithms
  //
  // The following algorithms search certain values in sorted ranges.  Checking
  // Whether One Element Is Present
  //
  // bool binary_search (ForwardIterator beg, ForwardIterator end, const T&
  // value)
  //
  // bool binary_search (ForwardIterator beg, ForwardIterator end, const T&
  // value, BinaryPredicate op)
  {
    //                     0  1  2  3   4   5   6   7   8   9  10  11  12
    std::vector<int> coll1{2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};
    EXPECT_THAT(std::binary_search(coll1.cbegin(), coll1.cend(), 15), true);
    EXPECT_THAT(std::binary_search(coll1.cbegin(), coll1.cend(), 32), false);

    //                     0  1  2  3
    std::vector<int> coll2{1, 3, 5, 6};
    EXPECT_THAT(std::binary_search(coll2.cbegin(), coll2.cend(), 5), true);
    EXPECT_THAT(std::binary_search(coll2.cbegin(), coll2.cend(), 2), false);
  }

  //                    0  1  2  3
  std::vector<int> coll{1, 3, 5, 6};

  // GT(Greater) version, use iterator, return bool.
  {
    EXPECT_THAT(my_binary_search_1_1(coll.cbegin(), coll.cend(), 5), true);
    EXPECT_THAT(my_binary_search_1_1(coll.cbegin(), coll.cend(), 2), false);
    EXPECT_THAT(my_binary_search_1_1(coll.cbegin(), coll.cend(), 7), false);
    EXPECT_THAT(my_binary_search_1_1(coll.cbegin(), coll.cend(), 0), false);
  }

  {
    EXPECT_THAT(my_binary_search_1_1_1(coll.cbegin(), coll.cend(), 5), true);
    EXPECT_THAT(my_binary_search_1_1_1(coll.cbegin(), coll.cend(), 2), false);
    EXPECT_THAT(my_binary_search_1_1_1(coll.cbegin(), coll.cend(), 7), false);
    EXPECT_THAT(my_binary_search_1_1_1(coll.cbegin(), coll.cend(), 0), false);
  }

  // GT(Greater) version, use iterator, return index.
  {
    EXPECT_THAT(my_binary_search_1_2(coll.cbegin(), coll.cend(), 5), 2);
    EXPECT_THAT(my_binary_search_1_2(coll.cbegin(), coll.cend(), 2), 1);
    EXPECT_THAT(my_binary_search_1_2(coll.cbegin(), coll.cend(), 7), 4);
    EXPECT_THAT(my_binary_search_1_2(coll.cbegin(), coll.cend(), 0), 0);
  }

  // GT(Greater) version, use index, return index.
  {
    EXPECT_THAT(my_binary_search_1_3(coll, 5), 2);
    EXPECT_THAT(my_binary_search_1_3(coll, 2), 1);

    // fails
    // EXPECT_THAT(my_binary_search_1_3(coll, 7), 4);
    // wrong value
    EXPECT_THAT(my_binary_search_1_3(coll, 7), 3);

    EXPECT_THAT(my_binary_search_1_3(coll, 0), 0);
  }

  // GT(Greater) version, use index, return *bool*.
  {
    EXPECT_THAT(my_binary_search_1_4(coll, 5), true);
    EXPECT_THAT(my_binary_search_1_4(coll, 2), false);
    EXPECT_THAT(my_binary_search_1_4(coll, 7), false);
    EXPECT_THAT(my_binary_search_1_4(coll, 0), false);
  }

  // EQ(Equality) version, use index, return index
  {
    EXPECT_THAT(my_binary_search_2_1(coll, 5), 2);
    EXPECT_THAT(my_binary_search_2_1(coll, 2), 1);
    EXPECT_THAT(my_binary_search_2_1(coll, 7), 4);
    EXPECT_THAT(my_binary_search_2_1(coll, 0), 0);
  }

  // EQ(Equality) version, use index, return bool
  {
    EXPECT_THAT(my_binary_search_2_2(coll, 5), true);
    EXPECT_THAT(my_binary_search_2_2(coll, 2), false);
    EXPECT_THAT(my_binary_search_2_2(coll, 7), false);
    EXPECT_THAT(my_binary_search_2_2(coll, 0), false);
  }

  // EQ(Equality) version, use iterator, return bool
  {
    EXPECT_THAT(my_binary_search_2_3(coll.cbegin(), coll.cend(), 5), true);
    EXPECT_THAT(my_binary_search_2_3(coll.cbegin(), coll.cend(), 2), false);
    EXPECT_THAT(my_binary_search_2_3(coll.cbegin(), coll.cend(), 7), false);
    EXPECT_THAT(my_binary_search_2_3(coll.cbegin(), coll.cend(), 0), false);
  }

  // EQ(Equality) version, use iterator, return index
  {
    EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 5), 2);
    EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 2), 1);

    // *cxx-undefined*
    // EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 7), 4);

    EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 0), 0);
  }
}

namespace algo_binary_search
{
  // /usr/include/c++/4.9/bits/stl_algobase.h
  // /**
  //  *  @brief Finds the first position in which @p __val could be inserted
  //  *         without changing the ordering.
  //  *  @ingroup binary_search_algorithms
  //  *  @param  __first   An iterator.
  //  *  @param  __last    Another iterator.
  //  *  @param  __val     The search term.
  //  *  @param  __comp    A functor to use for comparisons.
  //  *  @return An iterator pointing to the first element <em>not less
  //  *           than</em> @p __val, or end() if every element is less
  //  *           than @p __val.
  //  *  @ingroup binary_search_algorithms
  //  *
  //  *  The comparison function should have the same effects on ordering as
  //  *  the function used for the initial sort.
  // */
  //
  // template<typename _ForwardIterator, typename _Tp>
  //   inline _ForwardIterator
  //   lower_bound(
  //      _ForwardIterator __first,
  //      _ForwardIterator __last,
  //      const _Tp& __val)
  //   {
  //     return std::__lower_bound(__first, __last, __val,
  //         __gnu_cxx::__ops::__iter_less_val());
  //   }

  // exact copy from stl but removes _Compare
  template <typename _ForwardIterator, typename _Tp>
  _ForwardIterator __my_lower_bound(_ForwardIterator __first,
                                    _ForwardIterator __last,
                                    const _Tp &__val)
  {
    typedef
      typename iterator_traits<_ForwardIterator>::difference_type _DistanceType;

    _DistanceType __len = std::distance(__first, __last);

    while (__len > 0)
    {
      _DistanceType __half      = __len >> 1;
      _ForwardIterator __middle = __first;
      std::advance(__middle, __half);

      // middle < val
      // if (__comp(__middle, __val))
      if (*__middle < __val)
      {
        // advance first
        __first = __middle;

        // do not include `middle`
        ++__first;
        __len = __len - __half - 1;
      }
      else
        __len = __half;
    }

    return __first;
  }

  template <typename _ForwardIterator, typename _Tp>
  inline _ForwardIterator my_lower_bound(_ForwardIterator __first,
                                         _ForwardIterator __last,
                                         const _Tp &__val)
  {
    return __my_lower_bound(__first, __last, __val);
  }

  // if implements binary search using lower_bound() way
  template <typename _Iterator, typename _T>
  _T my_binary_search_3(_Iterator first, _Iterator last, _T const value)
  {
    auto _saved_first{first};
    auto _length = std::distance(first, last);

    // _length is "distance between two elements"; (b,e)
    while (_length > 0)
    {
      decltype(_length) _half = _length >> 1;
      _Iterator _middle       = first;
      std::advance(_middle, _half);

      if (*_middle < value)
      {
        first = _middle;
        ++first;
        _length = _length - _half - 1;
      }
      else
      {
        _length = _half;
      }
    }

    // found index or index to insert
    return std::distance(_saved_first, first);
  }

  // template<typename _ForwardIterator, typename _Tp>
  //   bool
  //   binary_search(_ForwardIterator __first, _ForwardIterator __last,
  //       const _Tp& __val)
  //   {
  //     typedef typename iterator_traits<_ForwardIterator>::value_type
  //       _ValueType;
  //
  //     _ForwardIterator __i
  //       = std::__lower_bound(__first, __last, __val,
  //           __gnu_cxx::__ops::__iter_less_val());
  //     return __i != __last && !(__val < *__i);
  //   }

  // since __my_lower_bound() can return `end` or `iterator for element >=
  // value. so two checks
  //
  // return __i != __last && !(__val < *__i);

  template <typename _ForwardIterator, typename _Tp>
  bool my_binary_search_4(_ForwardIterator __first,
                          _ForwardIterator __last,
                          const _Tp &__val)
  {
    _ForwardIterator __i = __my_lower_bound(__first, __last, __val);

    return __i != __last && !(__val < *__i);
  }
} // namespace algo_binary_search

TEST(AlgoSearch, check_binary_search_stl_version)
{
  //  0  1  2  3  4  5  6  7  8  9  0  1  2
  // {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9};
  //                          ^^^^^^^
  // lower_bound() returns the position of the first element that has a value
  // equal to or greater than `value`. This is the first position where an
  // element with value could get inserted without breaking the sorting of the
  // range [beg,end).

  // since lower_bound() uses cbegin().
  //
  // : error: no matching function for call to
  // ‘distance(std::vector<int>::iterator, __gnu_cxx::__normal_iterator<const int*, std::vector<int> >&)’
  //
  // EXPECT_THAT(distance(coll.begin(), first), 8);

  // this is not member function as:
  // std::map<Key,T,Compare,Allocator>::lower_bound
  // std::set<Key,Compare,Allocator>::lower_bound

  {
    std::vector<int> coll{1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9};

    auto pos = std::lower_bound(coll.cbegin(), coll.cend(), 5);
    EXPECT_THAT(*pos, 5);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 8);
  }

  {
    std::vector<int> coll{2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};

    auto pos = std::lower_bound(coll.cbegin(), coll.cend(), 1);
    EXPECT_THAT(*pos, 2);
  }

  // // EQ(Equality) version, use iterator, return index
  // {
  //   EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 5), 2);
  //   EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 2), 1);
  //   // *cxx-undefined*
  //   // EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 7), 4);
  //   EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 0), 0);
  // }

  // can do this with std::lower_bound()? YES
  {
    std::vector<int> coll{1, 3, 5, 6};

    auto pos = std::lower_bound(coll.cbegin(), coll.cend(), 5);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 2);

    pos = std::lower_bound(coll.cbegin(), coll.cend(), 2);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 1);

    pos = std::lower_bound(coll.cbegin(), coll.cend(), 7);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 4);

    pos = std::lower_bound(coll.cbegin(), coll.cend(), 0);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 0);
  }

  {
    using namespace algo_binary_search;

    std::vector<int> coll{1, 3, 5, 6};

    auto pos = my_lower_bound(coll.cbegin(), coll.cend(), 5);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 2);

    pos = my_lower_bound(coll.cbegin(), coll.cend(), 2);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 1);

    pos = my_lower_bound(coll.cbegin(), coll.cend(), 7);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 4);

    pos = my_lower_bound(coll.cbegin(), coll.cend(), 0);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 0);
  }

  // return index
  {
    using namespace algo_binary_search;

    std::vector<int> coll{1, 3, 5, 6};

    EXPECT_THAT(my_binary_search_3(coll.cbegin(), coll.cend(), 5), 2);
    EXPECT_THAT(my_binary_search_3(coll.cbegin(), coll.cend(), 2), 1);
    EXPECT_THAT(my_binary_search_3(coll.cbegin(), coll.cend(), 7), 4);
    EXPECT_THAT(my_binary_search_3(coll.cbegin(), coll.cend(), 0), 0);
  }

  {
    using namespace algo_binary_search;

    std::vector<int> coll2{1, 3, 5, 6};

    EXPECT_THAT(my_binary_search_4(coll2.cbegin(), coll2.cend(), 5), true);
    EXPECT_THAT(my_binary_search_4(coll2.cbegin(), coll2.cend(), 2), false);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

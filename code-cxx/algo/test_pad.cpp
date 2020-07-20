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

namespace algopad
{
  struct my_less_1
  {
    template <typename _Iterator1, typename _Iterator2>
    bool operator()(_Iterator1 it1, _Iterator2 it2)
    {
      // see *cxx-pair-comparison*
      return *it1 < *it2;
    }
  };

  // use single iterator type
  struct my_less_2
  {
    template <typename _Iterator1>
    bool operator()(_Iterator1 it1, _Iterator1 it2)
    {
      // see *cxx-pair-comparison*
      return *it1 < *it2;
    }
  };

  // should make it template structure and if not, see:
  // error: ‘algopad::my_less_2’ is not a template
  //  template <typename _Iterator, typename _Compare = my_less_2<_Iterator>>

  template <typename _Iterator1>
  struct my_less_3
  {
    bool operator()(_Iterator1 it1, _Iterator1 it2)
    {
      // see *cxx-pair-comparison*
      return *it1 < *it2;
    }
  };

  // implement own min/max_element()
  // max_element(coll.begin(), coll.end();
  //
  // 1. how to define value_type of _Iterator? no need to define var for current
  // max value and comp function. All we need are iterators
  // 2. see comp is a copy

  template <typename _Iterator, typename _Compare>
  _Iterator my_max_element_1(_Iterator it1, _Iterator it2, _Compare comp)
  {
    // need to handle when there is only one element. otherwise, would access
    // to end which is error.
    if (it1 == it2)
      return it1;

    _Iterator result = it1;

    for (++it1; it1 != it2; ++it1)
    {
      // if "current < *it", that is get new max
      if (comp(result, it1))
        result = it1;
    }

    return result;
  }

  // "first and last" var naming seems better.
  template <typename _Iterator, typename _Compare>
  _Iterator
  my_max_element_2(_Iterator __first, _Iterator __last, _Compare __comp)
  {
    // if thre is only one
    if (__first == __last)
      return __first;

    _Iterator __result = __first;

    // if *__result < *__first
    while (++__first != __last)
      if (__comp(__result, __first))
        __result = __first;

    return __result;
  }

  // "first and last" var naming seems better. default comp
  template <typename _Iterator, typename _Compare = my_less_3<_Iterator>>
  _Iterator my_max_element_3(_Iterator __first,
                             _Iterator __last,
                             _Compare __comp = _Compare())
  {
    // if thre is only one
    if (__first == __last)
      return __first;

    _Iterator __result = __first;

    // if *__result < *__first
    while (++__first != __last)
      if (__comp(__result, __first))
        __result = __first;

    return __result;
  }

  template <typename _Iterator, typename _Compare>
  _Iterator
  my_min_element_2(_Iterator __first, _Iterator __last, _Compare __comp)
  {
    if (__first == __last)
      return __first;

    _Iterator __result = __first;

    while (++__first != __last)
      if (!__comp(__result, __first))
        __result = __first;

    return __result;
  }

  // same but "if (__comp(__first, __result))" which seems better
  template <typename _Iterator, typename _Compare>
  _Iterator
  my_min_element_3(_Iterator __first, _Iterator __last, _Compare __comp)
  {
    if (__first == __last)
      return __first;

    _Iterator __result = __first;

    while (++__first != __last)
      if (__comp(__first, __result))
        __result = __first;

    return __result;
  }

  // std-minmax
  // If more than one minimum or maximum element exists, min_element() and
  // max_element() return `the first` found;
  //
  // minmax_element() returns the first minimum but the last maximum element, so
  // max_element() and minmax_element() don’t yield the same maximum element.

  template <typename _Iterator>
  std::pair<_Iterator, _Iterator> my_minmax_element_1(_Iterator __first,
                                                      _Iterator __last)
  {
    if (__first == __last)
      return std::make_pair(__first, __last);

    _Iterator __max = __first;
    _Iterator __min = __first;

    while (++__first != __last)
    {
      // get max. add "=" to get the last max as std version
      if (*__max <= *__first)
        __max = __first;

      // get min
      if (*__first < *__min)
        __min = __first;
    }

    return std::make_pair(__min, __max);
  }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  // on std-map
  {
    // sorted by key
    std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

    auto e = std::max_element(counts.begin(), counts.end());
    EXPECT_THAT(*e, std::make_pair(13, 1));

    // ForwardIterator
    // max_element (ForwardIterator beg, ForwardIterator end, CompFunc op)
    // op is used to compare two elements:
    // op(elem1,elem2)
    // It should return true when the first element is less than the second
    // element.

    auto maxelem = std::max_element(
      std::begin(counts),
      std::end(counts),
      [](pair<int, size_t> const &e1, pair<int, size_t> const &e2) {
        return e1.second < e2.second;
      });

    EXPECT_THAT(*maxelem, std::make_pair(5, 3));
  }

  // multimap
  {
    // sorted by key and the order in the equal range are the order of input
    std::multimap<int, size_t> counts{{1, 2},
                                      {3, 9},
                                      {3, 8},
                                      {5, 3},
                                      {8, 3},
                                      {13, 2},
                                      {13, 4},
                                      {13, 12},
                                      {13, 1}};

    // for (auto &e : counts)
    //   cout << e.first << ", " << e.second << endl;

    // Q: how max_element() finds the max on the second?
    // see *cxx-pair-comparison*

    auto e = std::max_element(counts.begin(), counts.end());
    EXPECT_THAT(*e, std::make_pair(13, 12));
  }

  // max_element
  {
    // sorted by key
    std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

    {
      auto pos = my_max_element_1(counts.begin(), counts.end(), my_less_1());
      EXPECT_THAT(*pos, std::make_pair(13, 1));
    }

    {
      auto pos = my_max_element_2(counts.begin(), counts.end(), my_less_1());
      EXPECT_THAT(*pos, std::make_pair(13, 1));
    }
  }

  // max_element with default comp
  {
    // sorted by key
    std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

    {
      auto pos = my_max_element_3(counts.begin(), counts.end());
      EXPECT_THAT(*pos, std::make_pair(13, 1));
    }
  }

  // min_element
  {
    // sorted by key
    std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

    {
      auto pos = my_min_element_2(counts.begin(), counts.end(), my_less_1());
      EXPECT_THAT(*pos, std::make_pair(1, 2));
    }

    {
      auto pos = my_min_element_3(counts.begin(), counts.end(), my_less_1());
      EXPECT_THAT(*pos, std::make_pair(1, 2));
    }
  }

  // min_element and my_less_2
  {
    // sorted by key
    std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

    {
      auto pos = my_min_element_2(counts.begin(), counts.end(), my_less_2());
      EXPECT_THAT(*pos, std::make_pair(1, 2));
    }

    {
      auto pos = my_min_element_3(counts.begin(), counts.end(), my_less_2());
      EXPECT_THAT(*pos, std::make_pair(1, 2));
    }
  }

  {
    std::vector<int> coll{2, 3, 4, 5, 6, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6};

    // return iterator pair
    // Note also that minmax_element() yields `the last maximum`, so the distance
    // 9.
    auto minmax = my_minmax_element_1(coll.begin(), coll.end());
    EXPECT_THAT(*(minmax.first), -3);
    EXPECT_THAT(*(minmax.second), 6);
    EXPECT_THAT(std::distance(minmax.first, minmax.second), 9);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

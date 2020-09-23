#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <unordered_set>
#include <vector>
// #include <algorithm>
#include <limits>  // std::numeric_limits
#include <numeric> // std::accumulate for gcc 6
#include <random>

#include "gmock/gmock.h"

using namespace std;
using namespace testing;

template <typename T>
void INSERT_ELEMENTS(T &coll, int first, int last)
{
  for (auto i = first; i <= last; i++)
    coll.insert(coll.end(), i);
}

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
    cout << " {" << elem.first << ", " << elem.second << "}";
    ++count;
  }

  cout << " (" << count << ")" << endl;
}

// template <typename T>
// void PRINT_Q_ELEMENTS(const T& coll, const string &optstr = "")
// {
//   size_t count = coll.size();
//   cout << optstr;
//
//   for(size_t i = 0; i < count; ++i)
//     cout << coll.top() << " ";
//
//   cout << "(" << count << ")" << endl;
// }

// ={=========================================================================
// cxx-iter

// core will be generated
TEST(CxxIterator, invalidated)
{
  std::vector<int> coll{1, 2, 3, 4};

  auto it = find(coll.begin(), coll.end(), 2);
  if (it != coll.end())
  {
    it = coll.insert(it, 7);
    it = coll.insert(it, 6);
    it = coll.insert(it, 5);

    // Q: This seems okay since inserting element at the same pos,2? what will
    // happen?
    //
    // veci.insert( it, 7 );
    // veci.insert( it, 6 );
    // veci.insert( it, 5 );
    //
    // core dump since "it" gets invalidated.
  }

  EXPECT_THAT(coll, ElementsAre(1, 5, 6, 7, 2, 3, 4));
}

// ={=========================================================================
// *cxx-undefined*
// may cause runtime error but not always so undefined.
//
// Calling erase() for the element to which you are referring with it
// invalidates "it" as an iterator of coll and calling ++it results in
// undefined behavior.
//
// However, when use -D_GLIBCXX_DEBUG:
//
// /usr/include/c++/6/debug/safe_iterator.h:298:
// Error: attempt to increment a singular iterator.
//
// Objects involved in the operation:
//     iterator "this" @ 0x0x7ffcae5ac5d0 {
//       type =
//       __gnu_debug::_Safe_iterator<std::_Rb_tree_const_iterator<std::pair<std::__cxx11::basic_string<char,
//       std::char_traits<char>, std::allocator<char> > const, int> >,
//       std::__debug::map<std::__cxx11::basic_string<char,
//       std::char_traits<char>, std::allocator<char> >, int,
//       std::less<std::__cxx11::basic_string<char, std::char_traits<char>,
//       std::allocator<char> > >,
//       std::allocator<std::pair<std::__cxx11::basic_string<char,
//       std::char_traits<char>, std::allocator<char> > const, int> > > >
//       (constant iterator);
//
//       state = singular;
//
//       references sequence with type
//       'std::__debug::map<std::__cxx11::basic_string<char,
//       std::char_traits<char>, std::allocator<char> >, int,
//       std::less<std::__cxx11::basic_string<char, std::char_traits<char>,
//       std::allocator<char> > >,
//       std::allocator<std::pair<std::__cxx11::basic_string<char,
//       std::char_traits<char>, std::allocator<char> > const, int> > >' @
//       0x0x7ffcae5ac600 }

TEST(CxxIterator, DISABLED_invalidated_on_map)
{
  std::map<std::string, int> coll{{"one", 1},
                                  {"two", 2},
                                  {"three", 3},
                                  {"four", 4},
                                  {"five", 5}};

  int value = 2;

  for (auto it = coll.cbegin(); it != coll.cend(); ++it)
  {
    if (it->second == value)
      coll.erase(it);
  }
}

// ={=========================================================================
TEST(CxxIterator, map)
{
  // before C++11
  {
    std::map<std::string, int> coll{{"one", 1},
                                    {"two", 2},
                                    {"three", 3},
                                    {"four", 4},
                                    {"five", 5}};

    int value = 2;

    for (auto it = coll.cbegin(); it != coll.cend();)
    {
      // before C++11 since erase returns nothing. *cxx-side-effect*
      if (it->second == value)
        coll.erase(it++);
      else
        ++it;
    }
  }

  // after C++11
  {
    std::map<std::string, int> coll{{"one", 1},
                                    {"two", 2},
                                    {"three", 3},
                                    {"four", 4},
                                    {"five", 5}};

    int value = 2;

    for (auto it = coll.cbegin(); it != coll.cend();)
    {
      if (it->second == value)
        it = coll.erase(it);
      else
        ++it;
    }
  }
}

// ={=========================================================================
// https://stackoverflow.com/questions/37280744/got-singular-iterator-error-in-looping-with-iterator-and-pop-back

TEST(CxxIterator, deque)
{
  // why this work?
  {
    deque<int> coll{1, 2, 3};

    for (auto it = coll.rbegin(); it != coll.rend(); ++it)
    {
      cout << "it : " << *it << endl;
      coll.pop_back();
    }
  }
  // why this work?
  {
    deque<int> coll{1, 2, 3};

    for (auto it = coll.rbegin(); it != coll.rend();)
    {
      cout << "it : " << *it << endl;
      coll.pop_back();
      ++it;
    }
  }

  {
    deque<int> coll{1, 2, 3};

    for (auto it = coll.rbegin(); it != coll.rend();)
    {
      cout << "it : " << *it << endl;
      coll.pop_back();
      it = coll.rbegin();
    }
  }
}

// /usr/include/c++/6/debug/safe_iterator.h:149:
// Error: attempt to copy-construct an iterator from a singular iterator.
//
// Objects involved in the operation:
//     iterator "this" @ 0x0x7ffde23ae160 {
//       type = __gnu_debug::_Safe_iterator<std::__cxx1998::_Deque_iterator<int,
//       int&, int*>, std::__debug::deque<int, std::allocator<int> > > (mutable
//       iterator); state = singular;
//     }
//     iterator "other" @ 0x0x7ffde23ae1f0 {
//       type = __gnu_debug::_Safe_iterator<std::__cxx1998::_Deque_iterator<int,
//       int&, int*>, std::__debug::deque<int, std::allocator<int> > > (mutable
//       iterator); state = singular; references sequence with type
//       'std::__debug::deque<int, std::allocator<int> >' @ 0x0x7ffde23ae230
//     }
// Aborted

TEST(DISABLED_Iterator, InvalidOnDeque_Errors)
{
  {
    deque<int> coll{1, 2, 3};

    for (auto it = coll.rbegin(); it != coll.rend();)
    {
      it = coll.rbegin();
      cout << "it : " << *it << endl;
      coll.pop_back();
    }
  }
  {
    deque<int> coll{1, 2, 3};

    for (auto it = coll.rbegin(); it != coll.rend();)
    {
      ++it;
      cout << "it : " << *it << endl;
      coll.pop_back();
    }
  }
}

int ReturnInteger()
{
  int value{3301};
  return value;
}

struct StructValue
{
  int value_;

  StructValue(int value)
      : value_(value)
  {}

  int operator++() { return ++value_; }
};

StructValue ReturnStruct()
{
  StructValue value{3301};
  return value;
}

TEST(Iterator, NativeAndStructTemporary)
{
  // show that not allowed to modify tempory

  // cxx.cpp: In member function ‘virtual void
  // Temporary_NativeAndStruct_Test::TestBody()’: cxx.cpp:1539:45: error: lvalue
  // required as increment operand
  //    cout << "return int: " << ++ReturnInteger() << endl;
  // cout << "return int: " << ++ReturnInteger() << endl;

  EXPECT_THAT(++ReturnStruct(), 3302);
}

TEST(Iterator, OperationOnTemporary)
{
  {
    vector<int> coll{4, 2, 6, 3, 1, 5};
    sort(++coll.begin(), coll.end());
    EXPECT_THAT(coll, ElementsAre(4, 1, 2, 3, 5, 6));
  }

  {
    string coll{"this is a string object"};
    sort(++coll.begin(), coll.end());
    EXPECT_THAT(coll, Eq("t    abceghiiijnorssstt"));
  }
}

// ={=========================================================================
TEST(CxxIterator, next)
{
  // why use cxx-next
  //
  // 9.2.6 The Increment and Decrement Problem of Vector Iterators The
  //
  // Depending on the platform, the compilation of ++coll.begin() might fail.
  // However, if you use, for example, a deque rather than a vector, the
  // compilation always succeeds.  The reason for this strange problem lies in the
  // fact that iterators of vectors, arrays, and strings might be implemented as
  // ordinary pointers. And for all fundamental data types, such as pointers, you
  // are not allowed to modify temporary values. For structures and classes,
  // however, doing so is allowed. Thus, if the iterator is implemented as an
  // ordinary pointer, the compilation fails; if implemented as a class, it
  // succeeds.
  //
  // To make your code portable, the utility function next() is provided since
  // C++11 (see Sec- tion 9.3.2, page 443), so you can write:

  {
    std::vector<int> coll{2, 4, 1, 5, 6};

    std::sort(++coll.begin(), coll.end());

    EXPECT_THAT(coll, ElementsAre(2, 1, 4, 5, 6));
  }

  {
    std::vector<int> coll{2, 4, 1, 5, 6};

    std::sort(std::next(coll.begin()), coll.end());

    EXPECT_THAT(coll, ElementsAre(2, 1, 4, 5, 6));
  }

  // since next() uses copy, do not change "pos"
  {
    std::vector<int> coll{1, 2, 3, 4, 5};

    auto pos = coll.begin();
    EXPECT_EQ(*pos, 1);

    ++pos;

    EXPECT_EQ(*pos, 2);

    auto next_pos = std::next(pos);

    EXPECT_EQ(*pos, 2);
    EXPECT_EQ(*next_pos, 3);
  }

  // 9.3.2 next() and prev()
  // Note that next() does not check whether it crosses the end() of a sequence.
  // Thus, it is up to the caller to ensure that the result is valid.
  {
    std::vector<int> coll{1, 2, 3, 4, 5};
    std::vector<int> result{};

    for (auto pos = coll.begin(); pos != coll.end(); pos = std::next(pos))
      result.push_back(*pos);

    EXPECT_THAT(coll, result);
  }

  // what would happen when call next() on empty? cxx-undefined
  //
  // /usr/include/c++/4.9/debug/safe_iterator.h:356:error: attempt to advance a
  //     past-the-end iterator 1 steps, which falls outside its valid range.
  //
  // ok here since do not access through "result"
  {
    std::vector<int> coll{};

    auto pos    = coll.begin();
    auto result = std::next(pos);

    (void)result;
  }
}

// ={=========================================================================
TEST(CxxIterator, distance)
{
  // cxx-distance which returns positive/negative
  {
    std::vector<int> coll{1, 2, 3, 4, 5};

    auto pos = std::find(coll.begin(), coll.end(), 3);

    EXPECT_EQ(std::distance(coll.begin(), pos), 2);
    EXPECT_EQ(std::distance(pos, coll.begin()), -2);

    EXPECT_EQ(std::distance(coll.begin(), coll.end()), 5);
    EXPECT_EQ(std::distance(coll.end(), coll.begin()), -5);
  }

  {
    std::set<int> coll{1, 2, 3, 4, 5};

    auto pos = std::find(coll.begin(), coll.end(), 3);

    EXPECT_EQ(std::distance(coll.begin(), pos), 2);

    // /usr/include/c++/4.9/debug/safe_iterator.h:289:error: attempt to
    // increment
    //     a past-the-end iterator.
    //
    // Objects involved in the operation:
    // iterator "this" @ 0x0x7fff1a6a0580 {
    // type =
    // N11__gnu_debug14_Safe_iteratorISt23_Rb_tree_const_iteratorIiENSt7__debug3setIiSt4lessIiESaIiEEEEE
    // (mutable iterator);
    //   state = past-the-end;
    //   references sequence with type `NSt7__debug3setIiSt4lessIiESaIiEEE' @
    //   0x0x7fff1a6a0610
    // }
    // Aborted (core dumped)
    //
    // or when without DEBUG, stucks which seems to be looping.
    // EXPECT_EQ(distance(pos, coll.begin()), -2);
    //
    // since `++first` for non-random iterators:
    //
    // template<typename _InputIterator>
    //   inline typename iterator_traits<_InputIterator>::difference_type
    //   __distance(_InputIterator __first, _InputIterator __last,
    //              input_iterator_tag)
    //   {
    //     // concept requirements
    //     __glibcxx_function_requires(_InputIteratorConcept<_InputIterator>)
    //
    //     typename iterator_traits<_InputIterator>::difference_type __n = 0;
    //     while (__first != __last)
    // {
    //   ++__first;
    //   ++__n;
    // }
    //     return __n;
    //   }
    //
    // template<typename _RandomAccessIterator>
    //   inline typename iterator_traits<_RandomAccessIterator>::difference_type
    //   __distance(_RandomAccessIterator __first, _RandomAccessIterator __last,
    //              random_access_iterator_tag)
    //   {
    //     // concept requirements
    //     __glibcxx_function_requires(_RandomAccessIteratorConcept<
    // 			  _RandomAccessIterator>)
    //     return __last - __first;
    //   }

    EXPECT_EQ(std::distance(coll.begin(), coll.end()), 5);
    // EXPECT_EQ(distance(coll.end(), coll.begin()), -5);
  }
}

// ={=========================================================================
TEST(CxxIterator, array)
{
  {
    int vals[] = {33, 67, -4, 13, 5, 2};

    std::vector<int> coll(std::begin(vals), std::end(vals));

    decltype(coll) out;

    // These functions are also overloaded, so you can use STL containers or all
    // classes that provide begin() and end() as member functions:
    //
    // std::begin(v)  // yields v.begin()
    // std::end(v)    // yields v.end()

    // cxx-algo-copy
    std::copy(std::begin(coll), std::end(coll), back_inserter(out));

    EXPECT_THAT(out, ElementsAre(33, 67, -4, 13, 5, 2));
  }

  {
    int values[] = {33, 67, -4, 13, 5, 2};
    vector<int> vec(begin(values), end(values));

    copy(vec.begin(), vec.end(), ostream_iterator<int>(cout, " "));
    cout << endl;

    // on ordinary array
    copy(begin(values), end(values), ostream_iterator<int>(cout, " "));
    cout << endl;

    // on container
    copy(begin(vec), end(vec), ostream_iterator<int>(cout, " "));
    cout << endl;
  }
}

// ={=========================================================================
// Kinds of Insert Iterators
// The C++ standard library provides three kinds of insert iterators: back
// inserters, front inserters, and general inserters.
//
// They differ in their handling of the position at which to insert a value. In
// fact, each uses a different member function, which it calls for the container
// to which it belongs. Thus, an insert iterator must be always initialized with
// its container.
//
// c.insert(pos,elem)
//
// For this reason, back inserters are available only for vectors, deques,
// lists, and strings; front inserters are available only for deques and lists.
//
// Each kind of insert iterator has a convenience function for its creation and
// initialization.

TEST(CxxIterator, adapters_insert_kinds)
{
  std::list<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};

  // cxx-inserter-back
  {
    std::vector<int> ret;

    // cxx-algo-copy
    std::copy(coll.cbegin(), coll.cend(), std::back_inserter(ret));
    EXPECT_THAT(ret, ElementsAreArray({1, 2, 3, 4, 5, 6, 7, 8, 9}));
  }

  // *cxx-reverse*
  // cxx-inserter-front
  // have to use cxx-deque since it uses push_front
  {
    std::deque<int> ret;

    // cxx-algo-copy
    std::copy(coll.cbegin(), coll.cend(), std::front_inserter(ret));
    EXPECT_THAT(ret, ElementsAreArray({9, 8, 7, 6, 5, 4, 3, 2, 1}));
  }

  // *cxx-inserter*
  {
    std::vector<int> ret{};

    // cxx-algo-copy
    std::copy(coll.cbegin(), coll.cend(), std::inserter(ret, ret.begin()));

    EXPECT_THAT(ret, ElementsAreArray({1, 2, 3, 4, 5, 6, 7, 8, 9}));
  }
}

// ={=========================================================================
// shows that inserter() uses insert_iterator internally and inserter is helper
// 9.4.2 Insert Iterators

TEST(CxxIterator, adaptor_insert)
{
  {
    std::list<int> coll;

    coll.push_back(1);
    coll.push_back(2);
    coll.push_back(3);
    coll.push_back(44);
    coll.push_back(55);

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 44, 55));
  }

  // when not use insert interator, no insertion.
  {
    std::list<int> coll;

    auto it = coll.begin();

    EXPECT_THAT(coll.size(), 0);

    *it++ = 1;
    *it++ = 2;
    *it++ = 3;
    *it++ = 44;
    *it++ = 55;

    // NOTE: Q: WHY 55 but not 0??
    // EXPECT_THAT(coll.size(), 0);
  }

  // when use insert iterator
  // insert_iterator( Container& c, typename Container::iterator i );
  {
    std::list<int> coll;

    std::insert_iterator<std::list<int>> it(coll, coll.begin());

    *it = 1;
    *it = 2;
    *it = 3;
    *it = 44;
    *it = 55;

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 44, 55));
  }

  // inserter is a helper function
  {
    std::list<int> coll;

    auto it = std::inserter(coll, coll.begin());

    *it = 1;
    *it = 2;
    *it = 3;
    *it = 44;
    *it = 55;

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 44, 55));
  }

  {
    std::list<int> coll{1, 2, 3, 44, 55};
    std::list<int> ret{};

    std::copy(coll.begin(), coll.end(), std::inserter(ret, ret.begin()));

    EXPECT_THAT(ret, ElementsAre(1, 2, 3, 44, 55));
  }
}

// ={=========================================================================
// cxx-reverse
// 9.4.1 Reverse Iterators
// Most container classes — all except forward lists and unordered containers —
// as well as strings provide the ability to use reverse iterators to iterate
// over their elements.

TEST(CxxIterator, adaptor_reverse)
{
  {
    std::string coll{"PARK"};
    std::string result{};

    std::string::const_reverse_iterator it = coll.crbegin();

    while (it != coll.crend())
      result.push_back(*it++);

    EXPECT_THAT(result, "KRAP");
  }

  // pos vs rpos
  // You can convert normal iterators into reverse iterators. Naturally, the
  // iterators must be bidirectional iterators, but note that the logical
  // position of an iterator is moved during the conversion.

  {
    // create deque with elements from 1 to 9
    std::deque<int> coll = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::deque<int>::const_iterator pos;

    pos = std::find(coll.cbegin(), coll.cend(), 7);
    EXPECT_THAT(*pos, 7);

    std::deque<int>::const_reverse_iterator rpos(pos);
    EXPECT_THAT(*rpos, 6);
  }

  {
    std::string coll{"FIRST,MIDDLE,LAST"};

    auto delim = std::find(coll.cbegin(), coll.cend(), ',');

    EXPECT_THAT(std::string(coll.cbegin(), delim), "FIRST");

    // The result is "TSAL" since iterator goes backward and means [crbegin(),
    // rcomma). To get the expected result, shall use instead [rcomma.base(),
    // cend() ) which converts reverse iterator to normal interator.

    auto rdelim = std::find(coll.crbegin(), coll.crend(), ',');

    EXPECT_THAT(std::string(coll.crbegin(), rdelim), "TSAL");

    // Converting Reverse Iterators Back Using base()
    EXPECT_THAT(std::string(rdelim.base(), coll.cend()), "LAST");
  }

  {
    std::ostringstream os{};

    // create deque with elements from 1 to 9
    std::deque<int> coll = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // find range [2,7]
    deque<int>::const_iterator pos1;
    pos1 = std::find(coll.cbegin(), coll.cend(), 2);

    deque<int>::const_iterator pos2;
    pos2 = std::find(coll.cbegin(), coll.cend(), 7);

    // [2,6]
    std::for_each(pos1, pos2, [&](int e) { os << e << ","; });

    EXPECT_THAT(os.str(), "2,3,4,5,6,");

    deque<int>::const_reverse_iterator rpos1(pos1);
    deque<int>::const_reverse_iterator rpos2(pos2);

    os.str("");

    // *cxx-error* cause runtime error -D_GLIBCXX_DEBUG
    // for_each(rpos1, rpos2, [&](int e)
    //
    // since rpos1 that point 2 and rpos1++ means pos1-- and
    // goes below begin()

    std::for_each(rpos2, rpos1, [&](int e) { os << e << ","; });

    EXPECT_THAT(os.str(), "6,5,4,3,2,");
  }
}

// ={=========================================================================
TEST(CxxIterator, adaptor_stream)
{
  // as gets from std::cin, use stream
  {
    std::istringstream is{"1 2 3 4 5 6"};
    std::vector<int> coll;

    int value{};

    while (is >> value)
      coll.push_back(value);

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6));
  }

  // as gets from std::cin, use iterator
  {
    std::istringstream is{"1 2 3 4 5 6"};
    std::vector<int> coll;

    std::istream_iterator<int> isi(is), eof;

    while (isi != eof)
      coll.push_back(*isi++);

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6));
  }

  // use iterator but direct
  {
    std::istringstream is{"1 2 3 4 5 6"};

    std::istream_iterator<int> isi(is), eof;

    // *cxx-vector-ctor*
    // no loop and no isi++
    std::vector<int> coll(isi, eof);

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6));
  }

  // use iterator but direct
  {
    std::istringstream is{"1 2 3 4 5 6"};

    std::vector<int> coll((std::istream_iterator<int>(is)),
                          std::istream_iterator<int>());

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6));
  }

  // use iterator but direct
  {
    std::istringstream is{"1 2 3 4 5 6"};

    std::istream_iterator<int> isi(is), eof;

    // no loop and no isi++
    std::vector<int> coll;
    std::copy(isi, eof, std::back_inserter(coll));

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6));
  }

  // as gets from std::cin, use stream
  {
    std::istringstream is{"1 2 3 4 5 6"};
    std::istream_iterator<int> isi(is), eof;

    auto result = std::accumulate(isi, eof, 0);

    EXPECT_THAT(result, 21);
  }

  // link ostream_iterator to ostringstream
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6};
    std::ostringstream os;

    std::ostream_iterator<int> osi(os, ",");

    for (auto e : coll)
      *osi++ = e;

    EXPECT_THAT(os.str(), "1,2,3,4,5,6,");
  }

  // Table 9.9. Operations of ostream Iterators
  // same as cxx-inserter since ++ is nop.
  //
  // With/without "++oo", the result is the same. The ++oo is done to mimic
  // writing into an array through a pointer.

  {
    std::ostringstream os;
    std::ostream_iterator<string> osi(os);

    *osi = "Hello, ";
    ++osi;
    *osi = "world!";

    EXPECT_THAT(os.str(), "Hello, world!");
  }
  {
    std::ostringstream os;
    std::ostream_iterator<string> osi(os);

    *osi = "Hello, ";
    *osi = "world!";

    EXPECT_THAT(os.str(), "Hello, world!");
  }
}

// ={=========================================================================
// cxx-vector

// CXXSLR-7.3.5 Examples of Using Vectors
//
//  max_size(): 2305843009213693951
//  size()    : 0
//  capacity(): 5
// Hello, how are you ?
//  max_size(): 2305843009213693951
//  size()    : 5
//  capacity(): 5
// Hello, you are how always !
//  size()    : 6
//  capacity(): 10
//  size()    : 4
//  capacity(): 4

TEST(CxxVector, check_capacity)
{
  // create empty vector for strings
  vector<string> sentence;
  // reserve memory for five elements to avoid reallocation
  sentence.reserve(5);

  // *TN* reserve() do not change size()
  cout << " max_size(): " << sentence.max_size() << endl;
  cout << " size()    : " << sentence.size() << endl;
  cout << " capacity(): " << sentence.capacity() << endl;

  // append some elements
  sentence.push_back("Hello,");
  sentence.insert(sentence.end(), {"how", "are", "you", "?"});

  // print elements separated with spaces
  copy(sentence.cbegin(), sentence.cend(), ostream_iterator<string>(cout, " "));
  cout << endl;

  // print ‘‘technical data’’
  cout << " max_size(): " << sentence.max_size() << endl;
  cout << " size()    : " << sentence.size() << endl;
  cout << " capacity(): " << sentence.capacity() << endl;

  // swap second and fourth element
  swap(sentence[1], sentence[3]);
  // insert element "always" before element "?"
  sentence.insert(find(sentence.begin(), sentence.end(), "?"), "always");
  // assign "!" to the last element
  sentence.back() = "!";

  // print elements separated with spaces
  copy(sentence.cbegin(), sentence.cend(), ostream_iterator<string>(cout, " "));
  cout << endl;

  // print some ‘‘technical data’’ again
  cout << " size()    : " << sentence.size() << endl;
  cout << " capacity(): " << sentence.capacity() << endl;

  // delete last two elements
  sentence.pop_back();
  sentence.pop_back();

  // shrink capacity (since C++11)
  sentence.shrink_to_fit();
  // print some ‘‘technical data’’ again
  cout << " size()    : " << sentence.size() << endl;
  cout << " capacity(): " << sentence.capacity() << endl;
}

class VectorCtorsTest
{
public:
  VectorCtorsTest(int size, int value = 10)
      : icoll(size, value)
  {}
  int size() { return icoll.size(); }
  void print() { PRINT_ELEMENTS(icoll, "clsss : "); }

private:
  vector<int> icoll;
};

// ={=========================================================================
TEST(CxxVector, check_ctor)
{
  // see TEST(CxxCtor, check_init_list_3)
  // for difference between coll{} and coll()

  {
    std::vector<int> coll{5};
    ASSERT_THAT(coll.size(), Eq(1));
    EXPECT_THAT(coll, ElementsAre(5));
  }

  {
    std::vector<int> coll(5);
    ASSERT_THAT(coll.size(), Eq(5));
    EXPECT_THAT(coll, ElementsAre(0, 0, 0, 0, 0));
  }

  // *cxx-vector-resize*
  {
    std::vector<int> coll;
    coll.resize(5);
    ASSERT_THAT(coll.size(), Eq(5));
    EXPECT_THAT(coll, ElementsAre(0, 0, 0, 0, 0));
  }

  {
    std::vector<int> coll(5, 10);
    ASSERT_THAT(coll.size(), Eq(5));
    EXPECT_THAT(coll, ElementsAre(10, 10, 10, 10, 10));
  }

  // *cxx-vector-resize*
  // so vector-resize() do the same as ctor(n, e)

  {
    std::vector<int> coll;
    coll.resize(5, 10);
    ASSERT_THAT(coll.size(), Eq(5));
    EXPECT_THAT(coll, ElementsAre(10, 10, 10, 10, 10));
  }

  {
    VectorCtorsTest coll(10, 100);
    // icoll3.print();
    ASSERT_THAT(coll.size(), Eq(10));
  }

  // initializer_list
  // unlike vecor(n), vector{n} creates single element.
  {
    std::vector<int> coll1{1, 2, 3, 4, 5, 6};
    EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4, 5, 6));

    std::vector<int> coll2{1};
    ASSERT_THAT(coll2.size(), Eq(1));
    EXPECT_THAT(coll2, ElementsAre(1));

    std::vector<int> coll3{2};
    ASSERT_THAT(coll3.size(), Eq(1));
    EXPECT_THAT(coll3, ElementsAre(2));
  }

  // iterator
  {
    istringstream is{"1 2 3 4 5 6"};

    vector<int> coll((istream_iterator<int>(is)), istream_iterator<int>());

    // *cxx-error* without additional (). WHY?
    // vector<int> coll(istream_iterator<int>(is), istream_iterator<int>());

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6));
  }

  // see that cxx-array can be created with the value, size() and this is
  // dynamic.

  {
    std::vector<int> coll(20, 1);

    EXPECT_THAT(coll.size(), 20);

    // okay as well
    // int value;
    // cin >> value;
    // vector<bool> table1(value);

    vector<bool> table1(coll.size());
    vector<bool> table2(10);

    int arr[coll.size()];

    EXPECT_THAT(table1.size(), 20);
    EXPECT_THAT(table2.size(), 10);
    EXPECT_THAT(sizeof(arr) / sizeof(arr[0]), 20);
  }
}

/*
={=========================================================================

RUN      ] CxxVector.check_ctor_variable
i: 1, coll size: 1
i: 2, coll size: 2
i: 3, coll size: 3
i: 4, coll size: 4
i: 5, coll size: 5
i: 6, coll size: 6
i: 7, coll size: 7
i: 8, coll size: 8
i: 9, coll size: 9

i: 1, coll size: 1
i: 2, coll size: 1
i: 3, coll size: 1
i: 4, coll size: 1
i: 5, coll size: 1
i: 6, coll size: 1
i: 7, coll size: 1
i: 8, coll size: 1
i: 9, coll size: 1

i: 1, coll size: 1
i: 2, coll size: 1
i: 3, coll size: 1
[       OK ] CxxVector.check_ctor_variable (0 ms)
*/

TEST(CxxVector, check_ctor_variable)
{
  for (int i = 1; i < 10; i++)
  {
    std::vector coll(i, 0);
    std::cout << "i: " << i << ", coll size: " << coll.size() << std::endl;
  }

  for (int i = 1; i < 10; i++)
  {
    std::vector coll{i};
    std::cout << "i: " << i << ", coll size: " << coll.size() << std::endl;
  }

  // compile error
  // for (int i = 1; i < 10; i++)
  // {
  //   std::vector coll(i);
  //   std::cout << "i: " << i << ", coll size: " << coll.size() << std::endl;
  // }

  {
    std::vector coll{1};
    std::cout << "i: " << 1 << ", coll size: " << coll.size() << std::endl;
  }
  {
    std::vector coll{2};
    std::cout << "i: " << 2 << ", coll size: " << coll.size() << std::endl;
  }
  {
    std::vector coll{3};
    std::cout << "i: " << 3 << ", coll size: " << coll.size() << std::endl;
  }
}

namespace cxx_vector
{
  void GetVectorArg(const vector<int> &coll)
  {
    std::vector<int> coll_;

    coll_ = coll;

    ASSERT_THAT(coll_.size(), 6);
  }
} // namespace cxx_vector

// ={=========================================================================
TEST(CxxVector, check_assign)
{
  using namespace cxx_vector;

  // assign
  {
    std::vector<int> coll1{1, 2, 3, 4, 5, 6};
    std::vector<int> coll2{};

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 0);

    coll2 = coll1;

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 6);
  }

  // assign
  {
    std::vector<int> coll1{1, 2, 3, 4, 5, 6};
    std::vector<int> coll2{};

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 0);

    coll2 = {1, 2, 3, 4, 5, 6};

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 6);
  }

  // cxx-vector-assign
  {
    std::vector<int> coll1{1, 2, 3, 4, 5, 6};
    std::vector<int> coll2{};

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 0);

    coll1.assign({1, 2, 3, 4, 5, 6});

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 0);
  }

  // assign
  {
    std::vector<int> coll1{1, 2, 3, 4, 5, 6};
    GetVectorArg(coll1);
  }
}

// ={=========================================================================
TEST(CxxVector, check_insert)
{
  {
    std::vector<int> coll{2, 3, 4, 5};
    coll.insert(coll.begin(), 1);
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5));
  }
}

// ={=========================================================================
TEST(CxxVector, check_push_and_pop)
{
  std::vector<int> coll;

  coll.push_back(1);
  coll.push_back(2);
  coll.push_back(3);

  EXPECT_THAT(coll, ElementsAre(1, 2, 3));

  // void pop_back();
  EXPECT_THAT(coll.back(), 3);
  coll.pop_back();

  EXPECT_THAT(coll.back(), 2);
  coll.pop_back();

  EXPECT_THAT(coll, ElementsAre(1));
}

// ={=========================================================================
// cxx-array
TEST(CxxVector, check_as_array)
{
  {
    // c array.
    const char arr[]{"this is char array"};

    std::vector<char> coll{};

    ASSERT_THAT(coll.size(), 0);

    // cxx-vector-assign cxx-array-begin
    //
    // template< class InputIt >
    // void assign( InputIt first, InputIt last );

    coll.assign(arr, arr + sizeof(arr));

    ASSERT_THAT(coll.size(), sizeof(arr));
    EXPECT_THAT(strcmp(coll.data(), arr), 0);
  }

  {
    std::vector<char> coll;
    coll.resize(41);

    // stl.cpp:943:32: error: invalid conversion from
    // ‘__gnu_cxx::__alloc_traits<std::allocator<char> >::value_type {aka char}’
    // to ‘char*’ [-fpermissive]
    //
    //    strcpy(coll[0], "hello world");

    strcpy(&coll[0], "hello world");
    printf("%s\n", &coll[0]);
  }

  // Note that since C++11, you don’t have to use the expression
  // &a[0] to get direct access to
  // the elements in the vector, because the member function data() is
  // provided for this purpose:

  {
    std::vector<char> coll;
    coll.resize(41);

    strcpy(coll.data(), "hello world");
    printf("%s\n", coll.data());
  }
}

// ={=========================================================================
TEST(CxxVector, check_move)
{
  // move
  {
    std::vector<int> coll1{1, 2, 3, 4, 5, 6};
    std::vector<int> coll2{};

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 0);

    coll2 = std::move(coll1);

    ASSERT_THAT(coll1.size(), 0);
    ASSERT_THAT(coll2.size(), 6);
  }

  // cxx-algo-move
  // OutputIterator
  // move (InputIterator sourceBeg, InputIterator sourceEnd,
  //        OutputIterator destBeg)

  {
    std::vector<int> coll1{1, 2, 3, 4, 5, 6};
    std::vector<int> coll2{};

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 0);

    std::move(coll1.begin(), coll1.end(), std::back_inserter(coll2));

    // Note that the elements in coll1 have an undefined state after their first
    // output because move() is used. However, coll1 still has the size of 6
    // elements
    //
    // so coll1 do not change and that may explain why can use const iterator on
    // move algorithm.

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 6);
  }
}

// no compile error but errors when built with -D_GLIBCXX_DEBUG and run
//
// /usr/include/c++/6/debug/safe_iterator.h:191:
// Error: attempt to construct a constant iterator from a singular mutable
// iterator.
//
// Objects involved in the operation:
//     iterator "this" @ 0x0x7ffcc61e9980 {
//       type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<int
//       const*, std::__cxx1998::vector<int, std::allocator<int> > >,
//       std::__debug::vector<int, std::allocator<int> > > (constant iterator);
//       state = singular;
//     }
//     iterator "other" @ 0x0x7ffcc61e97f0 {
//       type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<int*,
//       std::__cxx1998::vector<int, std::allocator<int> > >,
//       std::__debug::vector<int, std::allocator<int> > > (mutable iterator);
//       state = singular;
//       references sequence with type 'std::__debug::vector<int,
//       std::allocator<int> >' @ 0x0x7ffcc61e9820
//     }
// Aborted

// ={=========================================================================
TEST(CxxVector, DISABLED_EraseRuntimeError)
{
  vector<int> coll1;
  INSERT_ELEMENTS(coll1, 0, 8);
  EXPECT_THAT(coll1, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));

  auto it = coll1.begin() + 1;

  // note: `it` is not valid after this
  coll1.erase(it);
  EXPECT_THAT(coll1, ElementsAre(0, 2, 3, 4, 5, 6, 7, 8));

  // not cause exception
  try
  {
    coll1.erase(it);
    EXPECT_THAT(coll1, ElementsAre(0, 3, 4, 5, 6, 7, 8));
  } catch (const exception &e)
  {
    std::cout << "what is " << e.what() << std::endl;
  }

  vector<int> coll2{0, 1, 2, 3, 4, 5, 6, 7, 8};

  // in every iteration, update it which is invalidated after insert/erase.
  for (auto it = coll2.begin(); it != coll2.end(); /* no */)
  {
    // if see even values, remove it
    if (!(*it % 2))
      it = coll2.erase(it);
    else
      ++it;
  }

  EXPECT_THAT(coll2, ElementsAre(1, 3, 5, 7));
}

// ={=========================================================================
TEST(CxxVector, check_erase)
{
  {
    std::vector<int> coll{0, 1, 2, 3, 4, 5, 6, 7, 8};

    auto it = coll.begin() + 1;

    it = coll.erase(it);
    EXPECT_THAT(coll, ElementsAre(0, 2, 3, 4, 5, 6, 7, 8));

    coll.erase(it);
    EXPECT_THAT(coll, ElementsAre(0, 3, 4, 5, 6, 7, 8));
  }

  {
    std::vector<int> coll{0, 1, 2, 3, 4, 5, 6, 7, 8};

    // in every iteration, update it which is invalidated after insert/erase.
    // update it from erase() and calls end() in every iteration.
    for (auto it = coll.begin(); it != coll.end(); /* no */)
    {
      // if see even values, remove it
      if (!(*it % 2))
        it = coll.erase(it);
      else
        ++it;
    }

    EXPECT_THAT(coll, ElementsAre(1, 3, 5, 7));
  }

  {
    std::vector<int> coll{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    // in every iteration, update it which is invalidated after insert/erase.
    for (auto it = coll.begin(); it != coll.end(); /* no */)
    {
      // if see odd values, repeat it in front of it.
      if (*it % 2)
      {
        it = coll.insert(it, *it);
        it += 2;
      }
      else
        it = coll.erase(it);
    }

    EXPECT_THAT(coll, ElementsAre(1, 1, 3, 3, 5, 5, 7, 7, 9, 9));
  }

  // find/erase single 3. use index
  {
    std::vector<int> coll{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (size_t i = 0; i < coll.size(); ++i)
    {
      if (3 == coll[i])
      {
        coll.erase(coll.begin() + i);
        // since no need to continue looping.
        break;
      }
    }

    EXPECT_THAT(coll, ElementsAre(0, 1, 2, 4, 5, 6, 7, 8, 9));
  }

  // find/erase single 3
  {
    std::vector<int> coll{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto it = std::find(coll.begin(), coll.end(), 3);
    if (coll.end() != it)
    {
      coll.erase(it);
    }

    EXPECT_THAT(coll, ElementsAre(0, 1, 2, 4, 5, 6, 7, 8, 9));
  }

  // find/erase multiple 3
  {
    std::vector<int> coll{0, 1, 2, 3, 3, 3, 6, 7, 8, 9};

    coll.erase(std::remove(coll.begin(), coll.end(), 3), coll.end());

    EXPECT_THAT(coll, ElementsAre(0, 1, 2, 6, 7, 8, 9));
  }
}

// 1. cxx-vector-reallocation is not something when create a vector with big
// numbers. This happens from the start.
//
// 2. do not predict how rellocation or destruction gets called.
//
// When doing push_back(), cannot predict the output since relocation is
// involved.
//
// When erasing from the beginning, cannot predict the output since moves
// element from the end to the beginning.
//
// -for ---------
// VectorEraseCallsDtor::ctor: name 0
// VectorEraseCallsDtor::dtor: name 0
// -for ---------
// VectorEraseCallsDtor::ctor: name 1
// VectorEraseCallsDtor::dtor: name 0
// VectorEraseCallsDtor::dtor: name 1
// -for ---------
// VectorEraseCallsDtor::ctor: name 2
// VectorEraseCallsDtor::dtor: name 0
// VectorEraseCallsDtor::dtor: name 1
// VectorEraseCallsDtor::dtor: name 2
// -for ---------
// VectorEraseCallsDtor::ctor: name 3
// VectorEraseCallsDtor::dtor: name 3
// -for ---------
// VectorEraseCallsDtor::ctor: name 4
// VectorEraseCallsDtor::dtor: name 0
// VectorEraseCallsDtor::dtor: name 1
// VectorEraseCallsDtor::dtor: name 2
// VectorEraseCallsDtor::dtor: name 3
// VectorEraseCallsDtor::dtor: name 4
// for: name 0
// for: name 1
// for: name 2
// for: name 3
// for: name 4
// -erase---------
// VectorEraseCallsDtor::dtor: name 4
// -erase---------
// VectorEraseCallsDtor::dtor: name 4
// -erase---------
// VectorEraseCallsDtor::dtor: name 4
// VectorEraseCallsDtor::dtor: name 3
// VectorEraseCallsDtor::dtor: name 4

namespace cxx_vector
{

  class VectorRelocation
  {
  public:
    VectorRelocation(const string name = "vector")
        : name_(name)
    {
      name_ += " called ctor";
      // cout << "VectorRelocation::ctor: " << name_ << endl;
    }
    ~VectorRelocation()
    {
      name_ += " called dtor";
      // cout << "VectorRelocation::dtor: " << name_ << endl;
    }

    string getName() { return name_; }

  private:
    string name_;
  };

}; // namespace cxx_vector

// ={=========================================================================
TEST(CxxVector, seeRelocation)
{
  using namespace cxx_vector;

  vector<VectorRelocation> ovec{};

  for (int i = 0; i < 5; ++i)
  {
    cout << "-for ---------" << endl;
    string name = "name " + to_string(i);
    ovec.push_back(VectorRelocation(name));
  }

  for (auto &e : ovec)
    cout << "for: " << e.getName() << endl;

  cout << "-erase---------" << endl;
  auto it = ovec.begin();
  it      = ovec.erase(it);

  cout << "-erase---------" << endl;
  it = ovec.erase(it);

  cout << "-erase---------" << endl;
  it = ovec.erase(it);
}

// see no relocation happens
//
// -for ---------
// VectorEraseCallsDtor::ctor: name 0
// VectorEraseCallsDtor::dtor: name 0
// -for ---------
// VectorEraseCallsDtor::ctor: name 1
// VectorEraseCallsDtor::dtor: name 1
// -for ---------
// VectorEraseCallsDtor::ctor: name 2
// VectorEraseCallsDtor::dtor: name 2
// -for ---------
// VectorEraseCallsDtor::ctor: name 3
// VectorEraseCallsDtor::dtor: name 3
// -for ---------
// VectorEraseCallsDtor::ctor: name 4
// VectorEraseCallsDtor::dtor: name 4
// -erase---------
// VectorEraseCallsDtor::dtor: name 4
// -erase---------
// VectorEraseCallsDtor::dtor: name 4
// -erase---------
// VectorEraseCallsDtor::dtor: name 4
// VectorEraseCallsDtor::dtor: name 3
// VectorEraseCallsDtor::dtor: name 4

// ={=========================================================================
TEST(CxxVector, seeRelocationAndReserve)
{
  using namespace cxx_vector;

  vector<VectorRelocation> ovec{};
  ovec.reserve(10);

  for (int i = 0; i < 5; ++i)
  {
    cout << "-for ---------" << endl;
    string name = "name " + to_string(i);
    ovec.push_back(VectorRelocation(name));
  }

  cout << "-erase---------" << endl;
  auto it = ovec.begin();
  it      = ovec.erase(it);

  cout << "-erase---------" << endl;
  it = ovec.erase(it);

  cout << "-erase---------" << endl;
  it = ovec.erase(it);
}

// VectorEraseCallsDtor::ctor: vector
// VectorEraseCallsDtor::ctor: vector
// VectorEraseCallsDtor::ctor: vector
// VectorEraseCallsDtor::ctor: vector
// VectorEraseCallsDtor::ctor: vector
// VectorEraseCallsDtor::ctor: vector
// VectorEraseCallsDtor::ctor: vector
// VectorEraseCallsDtor::ctor: vector
// VectorEraseCallsDtor::ctor: vector
// VectorEraseCallsDtor::ctor: vector
// -erase---------
// VectorEraseCallsDtor::dtor: vector
// -erase---------
// VectorEraseCallsDtor::dtor: vector
// -erase---------
// VectorEraseCallsDtor::dtor: vector
// VectorEraseCallsDtor::dtor: vector
// VectorEraseCallsDtor::dtor: vector
// VectorEraseCallsDtor::dtor: vector
// VectorEraseCallsDtor::dtor: vector
// VectorEraseCallsDtor::dtor: vector
// VectorEraseCallsDtor::dtor: vector
// VectorEraseCallsDtor::dtor: vector

// ={=========================================================================
TEST(CxxVector, seeRelocationAndPreAllocation)
{
  using namespace cxx_vector;

  vector<VectorRelocation> ovec(10);

  cout << "-erase---------" << endl;
  auto it = ovec.begin();
  it      = ovec.erase(it);

  cout << "-erase---------" << endl;
  it = ovec.erase(it);

  cout << "-erase---------" << endl;
  it = ovec.erase(it);
}

namespace cxx_vector
{
  const int NUMBER_ALLOCATION{100000};
};

// ={=========================================================================
// to see time difference between reserve() and pre-allocation
TEST(CxxVector, seeRelocationAndReserveTime)
{
  using namespace cxx_vector;

  vector<VectorRelocation> ovec{};
  ovec.reserve(NUMBER_ALLOCATION);

  for (int i = 0; i < NUMBER_ALLOCATION; ++i)
  {
    string name = "name " + to_string(i);
    ovec.push_back(VectorRelocation(name));
  }
}

// ={=========================================================================
TEST(CxxVector, seeRelocationAndPreAllocationTime)
{
  using namespace cxx_vector;

  vector<VectorRelocation> ovec(NUMBER_ALLOCATION);
}

// ={=========================================================================
// cxx-seg-fault
//
// can benefit from -D_GLIBCXX_DEBUG
//
// /usr/include/c++/4.9/debug/vector:357:error: attempt to subscript container
//     with out-of-bounds index 8, but container only holds 5 elements.
//
// Objects involved in the operation:
// sequence "this" @ 0x0x7ffe287d4590 {
//   type = NSt7__debug6vectorI20VectorEraseCallsDtorSaIS1_EEE;
// }
// Aborted

TEST(DISABLED_Vector, AccessInvalidIndex)
{
  using namespace cxx_vector;

  vector<VectorRelocation> ovec{};

  for (int i = 0; i < 5; ++i)
  {
    string name = "name " + to_string(i);
    ovec.push_back(VectorRelocation(name));
  }

  cout << "name: " << ovec[8].getName() << endl;
}

// ={=========================================================================
// cxx-undefined
//
// can benefit from -D_GLIBCXX_DEBUG
//
// /usr/include/c++/4.9/debug/vector:357:error: attempt to subscript container
//     with out-of-bounds index 8, but container only holds 5 elements.
//
// Objects involved in the operation:
// sequence "this" @ 0x0x7ffe5c578460 {
//   type = NSt7__debug6vectorI20VectorEraseCallsDtorSaIS1_EEE;
// }
// Aborted

TEST(DISABLED_Vector, AccessInvalidIndexWithReserve)
{
  using namespace cxx_vector;

  vector<VectorRelocation> ovec{};
  ovec.reserve(10);

  for (int i = 0; i < 5; ++i)
  {
    string name = "name " + to_string(i);
    ovec.push_back(VectorRelocation(name));
  }

  cout << "name: " << ovec[8].getName() << endl;
}

// ={=========================================================================
TEST(CxxVector, check_vector_bool)
{
  std::vector<bool> coll{};
  std::ostringstream oss{};

  coll.push_back(true);
  coll.push_back(true);
  coll.push_back(false);
  coll.push_back(false);
  coll.push_back(false);
  coll.push_back(true);
  coll.push_back(false);
  coll.push_back(true);
  coll.push_back(false);
  coll.push_back(true);
  coll.push_back(false);
  coll.push_back(true);
  coll.push_back(false);
  coll.push_back(true);
  coll.push_back(false);
  coll.push_back(true);
  coll.push_back(false);

  for (size_t i = 0; i < coll.size(); i++)
  {
    oss << coll[i];
  }

  EXPECT_THAT(oss.str(), "11000101010101010");

  // std::cout << oss.str() << std::endl;
}

// ={=========================================================================
// cxx-array

// 7.2.2 Array Operations
//
// Create, Copy, and Destroy
//
// Table 7.4 lists the constructors and destructors for arrays. Because class
// array<> is an aggregate, these constructors are only implicitly defined. You
// can create arrays with and without elements for initialization. The default
// constructor default initializes the elements, which means that the value of
// fundamental types is undefined.

TEST(CxxArray, check_ctors)
{
  // all fundamental types are initialised.
  {
    std::array<int, 8> coll = {};

    EXPECT_THAT(coll, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
  }

  {
    std::array<int, 8> coll = {11, 22, 33};

    EXPECT_THAT(coll.size(), 8);

    coll.back()           = 999;
    coll[coll.size() - 2] = 42;

    EXPECT_THAT(coll, ElementsAre(11, 22, 33, 0, 0, 0, 42, 999));
  }

  {
    array<int, 3> coll{1, 2, 3};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3));
  }
}

// show access and also usage with C API
//
// [ RUN      ] CxxArray.check_access
// initialized: t h i s   i s   a n   c h a r   a r r a y                     (41)
// strcpyed   : u s e   t h e   a d d r e s s   o f   f i r s t                  (41)
// strcpyed   : u s e   t h e   a d d r e s s   o f   f i r s t                  (41)
// [       OK ] CxxArray.check_access (0 ms)

TEST(CxxArray, check_access)
{
  {
    std::array<char, 41> coll = {"this is an char array"};
    PRINT_ELEMENTS(coll, "initialized: ");

    strcpy(&coll[0], "use the address of first");
    PRINT_ELEMENTS(coll, "strcpyed   : ");

    // compile error
    // error: invalid conversion from ‘std::array<char, 41>::value_type {aka char}’
    // to ‘char*’ [-fpermissive]
    //
    // strcpy(coll[0], "use the address of first");
    // PRINT_ELEMENTS(coll, "strcpyed   : ");

    strcpy(coll.data(), "use the address of first");
    PRINT_ELEMENTS(coll, "strcpyed   : ");
  }

  // array size is smaller than the initializer. does array handle
  // this?
  //
  // error: initializer-string for array of chars is too long [-fpermissive]
  // {
  //   std::array<char, 10> coll = {"this is an char array"};
  //   PRINT_ELEMENTS(coll, "initialized: ");
  // }
}

TEST(CxxArray, check_multi_dimention)
{
  const size_t rows = 5;
  const size_t cols = 5;

  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0

  {
    std::vector<std::vector<int>> coll(rows, std::vector<int>(cols, 0));

    cout << "{" << endl;
    for (size_t i = 0; i < rows; ++i)
    {
      for (size_t j = 0; j < cols; ++j)
        cout << coll[i][j] << " ";
      cout << endl;
    }
    cout << "}" << endl;
  }

  // 0 0 0 0 0
  // 1 1 1 1 0
  // 0 0 0 0 0
  // 0 1 1 1 1
  // 2 0 0 0 0

  {
    std::vector<std::vector<int>> coll{{0, 0, 0, 0, 0},
                                       {1, 1, 1, 1, 0},
                                       {0, 0, 0, 0, 0},
                                       {0, 1, 1, 1, 1},
                                       {2, 0, 0, 0, 0}};

    cout << "{" << endl;
    for (size_t i = 0; i < rows; ++i)
    {
      for (size_t j = 0; j < cols; ++j)
        cout << coll[i][j] << " ";
      cout << endl;
    }
    cout << "}" << endl;
  }

  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0

  {
    std::array<std::array<int, cols>, rows> coll{0};

    cout << "{" << endl;
    for (size_t i = 0; i < rows; ++i)
    {
      for (size_t j = 0; j < cols; ++j)
        cout << coll[i][j] << " ";
      cout << endl;
    }
    cout << "}" << endl;
  }

  // 0 0 0 0 0
  // 1 1 1 1 0
  // 0 0 0 0 0
  // 0 1 1 1 1
  // 2 0 0 0 0

  {
    std::array<std::array<int, 5>, 5> coll{0, 0, 0, 0, 0, 1, 1, 1, 1,
                                           0, 0, 0, 0, 0, 0, 0, 1, 1,
                                           1, 1, 2, 0, 0, 0, 0};

    cout << "{" << endl;
    for (size_t i = 0; i < rows; ++i)
    {
      for (size_t j = 0; j < cols; ++j)
        cout << coll[i][j] << " ";
      cout << endl;
    }
    cout << "}" << endl;
  }
}

// ={=========================================================================
// cxx-deque

// the reference say: Calling back on an empty container is undefined.
// when runs, see seg fault(core dumped).

TEST(CxxDeque, DISABLED_check_on_empty)
{
  std::deque<int> coll;

  try
  {
    auto e = coll.back();
    (void)e;
  } catch (...)
  {
    std::cout << "exception" << std::endl;
  }
}

// As with cxx-vector, deque supports pre-allocation.

TEST(CxxDeque, check_ctor)
{
  std::deque<int> coll(1000);

  EXPECT_THAT(coll.size(), 1000);
}

// ={=========================================================================
// cxx-queue-priority *cxx-queue*

TEST(StlQueue, check_operation)
{
  {
    ostringstream os;
    std::queue<std::string> q;

    q.push("These ");
    q.push("are ");
    q.push("more than ");

    // "These"
    os << q.front();
    q.pop();

    // "These are"
    os << q.front();
    q.pop();

    q.push("four ");
    q.push("words!");

    // discard one element
    q.pop();

    // "These are four"
    os << q.front();
    q.pop();

    // "These are four words!"
    os << q.front();
    q.pop();

    EXPECT_THAT(os.str(), "These are four words!");
    EXPECT_THAT(q.size(), 0);
  }

  // cxx-queue-emplace but not emplace_back
  // Pushes a new element to the end of the queue. The element is constructed
  // in-place, i.e. no copy or move operations are performed. The constructor of
  // the element is called with exactly the same arguments as supplied to the
  // function.

  {
    ostringstream os;
    std::queue<std::string> q;

    q.emplace("These ");
    q.emplace("are ");
    q.emplace("more than ");

    // "These"
    os << q.front();
    q.pop();

    // "These are"
    os << q.front();
    q.pop();

    // "These are more than "
    os << q.front();
    q.pop();

    EXPECT_THAT(os.str(), "These are more than ");
    EXPECT_THAT(q.size(), 0);
  }

  // there is no clear() to remove all
  {
    // cxx-error
    // std::queue<int> coll{1,2,3,4,5};

    std::queue<int> coll{};
    coll.push(1);
    coll.push(2);
    coll.push(3);
    coll.push(4);
    coll.push(5);

    EXPECT_THAT(coll.size(), 5);

    while (!coll.empty())
      coll.pop();

    EXPECT_THAT(coll.size(), 0);
  }

  {
    std::queue<int> pq;

    pq.push(22);
    pq.push(66);
    pq.push(44);

    //         top
    // 44, 66, 22

    // queue do not have top()

    EXPECT_THAT(pq.front(), Eq(22));
    pq.pop();

    EXPECT_THAT(pq.front(), Eq(66));
    pq.pop();

    pq.push(11);
    pq.push(55);
    pq.push(33);

    //             top
    // 33, 55, 11, 44

    pq.pop();

    //         top
    // 33, 55, 11

    // since queue do not support begin()/end(), cannot use ElementsAre and copy
    // to transform it to a vector. Did it manually.
    // copy(pq.begin(), pq.end(), back_inserter(coll));

    std::vector<int> coll;

    while (!pq.empty())
    {
      coll.push_back(pq.front());
      pq.pop();
    }

    EXPECT_THAT(coll, ElementsAre(11, 55, 33));
  }
}

/*
={=========================================================================
namespace std {
  template <typename T,
           typename Container = vector<T>,
           typename Compare = less<typename Container::value_type>>
             class priority_queue;
}

*/

TEST(StlQueue, check_priority_queue)
{
  // when use default less()
  {
    std::priority_queue<int> pq;

    pq.push(22);
    pq.push(66);
    pq.push(44);

    // not the order inserted.
    //         top
    // 44, 66, 22

    //         top
    // 22, 44, 66

    EXPECT_THAT(pq.top(), Eq(66));
    pq.pop();

    EXPECT_THAT(pq.top(), Eq(44));
    pq.pop();

    pq.push(11);
    pq.push(55);
    pq.push(33);

    // 11, 22, 33, 55

    pq.pop();

    // since queue do not support begin()/end(), cannot use ElementsAre and copy
    // to transform it to a vector. Did it manually.
    // copy(pq.begin(), pq.end(), back_inserter(coll));

    std::vector<int> coll;

    while (!pq.empty())
    {
      coll.push_back(pq.top());
      pq.pop();
    }

    EXPECT_THAT(coll, ElementsAre(33, 22, 11));
  }

  // TODO to have custom comparison
#if 0 
  // when use greater()
  {
    // cxx-greater > like cxx-sp, unique pointer, use typename
    std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
      std::greater<int>());

    pq.push((int)23);
    pq.push(66);
    pq.push(44);

    //         top
    // 22, 44, 66

    // since queue do not support begin()/end(), cannot use ElementsAre and copy
    // to transform it to a vector. Did it manually.
    // copy(pq.begin(), pq.end(), back_inserter(coll));

    std::vector<int> coll;

    while (!pq.empty())
    {
      coll.push_back(pq.top());
      pq.pop();
    }

    EXPECT_THAT(coll, ElementsAre(66, 44, 22));
  }
#endif
}

// ={=========================================================================
// cxx-set

TEST(CxxSet, check_insert_and_emplace)
{
  std::set<int> coll1{};

  for (int i = 0; i < 9; i++)
    coll1.insert(i);

  EXPECT_THAT(coll1, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));

  std::set<int> coll2{};

  for (int i = 0; i < 9; i++)
    coll2.emplace(i);

  EXPECT_THAT(coll2, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));
}

// ={=========================================================================
TEST(CxxSet, check_order)
{
  {
    // cxx-less <
    std::set<int> coll{13, 9, 7, 10, 2, 11, 12, 8, 7};
    EXPECT_THAT(coll, ElementsAre(2, 7, 8, 9, 10, 11, 12, 13));
  }

  {
    // cxx-greater > like cxx-sp, unique pointer, use typename
    std::set<int, std::greater<int>> coll{13, 9, 7, 10, 2, 11, 12, 8, 7};
    EXPECT_THAT(coll, ElementsAre(13, 12, 11, 10, 9, 8, 7, 2));
  }
}

// ={=========================================================================
TEST(CxxSet, check_assign)
{
  std::set<int> coll1{13, 9, 10, 2, 11, 12, 8, 7};
  EXPECT_THAT(coll1.size(), 8);

  std::set<int> coll2{4, 5, 6};
  EXPECT_THAT(coll2.size(), 3);

  coll1 = coll2;

  EXPECT_THAT(coll1, ElementsAre(4, 5, 6));
  EXPECT_THAT(coll2, ElementsAre(4, 5, 6));
}

// ={=========================================================================
TEST(CxxSet, check_duplicate)
{
  {
    std::set<int> coll{13, 9, 7, 10, 2, 11, 12, 8, 7};

    // duplicate, 7, is not inserted
    EXPECT_THAT(coll.size(), 8);

    auto begin = coll.begin();
    coll.erase(begin);

    begin = coll.begin();
    coll.erase(begin);

    EXPECT_THAT(coll, ElementsAre(8, 9, 10, 11, 12, 13));
    EXPECT_THAT(coll.size(), 6);
  }

  {
    std::multiset<int, greater<int>> coll{13, 9, 7, 10, 2, 11, 12, 8, 7};
    size_t duplicate_count{};

    // see duplicates
    EXPECT_THAT(coll, ElementsAre(13, 12, 11, 10, 9, 8, 7, 7, 2));

    // for (auto e : coll)
    //   if (coll.count(e) % 2 == 0)
    //     ++duplicate_count;

    for (auto e : coll)
      if (1 < coll.count(e))
        ++duplicate_count;

    // why 2 since see 7 two times in a loop
    EXPECT_THAT(duplicate_count, 2);
  }
}

/*

The difference in return types results because unordered multisets and multimaps
allow duplicates, whereas unordered sets and maps do not. Thus, the insertion of
an element might fail for an un- ordered set if it already contains an element
with the same value.

pair<iterator,bool> insert(const value_type& val);

1. The member second of the pair structure returns whether the insertion was
successful.

2. The member first of the pair structure returns the position of the newly
inserted element or the position of the still existing element.

*/

TEST(CxxSet, check_insert_and_duplicate)
{
  std::set<int> coll{13, 9, 7, 10, 2, 11, 12, 8, 7};

  // duplicate, 7, is not inserted
  EXPECT_THAT(coll.size(), 8);

  auto ret = coll.insert(7);

  EXPECT_THAT(ret.second, false);

  ret = coll.insert(20);

  EXPECT_THAT(ret.second, true);
}

// ={=========================================================================
// note that use lower_bound() to see if serch item falls in between items.
//
// std::map<Key,T,Compare,Allocator>::lower_bound
// std::set<Key,Compare,Allocator>::lower_bound
//
// 1) Returns an iterator pointing to the first element that is not less than
// (i.e. greater or equal to) key.

TEST(CxxSet, check_equal_range)
{
  {
    std::set<int> coll;

    coll.insert(1);
    coll.insert(2);
    coll.insert(4);
    coll.insert(5);
    coll.insert(6);

    EXPECT_THAT(coll, ElementsAre(1, 2, 4, 5, 6));

    // Returns an iterator to the first element `with key not less than val.`
    EXPECT_THAT(*coll.lower_bound(3), 4);

    // Returns an iterator to the first element with key greater than val.
    EXPECT_THAT(*coll.upper_bound(3), 4);

    // Returns a `range, pair of iterators`, denoting the elements with val
    EXPECT_THAT(*coll.equal_range(3).first, 4);
    EXPECT_THAT(*coll.equal_range(3).second, 4);

    EXPECT_THAT(*coll.lower_bound(5), 5);
    EXPECT_THAT(*coll.upper_bound(5), 6);
    EXPECT_THAT(*coll.equal_range(5).first, 5);
    EXPECT_THAT(*coll.equal_range(5).second, 6);

    // how about not found?
    EXPECT_THAT(coll.equal_range(7).first, coll.end());
    EXPECT_THAT(coll.equal_range(7).second, coll.end());
    EXPECT_THAT(coll.equal_range(7).first, coll.equal_range(7).second);
  }

  {
    std::multiset<int> coll;

    coll.insert(1);
    coll.insert(2);
    coll.insert(3); // 2
    coll.insert(3);
    coll.insert(3); // 4
    coll.insert(4); // 5
    coll.insert(5);
    coll.insert(6);

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 3, 3, 4, 5, 6));

    // since multiset is not contiguous, cannot use iterator arithmetic. use
    // distance().
    EXPECT_THAT(distance(coll.begin(), coll.lower_bound(3)), 2);

    {
      std::vector<int> coll1;
      for (auto it = coll.begin(); it != coll.lower_bound(3); ++it)
        coll1.push_back(*it);
      EXPECT_THAT(coll1, ElementsAre(1, 2));
    }

    EXPECT_THAT(distance(coll.begin(), coll.upper_bound(3)), 5);

    EXPECT_THAT(distance(coll.begin(), coll.equal_range(3).first), 2);
    EXPECT_THAT(distance(coll.begin(), coll.equal_range(3).second), 5);

    {
      std::vector<int> ret{};

      for (auto it = coll.lower_bound(3); it != coll.upper_bound(3); ++it)
        ret.push_back(*it);

      EXPECT_THAT(ret, ElementsAre(3, 3, 3));
    }

    // equal_range returns a pair of iterators
    {
      std::vector<int> ret{};

      for (auto it = coll.equal_range(3).first;
           it != coll.equal_range(3).second;
           ++it)
        ret.push_back(*it);

      EXPECT_THAT(ret, ElementsAre(3, 3, 3));
    }

    {
      std::vector<int> ret{};

      for (auto it = coll.equal_range(3); it.first != it.second; ++it.first)
        ret.push_back(*it.first);

      EXPECT_THAT(ret, ElementsAre(3, 3, 3));
    }
  }
}

// ={=========================================================================
TEST(CxxSet, check_erase)
{
  // if there is match at the beginning, removes them
  {
    multiset<int> coll{2, 2, 3, 3, 3, 3, 3, 3, 3, 6, 7};
    EXPECT_THAT(coll, ElementsAreArray({2, 2, 3, 3, 3, 3, 3, 3, 3, 6, 7}));

    auto it = coll.begin();
    while ((it != coll.end()) && (*it == 2))
    {
      it = coll.erase(it);
    }

    EXPECT_THAT(coll, ElementsAre(3, 3, 3, 3, 3, 3, 3, 6, 7));
  }

  // if there is no match at the beginning, do nothing
  {
    multiset<int> coll{2, 2, 3, 3, 3, 3, 3, 3, 3, 6, 7};
    EXPECT_THAT(coll, ElementsAreArray({2, 2, 3, 3, 3, 3, 3, 3, 3, 6, 7}));

    auto it = coll.begin();
    while ((it != coll.end()) && (*it == 3))
    {
      it = coll.erase(it);
    }

    // same as before
    EXPECT_THAT(coll, ElementsAreArray({2, 2, 3, 3, 3, 3, 3, 3, 3, 6, 7}));
  }
}

// ={=========================================================================
// cxx-17
TEST(CxxSet, check_merge)
{
  std::set<int> coll1{5, 6, 7, 8};
  std::set<int> coll2{3, 4, 5, 6};

  coll1.merge(coll2);

  EXPECT_THAT(coll1.size(), 6);
  EXPECT_THAT(coll1, ElementsAreArray({3, 4, 5, 6, 7, 8}));

  // If there is an element in *this with key equivalent to the key of an
  // element from source, then that element is not extracted from source.

  EXPECT_THAT(coll2.size(), 2);
  EXPECT_THAT(coll2, ElementsAreArray({5, 6}));
}

// ={=========================================================================
// cxx-map

TEST(CxxMap, check_begin_return)
{
  {
    std::map<int, int>
      coll{{1, 7}, {2, 4}, {3, 2}, {4, 3}, {5, 6}, {6, 1}, {7, 3}};

    auto it1 = coll.begin();
    auto it2 = ++coll.begin();

    EXPECT_THAT(*it1, std::make_pair(1, 7));
    EXPECT_THAT(*it2, std::make_pair(2, 4));
  }
}

// ={=========================================================================
TEST(CxxMap, check_compare)
{
  {
    std::map<int, int>
      coll{{1, 7}, {2, 4}, {3, 2}, {4, 3}, {5, 6}, {6, 1}, {7, 3}};

    auto it1 = coll.begin();   // {1, 7}
    auto it2 = ++coll.begin(); // {2, 4}

    // this is std::pair comparison and see *cxx-pair-comparison*
    EXPECT_THAT(*it1 < *it2, true);
    EXPECT_THAT(*it2 < *it1, false);
  }
}

// ={=========================================================================
TEST(CxxMap, check_find)
{
  {
    std::map<float, float>
      coll{{1, 7}, {2, 4}, {3, 2}, {4, 3}, {5, 6}, {6, 1}, {7, 3}};

    // find() returns a pair
    {
      // *cxx-error*
      // when tries to use custom matcher, get's link error
      // ASSERT_THAT(posKey, EqPair(make_pair(3,2)));

      auto it = coll.find(3.0);
      EXPECT_THAT(*it, std::make_pair(3, 2));

      EXPECT_THAT(it->first, 3);
      EXPECT_THAT(it->second, 2);
    }

    // at() returns value
    {
      auto value = coll.at(3.0);
      EXPECT_THAT(value, 2);
    }

    // Returns a reference to the mapped value of the element with key
    // equivalent to key. If no such element exists, an exception of type
    // `std::out_of_range` is thrown.
    {
      EXPECT_THROW(coll.at(8.0), std::out_of_range);
    }

    // find with value
    {
      // *algo-find-if-const* error if there is no const on predicate.
      // since it's *cxx-algo-non-modifying* ?

      // *cxx-decltype*
      auto it = find_if(coll.cbegin(),
                        coll.cend(),
                        // [] ( const pair<float,float> &elem )
                        // [] ( const map<float,float>::value_type &elem )
                        [](const decltype(coll)::value_type &elem) {
                          return elem.second == 3.0;
                        });
      EXPECT_THAT(it->first, Eq(4));
      EXPECT_THAT(it->second, Eq(3));
    }
  }
}

/*
// ={=========================================================================
 
The both maps and unordered maps can also be thought of as an
`associative-array`, an array whose index is not an integer value. As a
consequence, both containers provide the subscript operator[].

However, that the subscript operator does not behave like the usual subscript
operator for arrays: `Not having an element for an index is 'not' an error.` A
new index (or key) is taken as a reason `to create and insert a new element`
that has the index as the key. `Thus, you can't have an invalid index.`

c[key] 

Inserts an element with key, if it does not yet exist, and returns a reference
to the value of the element with key (only for nonconstant maps)

"Thus, to use this feature, you can't use a value type that has no default
constructor." 


c.at(key) *cxx-map-at*
Returns a reference to the value of the element with key (since C++11)

Returns a reference to the mapped value of the element with key equivalent to
key. If no such element exists, an exception of type `std::out_of_range` is
thrown.

coll.at("vat1") = 0.16;


`The fecth only works for map` but not multimaps since there are multiple
matches. Why not set? Since set has only key. Unlike other containers, [] op on
map returns `mapped_type` but dereferencing of iterator returns `value_type`. 


<ex>

word_count[word] = 1; 

Happens 3 setps but step 3 could be extra

o Search key and not found
o Insert temp element pair{word, 0}. value init since default ctor is called
o Assign it with 1

When accessing map with key which is not exist, cause addition. Inserts a new
element with key "ottto" and prints its value, which is 0 by default.

std::cout << coll["ottto"];

The real bug which meant to print out debug message but creates one elements
which is not added before due to if and else logic in the code.

NICKEL_TRACE("getSourceInformation: "
        << "configuration: " << sourceInfo["CONFIGURATION"]
        << "video size   : " << sourceInfo["VIDEO_WIDTH"]);

*/

TEST(CxxMap, fetch_can_insert)
{
  std::map<unsigned int, int> expected{{1, 0}, {2, 0}, {3, 0}};

  // use insert()
  {
    std::map<unsigned int, int> coll;

    coll.insert({1, 0});
    coll.insert({2, 0});
    coll.insert({3, 0});

    EXPECT_THAT(coll.size(), 3);

    EXPECT_THAT(coll == expected, true);
  }

  // do the same
  {
    std::map<unsigned int, int> coll;

    coll[1];
    coll[2];
    coll[3];

    EXPECT_THAT(coll.size(), 3);

    EXPECT_THAT(coll == expected, true);
  }

  // do the same
  {
    std::map<unsigned int, int> _expected{{1, 1}, {2, 1}, {3, 1}};
    std::map<unsigned int, int> coll;

    coll[1]++;
    coll[2]++;
    coll[3]++;

    EXPECT_THAT(coll.size(), 3);

    EXPECT_THAT(coll == _expected, true);
  }

  // so how can prevent "accidently" adding an item? check if it's already in
  // the coll and then add it.
  //
  // {
  //    auto q = coll.find("seal");
  //
  //    if (q == coll.end())
  //    {
  //      coll.emplace("seal");
  //    }
  // }
}

// ={=========================================================================
TEST(CxxMap, insert_and_emplace)
{
  std::map<unsigned int, std::string> coll{{1, "one"},
                                           {2, "two"},
                                           {3, "three"},
                                           {4, "four"}};

  {
    std::map<unsigned int, std::string> coll_1{};

    coll_1.insert({1, "one"});
    coll_1.insert({2, "two"});
    coll_1.insert({3, "three"});
    coll_1.insert({4, "four"});

    EXPECT_THAT(coll == coll_1, true);
  }

  {
    std::map<unsigned int, std::string> coll_1{};

    coll_1.emplace(std::make_pair(1, "one"));
    coll_1.emplace(std::make_pair(2, "two"));
    coll_1.emplace(std::make_pair(3, "three"));
    coll_1.emplace(std::make_pair(4, "four"));

    EXPECT_THAT(coll == coll_1, true);
  }
}

// ={=========================================================================
TEST(CxxMap, check_access_on_const)
{
  // cannot be a const map since operator[] is for non-const
  // also note that key is const
  {
    std::map<unsigned int, std::string> coll{{1, "one"},
                                             {2, "two"},
                                             {3, "three"},
                                             {4, "four"}};

    // can change
    coll[3] = "threee";
    coll[3] = "threeee";
    coll[3] = "threeeee";
    coll[3] = "threeeeee";

    ASSERT_THAT(coll[3], Eq("threeeeee"));

    // same as above? no
    // not changed since cxx-set and cxx-map do not allow duplicates and nothing
    // happens when key is already exist

    coll.insert({3, "third"});
    coll.insert({3, "thirdd"});
    coll.insert({3, "thirddd"});
    coll.insert({3, "thirdddd"});

    ASSERT_THAT(coll[3], Eq("threeeeee"));
  }

  // However, can use at() on const
  {
    const std::map<float, float>
      coll{{1, 7}, {2, 4}, {3, 2}, {4, 3}, {5, 6}, {6, 1}, {7, 3}};

    // again, operator[] do not work on const
    //
    // error: passing ‘const std::__debug::map<float, float>’ as ‘this’ argument discards qualifiers [-fpermissive]
    // {
    //   EXPECT_THAT(coll[1], 7);
    // }
    //
    // okay as: bits/stl_map.h
    // const mapped_type&
    // at(const key_type& __k) const
    // {}
    {
      EXPECT_THAT(coll.at(1), 7);
    }
  }
}

// ={=========================================================================
TEST(CxxMap, check_sorted)
{
  {
    std::map<unsigned int, std::string> coll{};
    std::vector<std::string> ret{};

    coll.insert({4, "string0"});
    coll.insert({3, "string1"});
    coll.insert({2, "string2"});
    coll.insert({1, "string3"});
    coll.insert({0, "string4"});

    ASSERT_THAT(coll.size(), 5);

    auto it = coll.begin();
    for (; it != coll.end(); ++it)
    {
      ret.push_back(it->second);
    }

    // as cxx-map is "sorted" on key, which is integer
    EXPECT_THAT(
      ret,
      ElementsAre("string4", "string3", "string2", "string1", "string0"));
  }

  // same as above
  {
    std::map<unsigned int, std::string> coll{};
    std::vector<std::string> ret{};

    // can do in single shot
    coll.insert({{4, "string0"},
                 {3, "string1"},
                 {2, "string2"},
                 {1, "string3"},
                 {0, "string4"}});

    ASSERT_THAT(coll.size(), 5);

    // *cxx-auto*
    // both cause cxx-error when use "++it" as both makes "it" const and "it"
    // is iterator
    //
    // auto const it = coll.begin();
    // const auto it = coll.begin();

    auto it = coll.cbegin();
    for (; it != coll.cend(); ++it)
    {
      ret.push_back(it->second);
    }

    EXPECT_THAT(
      ret,
      ElementsAre("string4", "string3", "string2", "string1", "string0"));
  }

  // same as above and use cxx-range-for
  {
    std::map<unsigned int, std::string> coll{};
    std::vector<std::string> ret{};

    coll.insert({4, "string0"});
    coll.insert({3, "string1"});
    coll.insert({2, "string2"});
    coll.insert({1, "string3"});
    coll.insert({0, "string4"});

    ASSERT_THAT(coll.size(), 5);

    // since *cxx-11*, range-for is supported
    for (const auto &e : coll)
    {
      ret.push_back(e.second);
    }

    EXPECT_THAT(
      ret,
      ElementsAre("string4", "string3", "string2", "string1", "string0"));
  }
}

// ={=========================================================================
TEST(CxxMap, check_sorted_custom_compare)
{
  // use cxx-less by default on string
  {
    std::map<std::string, unsigned int> coll{};
    std::vector<int> ret{};

    // can do in single shot
    coll.insert({{"C", 2}, {"E", 4}, {"A", 0}, {"D", 3}, {"B", 1}});

    ASSERT_THAT(coll.size(), 5);

    auto it = coll.begin();
    for (; it != coll.end(); ++it)
    {
      ret.push_back(it->second);
    }

    EXPECT_THAT(ret, ElementsAre(0, 1, 2, 3, 4));
  }

  // do the apposit.
  {
    struct CustomComp
    {
      bool operator()(const std::string &lhs, const std::string &rhs)
      {
        // would be if it was cxx-less
        // return lhs < rhs;

        // to make opposite
        return lhs > rhs;
      }
    };

    std::map<std::string, unsigned int, CustomComp> coll{};
    std::vector<int> ret{};

    // can do in single shot
    coll.insert({{"C", 2}, {"E", 4}, {"A", 0}, {"D", 3}, {"B", 1}});

    ASSERT_THAT(coll.size(), 5);

    auto it = coll.begin();
    for (; it != coll.end(); ++it)
    {
      ret.push_back(it->second);
    }

    // now it's reversed
    EXPECT_THAT(ret, ElementsAre(4, 3, 2, 1, 0));
  }

  // same but not use custom compare
  {
    std::map<std::string, unsigned int, std::greater<std::string>> coll{};
    std::vector<int> ret{};

    // can do in single shot
    coll.insert({{"C", 2}, {"E", 4}, {"A", 0}, {"D", 3}, {"B", 1}});

    ASSERT_THAT(coll.size(), 5);

    auto it = coll.begin();
    for (; it != coll.end(); ++it)
    {
      ret.push_back(it->second);
    }

    // now it's reversed
    EXPECT_THAT(ret, ElementsAre(4, 3, 2, 1, 0));
  }

  // sorted by case of string
  {
    std::map<std::string, std::string> coll{};
    std::vector<std::string> ret{};

    // can do in single shot
    coll.insert({{"one", "one"},
                 {"One", "One"},
                 {"Onwards", "Onwards"},
                 {"oragne", "orange"},
                 {"Paris", "Paris"}});

    ASSERT_THAT(coll.size(), 5);

    auto it = coll.begin();
    for (; it != coll.end(); ++it)
    {
      ret.push_back(it->second);
    }

    EXPECT_THAT(ret, ElementsAre("One", "Onwards", "Paris", "one", "orange"));
  }

  // sorted by less and non-case
  {
    // strcasecmp, strncasecmp - compare two strings ignoring case
    //
    // #include <strings.h>
    //
    // int strcasecmp(const char *s1, const char *s2);
    //
    // int strncasecmp(const char *s1, const char *s2, size_t n);

    struct CustomComp
    {
      bool operator()(const std::string &lhs, const std::string &rhs)
      {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
      }
    };

    std::map<std::string, std::string, CustomComp> coll{};
    std::vector<std::string> ret{};

    // can do in single shot
    coll.insert({{"one", "one"},
                 {"One", "One"},
                 {"Onwards", "Onwards"},
                 {"oragne", "orange"},
                 {"Paris", "Paris"}});

    // NOTE: it is 4 since non-case comp makes duplcates so comp do affect not
    // only the order but also `insert`
    ASSERT_THAT(coll.size(), 4);

    auto it = coll.begin();
    for (; it != coll.end(); ++it)
    {
      ret.push_back(it->second);
    }

    // so "sorted" by non-case
    EXPECT_THAT(ret, ElementsAre("one", "Onwards", "orange", "Paris"));
  }
}

// ={=========================================================================
// Q: what if:
//
// o string key which used to find an item in a map.
// o item is a struct which has time field.
// o want to sort them out on this time field as well.
//
// even if use custom compare, it still uses key type.
//
// one solution is:
//
// make a set<std::pair<key, elem>> using custom compare on the fly so that all
// elems are sorted time field.
//
// get a key for the first in the set which is the oldest and want to delete it.
//
// use that key to remove item in the map.
//
// TEST(CxxMap, check_sorted_custom_compare) { }
//
// any better way??

TEST(CxxMap, map_Copy)
{
  {
    std::map<unsigned int, std::string> coll1{};

    // can do in single shot
    coll1.insert({{4, "string0"},
                  {3, "string1"},
                  {2, "string2"},
                  {1, "string3"},
                  {0, "string4"}});

    ASSERT_THAT(coll1.size(), 5);

    std::map<unsigned int, std::string> coll2{};

    // can do in single shot
    coll2.insert({{14, "string10"}, {11, "string13"}, {10, "string14"}});

    ASSERT_THAT(coll2.size(), 3);

    coll1 = coll2;

    // now both are same
    ASSERT_THAT(coll1.size(), 3);
    ASSERT_THAT(coll2.size(), 3);
  }
}

// ={=========================================================================
TEST(CxxMapMulti, equal_range_1)
{
  // when found, returns iterator to the first item.
  {
    std::multimap<std::string, std::string> authors{
      {"Kit, Park", "How to get through"},
      {"Barth, John", "Sot-Weed Factor"},
      {"Barth, John", "Lost in the Funhouse"},
      {"Andy, Steve", "Enterprise"},
      {"Barth, John", "A way to success"}};

    auto it = authors.find("Barth, John");

    EXPECT_THAT(it->second, "Sot-Weed Factor");
  }

  // the order is maintained in the equal range
  {
    std::multimap<std::string, std::string> authors;

    // *cxx-map-insert*
    authors.insert({"Kit, Park", "How to get through"});
    authors.insert({"Barth, John", "Sot-Weed Factor"});
    authors.insert({"Barth, John", "Lost in the Funhouse"});
    authors.insert({"Andy, Steve", "Enterprise"});
    authors.insert({"Barth, John", "A way to success"});

    std::string search_item("Barth, John");

    auto entries = authors.count(search_item); // num of elements
    EXPECT_THAT(entries, 3);

    // use iter
    {
      vector<string> result{};
      auto iter = authors.find(search_item); // first entry

      while (entries)
      {
        result.push_back(iter->second);
        ++iter;
        --entries;
      }

      EXPECT_THAT(result,
                  ElementsAre("Sot-Weed Factor",
                              "Lost in the Funhouse",
                              "A way to success"));
    }

    // use _bound() calls
    {
      std::vector<string> result{};

      for (auto begin = authors.lower_bound(search_item),
                end   = authors.upper_bound(search_item);
           begin != end;
           ++begin)
      {
        result.push_back(begin->second);
      }

      EXPECT_THAT(result,
                  ElementsAre("Sot-Weed Factor",
                              "Lost in the Funhouse",
                              "A way to success"));
    }

    // use equal_range()
    // return pair of iter in range [first, off-the-end). Like above, if not found
    // return the same.
    {
      std::vector<string> result{};

      for (auto rpos = authors.equal_range(search_item);
           rpos.first != rpos.second;
           ++rpos.first)
      {
        result.push_back(rpos.first->second);
      }

      EXPECT_THAT(result,
                  ElementsAre("Sot-Weed Factor",
                              "Lost in the Funhouse",
                              "A way to success"));
    }

    // equal_range returns a pair
    {
      auto range = authors.equal_range(search_item);

      auto bound_begin = authors.lower_bound(search_item);
      auto bound_end   = authors.upper_bound(search_item);

      EXPECT_THAT(range.first, bound_begin);
      EXPECT_THAT(range.second, bound_end);
    }
  }
}

// ={=========================================================================
TEST(CxxMapMulti, equal_range_2)
{
  // build occurance multi-map<count, pair<value,count>>
  std::multimap<int, std::pair<int, int>> omap{};

  omap.emplace(1, std::make_pair(1, 10));
  omap.emplace(2, std::make_pair(2, 20));
  omap.emplace(3, std::make_pair(3, 30));
  omap.emplace(3, std::make_pair(3, 30));

  // get the last element
  EXPECT_THAT(omap.rbegin()->first, 3);
  EXPECT_THAT(omap.rbegin()->second, std::make_pair(3, 30));

  {
    std::vector<std::pair<int, int>> ret{};
    std::vector<std::pair<int, int>> expect{{3, 30}, {3, 30}};

    auto found = omap.rbegin()->first;

    for (auto it = omap.equal_range(found); it.first != it.second; ++it.first)
      ret.emplace_back(it.first->second);

    EXPECT_THAT(ret, expect);
  }

  {
    std::vector<std::pair<int, int>> ret{};
    std::vector<std::pair<int, int>> expect{{3, 30}, {3, 30}};

    auto found = omap.rbegin()->first;

    for (auto it = omap.equal_range(found).first;
         it != omap.equal_range(found).second;
         ++it)
      ret.emplace_back(it->second);

    EXPECT_THAT(ret, expect);
  }
}

// ={=========================================================================
// cxx-unordered

// *cxx-unordered-order*
// The only guarantee is that duplicates, which are possible because a multiset
// is used, are grouped together in the order of their insertion.

TEST(CxxUnordered, check_duplicates)
{
  std::unordered_multiset<string> cities{"Braunschweig",
                                         "Hanover",
                                         "Frankfurt",
                                         "New York",
                                         "Chicago",
                                         "Toronto",
                                         "Paris",
                                         "Frankfurt"};

  {
    std::vector<string> result{};

    for (const auto &elem : cities)
      result.push_back(elem);

    EXPECT_THAT(result,
                ElementsAre("Paris",
                            "Toronto",
                            "Chicago",
                            "New York",
                            "Frankfurt", // see
                            "Frankfurt", // see
                            "Hanover",
                            "Braunschweig"));
  }

  {
    // insert additional elements
    cities.insert({"London", "Munich", "Hanover", "Braunschweig"});

    std::vector<string> result{};

    for (const auto &elem : cities)
      result.push_back(elem);

    EXPECT_THAT(result,
                ElementsAreArray({"Munich",
                                  "London",
                                  "Frankfurt", // see
                                  "Frankfurt", // see
                                  "New York",
                                  "Braunschweig", // see
                                  "Braunschweig", // see
                                  "Chicago",
                                  "Toronto",
                                  "Hanover", // see
                                  "Hanover", // see
                                  "Paris"}));
  }
}

TEST(CxxUnordered, search)
{
  {
    std::unordered_multiset<std::string> coll{"Braunschweig",
                                              "Hanover",
                                              "Frankfurt",
                                              "New York",
                                              "Chicago",
                                              "Toronto",
                                              "Paris",
                                              "Frankfurt"};

    EXPECT_THAT(coll.count("Frankfurt"), 2);
    EXPECT_NE(coll.find("Frankfurt"), coll.end());
  }

  {
    std::vector<std::string> result{};

    std::unordered_multiset<std::string> coll{"Braunschweig",
                                              "Hanover",
                                              "Frankfurt",
                                              "New York",
                                              "Chicago",
                                              "Toronto",
                                              "Paris",
                                              "Frankfurt"};

    auto range = coll.equal_range("Frankfurt");
    EXPECT_THAT(std::distance(range.first, range.second), 2);

    auto loop = coll.count("Frankfurt");

    auto first = coll.find("Frankfurt");

    for (int i = 0; i < (int)loop; ++i)
    {
      result.emplace_back(*first);
      ++first;
    }

    EXPECT_THAT(result, ElementsAre("Frankfurt", "Frankfurt"));

    // when not found
    EXPECT_THAT(coll.count("Seoul"), 0);
  }

  {
    std::unordered_map<int, int> coll{{100, 1}, {200, 2}, {300, 3}};

    auto ret = coll.find(200);

    EXPECT_THAT(ret->first, 200);
    EXPECT_THAT(ret->second, 2);
  }
}

TEST(CxxUnordered, insert)
{
  // do not have duplicates
  std::unordered_set<std::string> coll{"Braunschweig",
                                       "Hanover",
                                       // "Frankfurt",
                                       "New York",
                                       "Chicago",
                                       "Toronto",
                                       "Paris",
                                       "Frankfurt"};

  // not allowed to add duplicate
  auto ret = coll.insert("Frankfurt");

  // to indicate "insert has failed"
  EXPECT_THAT(ret.second, false);
}

namespace collunordered
{
  class Todo
  {
    // friend since need to access private members
    friend std::ostream &operator<<(std::ostream &o, const Todo &t);
    friend struct std::hash<Todo>;

    int priority_{};
    std::string desc_{};

  public:
    Todo(int priority, std::string desc)
        : priority_(priority)
        , desc_(desc)
    {}

    // have it as a member
    bool operator==(const Todo &t) const
    {
      if ((priority_ == t.priority_) && (desc_ == t.desc_))
        return true;

      return false;
    }
  };

  // requires "operator<<()" for printing Todo
  template <typename T>
  void print_unordered_set(const std::unordered_set<T> &coll)
  {
    for (const auto &e : coll)
      std::cout << e << std::endl;
  }

  std::ostream &operator<<(std::ostream &o, const Todo &t)
  {
    o << "prio: " << t.priority_ << ", desc: " << t.desc_;
    return o;
  }
} // namespace collunordered

// to use user type in "unordered" coll, have to define hash function for it.
// cxx provides default hash functions for basic types; int, std::string, so we
// use them to make own hash function for our type.

namespace std
{
  using collunordered::Todo;

  template <>
  struct hash<Todo>
  {
    size_t operator()(const Todo &t) const
    {
      std::hash<std::string> hashf;

      // xor
      return t.priority_ ^ (hashf(t.desc_));
    }
  };
} // namespace std

TEST(CxxUnordered, check_user_type)
{
  using namespace collunordered;

  std::unordered_set<Todo> coll;

  coll.insert(Todo(1, "do sport"));
  coll.insert(Todo(2, "do revise"));
  coll.insert(Todo(1, "do programming"));
  coll.insert(Todo(3, "do socilise"));
  coll.insert(Todo(2, "do watching"));

  print_unordered_set(coll);
}

// ={=========================================================================
// cxx-list

TEST(CxxList, splice_merge_divide)
{
  {
    list<int> list_one, list_two;

    for (int i = 0; i < 6; ++i)
    {
      list_one.push_back(i);
      list_two.push_back(10 + i);
    }

    EXPECT_THAT(list_one, ElementsAre(0, 1, 2, 3, 4, 5));
    EXPECT_THAT(list_two, ElementsAre(10, 11, 12, 13, 14, 15));

    // splice, verb, to join two pieces of rope, film, etc. together at their
    // ends in order to form one long piece:
    //
    // c.splice(pos,c2) Moves all elements of c2 to c in front of the iterator
    // position pos
    // moves all elements of list_one before the pos of '3' element.

    list_two.splice(find(list_two.begin(), list_two.end(), 13), list_one);

    EXPECT_EQ(list_one.size(), 0);
    // 10 11 12 [0 1 2 3 4 5] 13 14 15
    EXPECT_THAT(list_two,
                ElementsAreArray({10, 11, 12, 0, 1, 2, 3, 4, 5, 13, 14, 15}));

    // c.splice(pos,c2,c2pos)
    // Moves the *single* element at c2pos in c2 in front of pos of list c
    // (c and c2 may be identical)
    //
    // move first element of list_two to the end

    list_two.splice(list_two.end(), list_two, list_two.begin());
    EXPECT_THAT(list_two,
                ElementsAreArray({11, 12, 0, 1, 2, 3, 4, 5, 13, 14, 15, 10}));

    list<int> list_three;
    // move first element of list_two to the first of list_three
    list_three.splice(list_three.begin(), list_two, list_two.begin());

    EXPECT_THAT(list_two,
                ElementsAreArray({12, 0, 1, 2, 3, 4, 5, 13, 14, 15, 10}));
    EXPECT_THAT(list_three, ElementsAre(11));

    list<int> coll1{20, 21, 22};
    list<int> coll2{30, 31, 32};
    list_three.splice(list_three.begin(), coll1);
    list_three.splice(list_three.end(), coll2);
    EXPECT_THAT(list_three, ElementsAreArray({20, 21, 22, 11, 30, 31, 32}));

    list<int> list4{0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
    list<int> list5{0, 1, 2, 3, 4, 5};

    list5.merge(list4);
    EXPECT_THAT(
      list5,
      ElementsAreArray({0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5}));

    // 8.8.1 Special Member Functions for Lists (and Forward Lists)
    //
    // Strictly speaking, the standard requires that both (forward) lists be
    // sorted on entry. In practice, however, merging is also possible for
    // unsorted lists.  However, you should check this before you rely on it.
    //
    // list<int> list5;
    // list5.merge({4, 1, 0, 3, 2, 5});
    // EXPECT_THAT(list5, ElementsAreArray({0, 1, 2, 3, 4, 5}));
    //
    // cxx-runtime-error
    //
    // /usr/include/c++/4.9/debug/list:691:error: elements in iterator range [
    //     __x.begin().base(), __x.end().base()) are not sorted.
    //
    // Aborted.
    //
    // list<int> list6;
    // list6.merge({4, 1, 0, 3, 2, 5});
    // EXPECT_THAT(list6, Not(ElementsAreArray({0, 1, 2, 3, 4, 5})));
  }

  {
    std::list<int> coll{26, 33, 35, 29, 19, 12, 22};
    auto slow = coll.begin();
    auto fast = next(slow);

    for (; fast != coll.end();)
    {
      ++fast;

      if (fast != coll.end())
      {
        ++fast;
        ++slow;
      }
    }

    list<int> coll1;
    list<int> coll2;

    // due to open end of iterator, increase one more compared to C version.
    ++slow;

    // c.splice(pos,c2, c2beg,c2end)
    // Moves all elements of the range [c2beg,c2end) in c2 in
    // front of pos of list c (c and c2 may be identical)

    coll1.splice(coll1.begin(), coll, coll.begin(), slow);
    coll2.splice(coll2.begin(), coll, slow, coll.end());

    EXPECT_THAT(coll1, ElementsAre(26, 33, 35, 29));
    EXPECT_THAT(coll2, ElementsAre(19, 12, 22));
  }
}

TEST(CxxList, operations)
{
  {
    std::list<int> coll1, coll2;

    for (int i = 0; i < 6; ++i)
    {
      coll1.push_back(i);
      coll2.push_front(i);
    }

    EXPECT_THAT(coll1, ElementsAre(0, 1, 2, 3, 4, 5));
    EXPECT_THAT(coll1.size(), 6);
    EXPECT_THAT(coll2, ElementsAre(5, 4, 3, 2, 1, 0));
    EXPECT_THAT(coll2.size(), 6);

    EXPECT_THAT(coll1.front(), 0);
    EXPECT_THAT(coll1.back(), 5);
    EXPECT_THAT(coll1.size(), 6);

    // void pop_front() so cannot use EXPECT_THAT()
    coll1.pop_front();
    EXPECT_THAT(coll1.size(), 5);

    coll1.pop_back();
    EXPECT_THAT(coll1.size(), 4);

    EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4));
  }

  // no such interface to assign.
  // {
  //   std::list<int> coll{0,1,2,3,4,5};
  //   EXPECT_THAT(coll, ElementsAre(0,1,2,3,4,5));
  //   coll.assign(++coll.begin(), 10);
  //   EXPECT_THAT(coll, ElementsAre(0,10,2,3,4,5));
  // }
}

TEST(CxxList, removeif)
{
  std::list<int> coll{0, 1, 2, 3, 4};

  // remove if e > 2

  coll.remove_if([](int e) {
    if (e > 2)
      return true;

    return false;
  });

  EXPECT_THAT(coll, ElementsAre(0, 1, 2));

  // remove if can be divided by 2

  coll.remove_if([](int e) { return e % 2 == 0; });

  EXPECT_THAT(coll, ElementsAre(1));
}

TEST(CxxList, erase)
{
  std::list<int> coll{10, 20, 30, 40, 50};

  auto iter = coll.begin();

  ++iter;
  ++iter;

  coll.erase(coll.begin(), iter);

  EXPECT_THAT(coll, ElementsAre(30, 40, 50));
}

TEST(CxxList, sort)
{
  list<int> coll{26, 33, 35, 29, 19, 12, 22};
  coll.sort();
}

TEST(CxxList, search)
{
  // when use cxx-algo
  {
    std::list<int> coll1{6, 5, 4, 3, 2, 1};
    std::vector<int> ret1{};

    std::list<int> coll2{1, 2, 3};
    std::vector<int> ret2{};

    // try to find coll1 elements which are not in coll2
    for (auto const e : coll1)
    {
      if (std::find(coll2.cbegin(), coll2.cend(), e) == coll2.end())
        ret1.push_back(e);
    }

    EXPECT_THAT(ret1, ElementsAre(6, 5, 4));

    // try to find coll2 elements which are not in coll1
    for (auto const e : coll2)
    {
      if (std::find(coll1.cbegin(), coll1.cend(), e) == coll1.end())
        ret2.push_back(e);
    }

    EXPECT_THAT(ret2.size(), 0);
  }

  // when use manual loop
  {
    std::list<int> coll1{6, 5, 4, 3, 2, 1};
    std::vector<int> ret1{};

    std::list<int> coll2{1, 2, 3};
    std::vector<int> ret2{};

    // try to find coll1 elements which are not in coll2
    for (auto const e : coll1)
    {
      auto it = coll2.cbegin();

      for (; it != coll2.cend(); ++it)
      {
        // found this e and try the next
        if (*it == e)
          break;
      }

      // not found in coll2
      if (coll2.cend() == it)
        ret1.push_back(e);
    }

    EXPECT_THAT(ret1, ElementsAre(6, 5, 4));
  }
}

// ={=========================================================================
// cxx-list-forward

TEST(ForwardList, InsertAfter)
{
  {
    forward_list<int> coll{1, 2, 3};
    coll.insert_after(coll.before_begin(), {77, 88, 99});
    EXPECT_THAT(coll, ElementsAre(77, 88, 99, 1, 2, 3));
  }

  // Note that calling an _after member function with end() or cend() results in
  // undefined behavior because to append a new element at the end of a forward
  // list, you have to pass the position of the last element (or before_begin()
  // if none):
  //
  // RUNTIME ERROR: appending element after end is undefined behavior
  //
  // fwlist.insert_after(fwlist.end(),9999);
  //
  // {
  //   forward_list<int> coll{1,2,3};
  //   coll.insert_after(coll.end(), {77, 88, 99});
  //   EXPECT_THAT(coll, ElementsAre(1, 2, 3, 77, 88, 99));
  // }
}

// The drawbacks of having a singly linked list, where you can only traverse
// forward, gets even worse when trying to find an element to insert or delete
// something there. The problem is that when you find the element, you are too
// far, because to insert or delete something there you have to manipulate the
// element before the element you are searching for. For this reason, you have
// to find an element by determining whether the next element fits a specific
// criterion

TEST(ForwardList, DrawBacks)
{
  forward_list<int> coll{1, 2, 3, 4, 5, 97, 98, 99};

  auto pos_before = coll.before_begin();
  for (auto pos = coll.begin(); pos != coll.end(); ++pos, ++pos_before)
  {
    if (*pos % 2 == 0)
      break;
  }

  // there is no insert_before()
  coll.insert_after(pos_before, 42);
  EXPECT_THAT(coll, ElementsAre(1, 42, 2, 3, 4, 5, 97, 98, 99));
}

// poor performance
TEST(ForwardList, Resize)
{
  forward_list<int> coll{2, 3, 4, 5, 6, 7};

  coll.resize(9);
  coll.resize(10, 99);

  EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6, 7, 0, 0, 0, 99));
}

namespace use_forward_list
{
  template <typename T>
  class ForwardList
  {
  public:
    typedef typename std::forward_list<T>::iterator iterator;

    ForwardList()
        : count_(0)
    {}

    size_t size() const { return count_; }
    void push_front(const T &elem)
    {
      ++count_;
      list_.push_front(elem);
    }

    iterator begin() { return list_.begin(); }
    iterator end() { return list_.end(); }

  private:
    size_t count_;
    std::forward_list<T> list_;
  };
} // namespace use_forward_list

TEST(ForwardList, ImplementedInTermsOf)
{
  using namespace use_forward_list;

  ForwardList<int> coll;

  for (int i = 0; i < 8; ++i)
  {
    coll.push_front(i);
  }

  // {7, 6, 5, 4, 3, 2, 1}

  ForwardList<int> rcoll;

  for (auto iter = coll.begin(); iter != coll.end(); ++iter)
  {
    rcoll.push_front(*iter);
  }

  // {1, 2, 3, 4, 5, 6, 7}

  // iter = rcoll.begin();
  // for (iter; iter != rcoll.end(); ++iter)
  //{
  //  cout << "iter: " << *iter << endl;
  //}

  // do not work since ForwardList is not complete and may not support gtest
  // expects
  // EXPECT_THAT(rcoll, ElementsAre(7, 6, 5, 4, 3, 2, 1, 0));
}

// CSTL, 7.6.4 Examples of Using Forward Lists
TEST(ForwadList, BookExample)
{
  forward_list<int> coll1{1, 2, 3, 4};
  forward_list<int> coll2{77, 88, 99};

  coll2.insert_after(coll2.before_begin(), 99);
  coll2.push_front(10);
  coll2.insert_after(coll2.before_begin(), {10, 11, 12, 13});

  EXPECT_THAT(coll2, ElementsAre(10, 11, 12, 13, 10, 99, 77, 88, 99));

  coll1.insert_after(coll1.before_begin(), coll2.begin(), coll2.end());

  EXPECT_THAT(
    coll1,
    ElementsAreArray({10, 11, 12, 13, 10, 99, 77, 88, 99, 1, 2, 3, 4}));

  // delete 2nd element
  coll2.erase_after(coll2.begin());
  coll2.erase_after(find(coll2.begin(), coll2.end(), 99), coll2.end());

  EXPECT_THAT(coll2, ElementsAreArray({10, 12, 13, 10, 99}));

  coll1.sort();
  coll2 = coll1;
  coll2.unique();

  EXPECT_THAT(coll2,
              ElementsAreArray({1, 2, 3, 4, 10, 11, 12, 13, 77, 88, 99}));
}

/*
={=========================================================================
cxx-stack

class Stack
{
   bool empty() const;
   void push(T const &elem);
   void pop();
   T &top();
}

*/

TEST(StlStack, check_basic_example)
{
  std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

  std::stack<int> result1;
  std::vector<int> result2;

  // fill in stack
  for (auto e : coll)
    result1.push(e);

  // cxx-stack not work with gtest
  // EXPECT_THAT(result1, ElementsAreArray({12,11,10,9,8,7,6,5,4,3,2,1}));

  // change top element
  result1.top() = 98;

  // EXPECT_THAT(result1, ElementsAreArray({98,11,10,9,8,7,6,5,4,3,2,1}));

  while (!result1.empty())
  {
    result2.push_back(result1.top());
    result1.pop();
  }

  EXPECT_THAT(result2,
              ElementsAreArray({98, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}));
}

namespace cxx_stack
{
  class ReadOnEmptyStack : public std::exception
  {
  public:
    virtual const char *what() const noexcept
    {
      return "connot read when statck is empty";
    }
  };

  template <typename T>
  class Stack
  {
  private:
    std::deque<T> _coll;

  public:
    bool empty() const { return _coll.empty(); }

    void push(const T &elem) { _coll.emplace_back(elem); }

    void pop()
    {
      if (_coll.empty())
        throw ReadOnEmptyStack();

      _coll.pop_back();
    }

    T &top()
    {
      if (_coll.empty())
        throw ReadOnEmptyStack();

      // note "back() but not pop_back()"
      return _coll.back();
    }
  };
} // namespace cxx_stack

TEST(StlStack, check_implemented_in_terms_of)
{
  using namespace cxx_stack;

  Stack<int> coll{};
  std::vector<int> result{};

  coll.push(10);
  coll.push(20);
  coll.push(30);

  coll.pop();

  coll.top() = 80;

  coll.push(100);
  coll.push(200);

  while (!coll.empty())
  {
    result.emplace_back(coll.top());
    coll.pop();
  }

  EXPECT_THAT(result, ElementsAre(200, 100, 80, 10));

  // stack is already empty
  EXPECT_THROW(coll.pop(), ReadOnEmptyStack);
}

// ={=========================================================================
// algo-predicate

namespace algo_predicate
{
  class PredicateWithState
  {
  public:
    PredicateWithState(int value)
        : nth_(value)
        , count_(0)
    {}

    // *cxx-unused*
    bool operator()(int) { return ++count_ == nth_; }

  private:
    int nth_;
    int count_;
  };

  class PredicateWithValue
  {
  public:
    void operator()(int value) { value += 10; }
  };

  class PredicateWithReference
  {
  public:
    void operator()(int &value) { value += 10; }
  };
} // namespace algo_predicate

TEST(Predicate, Stateless)
{
  using namespace algo_predicate;

  list<int> coll = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto pos = remove_if(coll.begin(), coll.end(), PredicateWithState(3));

  coll.erase(pos, coll.end());

  // removed 3 and 6. WHY?
  EXPECT_THAT(coll, ElementsAre(1, 2, 4, 5, 7, 8, 9, 10));
}

// template<typename _InputIterator, typename _Function>
//   _Function
// for_each(_InputIterator __first, _InputIterator __last, _Function __f)
// {
//   for (; __first != __last; ++__first)
//     __f(*__first);
//
//   return _GLIBCXX_MOVE(__f);
// }

TEST(Predicate, ValueAndReference)
{
  using namespace algo_predicate;

  {
    vector<int> coll{1, 2, 3, 4, 5};
    std::for_each(coll.begin(), coll.end(), PredicateWithValue());
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5));
  }

  {
    vector<int> coll{1, 2, 3, 4, 5};
    std::for_each(coll.begin(), coll.end(), PredicateWithReference());
    EXPECT_THAT(coll, ElementsAre(11, 12, 13, 14, 15));
  }
}

/*
// ={=========================================================================
// cxx-copy

[ RUN      ] AlgoCopy.Error
/usr/include/c++/6/bits/stl_algobase.h:452:
Error: function requires a valid iterator range [__first, __last).

Objects involved in the operation:
    iterator "__first" @ 0x0x7ffe7bb56d60 {
      type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<int*, std::__cxx1998::vector<int, std::allocator<int> > >, std::__debug::vector<int, std::allocator<int> > > (mutable iterator);
      state = past-the-end;
      references sequence with type 'std::__debug::vector<int, std::allocator<int> >' @ 0x0x7ffe7bb56c10
    }
    iterator "__last" @ 0x0x7ffe7bb56d30 {
      type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<int*, std::__cxx1998::vector<int, std::allocator<int> > >, std::__debug::vector<int, std::allocator<int> > > (mutable iterator);
      state = dereferenceable;
      references sequence with type 'std::__debug::vector<int, std::allocator<int> >' @ 0x0x7ffe7bb56c10
    }
Aborted

*/
TEST(AlgoCopy, DISABLED_invalid_range)
{
  {
    std::vector<int> coll;

    for (int i = 0; i <= 6; ++i)
      coll.push_back(i);

    auto first = std::find(coll.begin(), coll.end(), 300);
    auto last  = std::find(coll.begin(), coll.end(), 3);

    try
    {
      std::copy(first, last, std::ostream_iterator<int>(cout, ", "));
    } catch (const exception &e)
    {
      std::cout << "exection " << e.what() << std::endl;
    }
  }
}

// ={=========================================================================
// *cxx-algo-copy-if*
// OutputIterator
// copy_if (InputIterator sourceBeg, InputIterator sourceEnd,
// OutputIterator destBeg,
// UnaryPredicate op)

TEST(AlgoCopy, CopyIf)
{
  std::vector<int> coll1;
  std::vector<int> coll2;

  for (int i = 0; i <= 6; ++i)
    coll1.push_back(i);

  std::copy_if(
    coll1.begin(),
    coll1.end(),
    back_inserter(coll2),
    // [](int e)
    [](const decltype(coll2)::value_type &e) { return (e % 2) == 0; });

  // check if there are all even
  EXPECT_THAT(coll2, ElementsAre(0, 2, 4, 6));
}

// ={=========================================================================
TEST(AlgoCopy, UseOnDifferentCollections)
{
  {
    vector<int> coll{1, 2, 3, 4, 5, 6};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6));

    list<int> coll1;

    std::copy(coll.begin(), coll.end(), inserter(coll1, coll1.begin()));
    EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4, 5, 6));
  }
}

// ={=========================================================================
// cxx-count
TEST(AlgoCount, count)
{
  std::vector coll{1, 0, 1, 0, 1, 1, 0, 1};

  EXPECT_THAT(std::count(coll.cbegin(), coll.cend(), 1), 5);
}

/*
// ={=========================================================================
cxx-generate cxx-random

Assigning Generated Values

void
generate (ForwardIterator beg, ForwardIterator end, Func op)

void
generate_n (OutputIterator beg, Size num, Func op)

generate() assigns the values that are generated by a call of op()
to each element in the range [beg,end).

*/

// 16807, 282475249, 1622650073, 984943658, 1144108930, 470211272, 101027544,
// 1457850878, 1458777923, 2007237709, 823564440, 1115438165, 1784484492,
// 74243042, 114807987, 1137522503, 1441282327, 16531729,
//  0,  0,  5,  3,  3,  1,  0,  4,  4,  6,  2,  3,  5,  0,  0,  3,  4,  0,
//  8,  6,  8, 10, 10, 12, 11,  9,  6, 10,  8, 10, 12, 11,  7,  6, 11,  8,

namespace algo_generate
{
  class IntegerSequence
  {
  public:
    IntegerSequence(int value)
        : value_(value)
    {}

    int operator()() { return ++value_; }

  private:
    int value_;
  };

  class IntegerSequenceNoReturn
  {
  public:
    IntegerSequenceNoReturn(int value)
        : value_(value)
    {}

    void operator()() { ++value_; }

  private:
    int value_;
  };
} // namespace algo_generate

// _Generator shall return value and otherwise get errors:
//
// stl.cpp:2464:7:   required from here
// /usr/include/c++/4.9/bits/stl_algo.h:4325:11: error: no match for ‘operator=’
// (operand types are ‘std::back_insert_iterator<std::__debug::list<int> >’ and
// ‘void’)
//   *__first = __gen();

TEST(AlgoGenerate, check_generate)
{
  using namespace algo_generate;

  std::list<int> coll{};

  // insert five random numbers
  std::generate_n(back_inserter(coll), // beginning of destination range
                  5,                   // count
                  rand);               // new value generator

  // PRINT_ELEMENTS(coll);

  // `overwrite` with five new random numbers
  std::generate(coll.begin(),
                coll.end(),          // destination range
                IntegerSequence(1)); // new value generator

  EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6));
}

// note on "iota" but not "itoa" and see how it differs from generate()
//
// https://en.cppreference.com/w/cpp/algorithm/iota
// Fills the range [first, last) with sequentially increasing values, starting
// with value and repetitively evaluating ++value.

TEST(AlgoGenerate, check_iota)
{
  using namespace algo_generate;

  {
    std::list<int> coll{0, 0, 0, 0, 0};

    std::generate(coll.begin(),
                  coll.end(),          // destination range
                  IntegerSequence(1)); // new value generator

    EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6));
  }

  {
    std::list<int> coll{0, 0, 0, 0, 0};

    std::iota(coll.begin(),
              coll.end(), // destination range
              2);

    EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6));
  }
}

TEST(AlgoGenerate, Reference)
{
  using namespace algo_generate;

  std::list<int> coll;

  // starting from 1
  IntegerSequence seq(1);

  std::generate_n<back_insert_iterator<list<int>>, int, IntegerSequence &>(
    back_inserter(coll),
    4, // number of elements
    seq);

  EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5));

  // insert values from 43 to 46

  generate_n(back_inserter(coll), // start
             4,                   // number of elements
             IntegerSequence(42));

  EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 43, 44, 45, 46));

  // currently seq has 6 value and start from that since passed by value

  generate_n(back_inserter(coll), // start
             4,                   // number of elements
             seq);

  EXPECT_THAT(coll, ElementsAreArray({2, 3, 4, 5, 43, 44, 45, 46, 6, 7, 8, 9}));

  // no change of sequence values and repeat it again since seq is copied.

  generate_n(back_inserter(coll), // start
             4,                   // number of elements
             seq);

  EXPECT_THAT(
    coll,
    ElementsAreArray({2, 3, 4, 5, 43, 44, 45, 46, 6, 7, 8, 9, 6, 7, 8, 9}));
}

// 1 3 9 6 7 8 2 4 5 (9)
// 8 7 5 6 2 4 9 3 1 (9)

TEST(AlgoRandom, AlgoShuffle)
{
  vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};

  // shuffle randomly
  // The second form shuffles the order of the elements in the range [beg,end),
  // using an implementation- defined uniform distribution random-number
  // generator, such as the C function rand().

  random_shuffle(coll.begin(), coll.end());
  PRINT_ELEMENTS(coll);

  // sort again
  sort(coll.begin(), coll.end());

  default_random_engine dre;
  shuffle(coll.begin(), coll.end(), dre);
  PRINT_ELEMENTS(coll);
}

// ={=========================================================================
// cxx-algo-accumulate
//
// 11.11 Numeric Algorithms
//
// The first form computes and returns the sum of initValue and all elements in
// the range [beg,end).  In particular, it calls the following for each element:
//
// initValue = initValue + elem
//
// The second form computes and returns the result of calling op for initValue
// and all elements in the range [beg,end). In particular, it calls the
// following for each element:
//
// initValue = op(initValue,elem)
//
// Thus, for the values
//  a1 a2 a3 a4 ...
//
// they compute and return either
//  initValue + a1 + a2 + a3 + ...
//
// or
//  initValue op a1 op a2 op a3 op ...
// respectively.

TEST(CxxAlgoAccumulate, on_stream)
{
  std::istringstream is{"1 2 3 4 5 6"};
  std::istream_iterator<int> start(is), eof;

  auto sum = std::accumulate(start, eof, 0);

  EXPECT_THAT(sum, 21);
}

// ={=========================================================================
TEST(CxxAlgoAccumulate, on_container)
{
  std::vector<int> coll{1, 2, 3, 4, 5};

  // sum
  ASSERT_THAT(accumulate(coll.cbegin(), coll.cend(), 0), Eq(15));
  ASSERT_THAT(accumulate(coll.cbegin(), coll.cend(), -50), Eq(-35));

  // it does have return value
  auto sum = accumulate(coll.cbegin(), coll.cend(), 0);
  ASSERT_THAT(sum, Eq(15));

  sum = accumulate(coll.cbegin(), coll.cend(), -50);
  ASSERT_THAT(sum, Eq(-35));

  // product. multiplies is binary predicate
  // 1*1*2*3*4*5 = 120
  ASSERT_THAT(accumulate(coll.cbegin(), coll.cend(), 1, multiplies<int>()),
              Eq(120));
  ASSERT_THAT(accumulate(coll.cbegin(), coll.cend(), 0, multiplies<int>()),
              Eq(0));

  // if use cxx-rambda
  ASSERT_THAT(accumulate(coll.cbegin(),
                         coll.cend(),
                         1,
                         [](int init, int elem) { return init * elem; }),
              Eq(120));

  // sum, if use cxx-rambda
  ASSERT_THAT(accumulate(coll.cbegin(),
                         coll.cend(),
                         0,
                         [](int init, int elem) { return init + elem; }),
              Eq(15));
}

// ={=========================================================================
// see how to use acculumate on map

TEST(CxxAlgoAccumulate, on_map)
{
  std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

  // *cxx-error*
  // auto result = std::accumulate(counts.begin(), counts.end(), pair<int const,
  // size_t>());

  // *cxx-const* cause error:
  //     [](size_t sum, pair<int, size_t>& e)
  //
  auto result = std::accumulate(
    counts.begin(),
    counts.end(),
    0,
    [](size_t sum, pair<int const, size_t> &e) { return sum + e.second; });

  EXPECT_THAT(result, 11);
}

// ={=========================================================================
// cxx-for-each cxx-transform

namespace algo_code
{
  template <typename _InputIterator, typename _Function>
  _Function
  for_each(_InputIterator __first, _InputIterator __last, _Function __f)
  {
    // note: call op but not use return

    for (; __first != __last; ++__first)
      __f(*__first);

    return _GLIBCXX_MOVE(__f);
  }

  template <typename _InputIterator,
            typename _OutputIterator,
            typename _UnaryOperation>

  _OutputIterator transform(_InputIterator __first,
                            _InputIterator __last,
                            _OutputIterator __result,
                            _UnaryOperation __unary_op)
  {
    for (; __first != __last; ++__first, ++__result)
      // note: write to output to output iterator and unary
      *__result = __unary_op(*__first);
    return __result;
  }
} // namespace algo_code

int square_value_with_return(int value)
{
  value = value * value;
  return value;
}

void square_value_no_return(int value)
{
  value = value * value;
}

void square_refer_no_return(int &value)
{
  value = value * value;
}

TEST(StlForEach, use_function)
{
  // value
  {
    std::set<int> coll{1, 2, 3, 4, 5, 6, 7, 8};
    std::for_each(coll.begin(), coll.end(), square_value_no_return);
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
  }

  // value with return? same since do not use return value
  {
    std::set<int> coll{1, 2, 3, 4, 5, 6, 7, 8};
    for_each(coll.begin(), coll.end(), square_value_with_return);
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
  }

  // reference
  {
    // compile error since key in set are const if use set
    // set<int> coll{1,2,3,4,5,6,7,8};

    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};
    for_each(coll.begin(), coll.end(), square_refer_no_return);
    EXPECT_THAT(coll, ElementsAre(1, 4, 9, 16, 25, 36, 49, 64));
  }

  // but algo-transform() differs since it uses `dest` and use return
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> result;

    std::transform(coll.begin(),
                   coll.end(),
                   back_inserter(result),
                   square_value_with_return);

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
    EXPECT_THAT(result, ElementsAre(1, 4, 9, 16, 25, 36, 49, 64));
  }
}

TEST(AlgoForEach, UsePredicates)
{
  {
    vector<int> coll{1, 2, 3, 4, 5, 6};

    for_each(coll.begin(), coll.end(), [](int &elem) { elem += 10; });

    EXPECT_THAT(coll, ElementsAre(11, 12, 13, 14, 15, 16));
  }

  // to see how cxx-range-for could make for_each less useful
  {
    vector<int> coll{1, 2, 3, 4, 5, 6};

    for (auto &elem : coll)
      elem += 10;

    EXPECT_THAT(coll, ElementsAre(11, 12, 13, 14, 15, 16));
  }

  // add value of first element which is copied.
  {
    vector<int> coll{1, 2, 3, 4, 5, 6};

    for_each(coll.begin(), coll.end(), [=](int &elem) {
      elem += *coll.begin();
    });

    EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6, 7));
  }

  // add value of first element which is reference so use '2' but not '1'
  {
    vector<int> coll{1, 2, 3, 4, 5, 6};

    for_each(coll.begin(), coll.end(), [&](int &elem) {
      elem += *coll.begin();
    });

    EXPECT_THAT(coll, ElementsAre(2, 4, 5, 6, 7, 8));
  }
}

// 11.4 The for_each() Algorithm
// A third example demonstrates how to use the return value of the for_each()
// algorithm. Because for_each() has the special property that it returns its
// operation, you can process and return a result inside the operation:
//
// note that this is not predicate since it has its state.

namespace algo_for_each
{
  class MeanValue
  {
  public:
    MeanValue()
        : num_(0)
        , sum_(0)
    {}

    void operator()(int elem)
    {
      num_++;
      sum_ += elem;
    }

    operator double() { return static_cast<double>(sum_) / num_; }

  private:
    long num_;
    long sum_;
  };
} // namespace algo_for_each

TEST(AlgoForEach, GetMean)
{
  using namespace algo_for_each;

  vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};

  double mean = for_each(coll.begin(), coll.end(), MeanValue());

  EXPECT_THAT(mean, DoubleEq(4.5));
}

// ={=========================================================================
// cxx-gcd, great common divide/division?

TEST(StlGcd, stl_gcd)
{
  EXPECT_THAT(__gcd(10, 20), 10);
}

// ={=========================================================================
// cxx-algo-search

TEST(AlgoSearch, SearchN)
{
  deque<int> coll{1, 2, 7, 7, 6, 3, 9, 5, 7, 7, 7, 3, 6};

  auto pos = search_n(coll.begin(), coll.end(), 3, 7);
  if (pos != coll.end())
  {
    EXPECT_THAT(distance(coll.begin(), pos), 8);
  }
}

TEST(AlgoSearch, SearchFirstSubrange)
{
  deque<int> coll{1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
  list<int> subcoll{3, 4, 5};

  // first sub found
  auto maxpos = max_element(coll.begin(), coll.end());
  EXPECT_THAT(*maxpos, 6);

  // *algo-sort*
  sort(coll.begin(), coll.end());

  auto found = find(coll.begin(), coll.end(), 3);

  // *algo-reverse*
  // reverse the range
  reverse(found, coll.end());
  EXPECT_THAT(coll, ElementsAre(1, 2, 6, 5, 4, 3));
}

// ={=========================================================================
// cxx-algo-equal

TEST(AlgoEqual, VariousUseCases)
{
  vector<string> vec1{"Charles", "in", "Charge"};
  vector<string> vec2{"Charles", "in", "charge"};

  EXPECT_FALSE(vec1 == vec2);
  EXPECT_FALSE(equal(vec1.begin(), vec1.end(), vec2.begin()));

  string s1{"abcde"};
  string s2{"abcdf"};

  // Testing for “Less Than”
  //
  // Both forms return whether the elements in the range [beg1,end1) are
  // “lexicographically less than” the elements in the range [beg2,end2).

  EXPECT_FALSE(
    lexicographical_compare(s1.begin(), s1.end(), s1.begin(), s1.end()));
  EXPECT_TRUE(
    lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end()));

  // Search the First Difference
  //
  // The first form returns the first two corresponding elements of range
  // [beg,end) and the range starting with cmpBeg that differ.

  auto iter = mismatch(s1.begin(), s1.end(), s2.begin());
  // first mismatch = e second mismatch = f
  EXPECT_THAT(*iter.first, Eq('e'));
  EXPECT_THAT(*iter.second, Eq('f'));
}

TEST(AlgoEqual, UseOnDifferentCollections)
{
  vector<int> coll1;
  list<int> coll2;
  list<int> coll3;

  INSERT_ELEMENTS(coll1, 1, 7);
  INSERT_ELEMENTS(coll2, 3, 9);
  INSERT_ELEMENTS(coll3, 1, 7);

  EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4, 5, 6, 7));
  EXPECT_THAT(coll2, ElementsAre(3, 4, 5, 6, 7, 8, 9));
  EXPECT_THAT(coll3, ElementsAre(1, 2, 3, 4, 5, 6, 7));

  EXPECT_FALSE(equal(coll1.begin(), coll1.end(), coll2.begin()));

  EXPECT_TRUE(equal(coll1.begin(), coll1.end(), coll3.begin()));

  // coll1 and coll2 are different but are the same as to the order of
  // even/odd.
  EXPECT_TRUE(
    equal(coll1.begin(), coll1.end(), coll2.begin(), [](int lhs, int rhs) {
      return lhs % 2 == rhs % 2;
    }));
}

TEST(AlgoEqual, UsePermutation)
{
  string string1{"PARK"}, string2{"KRAP"};

  // Testing for Unordered Equality
  EXPECT_TRUE(is_permutation(string1.begin(), string1.end(), string2.begin()));
}

// ={=========================================================================
// cxx-reverse

TEST(CxxAlgo, reverse)
{
  std::vector<int> coll{1, 2, 3, 4, 5, 6, 7};

  // reverse all
  std::reverse(coll.begin(), coll.end());
  EXPECT_THAT(coll, ElementsAre(7, 6, 5, 4, 3, 2, 1));

  // reverse range
  std::reverse(coll.begin() + 1, coll.end() - 1);
  EXPECT_THAT(coll, ElementsAre(7, 2, 3, 4, 5, 6, 1));
}

namespace algo_reverse
{
  // reverse iterator(random)
  using RITERATOR = std::vector<int>::iterator;

  void my_reverse(RITERATOR first, RITERATOR last)
  {
    --last;

    for (; first < last; ++first, --last)
      std::swap(*first, *last);
  }

  // bi iterator since it's from cxx-list
  using BITERATOR = std::list<int>::iterator;

  void my_reverse_bi(BITERATOR first, BITERATOR last)
  {
    --last;

    // since `<` is only supported for random
    // for (;first < last; ++first, --last)

    for (; first != last; ++first, --last)
      std::swap(*first, *last);
  }
} // namespace algo_reverse

TEST(CxxAlgo, reverse_different_iterator)
{
  using namespace algo_reverse;

  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7};

    my_reverse(coll.begin(), coll.end());
    EXPECT_THAT(coll, ElementsAre(7, 6, 5, 4, 3, 2, 1));

    my_reverse(coll.begin() + 1, coll.end() - 1);
    EXPECT_THAT(coll, ElementsAre(7, 2, 3, 4, 5, 6, 1));
  }

  {
    std::list<int> coll{1, 2, 3, 4, 5, 6, 7};

    my_reverse_bi(coll.begin(), coll.end());
    EXPECT_THAT(coll, ElementsAre(7, 6, 5, 4, 3, 2, 1));

    // since `+/- is only supported for random
    // my_reverse_bi(coll.begin()+1, coll.end()-1);

    my_reverse_bi(std::next(coll.begin()), std::prev(coll.end()));
    EXPECT_THAT(coll, ElementsAre(7, 2, 3, 4, 5, 6, 1));
  }
}

// ={=========================================================================
// cxx-algo-mutating
// cxx-algo-partition algo-gather algo-remove algo-rotate

TEST(AlgoMutating, AlgoPartition)
{
  vector<int> coll1;
  vector<int> coll2;

  INSERT_ELEMENTS(coll1, 1, 9);
  EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

  auto pos1 = partition(coll1.begin(),
                        coll1.end(), // range
                        [](int elem) { return elem % 2 == 0; });

  EXPECT_THAT(coll1, ElementsAre(8, 2, 6, 4, 5, 3, 7, 1, 9));

  // algo-partition returns an iterator to the first element where the
  // predicate is not true, or the end of the range if all elements satisfy
  // the predicate. so first odd element:

  EXPECT_EQ(*pos1, 5);

  INSERT_ELEMENTS(coll2, 1, 9);
  EXPECT_THAT(coll2, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

  auto pos2 = stable_partition(coll2.begin(), coll2.end(), [](int elem) {
    return elem % 2 == 0;
  });
  EXPECT_THAT(coll2, ElementsAre(2, 4, 6, 8, 1, 3, 5, 7, 9));

  // first odd element:
  EXPECT_EQ(*pos2, 1);
}

// https://github.com/fenbf/review/blob/master/stl/beautiful_std_alg.cpp
// 4. gather (cpp seasoning)
//
// use case: list of items, select some of items (good guys) and move the to
// position around p.  for instance: multiple selection on a list
//
// problem with std::not1:
// http://channel9.msdn.com/Events/GoingNative/2013/Cpp-Seasoning#c635149692925101916

template <typename Iterator, typename Compare>
auto gather(Iterator _first, Iterator _last, Iterator _pos, Compare _comp)
  -> std::pair<Iterator, Iterator>
{
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  auto _begin =
    partition(_first, _pos, [&](const value_type e) { return !_comp(e); });
  auto _end = partition(_pos, _last, _comp);
  return {_begin, _end};
}

TEST(AlgoMutating, Gather)
{
  {
    vector<int> coll(10, 0);
    coll[0] = coll[2] = coll[7] = coll[8] = 1;
    EXPECT_THAT(coll, ElementsAre(1, 0, 1, 0, 0, 0, 0, 1, 1, 0));

    // gather(begin, end, pos, comp);
    std::partition(coll.begin(), coll.begin() + 4, [](const int x) {
      return x != 1;
    });
    std::partition(coll.begin() + 4, coll.begin() + 10, [](const int x) {
      return x == 1;
    });

    EXPECT_THAT(coll, ElementsAre(0, 0, 1, 1, 1, 1, 0, 0, 0, 0));
  }

  {
    vector<int> coll(10, 0);
    coll[0] = coll[2] = coll[7] = coll[8] = 1;
    EXPECT_THAT(coll, ElementsAre(1, 0, 1, 0, 0, 0, 0, 1, 1, 0));

    // rearrange coll to centre around `pos`
    // pos = 4
    // (1, 0, 1, 0, 0, 0, 0, 1, 1, 0));
    //              ^
    // (            ]
    //              (               ]
    // (0, 0, 1, 1, 1, 1, 0, 0, 0, 0));

    gather(coll.begin(), coll.end(), coll.begin() + 4, [](const int x) {
      return x == 1;
    });
    EXPECT_THAT(coll, ElementsAre(0, 0, 1, 1, 1, 1, 0, 0, 0, 0));
  }
}

// cxx-algo-remove algo-unique

TEST(AlgoMutating, AlgoRemove)
{
  // coll.erase() delete elements but algo-remove do not.
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 2, 7, 2, 8, 2, 9};

    for (auto it = coll.begin(); it != coll.end(); ++it)
    {
      if (*it == 2)
        it = coll.erase(it);
    }

    EXPECT_THAT(coll, ElementsAre(1, 3, 4, 5, 6, 7, 8, 9));
  }

  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 2, 7, 2, 8, 2, 9};

    auto end = remove(coll.begin(), coll.end(), 2);

    EXPECT_THAT(distance(end, coll.end()), 4);
    EXPECT_THAT(coll, ElementsAreArray({1, 3, 4, 5, 6, 7, 8, 9, 2, 8, 2, 9}));

    coll.erase(end, coll.end());
    EXPECT_THAT(coll, ElementsAre(1, 3, 4, 5, 6, 7, 8, 9));
  }

  // show that algo-remove() do not remove elements
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 2, 7, 2, 8, 2, 9};

    remove(coll.begin(), coll.end(), 2);

    // std::vector<int> coll{1,3,4,5,6,7,8,9,2,8,2,9};
    //                                       ^^^^^^^

    EXPECT_THAT(coll, ElementsAreArray({1, 3, 4, 5, 6, 7, 8, 9, 2, 8, 2, 9}));
  }

  // show that remove_if() returns end if not found
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 2, 7, 2, 8, 2, 9};

    auto it = remove_if(coll.begin(), coll.end(), [](int value) {
      return value == 10;
    });

    EXPECT_THAT(it, coll.end());
  }
}

// algo-permutation

// 588 Chapter 11: STL Algorithms

// bool
// next_permutation (BidirectionalIterator beg, BidirectionalIterator end)
//
// next_permutation() changes the order of the elements in [beg,end) according
// to the next permutation.
//
// Both algorithms return false if the elements got the “normal”
// (lexicographical) order: that is, ascending order for next_permutation() and
// descending order for prev_permutation(). So, to run through all
// permutations, you have to sort all elements (ascending or descending), and
// start a loop that calls next_permutation() or prev_permutation() as long as
// these algorithms return true.
//
// that means return false when coll is sorted.

// start:1 2 3 (3)
// next :1 3 2 (3)
// next :2 1 3 (3)
// next :2 3 1 (3)
// next :3 1 2 (3)
// next :3 2 1 (3)
// finis:1 2 3 (3)
// now  :3 2 1 (3)
// prev :3 1 2 (3)
// prev :2 3 1 (3)
// prev :2 1 3 (3)
// prev :1 3 2 (3)
// prev :1 2 3 (3)
// finis:3 2 1 (3)
//
// start:1 2 3 (3)
// 123
// 123
//
// start:1 2 3 (3)
// 123
// 132
// 213
// 231
// 312
// 321

TEST(AlgoMutating, AlgoPermutation)
{
  {
    vector<int> coll{1, 2, 3};

    PRINT_ELEMENTS(coll, "start:");

    // permute coll until they are sorted, ascending
    while (next_permutation(coll.begin(), coll.end()))
    {
      PRINT_ELEMENTS(coll, "next :");
    }

    // return false when sorted
    PRINT_ELEMENTS(coll, "finis:");

    // until descending sorted and the loop ends immediately
    while (prev_permutation(coll.begin(), coll.end()))
    {
      PRINT_ELEMENTS(coll, "prev :");
    }
    PRINT_ELEMENTS(coll, "now  :");

    while (prev_permutation(coll.begin(), coll.end()))
    {
      PRINT_ELEMENTS(coll, "prev :");
    }
    PRINT_ELEMENTS(coll, "finis:");
  }

  {
    string coll{"123"};

    PRINT_ELEMENTS(coll, "start:");

    while (std::next_permutation(coll.begin(), coll.end()))
      ;
    {
      cout << coll << endl;
    }

    cout << coll << endl;
  }

  // why need algo-sort? since input can be any of permutations, sort it first
  // to make sure permutations from sorted to sorted.
  {
    string coll{"123"};

    PRINT_ELEMENTS(coll, "start:");

    std::sort(coll.begin(), coll.end());

    do
    {
      cout << coll << endl;
    } while (std::next_permutation(coll.begin(), coll.end()));
  }
}

// algo-rotate, algo-slide, algo-reverse

TEST(AlgoMutating, AlgoRotate)
{
  vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};

  // rotate one to the left
  // before *cxx-11* void rotate() so comment out
  // auto pos = rotate(

  rotate(coll.begin(),     // begin
         coll.begin() + 1, // new begin
         coll.end()        // end
  );
  EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6, 7, 8, 1));

  // return the new position of the (pervious) first element.
  // EXPECT_THAT(*pos, 1);

  // pos = rotate(

  rotate(coll.begin(), coll.end() - 2, coll.end());
  EXPECT_THAT(coll, ElementsAre(8, 1, 2, 3, 4, 5, 6, 7));
  // EXPECT_THAT(*pos, 2);

  // rotate so that 4 is the beginning
  // pos = rotate(

  rotate(coll.begin(), find(coll.begin(), coll.end(), 4), coll.end());
  EXPECT_THAT(coll, ElementsAre(4, 5, 6, 7, 8, 1, 2, 3));
  // EXPECT_THAT(*pos, 8);
}

TEST(AlgoMutating, AlgoReverse)
{
  {
    vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};

    std::reverse(coll.begin(), coll.end());
    EXPECT_THAT(coll, ElementsAre(8, 7, 6, 5, 4, 3, 2, 1));

    std::reverse(coll.begin() + 1, coll.end() - 1);
    EXPECT_THAT(coll, ElementsAre(8, 2, 3, 4, 5, 6, 7, 1));

    vector<int> result;
    std::reverse_copy(coll.begin(), coll.end(), back_inserter(result));
    EXPECT_THAT(result, ElementsAre(1, 7, 6, 5, 4, 3, 2, 8));
  }
}

// ={=========================================================================
// algo-sort

namespace algo_code
{
  // template< class RandomIt, class Compare>
  // void sort( RandomIt first, RandomIt last, Compare comp );

  // /usr/include/c++/4.9/bits/stl_algo.h

  /**
   *  @brief Sort the elements of a sequence using a predicate for comparison.
   *  @ingroup sorting_algorithms
   *  @param  __first   An iterator.
   *  @param  __last    Another iterator.
   *  @param  __comp    A comparison functor.
   *  @return  Nothing.
   *
   *  Sorts the elements in the range @p [__first,__last) in ascending order,
   *  such that @p __comp(*(i+1),*i) is false for every iterator @e i in the
   *  range @p [__first,__last-1).
   *
   *  The relative ordering of equivalent elements is not preserved, use
   *  @p stable_sort() if this is needed.
   */
  template <typename _RandomAccessIterator, typename _Compare>
  inline void sort(_RandomAccessIterator __first,
                   _RandomAccessIterator __last,
                   _Compare __comp)
  {
    // concept requirements
    __glibcxx_function_requires(
      _Mutable_RandomAccessIteratorConcept<_RandomAccessIterator>)
      __glibcxx_function_requires(
        _BinaryPredicateConcept<
          _Compare,
          typename iterator_traits<_RandomAccessIterator>::value_type,
          typename iterator_traits<_RandomAccessIterator>::value_type>)
        __glibcxx_requires_valid_range(__first, __last);

    std::__sort(__first, __last, __gnu_cxx::__ops::__iter_comp_iter(__comp));
  }

  /**
   *  @brief Sort the elements of a sequence.
   *  @ingroup sorting_algorithms
   *  @param  __first   An iterator.
   *  @param  __last    Another iterator.
   *  @return  Nothing.
   *
   *  Sorts the elements in the range @p [__first,__last) in ascending order,
   *  such that for each iterator @e i in the range @p [__first,__last-1),
   *  *(i+1)<*i is false.
   *
   *  The relative ordering of equivalent elements is not preserved, use
   *  @p stable_sort() if this is needed.
   */
  template <typename _RandomAccessIterator>
  inline void sort(_RandomAccessIterator __first, _RandomAccessIterator __last)
  {
    // concept requirements
    __glibcxx_function_requires(
      _Mutable_RandomAccessIteratorConcept<_RandomAccessIterator>)
      __glibcxx_function_requires(
        _LessThanComparableConcept<
          typename iterator_traits<_RandomAccessIterator>::value_type>)
        __glibcxx_requires_valid_range(__first, __last);

    std::__sort(__first, __last, __gnu_cxx::__ops::__iter_less_iter());
  }

  struct _Iter_less_iter
  {
    template <typename _Iterator1, typename _Iterator2>
    bool operator()(_Iterator1 __it1, _Iterator2 __it2) const
    {
      return *__it1 < *__it2;
    }
  };

  inline _Iter_less_iter __iter_less_iter() { return _Iter_less_iter(); }

  template <typename _RandomAccessIterator, typename _Compare>
  inline void __sort(_RandomAccessIterator __first,
                     _RandomAccessIterator __last,
                     _Compare __comp)
  {
    if (__first != __last)
    {
      std::__introsort_loop(__first,
                            __last,
                            std::__lg(__last - __first) * 2,
                            __comp);
      std::__final_insertion_sort(__first, __last, __comp);
    }
  }
} // namespace algo_code

// *cxx-sort*
TEST(StlAlgo, stl_sort)
{
  {
    std::vector<int> coll{3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5};
    std::sort(coll.begin(), coll.end());

    EXPECT_THAT(
      coll,
      ElementsAreArray({1, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 7}));
  }

  {
    std::vector<int> coll{3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5};
    // *cxx-callable*
    std::sort(coll.begin(), coll.end(), std::greater<int>());

    EXPECT_THAT(
      coll,
      ElementsAreArray({7, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 1}));
  }
}

bool shorter(string const &s1, string const &s2)
{
  return s1.size() < s2.size();
}

TEST(AlgoSorting, AlgoStableSort)
{
  vector<string> coll{"over",
                      "quick",
                      "red",
                      "fox",
                      "jumps",
                      "red",
                      "the",
                      "slow",
                      "turtle",
                      "the"};

  // sort by alphabet and remove dups. algo-unique()
  {
    std::sort(coll.begin(), coll.end());
    coll.erase(std::unique(coll.begin(), coll.end()), coll.end());
  }

  // eliminated
  EXPECT_THAT(coll,
              ElementsAre("fox",
                          "jumps",
                          "over",
                          "quick",
                          "red",
                          "slow",
                          "the",
                          "turtle"));

  // by size
  stable_sort(coll.begin(), coll.end(), shorter);

  // sorted and see that the order before sort is maintained
  EXPECT_THAT(coll,
              ElementsAre("fox",
                          "red",
                          "the",
                          "over",
                          "slow",
                          "jumps",
                          "quick",
                          "turtle")); // by size
}

TEST(AlgoSorting, AlgoSortNth)
{
  // do not get that!

  // Both forms sort the elements in the range [beg,end), so the correct element
  // is at the nth position, and all elements in front are less than or equal to
  // this element, and all elements that follow are greater than or equal to it.
  //
  // Thus, you get two subsequences separated by the element at position n,
  // whereby each element of the first subsequence is less than or equal to each
  // element of the second subsequence. This is helpful if you need only the set
  // of the n highest or lowest elements without having all the elements sorted.

  {
    vector<int> coll{3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5};
    nth_element(coll.begin(), coll.begin() + 3, coll.end());

    // 2 1 2 3 3 4 3 4 5 6 4 7 6 5 5 (15)
    PRINT_ELEMENTS(coll);
  }

  {
    vector<int> coll{3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5};
    nth_element(coll.begin(), coll.end() - 4, coll.end());

    // 4 3 4 3 2 2 3 1 5 5 4 5 6 6 7 (15)
    PRINT_ELEMENTS(coll);
  }
}

// ={=========================================================================
// algo-fill

TEST(AlgoFill, Fill)
{
  vector<int> coll;
  fill_n(back_inserter(coll), 8, 7);
  EXPECT_THAT(coll, ElementsAre(7, 7, 7, 7, 7, 7, 7, 7));

  fill(coll.begin(), coll.end(), 1);
  EXPECT_THAT(coll, ElementsAre(1, 1, 1, 1, 1, 1, 1, 1));

  vector<int> coll1(8, 0);
  fill_n(coll1.begin(), 8, 7);
  EXPECT_THAT(coll1, ElementsAre(7, 7, 7, 7, 7, 7, 7, 7));
}

// ={=========================================================================
// algo-heap also-sort

TEST(AlgoHeap, Calls)
{
  vector<int> coll{3, 4, 5, 6, 7, 5, 6, 7, 8, 9, 1, 2, 3, 4};

  // convert collection into a heap
  make_heap(coll.begin(), coll.end());
  EXPECT_THAT(coll,
              ElementsAreArray({9, 8, 6, 7, 7, 5, 5, 3, 6, 4, 1, 2, 3, 4}));

  // pop next element, root out of the heap
  pop_heap(coll.begin(), coll.end());
  coll.pop_back();
  EXPECT_THAT(coll, ElementsAreArray({8, 7, 6, 7, 4, 5, 5, 3, 6, 4, 1, 2, 3}));

  // push new element into the heap
  coll.push_back(17);
  push_heap(coll.begin(), coll.end());
  EXPECT_THAT(coll,
              ElementsAreArray({17, 7, 8, 7, 4, 5, 6, 3, 6, 4, 1, 2, 3, 5}));

  // convert heap into a sorted collection but note that after this, it is no
  // lonner a heap
  sort_heap(coll.begin(), coll.end());
  EXPECT_THAT(coll,
              ElementsAreArray({1, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 17}));
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

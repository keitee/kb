#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <deque>
#include <list>
#include <forward_list>
#include <unordered_set>
#include <queue>
#include <stack>
// #include <algorithm>
#include <numeric>        // std::accumulate for gcc 6
#include <limits>         // std::numeric_limits
#include <random>

#include "gmock/gmock.h"

using namespace std;
using namespace testing;

template <typename T>
void INSERT_ELEMENTS( T& coll, int first, int last )
{
    for (auto i = first; i <= last; i++)
        coll.insert( coll.end(), i );
}

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
TEST(Iterator, InvalidOnVector)
{
  std::vector<int> coll{1,2,3,4};

  auto it = find(coll.begin(), coll.end(), 2);
  if( it != coll.end())
  {
    it = coll.insert( it, 7 );
    it = coll.insert( it, 6 );
    it = coll.insert( it, 5 );

    // Q: This seems okay since inserting element at the same pos,2? what will
    // happen?
    //
    // veci.insert( it, 7 );
    // veci.insert( it, 6 );
    // veci.insert( it, 5 );
    //
    // core dump since it gets invalidated.
  }

  EXPECT_THAT(coll, ElementsAre(1,5,6,7,2,3,4));
}


// *cxx-undefined*
// may cause runtime error but not always so undefined. 
// Calling erase() for the element to which you are referring with it
// invalidates it as an iterator of coll and calling ++it results in
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

TEST(DISABLED_Iterator, InvalidOnMap)
{
  std::map<std::string, int> coll{
    {"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5}
  };

  int value = 2;

  for (auto it = coll.cbegin(); it != coll.cend(); ++it)
  {
    if (it->second == value)
      coll.erase(it);
  }
}

TEST(Iterator, ValidOnMapBeforeC11)
{
  // before C++11
  {
    std::map<std::string, int> coll{
      {"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5}
    };

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
    std::map<std::string, int> coll{
      {"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5}
    };

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


// https://stackoverflow.com/questions/37280744/got-singular-iterator-error-in-looping-with-iterator-and-pop-back

TEST(Iterator, InvalidOnDeque_Okays)
{
  // why this work?
  {
    deque<int> coll{1,2,3};

    for (auto it = coll.rbegin(); it != coll.rend(); ++it)
    {
      cout << "it : " << *it << endl;
      coll.pop_back();
    }
  }
  // why this work?
  {
    deque<int> coll{1,2,3};

    for (auto it = coll.rbegin(); it != coll.rend();)
    {
      cout << "it : " << *it << endl;
      coll.pop_back();
      ++it;
    }
  }

  {
    deque<int> coll{1,2,3};

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
//       type = __gnu_debug::_Safe_iterator<std::__cxx1998::_Deque_iterator<int, int&, int*>, std::__debug::deque<int, std::allocator<int> > > (mutable iterator);
//       state = singular;
//     }
//     iterator "other" @ 0x0x7ffde23ae1f0 {
//       type = __gnu_debug::_Safe_iterator<std::__cxx1998::_Deque_iterator<int, int&, int*>, std::__debug::deque<int, std::allocator<int> > > (mutable iterator);
//       state = singular;
//       references sequence with type 'std::__debug::deque<int, std::allocator<int> >' @ 0x0x7ffde23ae230
//     }
// Aborted

TEST(DISABLED_Iterator, InvalidOnDeque_Errors)
{
  {
    deque<int> coll{1,2,3};

    for (auto it = coll.rbegin(); it != coll.rend();)
    {
      it = coll.rbegin();
      cout << "it : " << *it << endl;
      coll.pop_back();
    }
  }
  {
    deque<int> coll{1,2,3};

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

  StructValue(int value): value_(value) {}

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

  // cxx.cpp: In member function ‘virtual void Temporary_NativeAndStruct_Test::TestBody()’:
  // cxx.cpp:1539:45: error: lvalue required as increment operand
  //    cout << "return int: " << ++ReturnInteger() << endl;
  // cout << "return int: " << ++ReturnInteger() << endl;
  
  EXPECT_THAT(++ReturnStruct(), 3302);
}

TEST(Iterator, OperationOnTemporary)
{
  {
    vector<int> coll{4,2,6,3,1,5};
    sort(++coll.begin(), coll.end());
    EXPECT_THAT(coll, ElementsAre(4,1,2,3,5,6));
  }

  {
    string coll{"this is a string object"};
    sort(++coll.begin(), coll.end());
    EXPECT_THAT(coll, Eq("t    abceghiiijnorssstt"));
  }
}


TEST(Iterator, Next)
{
  {
    vector<int> coll{1,2,3,4,5};
    auto pos = coll.begin();
    EXPECT_EQ(*pos, 1);

    ++pos;
    EXPECT_EQ(*pos, 2);

    // since next() uses copy, do not change input iterator

    auto next_pos = next(pos);
    EXPECT_EQ(*pos, 2);
    EXPECT_EQ(*next_pos, 3);
  }

  // 9.3.2 next() and prev()
  // Note that next() does not check whether it crosses the end() of a sequence.
  // Thus, it is up to the caller to ensure that the result is valid.
  {
    vector<int> coll{1,2,3,4,5};
    vector<int> result{};

    for(auto pos = coll.begin(); pos != coll.end(); pos = next(pos))
      result.push_back(*pos);

    EXPECT_THAT(coll, result);
  }

  // what would happen when call next() on empty? cxx-undefined
  //
  // /usr/include/c++/4.9/debug/safe_iterator.h:356:error: attempt to advance a
  //     past-the-end iterator 1 steps, which falls outside its valid range.

  {
    vector<int> coll{};
    auto pos = coll.begin();
    auto result = next(pos);
  }
}


TEST(Iterator, Distance)
{
  // cxx-distance which returns positive/negative
  {
    vector<int> coll{1,2,3,4,5};
    auto pos = find(coll.begin(), coll.end(), 3);
    EXPECT_EQ(distance(coll.begin(), pos), 2);
    EXPECT_EQ(distance(pos, coll.begin()), -2);

    EXPECT_EQ(distance(coll.begin(), coll.end()), 5);
    EXPECT_EQ(distance(coll.end(), coll.begin()), -5);
  }

  {
    set<int> coll{1,2,3,4,5};
    auto pos = find(coll.begin(), coll.end(), 3);
    EXPECT_EQ(distance(coll.begin(), pos), 2);

    // /usr/include/c++/4.9/debug/safe_iterator.h:289:error: attempt to increment
    //     a past-the-end iterator.
    // 
    // Objects involved in the operation:
    // iterator "this" @ 0x0x7fff1a6a0580 {
    // type = N11__gnu_debug14_Safe_iteratorISt23_Rb_tree_const_iteratorIiENSt7__debug3setIiSt4lessIiESaIiEEEEE (mutable iterator);
    //   state = past-the-end;
    //   references sequence with type `NSt7__debug3setIiSt4lessIiESaIiEEE' @ 0x0x7fff1a6a0610
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

    EXPECT_EQ(distance(coll.begin(), coll.end()), 5);
    // EXPECT_EQ(distance(coll.end(), coll.begin()), -5);
  }
}


TEST(Iterator, Array)
{
  int vals[] = {33, 67, -4, 13, 5, 2};

  std::vector<int> coll(std::begin(vals), std::end(vals));
  decltype(coll) out;

  // These functions are also overloaded, so you can use STL containers or all
  // classes that provide begin() and end() as member functions:
  // 
  // std::begin(v)  // yields v.begin()
  // std::end(v)    // yields v.end()

  std::copy(std::begin(coll), std::end(coll), back_inserter(out));
  EXPECT_THAT(out, ElementsAre(33, 67, -4, 13, 5, 2));
}

TEST(Iterator, Adapters)
{
  list<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};

  // cxx-iter-back-inserter
  {
    vector<int> ocoll;
    std::copy(coll.cbegin(), coll.cend(),
        back_inserter(ocoll));
    EXPECT_THAT(ocoll, ElementsAreArray({1,2,3,4,5,6,7,8,9}));
  }

  // *cxx-reverse* 
  // cxx-iter-front-inserter
  // have to use cxx-deque since it uses push_front
  {
    deque<int> ocoll;
    std::copy(coll.cbegin(), coll.cend(),
        front_inserter(ocoll));
    EXPECT_THAT(ocoll, ElementsAreArray({9,8,7,6,5,4,3,2,1}));
  }

  // *cxx-iter-inserter*
  {
    vector<int> ocoll;
    std::copy(coll.cbegin(), coll.cend(),
        inserter(ocoll, ocoll.begin()));
    EXPECT_THAT(ocoll, ElementsAreArray({1,2,3,4,5,6,7,8,9}));
  }
}

// shows that inserter() uses insert_iterator internally and that how it works.

TEST(Iterator, InsertIterator)
{
  {
    list<int> coll;

    insert_iterator<list<int>> iter(coll, coll.begin());

    // use usual iterator interface which has the same result
    // no ++? since this is becuase operator=() do that as above
    //
    // *iter = 1; iter++;
    // *iter = 2; iter++;
    // *iter = 3; iter++;

    *iter = 1; *iter = 2; *iter = 3; *iter = 44; *iter = 55;
    EXPECT_THAT(coll, ElementsAre(1,2,3,44,55));
  }

  {
    list<int> coll;

    coll.push_back(1); coll.push_back(2); coll.push_back(3);
    coll.push_back(44); coll.push_back(55);

    EXPECT_THAT(coll, ElementsAre(1,2,3,44,55));
  }

  {
    list<int> coll{1,2,3,44,55};
    list<int> ocoll{};

    copy(coll.begin(), coll.end(), inserter(ocoll, ocoll.begin()));

    EXPECT_THAT(ocoll, ElementsAre(1,2,3,44,55));
  }
}

TEST(Iterator, ReverseIterator)
{
  {
    string coll{"PARK"};
    string result;

    string::const_reverse_iterator it = coll.crbegin();

    while (it != coll.crend())
      result.push_back(*it++);

    EXPECT_THAT(result, "KRAP");
  }

  {
    string coll{"FIRST,MIDDLE,LAST"};

    auto delim = find(coll.cbegin(), coll.cend(), ',');
    EXPECT_THAT(string(coll.cbegin(), delim), "FIRST");

    // The result is "TSAL" since iterator goes backward and means [crbegin(),
    // rcomma). To get the expected result, shall use instead [rcomma.base(),
    // cend() ) which converts reverse iterator to normal interator.

    auto rdelim = find(coll.crbegin(), coll.crend(), ',');
    EXPECT_THAT(string(coll.crbegin(), rdelim), "TSAL");

    EXPECT_THAT(string(rdelim.base(), coll.cend()), "LAST");
  }

  // reverse_iterator operator*() do --
  {
    // create deque with elements from 1 to 9
    deque<int> coll = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    deque<int>::const_iterator pos;
    pos = find(coll.cbegin(), coll.cend(), 7);
    EXPECT_THAT(*pos, 7);
    
    deque<int>::const_reverse_iterator rpos(pos);
    EXPECT_THAT(*rpos, 6);
  }

  {
    ostringstream os;

    // create deque with elements from 1 to 9
    deque<int> coll = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // find range [2,7]
    deque<int>::const_iterator pos1;
    pos1 = find(coll.cbegin(), coll.cend(), 2);

    deque<int>::const_iterator pos2;
    pos2 = find(coll.cbegin(), coll.cend(), 7);

    // [2,6]
    for_each(pos1, pos2, [&](int e)
        { os << e << ","; }
        );

    EXPECT_THAT(os.str(), "2,3,4,5,6,");

    deque<int>::const_reverse_iterator rpos1(pos1);
    deque<int>::const_reverse_iterator rpos2(pos2);

    os.str("");

    // *cxx-error* cause runtime error -D_GLIBCXX_DEBUG
    // for_each(rpos1, rpos2, [&](int e)
    //
    // since rpos1 that point 2 and rpos1++ means pos1-- and goes below begin()

    for_each(rpos2, rpos1, [&](int e)
        { os << e << ","; }
        );

    EXPECT_THAT(os.str(), "6,5,4,3,2,");
  }
}

TEST(Iterator, StreamIterator)
{
  // as gets from std::cin, use stream
  {
    istringstream is{"1 2 3 4 5 6"};
    vector<int> coll;

    int value{};

    while (is >> value)
      coll.push_back(value);

    EXPECT_THAT(coll, ElementsAre(1,2,3,4,5,6));
  }

  // as gets from std::cin, use iterator
  {
    istringstream is{"1 2 3 4 5 6"};
    vector<int> coll;

    istream_iterator<int> isi(is), eof;

    while (isi != eof)
      coll.push_back(*isi++);

    EXPECT_THAT(coll, ElementsAre(1,2,3,4,5,6));
  }

  // use iterator but direct
  {
    istringstream is{"1 2 3 4 5 6"};

    istream_iterator<int> isi(is), eof;

    // *cxx-vector-ctor*
    // no loop and no isi++
    vector<int> coll(isi, eof);

    EXPECT_THAT(coll, ElementsAre(1,2,3,4,5,6));
  }

  // use iterator but direct
  {
    istringstream is{"1 2 3 4 5 6"};

    vector<int> coll((istream_iterator<int>(is)), istream_iterator<int>());

    // *cxx-error* without additional (). WHY?
    // vector<int> coll(istream_iterator<int>(is), istream_iterator<int>());

    EXPECT_THAT(coll, ElementsAre(1,2,3,4,5,6));
  }

  // use iterator but direct
  {
    istringstream is{"1 2 3 4 5 6"};

    istream_iterator<int> isi(is), eof;

    // no loop and no isi++
    vector<int> coll;
    std::copy(isi, eof, back_inserter(coll));

    EXPECT_THAT(coll, ElementsAre(1,2,3,4,5,6));
  }

  // as gets from std::cin, use stream
  {
    // istream_iterator<int> isi(cin), eof;

    istringstream is{"1 2 3 4 5 6"};
    istream_iterator<int> isi(is), eof;

    auto result = std::accumulate(isi, eof, 0);

    EXPECT_THAT(result, 21);
  }

  // ostream_iterator as std::cout
  {
    vector<int> coll{1,2,3,4,5,6};
    ostringstream os;
    ostream_iterator<int> osi(os, ",");

    for (auto e : coll)
      *osi++ = e;

    EXPECT_THAT(os.str(), "1,2,3,4,5,6,");
  }

  // With/without "++oo", the result is the same. The ++oo is done to mimic
  // writing into an array through a pointer.

  {
    ostringstream os;
    ostream_iterator<string> osi(os);

    *osi = "Hello, ";
    ++osi;
    *osi = "world!";

    EXPECT_THAT(os.str(), "Hello, world!");
  }
  {
    ostringstream os;
    ostream_iterator<string> osi(os);

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

TEST(Vector, Capacity)
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
  sentence.insert(sentence.end(),{"how","are","you","?"});

  // print elements separated with spaces
  copy (sentence.cbegin(), sentence.cend(),
      ostream_iterator<string>(cout," "));
  cout << endl;

  // print ‘‘technical data’’
  cout << " max_size(): " << sentence.max_size() << endl;
  cout << " size()    : " << sentence.size() << endl;
  cout << " capacity(): " << sentence.capacity() << endl;

  // swap second and fourth element
  swap (sentence[1], sentence[3]);
  // insert element "always" before element "?"
  sentence.insert (find(sentence.begin(),sentence.end(),"?"), "always");
  // assign "!" to the last element
  sentence.back() = "!";

  // print elements separated with spaces
  copy (sentence.cbegin(), sentence.cend(),
      ostream_iterator<string>(cout," "));
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
  VectorCtorsTest(int size, int value = 10) : icoll(size, value) {}
  int size() { return icoll.size(); }
  void print() { PRINT_ELEMENTS(icoll, "clsss : "); }

  private:
  vector<int> icoll;
};

TEST(Vector, Ctors)
{
  {
    vector<int> coll(5);
    ASSERT_THAT(coll.size(), Eq(5));
    EXPECT_THAT(coll, ElementsAre(0,0,0,0,0));
  }

  // *cxx-vector-resize*
  {
    vector<int> coll;
    coll.resize(5);
    ASSERT_THAT(coll.size(), Eq(5));
    EXPECT_THAT(coll, ElementsAre(0,0,0,0,0));
  }

  {
    vector<int> coll(5, 10);
    ASSERT_THAT(coll.size(), Eq(5));
    EXPECT_THAT(coll, ElementsAre(10,10,10,10,10));
  }

  // *cxx-vector-resize*
  {
    vector<int> coll;
    coll.resize(5, 10);
    ASSERT_THAT(coll.size(), Eq(5));
    EXPECT_THAT(coll, ElementsAre(10,10,10,10,10));
  }

  {
    VectorCtorsTest coll(10, 100);
    // icoll3.print();
    ASSERT_THAT(coll.size(), Eq(10));
  }

  // initializer_list
  {
    vector<int> coll{1,2,3,4,5,6};
    EXPECT_THAT(coll, ElementsAre(1,2,3,4,5,6));
  }
  {
    vector<int> coll{1};
    EXPECT_THAT(coll, ElementsAre(1));
  }
  {
    vector<int> coll{2};
    EXPECT_THAT(coll, ElementsAre(2));
  }

  // iterator
  {
    istringstream is{"1 2 3 4 5 6"};

    vector<int> coll((istream_iterator<int>(is)), istream_iterator<int>());

    // *cxx-error* without additional (). WHY?
    // vector<int> coll(istream_iterator<int>(is), istream_iterator<int>());

    EXPECT_THAT(coll, ElementsAre(1,2,3,4,5,6));
  }
}


void StlVectorFillVector(vector<int> &coll)
{
  for (int i = 0; i < 20; ++i)
    coll.insert(coll.end(), i);
}

TEST(Vector, CtorFromExpression)
{
  vector<int> coll;
  StlVectorFillVector(coll);

  // okay as well
  // int value;
  // cin >> value;
  // vector<bool> table1(value);

  vector<bool> table1(coll.size());
  vector<bool> table2(10);

  int iarray[coll.size()];

  EXPECT_THAT(table1.size(), 20);
  EXPECT_THAT(table2.size(), 10);
  EXPECT_THAT(sizeof(iarray)/sizeof(iarray[0]), 20);
}


void GetVectorArg(const vector<int> &coll)
{
  vector<int> coll_;

  coll_ = coll;

  ASSERT_THAT(coll_.size(), 6);
}

TEST(Vector, CopyAndMoveAssign)
{
  // assign
  {
    vector<int> coll1{1,2,3,4,5,6};
    vector<int> coll2{};

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 0);

    coll2 = coll1;

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 6);
  }

  // assign
  {
    vector<int> coll1{1,2,3,4,5,6};
    vector<int> coll2{};

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 0);

    coll2 = {1,2,3,4,5,6};

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 6);
  }

  // move
  {
    vector<int> coll1{1,2,3,4,5,6};
    vector<int> coll2{};

    ASSERT_THAT(coll1.size(), 6);
    ASSERT_THAT(coll2.size(), 0);

    coll2 = std::move(coll1);

    ASSERT_THAT(coll1.size(), 0);
    ASSERT_THAT(coll2.size(), 6);
  }

  // assign
  {
    vector<int> coll1{1,2,3,4,5,6};
    GetVectorArg(coll1);
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
//       type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<int const*, std::__cxx1998::vector<int, std::allocator<int> > >, std::__debug::vector<int, std::allocator<int> > > (constant iterator);
//       state = singular;
//     }
//     iterator "other" @ 0x0x7ffcc61e97f0 {
//       type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<int*, std::__cxx1998::vector<int, std::allocator<int> > >, std::__debug::vector<int, std::allocator<int> > > (mutable iterator);
//       state = singular;
//       references sequence with type 'std::__debug::vector<int, std::allocator<int> >' @ 0x0x7ffcc61e9820
//     }
// Aborted

TEST(DISABLED_Vector, EraseRuntimeError)
{
  vector<int> coll1;
  INSERT_ELEMENTS(coll1, 0, 8);
  EXPECT_THAT(coll1, ElementsAre(0,1,2,3,4,5,6,7,8));

  auto it = coll1.begin()+1;

  // note: it is not valid after this
  coll1.erase(it);
  EXPECT_THAT(coll1, ElementsAre(0,2,3,4,5,6,7,8));

  coll1.erase(it);
  EXPECT_THAT(coll1, ElementsAre(0,3,4,5,6,7,8));

  vector<int> coll2{0, 1, 2, 3, 4, 5, 6, 7, 8};

  // in every iteration, update it which is invalidated after insert/erase.
  for(auto it = coll2.begin(); it != coll2.end(); /* no */)
  {
    // if see even values, remove it
    if(!(*it % 2))
      it = coll2.erase(it);
    else
      ++it;
  }

  EXPECT_THAT(coll2, ElementsAre(1,3,5,7));
}

TEST(Vector, EraseNoError)
{
  vector<int> coll1;
  INSERT_ELEMENTS(coll1, 0, 8);
  EXPECT_THAT(coll1, ElementsAre(0,1,2,3,4,5,6,7,8));

  auto it = coll1.begin()+1;

  it = coll1.erase(it);
  EXPECT_THAT(coll1, ElementsAre(0,2,3,4,5,6,7,8));

  coll1.erase(it);
  EXPECT_THAT(coll1, ElementsAre(0,3,4,5,6,7,8));

  vector<int> coll2{0, 1, 2, 3, 4, 5, 6, 7, 8};

  // in every iteration, update it which is invalidated after insert/erase.
  for(auto it = coll2.begin(); it != coll2.end(); /* no */)
  {
    // if see even values, remove it
    if(!(*it % 2))
      it = coll2.erase(it);
    else
      ++it;
  }

  EXPECT_THAT(coll2, ElementsAre(1,3,5,7));
}


TEST(Vector, InsertAndErase)
{
  vector<int> coll{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  EXPECT_THAT(coll, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));

  // in every iteration, update it which is invalidated after insert/erase.
  for(auto it = coll.begin(); it != coll.end(); /* no */)
  {
    // if see odd values, repeat it in front of it.
    if(*it % 2)
    {
      it = coll.insert(it, *it);
      it += 2;
    }
    else
      it = coll.erase(it);
  }

  EXPECT_THAT(coll, ElementsAre(1, 1, 3, 3, 5, 5, 7, 7, 9, 9));
}

class VectorEraseCallsDtor
{
  public:
    VectorEraseCallsDtor(const string name="vector"): name_(name) 
      { cout << "VectorEraseCallsDtor::ctor: " << name_ << endl; }
    ~VectorEraseCallsDtor() 
      { cout << "VectorEraseCallsDtor::dtor: " << name_ << endl; }

    string GetName() { return name_; }

  private:
    string name_;
};

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

TEST(Vector, CreateWithOnDemand)
{
  vector<VectorEraseCallsDtor> ovec{};

  for(int i = 0; i < 5; ++i)
  {
    cout << "-for ---------" << endl;
    string name = "name " + to_string(i);
    ovec.push_back(VectorEraseCallsDtor(name));
  }

  for (auto &e : ovec)
    cout << "for: " << e.GetName() << endl;

  cout << "-erase---------" << endl;
  auto it = ovec.begin();
  it = ovec.erase(it);

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

TEST(Vector, CreateWithReserve)
{
  vector<VectorEraseCallsDtor> ovec{};
  ovec.reserve(10);

  for(int i = 0; i < 5; ++i)
  {
    cout << "-for ---------" << endl;
    string name = "name " + to_string(i);
    ovec.push_back(VectorEraseCallsDtor(name));
  }

  cout << "-erase---------" << endl;
  auto it = ovec.begin();
  it = ovec.erase(it);

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

TEST(Vector, CreateWithPreAllocation)
{
  vector<VectorEraseCallsDtor> ovec(10);

  cout << "-erase---------" << endl;
  auto it = ovec.begin();
  it = ovec.erase(it);

  cout << "-erase---------" << endl;
  it = ovec.erase(it);

  cout << "-erase---------" << endl;
  it = ovec.erase(it);
}


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
  vector<VectorEraseCallsDtor> ovec{};

  for(int i = 0; i < 5; ++i)
  {
    string name = "name " + to_string(i);
    ovec.push_back(VectorEraseCallsDtor(name));
  }

  cout << "name: " << ovec[8].GetName() << endl;
}

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
  vector<VectorEraseCallsDtor> ovec{};
  ovec.reserve(10);

  for(int i = 0; i < 5; ++i)
  {
    string name = "name " + to_string(i);
    ovec.push_back(VectorEraseCallsDtor(name));
  }

  cout << "name: " << ovec[8].GetName() << endl;
}


TEST(Vector, AsCArray)
{
  // {
  //   vector<int> coll{1,2,3,4,5};
  //   cout << "coll: " << coll[-1] << endl;
  // }

  {
    vector<char> coll;
    coll.resize(41);

    // stl.cpp:943:32: error: invalid conversion from
    // ‘__gnu_cxx::__alloc_traits<std::allocator<char> >::value_type {aka char}’
    // to ‘char*’ [-fpermissive]
    //
    //    strcpy(coll[0], "hello world");

    strcpy(&coll[0], "hello world");
    printf("%s\n", &coll[0]);
  }

  {
    vector<char> coll;
    coll.resize(41);
    strcpy(coll.data(), "hello world");
    printf("%s\n", coll.data());
  }
}


// ={=========================================================================
// cxx-array

// 7.2.2 Array Operations
// 
// Create, Copy, and Destroy
// 
// Table 7.4 lists the constructors and destructors for arrays. Because class
// array<> is an aggregate, these constructors are only implicitly defined. You can
// create arrays with and without elements for initialization. The default
// constructor default initializes the elements, which means that the value of
// fundamental types is undefined.

TEST(Array, ArrayCtors)
{
  {
    array<int, 8> coll = {11,22,33};
    coll.back() = 999;
    coll[coll.size()-2] = 42;

    EXPECT_THAT(coll, ElementsAre(11,22,33,0,0,0,42,999));
  }

  {
    array<int, 3> coll{1,2,3};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3));
  }
}

// initialized: t.h.i.s. .i.s. .a.n. .c.h.a.r. .a.r.r.a.y.....................(41)
// strcpyed   : u.s.e. .t.h.e. .a.d.d.r.e.s.s. .o.f. .f.i.r.s.t..................(41)
// strcpyed   : u.s.e. .d.a.t.a. .m.e.m.b.e.r..o.f. .f.i.r.s.t..................(41)

TEST(Array, ArrayAccess)
{
  // *TN* 
  // array size is 41 and the initializer is less than this. does array handle
  // this?
  //
  // array<char,50> carr{"ABCDEDFHIJKLMN"};
  // PRINT_ELEMENTS(carr, "char array: ");
  //
  // changed PRINT_ELEMENTS() to print the # of iteration in a loop and see
  // blank outputs
  // ========
  // char array: A B C D E D F H I J K L M N                                     (50)

  array<char, 41> coll = {"this is an char array"};
  PRINT_ELEMENTS(coll, "initialized: " );

  strcpy( &coll[0], "use the address of first");
  PRINT_ELEMENTS(coll, "strcpyed   : " );

  strcpy( coll.data(), "use data member");
  PRINT_ELEMENTS(coll, "strcpyed   : " );
}

TEST(Array, AsCArray)
{
  {
    array<char, 41> coll;

    // stl.cpp:943:32: error: invalid conversion from
    // ‘__gnu_cxx::__alloc_traits<std::allocator<char> >::value_type {aka char}’
    // to ‘char*’ [-fpermissive]
    //
    //    strcpy(coll[0], "hello world");

    strcpy(&coll[0], "hello world");
    printf("%s\n", &coll[0]);
  }

  {
    array<char, 41> coll;

    strcpy(coll.data(), "hello world");
    printf("%s\n", coll.data());
  }
}


// ={=========================================================================
// cxx-multi-dimensitional

TEST(StlMultiArray, UseVectorAndArray)
{
  const size_t rows = 5;
  const size_t cols = 5;

  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0

  vector<vector<int>> coll_vector(rows, vector<int>(cols, 0)); 

  cout << "{" << endl;
  for (size_t i = 0; i < rows; ++i)
  {
    for(size_t j = 0; j < cols; ++j)
      cout << coll_vector[i][j] << " ";
    cout << endl;
  }
  cout << "}" << endl;

  // 0 0 0 0 0
  // 1 1 1 1 0
  // 0 0 0 0 0
  // 0 1 1 1 1
  // 2 0 0 0 0

  std::vector<std::vector<int>> coll_vector_5{
    {0,0,0,0,0},
    {1,1,1,1,0},
    {0,0,0,0,0},
    {0,1,1,1,1},
    {2,0,0,0,0}
  };

  cout << "{" << endl;
  for (size_t i = 0; i < rows; ++i)
  {
    for(size_t j = 0; j < cols; ++j)
      cout << coll_vector_5[i][j] << " ";
    cout << endl;
  }
  cout << "}" << endl;


  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0
  // 0 0 0 0 0

  array<array<int, cols>, rows> coll_array{0}; 

  cout << "{" << endl;
  for (size_t i = 0; i < rows; ++i)
  {
    for(size_t j = 0; j < cols; ++j)
      cout << coll_array[i][j] << " ";
    cout << endl;
  }
  cout << "}" << endl;

  // 0 0 0 0 0
  // 1 1 1 1 0
  // 0 0 0 0 0
  // 0 1 1 1 1
  // 2 0 0 0 0

  std::array<std::array<int, 5>, 5> coll_array_5 = {
    0,0,0,0,0,
    1,1,1,1,0,
    0,0,0,0,0,
    0,1,1,1,1,
    2,0,0,0,0
  };

  cout << "{" << endl;
  for (size_t i = 0; i < rows; ++i)
  {
    for(size_t j = 0; j < cols; ++j)
      cout << coll_array_5[i][j] << " ";
    cout << endl;
  }
  cout << "}" << endl;
}


// ={=========================================================================
// cxx-deque
// case seg-fault

TEST(DISABLED_StlDeque, HowDequeSupportEmpty)
{
    deque<int> iq;

    try {
        auto e = iq.back();
        (void) e;
    }
    catch(...)
    {
        cout << "exception" << endl;
    }
}


// ={=========================================================================
// cxx-queue-priority

TEST(Queue, Ops)
{
  {
    ostringstream os;
    std::queue<std::string> q;

    q.push("These "); q.push("are "); q.push("more than ");
    os << q.front(); q.pop();
    os << q.front(); q.pop();

    q.push("four "); q.push("words!");

    // discard one element
    q.pop();

    os << q.front(); q.pop();
    os << q.front(); q.pop();

    EXPECT_THAT(os.str(), "These are four words!");
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
}


TEST(Queue, Priority)
{
  {
    queue<int> pq;

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

    // since queue do not support begin()/end(), cannot use ElementsAre and copy
    // to transform it to a vector. Did it manually.
    // copy(pq.begin(), pq.end(), back_inserter(coll));

    vector<int> coll;

    while (!pq.empty())
    {
      coll.push_back(pq.front());
      pq.pop();
    }

    EXPECT_THAT(coll, ElementsAre(11, 55, 33));
  }

  {
    priority_queue<int> pq;

    pq.push(22);
    pq.push(66);
    pq.push(44);

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

    vector<int> coll;

    while (!pq.empty())
    {
      coll.push_back(pq.top());
      pq.pop();
    }

    EXPECT_THAT(coll, ElementsAre(33, 22, 11));
  }
}


// ={=========================================================================
// cxx-set

TEST(Set, SortOrder)
{
  {
    // less <
    set<int> coll{13, 9, 7, 10, 2, 11, 12, 8, 7};
    EXPECT_THAT(coll, ElementsAre(2, 7, 8, 9, 10, 11, 12, 13));
  }

  {
    // greater >
    set<int, greater<int>> coll{13, 9, 7, 10, 2, 11, 12, 8, 7};
    EXPECT_THAT(coll, ElementsAre(13, 12, 11, 10, 9, 8, 7, 2));

    // duplicate, 7, is removed
    EXPECT_THAT(coll.size(), 8);

    auto begin = coll.begin();
    coll.erase(begin);

    begin = coll.begin();
    coll.erase(begin);

    EXPECT_THAT(coll, ElementsAre(11, 10, 9, 8, 7, 2));
    EXPECT_THAT(coll.size(), 6);
  }
}

TEST(Set, Search)
{
  {
    set<int> coll;

    coll.insert(1);
    coll.insert(2);
    coll.insert(4);
    coll.insert(5);
    coll.insert(6);

    EXPECT_THAT(coll, ElementsAre(1, 2, 4, 5, 6));

    EXPECT_THAT(*coll.lower_bound(3), 4);

    // Returns an iterator to the first element with key greater than val.
    EXPECT_THAT(*coll.upper_bound(3), 4);

    EXPECT_THAT(*coll.equal_range(3).first, 4);
    EXPECT_THAT(*coll.equal_range(3).second, 4);


    EXPECT_THAT(*coll.lower_bound(5), 5);

    // Returns an iterator to the first element with key greater than val.
    EXPECT_THAT(*coll.upper_bound(5), 6);

    EXPECT_THAT(*coll.equal_range(5).first, 5);
    EXPECT_THAT(*coll.equal_range(5).second, 6);
  }

  {
    multiset<int> coll;

    coll.insert(1);
    coll.insert(2);
    coll.insert(3);   // 2
    coll.insert(3);
    coll.insert(3);   // 4
    coll.insert(4);   // 5
    coll.insert(5);
    coll.insert(6);

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 3, 3, 4, 5, 6));

    EXPECT_THAT(distance(coll.begin(), coll.lower_bound(3)), 2);

    // Returns an iterator to the first element with key greater than val.
    EXPECT_THAT(distance(coll.begin(), coll.upper_bound(3)), 5);

    EXPECT_THAT(distance(coll.begin(), coll.equal_range(3).first), 2);
    EXPECT_THAT(distance(coll.begin(), coll.equal_range(3).second), 5);
  }
}

TEST(SetMulti, Duplicate)
{
  {
    multiset<int, greater<int>> coll{13, 9, 7, 10, 2, 11, 12, 8, 7};
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


TEST(Set, Erase)
{
  // if there is match at the beginning, removes them
  {
    multiset<int> coll{2, 2, 3, 3, 3, 3, 3, 3, 3, 6, 7};
    EXPECT_THAT(coll, ElementsAre(2, 2, 3, 3, 3, 3, 3, 3, 3, 6, 7));

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
    EXPECT_THAT(coll, ElementsAre(2, 2, 3, 3, 3, 3, 3, 3, 3, 6, 7));

    auto it = coll.begin();
    while ((it != coll.end()) && (*it == 3))
    {
      it = coll.erase(it);
    }

    // same as before
    EXPECT_THAT(coll, ElementsAre(2, 2, 3, 3, 3, 3, 3, 3, 3, 6, 7));
  }
}


// ={=========================================================================
// cxx-map

TEST(Map, Insert)
{
  // cannot be a const map since operator[] is for non-const.

  map<unsigned int, string> coll{ 
    {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}
  };

  coll[3] = "threee";
  coll[3] = "threeee";
  coll[3] = "threeeee";
  coll[3] = "threeeeee";

  ASSERT_THAT(coll[3], Eq("threeeeee"));

  coll.insert({3, "third"});
  coll.insert({3, "thirdd"});
  coll.insert({3, "thirddd"});
  coll.insert({3, "thirdddd"});

  // not changed since cxx-set and cxx-map do not allow duplicates and nothing
  // happens when key is already exist

  ASSERT_THAT(coll[3], Eq("threeeeee"));
}

namespace cxx_map
{
  struct SampleEntry
  {
    int id;
    std::string name;
  };

  void print_name(std::map<unsigned int, SampleEntry> const &coll)
  {
    ASSERT_THAT(coll.size(), 3);
    // : error: passing ‘const std::map<unsigned int, cxx_map::SampleEntry>’ as ‘this’ argument discards qualifiers [-fpermissive]
    //      std::cout << "name: " << coll[1].name << std::endl;
    //                                     ^
    // std::cout << "name: " << coll[1].name << std::endl;

    // okay as: bits/stl_map.h
    // const mapped_type&
    // at(const key_type& __k) const
    // {}
    //
    // so operator[] of map do not support const

    std::cout << "name: " << coll.at(1).name << std::endl;
  }
} // namespace

TEST(Map, FindAndAccess)
{
  {
    map<float,float> coll{ {1,7}, {2,4}, {3,2}, {4,3}, {5,6}, {6,1}, {7,3} };

    // *cxx-error*
    // when tries to use custom matcher, get's link error
    // ASSERT_THAT(posKey, EqPair(make_pair(3,2)));

    auto posKey = coll.find(3.0);
    EXPECT_THAT(*posKey, make_pair(3,2));

    // *algo-find-if-const* error if there is no const on predicate. 
    // since it's *cxx-algo-non-modifying* ?

    // *cxx-decltype*
    auto posVal = find_if( coll.cbegin(), coll.cend(),
        // [] ( const pair<float,float> &elem ) 
        // [] ( const map<float,float>::value_type &elem )
        [] ( const decltype(coll)::value_type &elem ) {
        return elem.second == 3.0;
        } );
    EXPECT_THAT(posVal->first, Eq(4));
    EXPECT_THAT(posVal->second, Eq(3));
  }

  {
    map<float,float> coll{ {1,7}, {2,4}, {3,2}, {4,3}, {5,6}, {6,1}, {7,3} };

    auto value = coll.at(3.0);
    EXPECT_THAT(value, 2);

    auto it = coll.find(3.0);
    EXPECT_THAT(it->first, 3);
    EXPECT_THAT(it->second, 2);
  }

  {
    using namespace cxx_map;

    std::map<unsigned int, SampleEntry> coll{};

    coll.insert({0, {100, "string1"}});
    coll.insert({1, {200, "string2"}});
    coll.insert({2, {300, "string3"}});

    ASSERT_THAT(coll.size(), 3);

    EXPECT_THAT(coll[1].name, "string2");
    coll[1].name = "changed2";
    EXPECT_THAT(coll[1].name, "changed2");

    print_name(coll);

    // *cxx-error* compile error
    // auto it = coll.find(1);
    // it->name;
  }
}

TEST(Map, EqualRange)
{
  std::string str = "total";

  std::multimap<std::string, std::string> authors;

  // *cxx-map-insert*
  authors.insert( {"Kit, Park", "How to get through"} );
  authors.insert( {"Barth, John", "Sot-Weed Factor"} );
  authors.insert( {"Barth, John", "Lost in the Funhouse"});
  authors.insert( {"Andy, Steve", "Enterprise"});
  authors.insert( {"Barth, John", "A way to success"});

  std::string search_item("Barth, John");

  auto entries = authors.count( search_item );  // num of elements
  EXPECT_THAT(entries, 3);

  // use iter
  {
    vector<string> result{};
    auto iter = authors.find(search_item);      // first entry

    while( entries ) {
      result.push_back(iter->second);
      ++iter; --entries;
    }

    EXPECT_THAT(result, 
        ElementsAre("Sot-Weed Factor", "Lost in the Funhouse", "A way to success"));
  }

  // use _bound() calls
  {
    vector<string> result{};

    for (auto begin = authors.lower_bound(search_item), end = authors.upper_bound(search_item);
        begin != end; ++begin)
    {
      result.push_back(begin->second);
    }

    EXPECT_THAT(result, 
        ElementsAre("Sot-Weed Factor", "Lost in the Funhouse", "A way to success"));
  }

  // use equal_range()
  // return pair of iter in range [first, off-the-end). Like above, if not found
  // return the same.
  {
    vector<string> result{};
    auto iter = authors.find(search_item);      // first entry

    for (auto rpos = authors.equal_range(iter->first);
        rpos.first != rpos.second; ++rpos.first)
    {
      result.push_back(rpos.first->second);
    }

    EXPECT_THAT(result, 
        ElementsAre("Sot-Weed Factor", "Lost in the Funhouse", "A way to success"));
  }
}


// ={=========================================================================
// cxx-unordered

TEST(Unordered, MapDuplicates)
{
  unordered_multiset<string> cities{"Braunschweig", "Hanover", "Frankfurt", 
    "New York", "Chicago", "Toronto", "Paris", "Frankfurt"};

  {
    vector<string> result{};

    for( const auto &elem : cities )
      result.push_back(elem);

    EXPECT_THAT(result, 
        ElementsAre("Paris", "Toronto", "Chicago", "New York", 
          "Frankfurt", "Frankfurt", "Hanover", "Braunschweig")); 
  }

  {
    // insert additional elements
    cities.insert({"London", "Munich", "Hanover", "Braunschweig"});

    vector<string> result{};

    for( const auto &elem : cities )
      result.push_back(elem);

    EXPECT_THAT(result, 
        ElementsAreArray({"Munich", "London", 
          "Frankfurt", "Frankfurt",         // duplicates
          "New York", 
          "Braunschweig", "Braunschweig",   // duplicates
          "Chicago", "Toronto", 
          "Hanover", "Hanover",             // duplicates
          "Paris"})); 
  }
}


// ={=========================================================================
// cxx-list

TEST(List, SpliceAndMerge)
{
  list<int> list_one, list_two;
 
  for(int i=0; i < 6; ++i)
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
  EXPECT_THAT(list_three, 
      ElementsAreArray({20,21,22,11,30,31,32}));
 
  list<int> list4{0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
  list<int> list5{0, 1, 2, 3, 4, 5};
 
  list5.merge(list4);
  EXPECT_THAT(list5, 
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


TEST(List, Ops)
{
  {
    std::list<int> coll1, coll2;

    for (int i = 0; i < 6; ++i)
    {
      coll1.push_back(i);
      coll2.push_front(i);
    }

    EXPECT_THAT(coll1, ElementsAre(0,1,2,3,4,5));
    EXPECT_THAT(coll1.size(), 6); 
    EXPECT_THAT(coll2, ElementsAre(5,4,3,2,1,0));
    EXPECT_THAT(coll2.size(), 6); 

    EXPECT_THAT(coll1.front(), 0);
    EXPECT_THAT(coll1.back(), 5);
    EXPECT_THAT(coll1.size(), 6); 

    // void pop_front() so cannot use EXPECT_THAT()
    coll1.pop_front();
    EXPECT_THAT(coll1.size(), 5); 

    coll1.pop_back();
    EXPECT_THAT(coll1.size(), 4); 

    EXPECT_THAT(coll1, ElementsAre(1,2,3,4));
  }

  // no such interface to assign.
  // {
  //   std::list<int> coll{0,1,2,3,4,5};
  //   EXPECT_THAT(coll, ElementsAre(0,1,2,3,4,5));
  //   coll.assign(++coll.begin(), 10);
  //   EXPECT_THAT(coll, ElementsAre(0,10,2,3,4,5));
  // }
}


TEST(List, RemoveIf)
{
  std::list<int> coll{0, 1, 2, 3, 4};

  // remove if e > 2

  coll.remove_if([](int e)
      { 
        if (e > 2) 
          return true;

        return false;
      });

  EXPECT_THAT(coll, ElementsAre(0, 1, 2));

  // remove if can be divided by 2

  coll.remove_if([](int e)
      { return e % 2 == 0; });

  EXPECT_THAT(coll, ElementsAre(1));
}


TEST(List, Erase)
{
  std::list<int> coll{10,20,30,40,50};

  auto iter = coll.begin();

  ++iter;
  ++iter;

  coll.erase(coll.begin(), iter);

  EXPECT_THAT(coll, ElementsAre(30,40,50));
}

TEST(List, SpliceAndDivide)
{
  std::list<int> coll{26, 33, 35, 29, 19, 12, 22};
  auto slow = coll.begin();
  auto fast = next(slow);

  for(;fast != coll.end();)
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

  EXPECT_THAT(coll1, ElementsAre(26,33,35,29));
  EXPECT_THAT(coll2, ElementsAre(19,12,22));
}

TEST(List, Sort)
{
  list<int> coll{26, 33, 35, 29, 19, 12, 22};
  coll.sort();
}

TEST(List, Find)
{
  std::list<int> coll1{6,5,4,3,2,1};
  std::vector<int> ret1{};

  std::list<int> coll2{1,2,3};
  std::vector<int> ret2{};

  // try to find coll1 elements which are not in coll2
  for (auto const e : coll1)
    if (std::find(coll2.cbegin(), coll2.cend(), e) == coll2.end())
      ret1.push_back(e);

  EXPECT_THAT(ret1, ElementsAre(6,5,4));

  // try to find coll2 elements which are not in coll1
  for (auto const e : coll2)
    if (std::find(coll1.cbegin(), coll1.cend(), e) == coll1.end())
      ret2.push_back(e);

  EXPECT_THAT(ret2.size(),0);
}


// ={=========================================================================
// cxx-list-forward

TEST(ForwardList, InsertAfter)
{
  {
    forward_list<int> coll{1,2,3};
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
  forward_list<int> coll{2,3,4,5,6,7};

  coll.resize(9);
  coll.resize(10, 99);

  EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,0,0,0,99));
}

namespace use_forward_list
{
  template<typename T>
    class ForwardList
    {
      public:
        typedef typename std::forward_list<T>::iterator iterator;

        ForwardList() : count_(0) {}

        size_t size() const { return count_; }
        void push_front(const T& elem)
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
}

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
 
  //iter = rcoll.begin();
  //for (iter; iter != rcoll.end(); ++iter)
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
  forward_list<int> coll1{1,2,3,4};
  forward_list<int> coll2{77,88,99};

  coll2.insert_after(coll2.before_begin(), 99);
  coll2.push_front(10);
  coll2.insert_after(coll2.before_begin(), {10,11,12,13});

  EXPECT_THAT(coll2, ElementsAre(10,11,12,13,10,99,77,88,99));

  coll1.insert_after(coll1.before_begin(),
      coll2.begin(), coll2.end());

  EXPECT_THAT(coll1, ElementsAreArray({10,11,12,13,10,99,77,88,99,1,2,3,4}));

  // delete 2nd element
  coll2.erase_after(coll2.begin());
  coll2.erase_after(find(coll2.begin(), coll2.end(), 99), coll2.end());

  EXPECT_THAT(coll2, ElementsAreArray({10,12,13,10,99}));

  coll1.sort();
  coll2 = coll1;
  coll2.unique();

  EXPECT_THAT(coll2, ElementsAreArray({1,2,3,4,10,11,12,13,77,88,99}));
}


// ={=========================================================================
// cxx-stack

TEST(Stack, BookExample)
{
  std::vector<int> coll{1,2,3,4,5,6,7,8,9,10,11,12};

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

  EXPECT_THAT(result2, ElementsAreArray({98,11,10,9,8,7,6,5,4,3,2,1}));
}


namespace cxx_stack
{
  class ReadEmptyStack : public std::exception
  {
    public:
      virtual const char* what() const noexcept
      { return "cannot read on empty stack"; }
  };

  template <typename T>
    class Stack
    {
      public:
        typename std::deque<T>::size_type size() const
        { return coll_.size(); }

        bool empty() const
        { return coll_.empty(); }

        void push(T const& elem)
        { coll_.push_back(elem); }

        T pop()
        {
          if (coll_.empty())
            throw ReadEmptyStack();

          T elem(coll_.back());
          coll_.pop_back();
          return elem;
        }

        T& top()
        {
          if (coll_.empty())
            throw ReadEmptyStack();

          return coll_.back();
        }

      private:
        std::deque<T> coll_;
    };

} // namespace

TEST(Stack, ImplementedInTermsOf)
{
  using namespace cxx_stack;

  Stack<int> coll;
  vector<int> result;

  coll.push(10);
  coll.push(20);
  coll.push(30);

  EXPECT_THAT(coll.pop(), 30);

  coll.top() = 80;

  coll.push(100);
  coll.push(200);

  while (!coll.empty())
  {
    result.push_back(coll.pop());
  }

  EXPECT_THAT(result, ElementsAre(200, 100, 80, 10));

  // stack is already empty

  EXPECT_THROW(coll.pop(), ReadEmptyStack);
}


// ={=========================================================================
// algo-predicate

namespace algo_predicate
{
  class PredicateWithState
  {
    public:
      PredicateWithState(int value):
        nth_(value), count_(0) {}

      // *cxx-unused*
      bool operator()(int)
      {
        return ++count_ == nth_;
      }

    private:
      int nth_;
      int count_;
  };

  class PredicateWithValue
  {
    public:
      void operator()(int value)
      {
        value += 10;
      }
  };

  class PredicateWithReference
  {
    public:
      void operator()(int& value)
      {
        value += 10;
      }
  };
} // namespace

TEST(Predicate, Stateless)
{
  using namespace algo_predicate;

  list<int> coll = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  
  auto pos = remove_if(coll.begin(), coll.end(),
      PredicateWithState(3));

  coll.erase(pos, coll.end());

  // removed 3 and 6. WHY?
  EXPECT_THAT(coll, ElementsAre(1,2,4,5,7,8,9,10));
}

TEST(Predicate, Predefined)
{
  std::plus<int> op;
  EXPECT_THAT(op(10, 20), 30);
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
    vector<int> coll{1,2,3,4,5};
    std::for_each(coll.begin(), coll.end(), PredicateWithValue());
    EXPECT_THAT(coll, ElementsAre(1,2,3,4,5));
  }

  {
    vector<int> coll{1,2,3,4,5};
    std::for_each(coll.begin(), coll.end(), PredicateWithReference());
    EXPECT_THAT(coll, ElementsAre(11,12,13,14,15));
  }
}


// ={=========================================================================
// algo-swap
//
// 9.3.4 iter_swap()
//
// #include <algorithm>
//
// void iter_swap (ForwardIterator1 pos1, ForwardIterator2 pos2)
//
// Swaps the values to which iterators pos1 and pos2 refer.
// The iterators don’t need to have the same type. However, the values must be
// assignable.
//

TEST(AlgoSwap, IterSwap)
{
  {
    vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

    // std::iter_swap(coll.begin(), next(coll.begin()));
    std::iter_swap(coll.begin(), ++coll.begin());
    EXPECT_THAT(coll, ElementsAre(2, 1, 3, 4, 5, 6, 7, 8, 9));

    // std::iter_swap(coll.begin(), prev(coll.end()));
    std::iter_swap(coll.begin(), --coll.end());
    EXPECT_THAT(coll, ElementsAre(9, 1, 3, 4, 5, 6, 7, 8, 2));
  }

  // use operator[]()
  {
    vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

    std::swap(coll[0], coll[1]);
    EXPECT_THAT(coll, ElementsAre(2, 1, 3, 4, 5, 6, 7, 8, 9));

    std::swap(coll[0], coll[8]);
    EXPECT_THAT(coll, ElementsAre(9, 1, 3, 4, 5, 6, 7, 8, 2));
  }

  // use operator*()
  {
    vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

    vector<int>::iterator one, two;
    one = coll.begin();
    two = one+1;
    swap(*one, *two);   // calls std::swap()
    EXPECT_THAT(coll, ElementsAre(2, 1, 3, 4, 5, 6, 7, 8, 9));

    one = coll.begin();
    two = one+8;
    swap(*one, *two);
    EXPECT_THAT(coll, ElementsAre(9, 1, 3, 4, 5, 6, 7, 8, 2));
  }
}


// ={=========================================================================
// cxx-algo-copy
//

TEST(AlgoCopy, Error)
{
  {
    std::vector<int> coll;

    for (int i = 0; i <= 6; ++i)
      coll.push_back(i);

    auto first = std::find(coll.begin(), coll.end(), 300);
    auto last = std::find(coll.begin(), coll.end(), 3);

    std::copy(first, last, std::ostream_iterator<int>(cout, ", "));
  }
}

TEST(AlgoCopy, UseOnDifferentCollections)
{
  {
    vector<int> coll{1,2,3,4,5,6};
    EXPECT_THAT(coll, ElementsAre(1,2,3,4,5,6));

    list<int> coll1;

    copy(coll.begin(), coll.end(), inserter(coll1, coll1.begin()));
    EXPECT_THAT(coll1, ElementsAre(1,2,3,4,5,6));
  }
}


// ={=========================================================================
// cxx-algo-generate cxx-random

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
      IntegerSequence(int value) : value_(value) {}

      int operator()()
      { return ++value_; }

    private:
      int value_;
  };

  class IntegerSequenceNoReturn
  {
    public:
      IntegerSequenceNoReturn(int value) : value_(value) {}

      void operator()()
      { ++value_; }

    private:
      int value_;
  };
} // namespace


// this means that _Generator shall return value and otherwise get errors:
// 
// stl.cpp:2464:7:   required from here
// /usr/include/c++/4.9/bits/stl_algo.h:4325:11: error: no match for ‘operator=’ (operand types are ‘std::back_insert_iterator<std::__debug::list<int> >’ and ‘void’)
//   *__first = __gen();
// 
// TEST(AlgoGenerate, NoReturn)
// {
//   using namespace algo_generate;
// 
//   std::list<int> coll;
// 
//   // starting from 1
//   IntegerSequenceNoReturn seq(1);
// 
//   std::generate_n<back_insert_iterator<list<int>>, int, IntegerSequenceNoReturn&>(
//       back_inserter(coll),
//       4,                    // number of elements
//       seq
//       );
// 
//   EXPECT_THAT(coll, ElementsAre(2,3,4,5));
// }

TEST(AlgoGenerate, Types)
{
  using namespace algo_generate;

  list<int> coll;

  // insert five random numbers
  generate_n (back_inserter(coll), // beginning of destination range
      5, // count
      rand); // new value generator

  // PRINT_ELEMENTS(coll);

  // `overwrite` with five new random numbers
  generate (coll.begin(), coll.end(), // destination range
      IntegerSequence(1)); // new value generator

  EXPECT_THAT(coll, ElementsAre(2,3,4,5,6));
}

TEST(AlgoGenerate, Reference)
{
  using namespace algo_generate;

  std::list<int> coll;

  // starting from 1
  IntegerSequence seq(1);

  std::generate_n<back_insert_iterator<list<int>>, int, IntegerSequence&>(
      back_inserter(coll),
      4,                    // number of elements
      seq
      );

  EXPECT_THAT(coll, ElementsAre(2,3,4,5));

  // insert values from 43 to 46

  generate_n(
      back_inserter(coll),  // start
      4,                    // number of elements
      IntegerSequence(42)
      );

  EXPECT_THAT(coll, ElementsAre(2,3,4,5,43,44,45,46));

  // currently seq has 6 value and start from that since passed by value

  generate_n(
      back_inserter(coll),  // start
      4,                    // number of elements
      seq
      );

  EXPECT_THAT(coll, 
      ElementsAreArray({2,3,4,5,43,44,45,46,6,7,8,9}));

  // no change of sequence values and repeat it again since seq is copied.

  generate_n(
      back_inserter(coll),  // start
      4,                    // number of elements
      seq
      );

  EXPECT_THAT(coll, 
      ElementsAreArray({2,3,4,5,43,44,45,46,6,7,8,9,6,7,8,9}));
}


TEST(AlgoRandom, UseRandomEngineAndDistribution)
{
  default_random_engine dre;

  // always same sequence
  for (size_t i = 0; i < 18; ++i)
    cout << dre() << ", ";
  cout << endl;

  uniform_int_distribution<unsigned> udist1(0, 6);

  for (size_t i = 0; i < 18; ++i)
    cout << setw(2) << udist1(dre) << ", ";
  cout << endl;

  uniform_int_distribution<unsigned> udist2(6, 12);

  for (size_t i = 0; i < 18; ++i)
    cout << setw(2) << udist2(dre) << ", ";
  cout << endl;
}


class CardSequenceUseRand
{
  public:
    CardSequenceUseRand(int size): size_(size) {}

    int operator() () {
      // return rand() % 24;
      return rand() % (size_);
    }

  private:
    int size_{};
};

class CardSequenceUseRandomEngine
{
  public:
    int operator() () {
      return udist(dre);
    }

  private:
    static default_random_engine dre;
    static uniform_int_distribution<size_t> udist;
};

default_random_engine CardSequenceUseRandomEngine::dre;
uniform_int_distribution<size_t> CardSequenceUseRandomEngine::udist{0, 24};


class CardSequenceUseRandWithRange
{
  public:
    CardSequenceUseRandWithRange(int min, int max)
      : min_(min), max_(max) {}

    int operator() () {
      return min_ + (rand() % (max_ - min_ + 1));
    }

  private:
    int min_{};
    int max_{};
};


// 0 3 18 11 13 5 1 16 16 23 9 12 (12)
// 20 0 1 13 16 0 9 1 10 17 14 23 (12)
// 1 0 1 1 1 1 0 0 1 1 0 1 (12)
// 0 1 1 0 0 0 0 0 1 0 1 1 (12)
// 8 8 8 9 9 9 7 8 8 8 7 9 (12)

TEST(AlgoRandom, RandomEngineVsRand)
{
  // always same sequence
  vector<uint32_t> ivec1;
  generate_n(back_inserter(ivec1), 12, CardSequenceUseRandomEngine() );
  PRINT_ELEMENTS(ivec1);

  // always same sequence
  vector<uint32_t> ivec2;
  generate_n(back_inserter(ivec2), 12, CardSequenceUseRandomEngine() );
  PRINT_ELEMENTS(ivec2);

  vector<uint32_t> ivec3;
  generate_n(back_inserter(ivec3), 12, CardSequenceUseRand(24) );
  PRINT_ELEMENTS(ivec3);

  vector<uint32_t> ivec4;
  generate_n(back_inserter(ivec4), 12, CardSequenceUseRand(24) );
  PRINT_ELEMENTS(ivec4);

  vector<uint32_t> ivec5;
  generate_n(back_inserter(ivec5), 12, 
      CardSequenceUseRandWithRange(0, 23));
  PRINT_ELEMENTS(ivec5);
}


TEST(AlgoRandom, Sequence)
{
  {
    default_random_engine dre;

    vector<unsigned int> coll1{};

    for (size_t i = 0; i < 18; ++i)
    {
      coll1.push_back(dre());
    }

    vector<unsigned int> coll2{};

    for (size_t i = 0; i < 18; ++i)
    {
      coll2.push_back(dre());
    }

    // differnt sequence
    EXPECT_THAT(coll1==coll2, false);
  }

  {
    vector<unsigned int> coll1{};
    vector<unsigned int> coll2{};

    {
      default_random_engine dre;

      for (size_t i = 0; i < 18; ++i)
      {
        coll1.push_back(dre());
      }
    }
    {
      default_random_engine dre;

      for (size_t i = 0; i < 18; ++i)
      {
        coll2.push_back(dre());
      }
    }

    // same sequence
    EXPECT_THAT(coll1==coll2, true);
  }
}


TEST(AlgoRandom, Seed)
{
  default_random_engine e1;
  default_random_engine e2(2147483646);

  default_random_engine e3;
  e3.seed(32767);
  default_random_engine e4(32767);

  int unmatch_count{};
  int match_count{};

  for (size_t i = 0; i < 100; i++)
  {
    // two use different seed so expect no matches
    if (e1() == e2())
      ++match_count;

    // two use same seed so expect all matches
    if (e3() != e4())
      ++unmatch_count;
  }

  EXPECT_THAT(match_count, 0);
  EXPECT_THAT(unmatch_count, 0);
}


// 1 3 9 6 7 8 2 4 5 (9)
// 8 7 5 6 2 4 9 3 1 (9)

TEST(AlgoRandom, AlgoShuffle)
{
  vector<int> coll{1,2,3,4,5,6,7,8,9};

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

TEST(AlgoAccumulate, Stream)
{
  istringstream is{"1 2 3 4 5 6"};
  istream_iterator<int> start(is), eof;

  auto sum = std::accumulate(start, eof, 0);
  EXPECT_THAT(sum, 21);
}

TEST(AlgoAccumulate, Coll)
{
  vector<int> coll{1,2,3,4,5};

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
  ASSERT_THAT(accumulate(coll.cbegin(), coll.cend(), 1, multiplies<int>()), Eq(120));
  ASSERT_THAT(accumulate(coll.cbegin(), coll.cend(), 0, multiplies<int>()), Eq(0));

  // if use cxx-rambda
  ASSERT_THAT(accumulate(coll.cbegin(), coll.cend(), 1, 
        [](int init, int elem)
        { 
          return init*elem;
        })
        , Eq(120));

  // sum, if use cxx-rambda
  ASSERT_THAT(accumulate(coll.cbegin(), coll.cend(), 0, 
        [](int init, int elem)
        { 
          return init+elem;
        })
        , Eq(15));
}

// see how to use acculumate on map

TEST(AlgoAccumulate, Map)
{
  std::map<int, size_t> counts{
    {1, 2},
      {3, 2},
      {5, 3},
      {8, 3},
      {13, 1} 
  };

  // *cxx-error*
  // auto result = std::accumulate(counts.begin(), counts.end(), pair<int const, size_t>());

  // *cxx-const* cause error:
  //     [](size_t sum, pair<int, size_t>& e)
  //
  auto result = std::accumulate(counts.begin(), counts.end(), 0,
      [](size_t sum, pair<int const, size_t>& e)
      { return sum + e.second; });

  EXPECT_THAT(result, 11);
}


// ={=========================================================================
// cxx-algo-for-each cxx-algo-transform

namespace algo_code
{
  template<typename _InputIterator, typename _Function>
    _Function
    for_each(_InputIterator __first, _InputIterator __last, _Function __f)
    {
      // note: call op but not use return

      for (; __first != __last; ++__first)
        __f(*__first);

      return _GLIBCXX_MOVE(__f);
    }

  template<typename _InputIterator, typename _OutputIterator,
    typename _UnaryOperation>

      _OutputIterator
      transform(_InputIterator __first, _InputIterator __last,
          _OutputIterator __result, _UnaryOperation __unary_op)
      {
        for (; __first != __last; ++__first, ++__result)
          // note: write to output to output iterator and unary
          *__result = __unary_op(*__first);
        return __result;
      }
} // namespace

int square_value_with_return(int value)
{
  value = value*value;
  return value;
}

void square_value_no_return(int value)
{
  value = value*value;
}

void square_refer_no_return(int &value)
{
  value = value*value;
}

TEST(AlgoForEach, Transform)
{
  // value
  {
    set<int> coll{1, 2, 3, 4, 5, 6, 7, 8};
    for_each(coll.begin(), coll.end(), square_value_no_return);
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
  }

  // value with return? same since do not use return value
  {
    set<int> coll{1, 2, 3, 4, 5, 6, 7, 8};
    for_each(coll.begin(), coll.end(), square_value_with_return);
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
  }

  // reference
  {
    // compile error since key in set are const if use set
    // set<int> coll{1,2,3,4,5,6,7,8};

    vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};
    for_each(coll.begin(), coll.end(), square_refer_no_return);
    EXPECT_THAT(coll, ElementsAre(1, 4, 9, 16, 25, 36, 49, 64));
  }

  // algo-transform() differs in that it uses `dest` and use return
  {
    vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};
    vector<int> result;
    transform(coll.begin(), coll.end(), 
        back_inserter(result), square_value_with_return);
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
    EXPECT_THAT(result, ElementsAre(1, 4, 9, 16, 25, 36, 49, 64));
  }
}

TEST(AlgoForEach, UsePredicates)
{
  {
    vector<int> coll{1,2,3,4,5,6};

    for_each(coll.begin(), coll.end(),
        [](int &elem)
        {
          elem += 10;
        });

    EXPECT_THAT(coll, ElementsAre(11,12,13,14,15,16));
  }

  // to see how cxx-range-for could make for_each less useful
  {
    vector<int> coll{1,2,3,4,5,6};

    for (auto & elem : coll)
      elem += 10;

    EXPECT_THAT(coll, ElementsAre(11,12,13,14,15,16));
  }

  // add value of first element which is copied.
  {
    vector<int> coll{1,2,3,4,5,6};

    for_each(coll.begin(), coll.end(),
        [=](int &elem)
        {
          elem += *coll.begin();
        });

    EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7));
  }

  // add value of first element which is reference so use '2' but not '1'
  {
    vector<int> coll{1,2,3,4,5,6};

    for_each(coll.begin(), coll.end(),
        [&](int &elem)
        {
          elem += *coll.begin();
        });

    EXPECT_THAT(coll, ElementsAre(2,4,5,6,7,8));
  }
}

// 11.4 The for_each() Algorithm
// A third example demonstrates how to use the return value of the for_each()
// algorithm. Because for_each() has the special property that it returns its
// operation, you can process and return a result inside the operation:
//
// note that this is not predicate since it has its state.

namespace algo_for_each {
  class MeanValue {
    public:
      MeanValue(): num_(0), sum_(0) {}

      void operator()(int elem) {
        num_++;
        sum_ += elem;
      }

      operator double() {
        return static_cast<double>(sum_)/num_;
      }

    private:
      long num_;
      long sum_;
  };
}

TEST(AlgoForEach, GetMean)
{
  using namespace algo_for_each;

  vector<int> coll{1,2,3,4,5,6,7,8};

  double mean = for_each(coll.begin(), coll.end(),
      MeanValue());

  EXPECT_THAT(mean, DoubleEq(4.5));
}


// ={=========================================================================
// cxx-algo-min-max

namespace algo_min_max {

  bool AbsLess(int elem1, int elem2) {
    return abs(elem1) < abs(elem2);
  }

} // namespace

TEST(AlgoMinMax, Use)
{
  using namespace algo_min_max;

  deque<int> coll;
  INSERT_ELEMENTS(coll, 2, 6);
  INSERT_ELEMENTS(coll, -3, 6);
  EXPECT_THAT(coll, 
      ElementsAreArray({2, 3, 4, 5, 6, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6}));

  // If more than one minimum or maximum element exists, min_element() and
  // max_element() return `the first` found; minmax_element() returns the first
  // minimum but the last maximum element, so max_element() and minmax_element()
  // don’t yield the same maximum element.

  EXPECT_THAT(*min_element(coll.begin(), coll.end()), -3);

  EXPECT_THAT(*max_element(coll.begin(), coll.end()), 6);
  EXPECT_THAT(distance(coll.begin(),max_element(coll.begin(), coll.end())), 4);
 
  // return iterator pair
  // Note also that minmax_element() yields `the last maximum`, so the distance
  // 9.
  auto minmax = minmax_element(coll.begin(), coll.end());
  EXPECT_THAT(*(minmax.first), -3);   // first minimum
  EXPECT_THAT(*(minmax.second), 6);   // last maximum

  // last maximum is 6 which is the last element
  EXPECT_THAT(distance(coll.begin(), minmax.second), coll.size()-1);

  EXPECT_THAT(distance(minmax.first, minmax.second), 9);
  EXPECT_THAT(distance(
        min_element(coll.begin(), coll.end()),
        max_element(coll.begin(), coll.end()))
      , -1);

  // min/max of absolute values
  EXPECT_THAT(*min_element(coll.begin(), coll.end(), AbsLess), 0);
  EXPECT_THAT(*max_element(coll.begin(), coll.end(), AbsLess), 6);
}


// ={=========================================================================
// cxx-algo-search

TEST(AlgoSearch, SearchN)
{
  deque<int> coll{1, 2, 7, 7, 6, 3, 9, 5, 7, 7, 7, 3, 6};

  auto pos = search_n(coll.begin(), coll.end(),
      3,
      7);
  if (pos != coll.end()) {
    EXPECT_THAT(distance(coll.begin(), pos), 8);
  }
}

TEST(AlgoSearch, SearchFirstSubrange)
{
  deque<int> coll{1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
  list<int> subcoll{3,4,5};

  // first sub found
  auto pos = search(coll.begin(), coll.end(),
      subcoll.begin(), subcoll.end());
  if (pos != coll.end()) {
    EXPECT_THAT(distance(coll.begin(), pos), 2);
  }

  // second sub found
  ++pos;
  pos = search(pos, coll.end(),
      subcoll.begin(), subcoll.end());
  if (pos != coll.end()) {
    EXPECT_THAT(distance(coll.begin(), pos), 9);
  }
}

namespace algo_search {
  using ITERATOR = std::deque<int>::iterator;

  // struct _Iter_equal_to_iter
  // iterator type can be different but value type should be compatible.
  struct EqualToIter
  {
    template<typename Iterator1, typename Iterator2>
      bool operator()(Iterator1 it1, Iterator2 it2) const
      { return *it1 == *it2; }
  };

  ITERATOR my_search(ITERATOR first1, ITERATOR last1, ITERATOR first2, ITERATOR last2)
  {
    EqualToIter EQUAL_PREDICATE;

    for (;;)
    {
      // find_if() requires unary op and have tried varios attempts but not easy
      // to make all work.
      //
      // first1 = find_if(first1, last1, EQUAL_PREDICATE);
      // first1 = find_if(first1, last1, bind( EQUAL_PREDICATE, placeholders::_1, first2));
      // first1 = find_if(first1, last1, __iter_comp_iter(EQUAL_PREDICATE, first2));
      
      // works
      first1 = find_if(first1, last1, [=](int elem)
          { 
          return elem == *first2;
          });

      // not found
      if (first1 == last1)
        return last1;

      // cout << "find: " << *first1 << endl;

      ITERATOR p = first2;
      ITERATOR current = first1;

      // this make the following loop fails so my_search() returns end().
      // why since it's copied from stl code???
      //
      // if(++current == last1)
      //   return last1;

      while (EQUAL_PREDICATE(current, p))
      {
        // cout << "equal: " << *current << "," << *p << endl;

        // the second range matches up and exit loop
        if (++p == last2)
          return first1;

        if (++current == last1)
          return last1;
      } // while end

      // cout << "not equal: " << *current << "," << *p << endl;

      // wehn found no match, then search again
      ++first1;
    } // for end

    return first1;
  }
} // namespace

TEST(AlgoSearch, SearchFirstSubrange_UseOwn)
{
  using namespace algo_search;

  deque<int> coll{1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
  deque<int> subcoll{3,4,5};

  // first sub found
  auto pos = my_search(coll.begin(), coll.end(),
      subcoll.begin(), subcoll.end());
  if (pos != coll.end()) {
    cout << "first" << endl;
    EXPECT_THAT(distance(coll.begin(), pos), 2);
  }

  // second sub found
  ++pos;
  pos = my_search(pos, coll.end(),
      subcoll.begin(), subcoll.end());
  if (pos != coll.end()) {
    cout << "second" << endl;
    EXPECT_THAT(distance(coll.begin(), pos), 9);
  }
}

// algo-search algo-binary-search

TEST(AlgoSearch, AlgoBinarySearch)
{
  vector<int> coll{1,2,3,4,5,6,7,8,9};

  EXPECT_TRUE(binary_search(coll.begin(), coll.end(), 5));
  EXPECT_FALSE(binary_search(coll.begin(), coll.end(), 42));
}


// algo-search algo-include

TEST(AlgoSearch, AlgoInclude)
{ 
  {
    deque<int> coll{1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
    list<int> subcoll{3,4,5};

    // first sub found
    auto pos = search(coll.begin(), coll.end(),
        subcoll.begin(), subcoll.end());
    if (pos != coll.end()) {
      cout << "search() found" << endl;
      EXPECT_THAT(distance(coll.begin(), pos), 2);
    }
  }

  {
    deque<int> coll{1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
    list<int> subcoll{3,4,5};

    // first sub found
    // if do not sort before includes() then aborted when built with DEBUG.
    sort(coll.begin(), coll.end());

    auto pos = includes(coll.begin(), coll.end(),
        subcoll.begin(), subcoll.end());
    EXPECT_TRUE(pos);
  }
}


// algo-search algo-include
// Searching First or Last Possible Position

TEST(AlgoSearch, AlgoUpperLowerBound)
{
  {
    vector<int> coll{1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9};

    // lower_bound() returns the position of the first element that has a value
    // equal to or greater than value. This is the first position where an
    // element with value value could get inserted without breaking the sorting
    // of the range [beg,end).

    auto first = lower_bound(coll.cbegin(), coll.cend(), 5);

    // upper_bound() returns the position of the first element that has a value
    // greater than value. This is the last position where an element with value
    // value could get inserted without breaking the sorting of the range
    // [beg,end).

    auto last = upper_bound(coll.cbegin(), coll.cend(), 5);

    //  0  1  2  3  4  5  6  7  8  9 10 11 12
    // {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9};
    //                          ^^^^^^^

    EXPECT_THAT(distance(coll.cbegin(), first), 8);
    EXPECT_THAT(distance(coll.cbegin(), last), 10);
  }

  {
    vector<int> coll{2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};

    // lower_bound() returns the position of the first element that has a value
    // equal to or greater than value. This is the first position where an
    // element with value value could get inserted without breaking the sorting
    // of the range [beg,end).

    auto first = lower_bound(coll.cbegin(), coll.cend(), 1);
    EXPECT_THAT(*first, 2);
  }
}


// ={=========================================================================
// cxx-algo-find-if

namespace algo_find
{
  bool is_prime(int number)
  {
    number = abs(number);

    // 0 and 1 are not prime numbers
    if (number == 0 || number == 1)
      return false;

    int divisor;

    // until find divisor which leaves 0 remainder, that is when divisor is 1 or
    // when other divisor
    for (divisor = number/2; number % divisor != 0; --divisor)
      ;

    // if divisor is 1 then no ther is found.
    return divisor == 1;
  }
} // namespace

// is used to search for the 'first' element of the given range for which the
// passed unary predicate yields true. If it does not find any element that
// matches the predicate, the algorithm returns the end of the range (its
// 'second' argument).

TEST(AlgoFind, FindIf)
{
  using namespace algo_find;

  list<int> coll;

  for (int i = 24; i <= 30; ++i)
    coll.push_back(i);

  auto pos = find_if(coll.begin(), coll.end(), is_prime);
  EXPECT_THAT(*pos, 29);
}

TEST(AlgoFind, Find)
{
  vector<int> coll{2, 5, 4, 1, 6, 3};

  // *algo-min-element*
  // if there are duplicates, return the first
  auto minpos = min_element(coll.begin(), coll.end());
  EXPECT_THAT(*minpos, 1);

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

  EXPECT_FALSE(lexicographical_compare(s1.begin(), s1.end(), s1.begin(), s1.end())); 
  EXPECT_TRUE(lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end())); 

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

    EXPECT_THAT(coll1, ElementsAre(1,2,3,4,5,6,7));
    EXPECT_THAT(coll2, ElementsAre(3,4,5,6,7,8,9));
    EXPECT_THAT(coll3, ElementsAre(1,2,3,4,5,6,7));

    EXPECT_FALSE( equal(coll1.begin(), coll1.end(),
                coll2.begin()));

    EXPECT_TRUE( equal(coll1.begin(), coll1.end(),
                coll3.begin()));

    // coll1 and coll2 are different but are the same as to the order of
    // even/odd.
    EXPECT_TRUE( equal( coll1.begin(), coll1.end(),
                coll2.begin(), [](int lhs, int rhs)
                {
                    return lhs % 2 == rhs % 2; 
                }
                ));
}

TEST(AlgoEqual, UsePermutation)
{
  string string1{"PARK"}, string2{"KRAP"};

  // Testing for Unordered Equality
  EXPECT_TRUE(is_permutation(string1.begin(), string1.end(), string2.begin()));
}


// ={=========================================================================
// cxx-algo-reverse

TEST(AlgoReverse, Use)
{
  vector<int> coll{1,2,3,4,5,6,7};

  reverse(coll.begin(), coll.end());
  EXPECT_THAT(coll, ElementsAre(7,6,5,4,3,2,1));

  reverse(coll.begin()+1, coll.end()-1);
  EXPECT_THAT(coll, ElementsAre(7,2,3,4,5,6,1));
}

namespace algo_reverse 
{
  using RITERATOR = vector<int>::iterator;

  void my_reverse(RITERATOR first, RITERATOR last)
  {
    --last;

    for (;first < last; ++first, --last)
      swap(*first, *last);
  }

  using BITERATOR = list<int>::iterator;

  void my_reverse_bi(BITERATOR first, BITERATOR last)
  {
    --last;

    // since `<` is only supported for random
    // for (;first < last; ++first, --last)

    for (;first != last; ++first, --last)
      swap(*first, *last);
  }
}

TEST(AlgoReverse, UseOwn)
{
  using namespace algo_reverse;

  {
    vector<int> coll{1,2,3,4,5,6,7};

    my_reverse(coll.begin(), coll.end());
    EXPECT_THAT(coll, ElementsAre(7,6,5,4,3,2,1));

    my_reverse(coll.begin()+1, coll.end()-1);
    EXPECT_THAT(coll, ElementsAre(7,2,3,4,5,6,1));
  }

  {
    list<int> coll{1,2,3,4,5,6,7};

    my_reverse_bi(coll.begin(), coll.end());
    EXPECT_THAT(coll, ElementsAre(7,6,5,4,3,2,1));

    // since `+/- is only supported for random
    // my_reverse_bi(coll.begin()+1, coll.end()-1);

    my_reverse_bi(next(coll.begin()), prev(coll.end()));
    EXPECT_THAT(coll, ElementsAre(7,2,3,4,5,6,1));
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

  auto pos1 = partition(coll1.begin(), coll1.end(),    // range
      [](int elem)
      {
      return elem %2 == 0;
      });

  EXPECT_THAT(coll1, ElementsAre(8, 2, 6, 4, 5, 3, 7, 1, 9));

  // algo-partition returns an iterator to the first element where the
  // predicate is not true, or the end of the range if all elements satisfy
  // the predicate. so first odd element:

  EXPECT_EQ(*pos1, 5);

  INSERT_ELEMENTS(coll2, 1, 9);
  EXPECT_THAT(coll2, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

  auto pos2 = stable_partition(coll2.begin(), coll2.end(),
      [](int elem)
      {
      return elem %2 == 0;
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
auto gather(Iterator _first, Iterator _last, Iterator _pos, Compare _comp) -> std::pair<Iterator, Iterator>
{
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  auto _begin = partition(_first, _pos, [&](const value_type e) { return !_comp(e); });
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
    std::partition(coll.begin(), coll.begin()+4, [](const int x){ return x != 1; });
    std::partition(coll.begin()+4, coll.begin()+10, [](const int x){ return x == 1; });

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

    gather(coll.begin(), coll.end(), coll.begin()+4, [](const int x){ return x == 1; });
    EXPECT_THAT(coll, ElementsAre(0, 0, 1, 1, 1, 1, 0, 0, 0, 0));
  }
}


// cxx-algo-remove algo-unique

TEST(AlgoMutating, AlgoRemove)
{
  // coll.erase() delete elements but algo-remove do not.
  {
    std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

    for (auto it = coll.begin(); it != coll.end(); ++it)
    {
      if (*it == 2)
        it = coll.erase(it);
    }

    EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
  }

  {
    std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

    auto end = remove(coll.begin(), coll.end(), 2);

    EXPECT_THAT(distance(end, coll.end()), 4);
    EXPECT_THAT(coll, 
        ElementsAreArray({1,3,4,5,6,7,8,9,2,8,2,9}));

    coll.erase(end, coll.end());
    EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
  }

  // show that algo-remove() do not remove elements
  {
    std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

    remove(coll.begin(), coll.end(), 2);

    // std::vector<int> coll{1,3,4,5,6,7,8,9,2,8,2,9};
    //                                       ^^^^^^^ 

    EXPECT_THAT(coll, ElementsAreArray({1,3,4,5,6,7,8,9,2,8,2,9}));
  }

  // show that remove_if() returns end if not found
  {
    std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

    auto it = remove_if(coll.begin(), coll.end(), 
        [](int value)
        { return value == 10; }
        );

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
    vector<int> coll{1,2,3};

    PRINT_ELEMENTS(coll, "start:");

    // permute coll until they are sorted, ascending
    while(next_permutation(coll.begin(), coll.end()))
    {
      PRINT_ELEMENTS(coll, "next :");
    }

    // return false when sorted
    PRINT_ELEMENTS(coll, "finis:");

    // until descending sorted and the loop ends immediately
    while(prev_permutation(coll.begin(), coll.end()))
    {
      PRINT_ELEMENTS(coll, "prev :");
    }
    PRINT_ELEMENTS(coll, "now  :");

    while(prev_permutation(coll.begin(), coll.end()))
    {
      PRINT_ELEMENTS(coll, "prev :");
    }
    PRINT_ELEMENTS(coll, "finis:");
  }

  {
    string coll{"123"};

    PRINT_ELEMENTS(coll, "start:");

    while (std::next_permutation(coll.begin(), coll.end()));
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
  vector<int> coll{1,2,3,4,5,6,7,8};

  // rotate one to the left
  // before *cxx-11* void rotate() so comment out 
  // auto pos = rotate(

  rotate(
    coll.begin(),     // begin  
    coll.begin()+1,   // new begin
    coll.end()        // end
  );
  EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));

  // return the new position of the (pervious) first element.
  // EXPECT_THAT(*pos, 1);

  // pos = rotate(

  rotate(
    coll.begin(),
    coll.end()-2,
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(8,1,2,3,4,5,6,7));
  // EXPECT_THAT(*pos, 2);

  // rotate so that 4 is the beginning
  // pos = rotate(

  rotate(
    coll.begin(),
    find(coll.begin(), coll.end(), 4),
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(4,5,6,7,8,1,2,3));
  // EXPECT_THAT(*pos, 8);
}


TEST(AlgoMutating, AlgoReverse)
{
  {
    vector<int> coll{1,2,3,4,5,6,7,8};

    std::reverse(coll.begin(), coll.end());
    EXPECT_THAT(coll, ElementsAre(8,7,6,5,4,3,2,1));

    std::reverse(coll.begin()+1, coll.end()-1);
    EXPECT_THAT(coll, ElementsAre(8,2,3,4,5,6,7,1));

    vector<int> result;
    std::reverse_copy(coll.begin(), coll.end(), 
        back_inserter(result));
    EXPECT_THAT(result, ElementsAre(1,7,6,5,4,3,2,8));
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
  template<typename _RandomAccessIterator, typename _Compare>
    inline void
    sort(_RandomAccessIterator __first, _RandomAccessIterator __last,
        _Compare __comp)
    {
      // concept requirements
      __glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<
          _RandomAccessIterator>)
        __glibcxx_function_requires(_BinaryPredicateConcept<_Compare,
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
  template<typename _RandomAccessIterator>
    inline void
    sort(_RandomAccessIterator __first, _RandomAccessIterator __last)
    {
      // concept requirements
      __glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<
          _RandomAccessIterator>)
        __glibcxx_function_requires(_LessThanComparableConcept<
            typename iterator_traits<_RandomAccessIterator>::value_type>)
        __glibcxx_requires_valid_range(__first, __last);

      std::__sort(__first, __last, __gnu_cxx::__ops::__iter_less_iter());
    }

  struct _Iter_less_iter
  {
    template<typename _Iterator1, typename _Iterator2>
      bool
      operator()(_Iterator1 __it1, _Iterator2 __it2) const
      { return *__it1 < *__it2; }
  };

  inline _Iter_less_iter
    __iter_less_iter()
    { return _Iter_less_iter(); }

  template<typename _RandomAccessIterator, typename _Compare>
    inline void
    __sort(_RandomAccessIterator __first, _RandomAccessIterator __last,
        _Compare __comp)
    {
      if (__first != __last)
      {
        std::__introsort_loop(__first, __last,
            std::__lg(__last - __first) * 2,
            __comp);
        std::__final_insertion_sort(__first, __last, __comp);
      }
    }
} // namespace

TEST(AlgoSorting, AlgoSort)
{
  {
    vector<int> coll{3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5};
    sort(coll.begin(), coll.end());

    EXPECT_THAT(coll, 
        ElementsAreArray({1,2,2,3,3,3,4,4,4,5,5,5,6,6,7}));
  }

  {
    vector<int> coll{3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5};
    // *cxx-callable*
    sort(coll.begin(), coll.end(), std::greater<int>());

    EXPECT_THAT(coll, 
        ElementsAreArray({7,6,6,5,5,5,4,4,4,3,3,3,2,2,1}));
  }
}

bool shorter(string const& s1, string const& s2)
{
  return s1.size() < s2.size();
}

TEST(AlgoSorting, AlgoStableSort)
{
  vector<string> coll{"over", "quick", "red", "fox", "jumps", "red", "the", 
    "slow", "turtle", "the"};

  // sort by alphabet and remove dups. algo-unique()
  {
    std::sort(coll.begin(), coll.end());
    coll.erase(std::unique(coll.begin(), coll.end()), coll.end());
  }

  // eliminated
  EXPECT_THAT(coll, 
      ElementsAre("fox","jumps","over","quick","red","slow","the","turtle"));

  // by size
  stable_sort(coll.begin(), coll.end(), shorter);

  // sorted and see that the order before sort is maintained
  EXPECT_THAT(coll, 
      ElementsAre("fox","red","the","over","slow","jumps","quick","turtle"));         // by size
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
    nth_element(coll.begin(), coll.begin()+3, coll.end());

    // 2 1 2 3 3 4 3 4 5 6 4 7 6 5 5 (15)
    PRINT_ELEMENTS(coll);
  }

  {
    vector<int> coll{3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5};
    nth_element(coll.begin(), coll.end()-4, coll.end());

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
  EXPECT_THAT(coll, ElementsAre(7,7,7,7,7,7,7,7));  

  fill(coll.begin(), coll.end(), 1);
  EXPECT_THAT(coll, ElementsAre(1,1,1,1,1,1,1,1));  

  vector<int> coll1(8,0);
  fill_n(coll1.begin(), 8, 7);
  EXPECT_THAT(coll1, ElementsAre(7,7,7,7,7,7,7,7));  
}


// ={=========================================================================
// algo-heap also-sort

TEST(AlgoHeap, Calls)
{
  vector<int> coll{3, 4, 5, 6, 7, 5, 6, 7, 8, 9, 1, 2, 3, 4};

  // convert collection into a heap
  make_heap(coll.begin(), coll.end());
  EXPECT_THAT(coll, ElementsAreArray({9, 8, 6, 7, 7, 5, 5, 3, 6, 4, 1, 2, 3, 4}));

  // pop next element, root out of the heap
  pop_heap(coll.begin(), coll.end());
  coll.pop_back();
  EXPECT_THAT(coll, ElementsAreArray({8,7,6,7,4,5,5,3,6,4,1,2,3}));

  // push new element into the heap
  coll.push_back(17);
  push_heap(coll.begin(), coll.end());
  EXPECT_THAT(coll, ElementsAreArray({17,7,8,7,4,5,6,3,6,4,1,2,3,5}));

  // convert heap into a sorted collection but note that after this, it is no
  // lonner a heap
  sort_heap(coll.begin(), coll.end());
  EXPECT_THAT(coll, ElementsAreArray({1,2,3,3,4,4,5,5,6,6,7,7,8,17}));
}


// ={=========================================================================
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


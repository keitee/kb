#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <deque>
#include <list>
#include <unordered_set>
#include <queue>
// #include <algorithm>
#include <numeric>        // std::accumulate for gcc 6
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

template <typename T>
void PRINT_Q_ELEMENTS(const T& coll, const string &optstr = "")
{
  size_t count = coll.size();
  cout << optstr;

  for(size_t i = 0; i < count; ++i)
    cout << coll.top() << " ";

  cout << "(" << count << ")" << endl;
}


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
  // cxx.cpp: In member function ‘virtual void Temporary_NativeAndStruct_Test::TestBody()’:
  // cxx.cpp:1539:45: error: lvalue required as increment operand
  //    cout << "return int: " << ++ReturnInteger() << endl;
  // cout << "return int: " << ++ReturnInteger() << endl;
  
  cout << "return int: " << ++ReturnStruct() << endl;
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


// ={=========================================================================
// cxx-vector

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

TEST(DISABLED_StlVector, EraseChangesEnd_EmitRuntimeError)
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

TEST(StlVector, EraseChangesEnd_NoError)
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


TEST(StlVector, InsertAndErase)
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

TEST(StlVector, CreateOnDemand)
{
  vector<VectorEraseCallsDtor> ovec{};

  for(int i = 0; i < 5; ++i)
  {
    cout << "-for ---------" << endl;
    string name = "name " + to_string(i);
    ovec.push_back(VectorEraseCallsDtor(name));
  }

  auto it = ovec.begin();
  it = ovec.erase(it);
  cout << "-erase---------" << endl;

  it = ovec.erase(it);
  cout << "-erase---------" << endl;

  it = ovec.erase(it);
  cout << "-erase---------" << endl;
}

TEST(StlVector, CreateWithReserve)
{
  vector<VectorEraseCallsDtor> ovec{};
  ovec.reserve(10);

  for(int i = 0; i < 5; ++i)
  {
    cout << "-for ---------" << endl;
    string name = "name " + to_string(i);
    ovec.push_back(VectorEraseCallsDtor(name));
  }

  auto it = ovec.begin();
  it = ovec.erase(it);
  cout << "-erase---------" << endl;

  it = ovec.erase(it);
  cout << "-erase---------" << endl;

  it = ovec.erase(it);
  cout << "-erase---------" << endl;
}

TEST(StlVector, CreateWithPreAllocation)
{
  vector<VectorEraseCallsDtor> ovec(10);

  // for(int i = 0; i < 5; ++i)
  // {
  //   cout << "-for ---------" << endl;
  //   string name = "name " + to_string(i);
  //   ovec.push_back(VectorEraseCallsDtor(name));
  // }

  auto it = ovec.begin();
  it = ovec.erase(it);
  cout << "-erase---------" << endl;

  it = ovec.erase(it);
  cout << "-erase---------" << endl;

  it = ovec.erase(it);
  cout << "-erase---------" << endl;
}

void StlVectorFillVector(vector<int> &coll)
{
  for (int i = 0; i < 20; ++i)
    coll.insert(coll.end(), i);
}

TEST(StlVector, InitializeFromExpression)
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

TEST(StlVector, InitializeForms)
{
  // default init
  vector<int> coll1;

  for (int i = 0; i < 5; i++)
  {
    int val;
    coll1.push_back(val);
  }

  // default init and fails
  EXPECT_THAT(coll1, Ne(vector<int>({0,0,0,0,0})));

  // value init
  vector<int> coll2(5);
  EXPECT_THAT(coll2, ElementsAre(0,0,0,0,0));

  // value init
  vector<int> coll3(5, 10);
  EXPECT_THAT(coll3, ElementsAre(10,10,10,10,10));
}


// cause seg fault
TEST(DISABLED_StlVector, AccessInvalidIndex)
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
TEST(DISABLED_StlVector, AccessInvalidIndexWithReserve)
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


class VectorCtorsTest
{
  public:
  VectorCtorsTest(int size, int value = 10) : icoll(size, value) {}
  int size() { return icoll.size(); }
  void print() { PRINT_ELEMENTS(icoll, "clsss : "); }

  private:
  vector<int> icoll;
};


TEST(StlVector, VecorCtors)
{
  vector<int> icoll1(5);
  PRINT_ELEMENTS(icoll1, "default init: ");
  ASSERT_THAT(icoll1.size(), Eq(5));

  vector<int> icoll2(5, 10);
  PRINT_ELEMENTS(icoll2, "value   init: ");
  ASSERT_THAT(icoll2.size(), Eq(5));

  VectorCtorsTest icoll3(10, 100);
  icoll3.print();
  ASSERT_THAT(icoll3.size(), Eq(10));
}


// ={=========================================================================
// cxx-array

TEST(StlArray, ArrayCtors)
{
  array<int, 8> coll = {11,22,33};
  coll.back() = 999;
  coll[coll.size()-2] = 42;

  EXPECT_THAT(coll, ElementsAre(11,22,33,0,0,0,42,999));
}

// initialized: t.h.i.s. .i.s. .a.n. .c.h.a.r. .a.r.r.a.y.....................(41)
// strcpyed   : u.s.e. .t.h.e. .a.d.d.r.e.s.s. .o.f. .f.i.r.s.t..................(41)
// strcpyed   : u.s.e. .d.a.t.a. .m.e.m.b.e.r..o.f. .f.i.r.s.t..................(41)

TEST(StlArray, ArrayAccess)
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

  array<array<int, cols>, rows> coll_array{}; 

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

TEST(CxxStlTest, QueueProritySort)
{
  priority_queue<float> pq;

  pq.push(66.6);
  pq.push(22.2);
  pq.push(44.4);

  pq.pop();

  PRINT_Q_ELEMENTS(pq, "1: ");

  // insert more
  pq.push(11.1);
  pq.push(55.5);
  pq.push(33.3);

  PRINT_Q_ELEMENTS(pq, "2: ");
  pq.pop();
  PRINT_Q_ELEMENTS(pq, "3: ");
}


// ={=========================================================================
// cxx-set

TEST(CxxStlTest, HowSetSorted)
{
    set<int, greater<int>> iset{13, 9, 7, 10, 2, 11, 12, 8, 7};

    PRINT_ELEMENTS(iset);

    auto begin = iset.begin();
    cout << *begin << ", " << endl;
    iset.erase(begin);

    begin = iset.begin();
    cout << *begin << ", " << endl;
    iset.erase(begin);

    cout << "size : " << iset.size() << endl;
}


// ={=========================================================================
// cxx-map

TEST(CxxStlTest, HowMapInsertWorks)
{
    map<unsigned int, string> pmap{ 
        {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}
    };

    // PRINT_M_ELEMENTS(pmap);

    pmap[3] = "threee";
    pmap[3] = "threeee";
    pmap[3] = "threeeee";
    pmap[3] = "threeeeee";

    ASSERT_THAT(pmap[3], Eq("threeeeee"));

    // PRINT_M_ELEMENTS(pmap);

    pmap.insert({3, "third"});
    pmap.insert({3, "thirdd"});
    pmap.insert({3, "thirddd"});
    pmap.insert({3, "thirdddd"});

    ASSERT_THAT(pmap[3], Eq("threeeeee"));
}


// ={=========================================================================
// cxx-map

// key 3.0 found!(3,2)
// value 3.0 found!(4,3)

MATCHER_P(EqPair, expected, "")
{
    return arg->first == expected.first && arg->second == expected.second;
}

TEST(CxxStlTest, HowMapFindWorks)
{
    map<float,float> coll{ {1,7}, {2,4}, {3,2}, {4,3}, {5,6}, {6,1}, {7,3} };

    auto posKey = coll.find(3.0);
    if( posKey != coll.end() )
    {
        // cout << "key 3.0 found!(" << posKey->first << "," << 
        //     posKey->second << ")" << endl;
        ASSERT_THAT(posKey, EqPair(make_pair(3,2)));
        // ASSERT_THAT(posKey->first, Eq(3));
        // ASSERT_THAT(posKey->second, Eq(2));
    }


    // *algo-find-if-const* error if there is no const on predicate. 
    // since it's *cxx-algo-non-modifying* ?

    auto posVal = find_if( coll.cbegin(), coll.cend(),
            // [] ( const pair<float,float> &elem ) {
            // [] ( const map<float,float>::value_type &elem ) {
            [] ( const decltype(coll)::value_type &elem ) {
            return elem.second == 3.0;
            } );
    if( posVal != coll.end() )
    {
        // cout << "value 3.0 found!(" << posVal->first << "," << 
        //     posVal->second << ")" << endl;
        ASSERT_THAT(posVal->first, Eq(4));
        ASSERT_THAT(posVal->second, Eq(3));
    }
}


// ={=========================================================================
// cxx-unordered

TEST(CxxStlTest, UnorderedMapDuplicates)
{
  unordered_multiset<string> cities{"Braunschweig", "Hanover", "Frankfurt", "New York",
    "Chicago", "Toronto", "Paris", "Frankfurt"};

  for( const auto &elem : cities )
    cout << elem << ' ';

  cout << endl;

  // insert additional elements
  cities.insert({"London", "Munich", "Hanover", "Braunschweig"});

  for( const auto &elem : cities )
    cout << elem << ' ';

  cout << endl;
}


// ={=========================================================================
// cxx-list

void PrintLists(const list<int> &list_one, const list<int> &list_two)
{
    cout << "list 1: ";
    copy(list_one.cbegin(), list_one.cend(), ostream_iterator<int>(cout, " "));
    cout << endl << "list 2: ";
    copy(list_two.cbegin(), list_two.cend(), ostream_iterator<int>(cout, " "));
    cout << endl << endl;
}

TEST(CollList, UseSpliceOrMerge)
{
    list<int> list_one, list_two;

    for(int i=0; i < 6; ++i)
    {
        list_one.push_back(i);
        list_two.push_back(i);
    }

    EXPECT_THAT(list_one, ElementsAre(0, 1, 2, 3, 4, 5));
    EXPECT_THAT(list_two, ElementsAre(0, 1, 2, 3, 4, 5));

    // moves all elements of list_one before the pos of '3' element.
    list_two.splice(find(list_two.begin(), list_two.end(), 3), list_one);
    EXPECT_EQ(list_one.size(), 0); 
    // 0 1 2 [0 1 2 3 4 5] 3 4 5 
    EXPECT_THAT(list_two, ElementsAreArray({0, 1, 2, 0, 1, 2, 3, 4, 5, 3, 4, 5})); 

    // move first element of list_two to the end
    list_two.splice(list_two.end(), list_two, list_two.begin());
    // 1 2 [0 1 2 3 4 5] 3 4 5 0 
    EXPECT_THAT(list_two, ElementsAreArray({1, 2, 0, 1, 2, 3, 4, 5, 3, 4, 5, 0})); 

    list<int> list_three;
    // move first element of list_two to the first of list_three
    list_three.splice(list_three.begin(), list_two, list_two.begin());

    EXPECT_THAT(list_two, ElementsAreArray({2, 0, 1, 2, 3, 4, 5, 3, 4, 5, 0})); 
    EXPECT_THAT(list_three, ElementsAre(1)); 

    list<int> list4{0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
    list<int> list5{0, 1, 2, 3, 4, 5};
    
    // 8.8.1 Special Member Functions for Lists (and Forward Lists)
    //
    // Strictly speaking, the standard requires that both (forward) lists be
    // sorted on entry. In practice, however, merging is also possible for
    // unsorted lists.  However, you should check this before you rely on it.
    // 
    // So will be sorted?
    //
    // list<int> list5;
    // list5.merge({4, 1, 0, 3, 2, 5});
    // EXPECT_THAT(list5, ElementsAreArray({0, 1, 2, 3, 4, 5}));
    //
    // NO and failed on gcc.

    list5.merge(list4);
    EXPECT_THAT(list5, ElementsAreArray({0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5}));
}


TEST(CollList, UseListRemoveIf)
{
  list<int> int_list{0, 1, 2, 3, 4};

  int_list.remove_if([](int e)
      { 
        if (e > 2) 
          return true;

        return false;
      });

  EXPECT_THAT(int_list, ElementsAre(0, 1, 2));

  int_list.remove_if([](int e)
      { return e % 2 == 0; });

  EXPECT_THAT(int_list, ElementsAre(1));
}


// ={=========================================================================
// stl-swap
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

TEST(StlSwap, Swaps)
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
    swap(*one, *two);
    EXPECT_THAT(coll, ElementsAre(2, 1, 3, 4, 5, 6, 7, 8, 9));

    one = coll.begin();
    two = one+8;
    swap(*one, *two);
    EXPECT_THAT(coll, ElementsAre(9, 1, 3, 4, 5, 6, 7, 8, 2));
  }

  // use operator*()
  {
    vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

    vector<int>::iterator one, two;
    one = coll.begin();
    two = one+1;
    std::swap(*one, *two);
    EXPECT_THAT(coll, ElementsAre(2, 1, 3, 4, 5, 6, 7, 8, 9));

    one = coll.begin();
    two = one+8;
    std::swap(*one, *two);
    EXPECT_THAT(coll, ElementsAre(9, 1, 3, 4, 5, 6, 7, 8, 2));
  }
}


// ={=========================================================================
// cxx-algo-copy

TEST(AlgoCopy, UseOnDifferentCollections)
{
  vector<int> coll{1,2,3,4,5,6};
  EXPECT_THAT(coll, ElementsAre(1,2,3,4,5,6));

  list<int> coll1;
  
  copy(coll.begin(), coll.end(), inserter(coll1, coll1.begin()));
  EXPECT_THAT(coll1, ElementsAre(1,2,3,4,5,6));
}


// ={=========================================================================
// cxx-algo-generate cxx-random

// 16807, 282475249, 1622650073, 984943658, 1144108930, 470211272, 101027544, 1457850878, 1458777923, 2007237709, 823564440, 1115438165, 1784484492, 74243042, 114807987, 1137522503, 1441282327, 16531729,
//  0,  0,  5,  3,  3,  1,  0,  4,  4,  6,  2,  3,  5,  0,  0,  3,  4,  0,
//  8,  6,  8, 10, 10, 12, 11,  9,  6, 10,  8, 10, 12, 11,  7,  6, 11,  8,

TEST(AlgoRandom, UseRandomEngineAndDistribution)
{
  default_random_engine dre;

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


// 0 3 18 11 13 5 1 16 16 23 9 12 (12)
// 20 0 1 13 16 0 9 1 10 17 14 23 (12)
// 1 0 1 1 1 1 0 0 1 1 0 1 (12)
// 0 1 1 0 0 0 0 0 1 0 1 1 (12)
// 8 8 8 9 9 9 7 8 8 8 7 9 (12)

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

class CardSequenceUseRandom
{
  public:
    int operator() () {
      return udist(dre);
    }

  private:
    static default_random_engine dre;
    static uniform_int_distribution<size_t> udist;
};


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


default_random_engine CardSequenceUseRandom::dre;
uniform_int_distribution<size_t> CardSequenceUseRandom::udist{0, 24};

TEST(AlgoRandom, UseGenerate)
{
    vector<uint32_t> ivec1;
    generate_n( back_inserter(ivec1), 12, CardSequenceUseRandom() );
    PRINT_ELEMENTS(ivec1);

    vector<uint32_t> ivec2;
    generate_n( back_inserter(ivec2), 12, CardSequenceUseRandom() );
    PRINT_ELEMENTS(ivec2);

    vector<uint32_t> ivec3;
    generate_n( back_inserter(ivec3), 12, CardSequenceUseRand(2) );
    PRINT_ELEMENTS(ivec3);

    vector<uint32_t> ivec4;
    generate_n( back_inserter(ivec4), 12, CardSequenceUseRand(2) );
    PRINT_ELEMENTS(ivec4);

    vector<uint32_t> ivec5;
    generate_n(back_inserter(ivec5), 12, 
        CardSequenceUseRandWithRange(6, 9));
    PRINT_ELEMENTS(ivec5);
}


// ={=========================================================================
// cxx-algo-accumulate
//
// 11.11 Numeric Algorithms
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
//
// sum: 45
// sum: -55
// product: 362880
// product: 0

TEST(AlgoAccumulate, Use)
{
    vector<int> coll;

    INSERT_ELEMENTS( coll, 1, 9 );

    // PRINT_ELEMENTS( coll );
    // 
    // cout << "sum: "
    //     << accumulate( coll.cbegin(), coll.cend(), 0 ) << endl;
    //
    // cout << "sum: "
    //     << accumulate( coll.cbegin(), coll.cend(), -100 ) << endl;
    //
    // cout << "product: "
    //     << accumulate( coll.cbegin(), coll.cend(), 1, multiplies<int>()) << endl;
    //
    // cout << "product: "
    //     << accumulate( coll.cbegin(), coll.cend(), 0, multiplies<int>()) << endl;

    ASSERT_THAT( accumulate( coll.cbegin(), coll.cend(), 0 ), Eq(45) );
    ASSERT_THAT( accumulate( coll.cbegin(), coll.cend(), -100 ), Eq(-55) );

    ASSERT_THAT( accumulate( coll.cbegin(), coll.cend(), 1, multiplies<int>() ), Eq(362880) );
    ASSERT_THAT( accumulate( coll.cbegin(), coll.cend(), 0, multiplies<int>() ), Eq(0) );
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


// ={=========================================================================
// cxx-algo-for-each cxx-algo-transform

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

TEST(AlgoForEachAndTransform, Use)
{
  // value
  {
    set<int> coll{1, 2, 3, 4, 5, 6, 7, 8};
    for_each(coll.begin(), coll.end(), square_value_no_return);
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

  // transform() differs in that it uses `dest`
  {
    vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};
    vector<int> result;
    transform(coll.begin(), coll.end(), back_inserter(result), square_value_with_return);
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
    EXPECT_THAT(result, ElementsAre(1, 4, 9, 16, 25, 36, 49, 64));
  }
}


// ={=========================================================================
// cxx-algo-unique


// input: 1   2  3            4
// input: 1 2 3 4


TEST(AlgoUnique, Use)
{
  // • Both forms collapse consecutive equal elements by removing the following
  // duplicates.

  // • The first form removes from the range [beg,end) all elements that are equal
  // to the previous elements. Thus, only when the elements in the sequence are
  // sorted, or at least when all elements of the same value are adjacent, does it
  // remove all duplicates.

  {
    int source[] = {1, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 5, 7, 5, 4, 4};
    list<int> coll;
    copy(begin(source), end(source), back_inserter(coll));

    auto pos = unique(coll.begin(), coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 6, 1, 2, 3, 1, 6, 5, 7, 5, 4, 5, 7, 5, 4, 4}));

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 6, 1, 2, 3, 1, 6, 5, 7, 5, 4}));
    // 1 4 4 6 6 6 6 7
  }

  // • The second form removes all elements that follow an element e and for
  // which the binary predicate op(e,elem) yields true. In other words, the
  // predicate is not used to compare an element with its predecessor; the
  // element is compared with the previous element that was not removed (see the
  // following examples).

  // For example, the first 6 is greater than the following 1, 2, 2, 3, and 1,
  // so all these elements are removed. In other words, the predicate is not
  // used to compare an element with its predecessor; the element is compared
  // with the previous element that was not removed 
  {
    list<int> coll{1, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 5, 7, 5, 4, 4};

    auto pos = unique(coll.begin(), coll.end(), greater<int>());
    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 4, 6, 6, 6, 6, 7}));
  }

  {
    string input{"1   2  3            4           "};
    EXPECT_THAT(input, "1   2  3            4           ");

    auto new_end = unique(input.begin(), input.end(), [](const char &x, const char &y) {
      return x == y and x == ' ';
    });

    input.erase(new_end, input.end());
    EXPECT_THAT(input, "1 2 3 4 ");
  }
}


// ={=========================================================================
// cxx-algo-remove

// remove value 2 from coll
TEST(AlgoRemove, UseErase)
{
  std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

  for (auto it = coll.begin(); it != coll.end(); ++it)
  {
    if (*it == 2)
      it = coll.erase(it);
  }

  EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
}

TEST(AlgoRemove, UseRemoveAndErase)
{
  std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

  auto end = remove(coll.begin(), coll.end(), 2);

  EXPECT_THAT(distance(end, coll.end()), 4);

  coll.erase(end, coll.end());

  EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
}

// show that algo-remove() do not remove elements
TEST(AlgoRemove, RemoveDoNotRemove)
{
  std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

  remove(coll.begin(), coll.end(), 2);

  // coll{1,2,3,4,5,6,2,7,2,8,2,9};
  // coll{1,3,4,5,6,7,8,9,2,8,2,9};

  EXPECT_THAT(coll, ElementsAreArray({1,3,4,5,6,7,8,9,2,8,2,9}));
}

// show that remove_if() returns end if not found
TEST(AlgoRemove, UseRemoveIf)
{
  std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

  auto it = remove_if(coll.begin(), coll.end(), [](int e)
      {
        if (e == 10)
          return true;
        return false;
      });

  EXPECT_THAT(it, coll.end());
}

// implement own remove()
using ITERATOR = std::vector<int>::iterator;

ITERATOR my_remove(ITERATOR begin, ITERATOR end, int value)
{
  // move up to the first which matches to the value
  // ITERATOR first = find(begin, end, value);

  // if not use find()
  ITERATOR sub_list_end;
  for (sub_list_end = begin; sub_list_end != end; ++sub_list_end)
    if (*sub_list_end == value)
      break;

  // the key is that remvove() partitions the input into two lists; a list that
  // are not equal to the value and a list that are equals to the value. 
  //
  // [0, sub_list_end) ... end)
  
  ITERATOR runner = sub_list_end;
  for (++runner; runner != end; ++runner)
    if (*runner != value) 
    {
      *sub_list_end = *runner;
      ++sub_list_end;
    }

  return sub_list_end;
}

TEST(AlgoRemove, UseOwnRemove)
{
  std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

  auto end = my_remove(coll.begin(), coll.end(), 2);

  EXPECT_THAT(distance(end, coll.end()), 4);

  coll.erase(end, coll.end());

  EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
}


// ={=========================================================================
// cxx-algo-partition

// coll1: 1 2 3 4 5 6 7 8 9 (9)
// coll1: 8 2 6 4 5 3 7 1 9 (9)
//                ^ first odd element: 5
//
// coll2: 1 2 3 4 5 6 7 8 9 (9)
// coll2: 2 4 6 8 1 3 5 7 9 (9)
//                ^ first odd element: 1

TEST(AlgoPartition, Use)
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

  // algo-partition retuns returns an iterator to the first element where the
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


using PortfolioIterator = vector<unsigned int>::iterator;

PortfolioIterator RearrangeByQuantity(PortfolioIterator begin,
    PortfolioIterator end, unsigned int max_quanity)
{
  // how to get T of coll such as algo-remove? here, assumes that we know T
  vector<unsigned int> coll;

  PortfolioIterator start = begin;
  PortfolioIterator current{};

  // one pass to filter <=

  for (; start != end; ++start)
  {
    // not use push_back() since void push_back()
    if (*start <= max_quanity)
      current = coll.insert(coll.end(), *start);
  }

  start = begin;

  // second pass to filter >

  for (; start != end; ++start)
  {
    if (*start > max_quanity)
      coll.push_back(*start);
  }

  // copy it back
  copy(coll.begin(), coll.end(), begin);

  // here try to increase end() which is current
  //
  // /usr/include/c++/6/debug/safe_iterator.h:298:
  // Error: attempt to increment a singular iterator.
  // 
  // Objects involved in the operation:
  //     iterator "this" @ 0x0x7ffdeb5ea9a0 {
  //       type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<unsigned int*, std::__cxx1998::vector<unsigned int, std::allocator<unsigned int> > >, 
  //          std::__debug::vector<unsigned int, std::allocator<unsigned int> > > (mutable iterator);
  //       state = singular;
  //       references sequence with type 'std::__debug::vector<unsigned int, std::allocator<unsigned int> >' @ 0x0x7ffdeb5eaa00
  //     }
  // Aborted
  //
  // return ++current;

  // this causes the issue as below
  return current;
}


TEST(AlgoPartition, UseOwnPartitionTwoPass)
{
  vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

  RearrangeByQuantity(coll.begin(), coll.end(), 25);

  // 43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23,
  // 6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,37,48,26,41,30,
  //                                ^^

  EXPECT_THAT(coll, ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,37,48,26,41,30}));

  // this now fails since `current` is iterator of internal coll but not input
  // call. Have to work out one.
  // EXPECT_THAT(*iter, 43);
}


// unlike remove(), have to keep unmatched item as well

PortfolioIterator my_partition_one(PortfolioIterator begin,
    PortfolioIterator end, unsigned int max_quanity)
{
  PortfolioIterator first = begin;
  for (; first != end; ++first)
    if (*first > max_quanity)
      break;

  PortfolioIterator runner = first;
  ++runner;
  for (; runner != end; ++runner)
  {
    // if item matches to condition
    if (*runner <= max_quanity)
    {
      std::iter_swap(runner, first);
      ++first;
    }
  }

  return first;
}

// same as algo-partition /usr/include/c++/4.9.2/bits/stl_algo.h

PortfolioIterator my_partition_two(PortfolioIterator begin,
    PortfolioIterator end, unsigned int max_quanity)
{
  PortfolioIterator first = begin;
  while (*first <= max_quanity)
    if (++first == end)
      return first;

  PortfolioIterator runner = first;
  while (++runner != end)
  {
    // if item matches to condition
    if (*runner <= max_quanity)
    {
      std::iter_swap(runner, first);
      ++first;
    }
  }

  return first;
}

TEST(AlgoPartition, UseOwnPartitionOnePass)
{
  {
    vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

    PortfolioIterator iter = my_partition_one(coll.begin(), coll.end(), 25);

    EXPECT_THAT(coll, ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,26,41,30,37,48}));
    EXPECT_THAT(distance(coll.begin(), iter), 11);
    EXPECT_THAT(*iter, 43);
  }
  {
    vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

    PortfolioIterator iter = my_partition_two(coll.begin(), coll.end(), 25);

    EXPECT_THAT(coll, ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,26,41,30,37,48}));
    EXPECT_THAT(distance(coll.begin(), iter), 11);
    EXPECT_THAT(*iter, 43);
  }
}


// note: why different order from partition() when use the same logic?

TEST(AlgoPartition, UsePartitionToCompare)
{
  vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

  PortfolioIterator iter = partition(coll.begin(), coll.end(), [](int e)
      {
        if (e <= 25)
          return true;
        return false;
      }
      );

  // EXPECT_THAT(coll, ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,26,41,30,37,48}));
  // EXPECT_THAT(*iter, 43);
  EXPECT_THAT(coll, ElementsAreArray({23,6,11,6,13,23,21,19,20,15,24,48,37,34,29,26,41,30,42,43}));
  EXPECT_THAT(distance(coll.begin(), iter), 11);
  EXPECT_THAT(*iter, 48);
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
// algo-rotate

TEST(AlgoRotate, Rotate)
{
  vector<int> coll{1,2,3,4,5,6,7,8};

  // rotate one to the left
  auto pos = rotate(
    coll.begin(),     // begin  
    coll.begin()+1,   // new begin
    coll.end()        // end
  );
  EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));

  // return the new position of the (pervious) first element.
  EXPECT_THAT(*pos, 1);

  // rotate two to the right or think that rotate to the left since `no direction` 
  // in the call definition.
  // 
  // from stl
  //  *  This effectively swaps the ranges @p [__first,__middle) and
  //  *  @p [__middle,__last).

  pos = rotate(
    coll.begin(),
    coll.end()-2,
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(8,1,2,3,4,5,6,7));
  EXPECT_THAT(*pos, 2);

  // rotate so that 4 is the beginning
  pos = rotate(
    coll.begin(),
    find(coll.begin(), coll.end(), 4),
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(4,5,6,7,8,1,2,3));
  EXPECT_THAT(*pos, 8);
}

using ROTATE_ITER = vector<int>::iterator;

void my_rotate(ROTATE_ITER begin, ROTATE_ITER new_begin, ROTATE_ITER end)
{
  auto num_swap = distance(new_begin, end);
  for (; new_begin != begin; --new_begin)
  {
    auto start = new_begin;
    for (int i = 0; i < num_swap; ++i)
    {
      swap(*start, *(start-1));
      ++start;
    }
  }
}

// /// This is a helper function for the rotate algorithm.
// template<typename _ForwardIterator>
//   _ForwardIterator
//   __rotate(_ForwardIterator __first,
//      _ForwardIterator __middle,
//      _ForwardIterator __last,
//      forward_iterator_tag)
// {}
//
// /// This is a helper function for the rotate algorithm.
// template<typename _RandomAccessIterator>
//   _RandomAccessIterator
//   __rotate(_RandomAccessIterator __first,
//      _RandomAccessIterator __middle,
//      _RandomAccessIterator __last,
//      random_access_iterator_tag)
// {}

TEST(AlgoRotate, OwnRotate)
{
  vector<int> coll{1,2,3,4,5,6,7,8};

  // rotate one to the left
  my_rotate(
    coll.begin(),     // begin  
    coll.begin()+1,   // new begin
    coll.end()        // end
  );
  EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));

  my_rotate(
    coll.begin(),
    coll.end()-2,
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(8,1,2,3,4,5,6,7));

  my_rotate(
    coll.begin(),
    find(coll.begin(), coll.end(), 4),
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(4,5,6,7,8,1,2,3));
}


// ={=========================================================================
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


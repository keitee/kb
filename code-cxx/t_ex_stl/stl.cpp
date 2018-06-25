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
void PRINT_Q_ELEMENTS(const T& coll, const string &optsrt = "")
{
  size_t count = coll.size();

  for(int i = 0; i < count; ++i)
    cout << coll.top() << " ";

  cout << "(" << count << ")" << endl;
}

// ={=========================================================================
// cxx-vector

// 0 1 2 3 4 5 6 7 8 9 (10)
// size: 10
// 0 2 3 4 5 6 7 8 9 (9)
// size: 9
// 0 3 4 5 6 7 8 9 (8)
// size: 8

// 0 1 2 3 4 5 6 7 8 9 (10)
// 1 3 5 7 9 (5)

TEST(DISABLED_StlVector, VecorEraseChangesEnd)
{
  vector<int> ivec;
  INSERT_ELEMENTS(ivec, 0, 9);
  PRINT_ELEMENTS(ivec);
  cout << "size: " << ivec.size() << endl;

  auto it = ivec.begin()+1;

  ivec.erase(it);
  PRINT_ELEMENTS(ivec);
  cout << "size: " << ivec.size() << endl;

  ivec.erase(it);
  PRINT_ELEMENTS(ivec);
  cout << "size: " << ivec.size() << endl;

  vector<int> ivec2{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  PRINT_ELEMENTS(ivec2);

  // in every iteration, update it which is invalidated after insert/erase.
  for(auto it = ivec2.begin(); it != ivec2.end(); /* no */)
  {
    // if see even values, remove it
    if(!(*it % 2))
      it = ivec2.erase(it);
    else
      ++it;
  }

  PRINT_ELEMENTS(ivec2);
}

// 0 1 2 3 4 5 6 7 8 9 (10)
// 1 1 3 3 5 5 7 7 9 9 (10)

TEST(StlVector, VectorInsertErase)
{
  vector<int> ivec{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  PRINT_ELEMENTS(ivec);

  // in every iteration, update it which is invalidated after insert/erase.
  for(auto it = ivec.begin(); it != ivec.end(); /* no */)
  {
    // if see odd values, repeat it in front of it.
    if(*it % 2)
    {
      it = ivec.insert(it, *it);
      it += 2;
    }
    else
      it = ivec.erase(it);
  }

  PRINT_ELEMENTS(ivec);
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

void FillVector(vector<int> &coll)
{
  for (int i = 0; i < 10; ++i)
    coll.insert(coll.end(), i);
}

// TEST(StlVector, CreateWithPreAllocationFromFunction)
// {
//   vector<int> coll;
//   FillVector(coll);

//   vector<bool> table1(coll.size());
//   vector<bool> table2(10);

//   EXPECT_THAT(table1.size(), 10);
//   EXPECT_THAT(table2.size(), 10);
// }

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
// cxx-deque
// case seg-fault

// TEST(CxxStlTest, HowDequeSupportEmpty)
// {
//     deque<int> iq;
// 
//     try {
//         auto e = iq.back();
//     }
//     catch(...)
//     {
//         cout << "exception" << endl;
//     }
// }


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
// iter-swap
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
// (1,2,3,4,5,6,7,8,9)
//  ^^^
// (2,1,3,4,5,6,7,8,9)
//  ^               ^
// (9,1,3,4,5,6,7,8,2)

TEST(StlIter, IterSwap)
{
  vector<int> coll;

  INSERT_ELEMENTS(coll, 1, 9);
  PRINT_ELEMENTS(coll);
  EXPECT_THAT(coll, ElementsAre(1,2,3,4,5,6,7,8,9));

  // std::iter_swap(coll.begin(), next(coll.begin()));
  std::iter_swap(coll.begin(), ++coll.begin());
  PRINT_ELEMENTS(coll);
  EXPECT_THAT(coll, ElementsAre(2,1,3,4,5,6,7,8,9));

  // std::iter_swap(coll.begin(), prev(coll.end()));
  std::iter_swap(coll.begin(), --coll.end());
  PRINT_ELEMENTS(coll);
  EXPECT_THAT(coll, ElementsAre(9,1,3,4,5,6,7,8,2));
}


// ={=========================================================================
// algo-equal

TEST(AlgoEqual, UseVariousCases)
{
  vector<string> vec1{"Charles", "in", "Charge"};
  vector<string> vec2{"Charles", "in", "charge"};

  // false
  cout << (vec1 == vec2 ? "true" : "false") << endl;

  // false
  cout << boolalpha 
    << equal(vec1.begin(), vec1.end(), vec2.begin()) << endl;

  string s1{"abcde"};
  string s2{"abcdf"};

  // Testing for “Less Than”
  //
  // Both forms return whether the elements in the range [beg1,end1) are
  // “lexicographically less than” the elements in the range [beg2,end2).

  cout << boolalpha
    // false
    << lexicographical_compare(s1.begin(), s1.end(), s1.begin(), s1.end()) 
    << endl
    // true
    << lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end()) 
    << endl;

  // Search the First Difference
  //
  // The first form returns the first two corresponding elements of range
  // [beg,end) and the range starting with cmpBeg that differ.
  // 
  // first mismatch = e second mismatch = f

  auto iter = mismatch(s1.begin(), s1.end(), s2.begin());
  cout << "first mismatch = " << *(iter.first) << endl;
  cout << "second mismatch = " << *(iter.second) << endl;
}


// ={=========================================================================
// cxx-algo-copy

// when use list<int>
// init: 1 2 3 4 5 6 (6)
// list: 1 2 3 4 5 6 (6)

TEST(CxxStlTest, UseAlgoCopy)
{
  vector<int> coll{1,2,3,4,5,6};

  PRINT_ELEMENTS(coll, "init: " );

  list<int> coll1;

  // error
  // list<string> coll1;
  
  copy( coll.begin(), coll.end(), inserter(coll1, coll1.begin()));

  PRINT_ELEMENTS(coll1, "list: " );
}


// ={=========================================================================
// cxx-algo-generate cxx-random

// 16807, 282475249, 1622650073, 984943658, 1144108930, 470211272, 101027544, 1457850878, 1458777923, 2007237709, 823564440, 1115438165, 1784484492, 74243042, 114807987, 1137522503, 1441282327, 16531729,
//  0,  0,  5,  3,  3,  1,  0,  4,  4,  6,  2,  3,  5,  0,  0,  3,  4,  0,
//  8,  6,  8, 10, 10, 12, 11,  9,  6, 10,  8, 10, 12, 11,  7,  6, 11,  8,

TEST(Random, UseRandomEngineAndDistribution)
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

TEST(Algo, UseGenerate)
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

TEST(CxxStlTest, UseAlgoAccumulate)
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

// coll1 (vector): 1 2 3 4 5 6 7 (7)
// coll2 (list)  : 3 4 5 6 7 8 9 (7)
// coll3 (list)  : 1 2 3 4 5 6 7 (7)

TEST(CxxStlTest, UseAlgoEqual)
{
    vector<int> coll1;
    list<int> coll2;
    list<int> coll3;

    INSERT_ELEMENTS(coll1, 1, 7);
    INSERT_ELEMENTS(coll2, 3, 9);
    INSERT_ELEMENTS(coll3, 1, 7);

    PRINT_ELEMENTS(coll1, "coll1 (vector): ");
    PRINT_ELEMENTS(coll2, "coll2 (list)  : ");
    PRINT_ELEMENTS(coll3, "coll3 (list)  : ");

    EXPECT_FALSE( equal( coll1.begin(), coll1.end(),
                coll2.begin()));

    EXPECT_TRUE( equal( coll1.begin(), coll1.end(),
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
// cxx-algo-partition

// coll1: 1 2 3 4 5 6 7 8 9 (9)
// coll1: 8 2 6 4 5 3 7 1 9 (9)
//                ^ first odd element: 5
//
// coll2: 1 2 3 4 5 6 7 8 9 (9)
// coll2: 2 4 6 8 1 3 5 7 9 (9)
//                ^ first odd element: 1

TEST(StlAlgo, UsePartition)
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

TEST(Algo, UseForEachAndTransform)
{
  set<int> coll1{1,2,3,4,5,6,7,8};
  for_each(coll1.begin(), coll1.end(), square_value_no_return);
  EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  // error since key in set are const
  // set<int> coll11{1,2,3,4,5,6,7,8};
  
  vector<int> coll11{1,2,3,4,5,6,7,8};
  for_each(coll11.begin(), coll11.end(), square_refer_no_return);
  EXPECT_THAT(coll11, ElementsAre(1, 4, 9, 16, 25, 36, 49, 64));

  vector<int> coll2;
  transform(coll1.begin(), coll1.end(), back_inserter(coll2), square_value_with_return);
  EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
  EXPECT_THAT(coll2, ElementsAre(1, 4, 9, 16, 25, 36, 49, 64));
}


// ={=========================================================================
// cxx-algo-unique
TEST(CxxStl, AlgoUnique)
{
  string input{"1   2  3            4           "};

  cout << "input: " << input << endl;

  auto new_end = unique(input.begin(), input.end(), [](const char &x, const char &y)
      {
        return x == y and x == ' ';
      });

  input.erase(new_end, input.end());

  cout << "input: " << input << endl;
}

// ={=========================================================================
// cxx-algo-remove

// remove value 2 from coll
TEST(StlAlgoRemove, UseErase)
{
  std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

  for (auto it = coll.begin(); it != coll.end(); ++it)
  {
    if (*it == 2)
      it = coll.erase(it);
  }

  EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
}

TEST(StlAlgoRemove, UseRemoveAndErase)
{
  std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

  auto end = remove(coll.begin(), coll.end(), 2);

  EXPECT_THAT(distance(end, coll.end()), 4);

  coll.erase(end, coll.end());

  EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
}

// show that algo-remove() do not remove elements
TEST(StlAlgoRemove, RemoveDoNotRemove)
{
  std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

  remove(coll.begin(), coll.end(), 2);

  // std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};
  // std::vector<int> coll{1,3,4,5,6,7,8,9,2,8,2,9};
  //                                       ^^^^^^^ 

  EXPECT_THAT(coll, ElementsAreArray({1,3,4,5,6,7,8,9,2,8,2,9}));
}

// show that remove_if() returns end if not found
TEST(StlAlgoRemove, UseRemoveIf)
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

// the key is to move items that are not equal to the value forward in the list
// and keeps the runner.
ITERATOR my_remove(ITERATOR begin, ITERATOR end, int value)
{
  // move up to the first which matches to the value
  // ITERATOR first = find(begin, end, value);

  // if not use find()
  ITERATOR first;
  for (first = begin; first != end; ++first)
    if (*first == value)
      break;

  // moves runner and move item which runner points to only when it see item
  // thatthat do not match since here we care about items that do not match.
  
  ITERATOR runner = first;
  ++runner;
  for (; runner != end; ++runner)
    if (*runner != value) 
    {
      *first = *runner;
      ++first;
    }

  return first;
}

TEST(StlAlgoRemove, UseOwnRemove)
{
  std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

  auto end = my_remove(coll.begin(), coll.end(), 2);

  EXPECT_THAT(distance(end, coll.end()), 4);

  coll.erase(end, coll.end());

  EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
}

// ={=========================================================================

using PortfolioIterator = vector<unsigned int>::iterator;

PortfolioIterator RearrangeByQuantity(PortfolioIterator begin,
    PortfolioIterator end, unsigned int max_quanity)
{
  // how to get T of coll such as algo-remove? here, assumes that we know T
  vector<unsigned int> coll;

  PortfolioIterator start = begin;
  PortfolioIterator current{};

  for (; start != end; ++start)
  {
    // not use push_back() since void push_back()
    if (*start <= max_quanity)
      current = coll.insert(coll.end(), *start);
  }

  start = begin;

  for (; start != end; ++start)
  {
    if (*start > max_quanity)
      coll.push_back(*start);
  }

  // copy it back
  copy(coll.begin(), coll.end(), begin);

  return ++current;
}


TEST(StlAlgoPartition, UseOwnPartitionTwoPass)
{
  vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

  PortfolioIterator iter = RearrangeByQuantity(coll.begin(), coll.end(), 25);

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
  // move up to the first which *do not* matches to the condition
  
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

TEST(StlAlgoPartition, UseOwnPartitionOnePass)
{
  vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

  PortfolioIterator iter = my_partition_one(coll.begin(), coll.end(), 25);

  // 43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23,
  // 6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,37,48,26,41,30,
  //                                ^^

  // see different order to the second pass one
  // EXPECT_THAT(coll, ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,37,48,26,41,30}));
  EXPECT_THAT(coll, ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,26,41,30,37,48}));
  EXPECT_THAT(distance(coll.begin(), iter), 11);
  EXPECT_THAT(*iter, 43);
}

// same as algo-partition
PortfolioIterator my_partition_two(PortfolioIterator begin,
    PortfolioIterator end, unsigned int max_quanity)
{
  // move up to the first which *do not* matches to the condition
  
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

TEST(StlAlgoPartition, UseOwnPartitionOnePassTwo)
{
  vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

  PortfolioIterator iter = my_partition_two(coll.begin(), coll.end(), 25);

  EXPECT_THAT(coll, ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,26,41,30,37,48}));
  EXPECT_THAT(distance(coll.begin(), iter), 11);
  EXPECT_THAT(*iter, 43);
}

// note: why different order?

TEST(StlAlgoPartition, UsePartition)
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
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


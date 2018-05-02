#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <deque>
#include <list>

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
// cxx-list

void PrintLists(const list<int> &list_one, const list<int> &list_two)
{
    cout << "list 1: ";
    copy(list_one.cbegin(), list_one.cend(), ostream_iterator<int>(cout, " "));
    cout << endl << "list 2: ";
    copy(list_two.cbegin(), list_two.cend(), ostream_iterator<int>(cout, " "));
    cout << endl << endl;
}

// list 0: 0 1 2 3 4 5 
// list 2: 0 1 2 3 4 5 
//
// list 1: 
// list 2: 0 1 2 [0 1 2 3 4 5] 3 4 5 
//
// list 1: 
// list 2: 1 2 [0 1 2 3 4 5] 3 4 5 0 
//
// list 1: 
// list 2: 2 0 1 2 3 4 5 3 4 5 0 
// list 3: 1 

TEST(CxxStlTest, UseListMemberFuntions)
{
    list<int> list_one, list_two;

    for(int i=0; i < 6; ++i)
    {
        list_one.push_back(i);
        list_two.push_back(i);
    }

    PrintLists(list_one, list_two);

    // moves all elements of list_one before the pos of '3' element.
    list_two.splice(find(list_two.begin(), list_two.end(), 3), list_one);
    PrintLists(list_one, list_two);

    // move first element of list_two to the end
    list_two.splice(list_two.end(), list_two, list_two.begin());
    PrintLists(list_one, list_two);

    list<int> list_three;
    // move first element of list_two to the first of list_three
    list_three.splice(list_three.begin(), list_two, list_two.begin());
    PrintLists(list_one, list_two);
    cout << "list 3: ";
    copy(list_three.begin(), list_three.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
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
// cxx-random

// 0, 1, 9, 5, 6, 2, 0, 8, 8, 12, 4, 6, 10, 0, 0, 6, 8, 0, 4, 0, 5, 8, 7, 12, 11, 6, 1, 8, 5, 9, 

TEST(CxxStlTest, UseRandom)
{
  default_random_engine dre;
  uniform_int_distribution<unsigned> udist(0, 12);

  for(size_t i = 0; i < 30; ++i)
    cout << udist(dre) << ", ";
  cout << endl;
}

unsigned GetNext(int size)
{
  static default_random_engine dre;
  static uniform_int_distribution<unsigned> udist(0, size);
  return udist(dre);
  // return rand()%12;
}

TEST(CxxStlTest, UseRandomForGettingIndex)
{
  for(int i = 0; i < 6; ++i)
    cout << "1: " << GetNext(5) << endl;

  for(int i = 0; i < 7; ++i)
    cout << "2: " << GetNext(6) << endl;

  for(int i = 0; i < 8; ++i)
    cout << "3: " << GetNext(7) << endl;

  for(int i = 0; i < 9; ++i)
    cout << "4: " << GetNext(8) << endl;
}


// ={=========================================================================
// cxx-algo-generate

// 1804289383 846930886 1681692777 1714636915 1957747793 424238335 719885386 1649760492 596516649 1189641421 1025202362 1350490027 (12)
// 2 19 11 22 12 18 4 16 11 8 15 21 (12)

class CardSequence
{
    public:
        int operator() () {
            return rand() % 24;
        }
};

TEST(CxxStlTest, UseAlgoGenerate)
{
    vector<uint32_t> ivec1;
    generate_n( back_inserter(ivec1), 12, rand );
    PRINT_ELEMENTS(ivec1);

    vector<uint32_t> ivec2;
    generate_n( back_inserter(ivec2), 12, CardSequence() );
    PRINT_ELEMENTS(ivec2);
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
// first odd element: 5
// coll2: 1 2 3 4 5 6 7 8 9 (9)
// coll2: 2 4 6 8 1 3 5 7 9 (9)
// first odd element: 1

TEST(CxxStlTest, UseAlgoPartition)
{
    vector<int> coll1;
    vector<int> coll2;

    INSERT_ELEMENTS(coll1, 1, 9);
    PRINT_ELEMENTS(coll1, "coll1: ");

    vector<int>::iterator pos1, pos2;
    pos1 = partition(coll1.begin(), coll1.end(),    // range
                [](int elem)
                {
                    return elem %2 == 0;
                });
    PRINT_ELEMENTS(coll1, "coll1: ");
    cout << "first odd element: " << *pos1 << endl;

    INSERT_ELEMENTS(coll2, 1, 9);
    PRINT_ELEMENTS(coll2, "coll2: ");

    pos2 = stable_partition(coll2.begin(), coll2.end(),
                [](int elem)
                {
                    return elem %2 == 0;
                });
    PRINT_ELEMENTS(coll2, "coll2: ");
    cout << "first odd element: " << *pos2 << endl;
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


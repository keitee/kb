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
// cxx-algo-generate

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

    EXPECT_TRUE( equal( coll1.begin(), coll1.end(),
                coll2.begin(), [](int lhs, int rhs)
                {
                    return lhs 
                }
                ));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


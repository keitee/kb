#include <iostream>
#include <set>
#include <vector>
#include <memory>

#include "gtest/gtest.h"

using namespace std;

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

TEST(CxxStlTest, AlgoGenerate)
{
    vector<uint32_t> ivec1;
    generate_n( back_inserter(ivec1), 12, rand );
    PRINT_ELEMENTS(ivec1);

    vector<uint32_t> ivec2;
    generate_n( back_inserter(ivec2), 12, CardSequence() );
    PRINT_ELEMENTS(ivec2);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


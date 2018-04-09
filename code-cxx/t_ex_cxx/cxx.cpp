#include <iostream>
#include <set>
#include <vector>
#include <memory>
// #include <typeinfo>

#include "gtest/gtest.h"

using namespace std;

// ={=========================================================================
// cxx-pair

TEST(CxxFeaturesTest, UsePairType)
{
    const auto map{
        make_pair(10, "X"),
        make_pair(9, "IX"),
        make_pair(5, "V")
    };

    for(const auto &e : map)
        cout << "{" << e.first << ", " << e.second << "}" << endl;

    cout << "typeid : " << typeid(map).name() << endl;
}


// ={=========================================================================
// cxx-enum

enum class EnumFlags :char { SPORT=1, KIDS=2, MUSIC=4 };

constexpr EnumFlags operator| (EnumFlags lhs, EnumFlags rhs)
{
    // C++PL 220
    // explicit converison is nessary since enum class does not support implicit
    // conversions.
    return static_cast<EnumFlags>(static_cast<char>(lhs)|static_cast<char>(rhs));
}

constexpr EnumFlags operator& (EnumFlags lhs, EnumFlags rhs)
{
    return static_cast<EnumFlags>(static_cast<char>(lhs)&static_cast<char>(rhs));
}

class ScopedEnum {
    public:
        int checkFlags(EnumFlags flag)
        {
            int result{};

            switch(flag)
            {
                // used constexpr on oeprator| and & since someone might want to
                // use them in constant expression.
                case EnumFlags::SPORT:
                    cout << "has sport flag" << endl;
                    result = 0;
                    break;

                case EnumFlags::KIDS:
                    cout << "has kids flas" << endl;
                    result = 1;
                    break;

                case EnumFlags::MUSIC:
                    cout << "has music flag" << endl;
                    result = 2;
                    break;

                // warning: case value ‘5’ not in enumerated type ‘EnumFlags’ [-Wswitch]
                case EnumFlags::SPORT|EnumFlags::MUSIC:
                    cout << "has sport and music flag" << endl;
                    result = 3;
                    break;

                default:
                    cout << "has unknown flag" << endl;
                    result = 100;
            }

            return result;
        }
};


TEST(CxxFeaturesTest, UseScopedEnum)
{
    ScopedEnum scoped;

    EXPECT_EQ(0, scoped.checkFlags(EnumFlags::SPORT));
    EXPECT_EQ(1, scoped.checkFlags(EnumFlags::KIDS));
    EXPECT_EQ(2, scoped.checkFlags(EnumFlags::MUSIC));
    EXPECT_EQ(3, scoped.checkFlags(EnumFlags::SPORT|EnumFlags::MUSIC));
    EXPECT_EQ(100, scoped.checkFlags(EnumFlags::SPORT|EnumFlags::KIDS));
    // EXPECT_EQ(100, scoped.checkFlags(200));
}


// ={=========================================================================
// cxx-enum, unscoped enum, enum hack

enum color { red, yellow, green };          // unscoped
enum class peppers { red, yellow, green };  // scoped

TEST(CxxFeaturesTest, UseEnumHack)
{
    int value_1 = color::yellow;

    // error: cannot convert ‘peppers’ to ‘int’ in initialization
    // int value_2 = peppers::red;

    color set_1 = yellow;

    // error: invalid conversion from ‘int’ to ‘color’ [-fpermissive]
    // color set_2 = 2;
    
    EXPECT_EQ(value_1, set_1);
}


// ={=========================================================================
// cxx-rvo

struct Snitch {   // Note: All methods have side effects
    Snitch(int value): value_(value) { cout << "c'tor" << endl; }
    ~Snitch() { cout << "d'tor" << endl; }

    Snitch(const Snitch&) { cout << "copy c'tor" << endl; }
    Snitch(Snitch&&) { cout << "move c'tor" << endl; }

    Snitch& operator=(const Snitch&) {
        cout << "copy assignment" << endl;
        return *this;
    }

    Snitch& operator=(Snitch&&) {
        cout << "move assignment" << endl;
        return *this;
    }

    int getValue() const { return value_;}

    private:
        int value_{0};
};

Snitch ExampleRVO() {

  Snitch sn(100);

  cout << "in example rvo: " << sn.getValue() << endl;

  return sn;
}

TEST(CxxFeaturesTest, UseRVO)
{
    cout << "----------" << endl;
    Snitch snitch = ExampleRVO();
    cout << "----------" << endl;
}


vector<Snitch> ReturnVector() {
    // vector<Snitch> ivec(1000000000, 1);
    // vector(n, elem); creates n elements
    vector<Snitch> ivec(10, Snitch(200));
    cout << "size of vector: " << ivec.size() << endl;
  return ivec;
}

TEST(CxxFeaturesTest, UseRVOReturnBigVector)
{
    cout << "----------" << endl;
    vector<Snitch> ivec = ReturnVector();
    cout << "----------" << endl;
}

void foo(Snitch s) {
    cout << "snitch value is: " << s.getValue() << endl;
}

TEST(CxxFeaturesTest, UseCopyElison)
{
    cout << "----------" << endl;
    foo(Snitch(200));
    cout << "----------" << endl;
}

Snitch createSnitch() {
    return Snitch(200);
}

TEST(CxxFeaturesTest, UseAssignment)
{
    cout << "----------" << endl;
    Snitch s = createSnitch();
    cout << "----------" << endl;
    s = createSnitch();
    cout << "----------" << endl;
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// [==========] Running 1 test from 1 test case.
// [----------] Global test environment set-up.
// [----------] 1 test from BulkQuote1Test
// [ RUN      ] BulkQuote1Test.checkTotal1
// has sport flag
// has kids flas
// has music flag
// has sport and music flag
// has unknown flag
// [       OK ] BulkQuote1Test.checkTotal1 (1 ms)
// [----------] 1 test from BulkQuote1Test (1 ms total)
// 
// [----------] Global test environment tear-down
// [==========] 1 test from 1 test case ran. (1 ms total)
// [  PASSED  ] 1 test.


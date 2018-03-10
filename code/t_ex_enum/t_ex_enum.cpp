#include <iostream>
#include <set>
#include <memory>

#include "gtest/gtest.h"

using namespace std;

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


// Quote sales which has no discount. 45*3 = 135
TEST(BulkQuote1Test, checkTotal1)
{
    ScopedEnum scoped;

    EXPECT_EQ(0, scoped.checkFlags(EnumFlags::SPORT));
    EXPECT_EQ(1, scoped.checkFlags(EnumFlags::KIDS));
    EXPECT_EQ(2, scoped.checkFlags(EnumFlags::MUSIC));
    EXPECT_EQ(3, scoped.checkFlags(EnumFlags::SPORT|EnumFlags::MUSIC));
    EXPECT_EQ(100, scoped.checkFlags(EnumFlags::SPORT|EnumFlags::KIDS));
    // EXPECT_EQ(100, scoped.checkFlags(200));
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


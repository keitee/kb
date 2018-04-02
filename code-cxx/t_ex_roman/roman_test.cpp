#include "gmock/gmock.h"
#include "roman.h"

using namespace testing;

class RomanConvertTest : public Test
{
public:
    RomanConvert converter;
};

// 1. the roman number has fixed mappings:
//
//  1       : I
//  5       : V
//  10      : X
//  50      : L
//  100     : C
//  500     : D
//  1000    : M
//
// 2. As itoa(), use loop, %, and / to get a digit to convert:
//  
//      tens    2   1   0
//              X   X   X
//      digit   D   D   D
//
// 3. Divide 0-9 into sub-groups to get:
//
//  * 0 < digit < 4:
//
//      tens = 0:
//          1       (1, 10**0)      I
//          2       (2, 10**0)      II
//          3       (3, 10**0)      III
//
//      tens = 1:
//          10      (1, 10**1)      X
//          20      (2, 10**1)      XX
//          30      (3, 10**1)      XXX
//
//      tens = 2:
//          100     (1, 10**2)      C
//          200     (2, 10**2)      CC
//          300     (3, 10**2)      CCC
//      ...
//
//      To use the same function as 5 < digit < 9 case:
//
//      tens = 0:
//                          (base, repeat, roman to repeat)
//          1, 0+1          (0*10**0, 1, 10**0)     I
//          2, 0+1          (0*10**0, 2, 10**0)     II
//          3, 0+1          (0*10**0, 3, 10**0)     III
//
//  * 5 < digit < 9:
//
//      tens = 0:
//                          (base, repeat, roman to repeat)
//          6, 5+1          (5*10**0, 1, 10**0)     VI
//          7, 5+2          (5*10**0, 2, 10**0)     VII
//          8, 5+3          (5*10**0, 3, 10**0)     VIII
//
//      tens = 1:
//          60, 50+10       (5*10**1, 1, 10**1)     LX
//          70, 50+20       (5*10**1, 2, 10**1)     LXX
//          89, 50+30       (5*10**1, 3, 10**1)     LXXX
//
//      tens = 2:
//          600, 500+100    (5*10**1, 1, 10**1)     DC
//          700, 500+200    (5*10**1, 2, 10**1)     DCC
//          890, 500+300    (5*10**1, 3, 10**1)     DCCC
//      ...
//
//  * 4 or 9
//
//      tens = 0:
//          4, 5-1          (10**0 + (4+1)*10**0)   IV
//          9, 10-1         (10**0 + (9+1)*10**0)   IX
//
//      tens = 1:
//          40, 50-10       (10**1 + (4+1)*10**1)   XL
//          90, 100-10      (10**1 + (9+1)*10**1)   XC
//      ...
//
//  * 5
//
//      tens = 0:
//          5,              (10**0*5)
//
//      tens = 1:
//          50              (10**1*5)
//
//      tens = 2:
//          500             (10**2*5)


TEST_F(RomanConvertTest, Convert_RomansFixed) {
    ASSERT_THAT(converter.convert(1), Eq("I"));
    ASSERT_THAT(converter.convert(5), Eq("V"));
    ASSERT_THAT(converter.convert(10), Eq("X"));
    ASSERT_THAT(converter.convert(50), Eq("L"));
    ASSERT_THAT(converter.convert(100), Eq("C"));
    ASSERT_THAT(converter.convert(500), Eq("D"));
    ASSERT_THAT(converter.convert(1000), Eq("M"));
}

TEST_F(RomanConvertTest, ConvertTensZero_RomansBiggerThan0LessThan4) {
    EXPECT_THAT(converter.convert(1), Eq("I"));
    EXPECT_THAT(converter.convert(2), Eq("II"));
    EXPECT_THAT(converter.convert(3), Eq("III"));
}

TEST_F(RomanConvertTest, ConvertTensZero_RomansBiggerThan5LessThan9) {
    EXPECT_THAT(converter.convert(6), Eq("VI"));
    EXPECT_THAT(converter.convert(7), Eq("VII"));
    EXPECT_THAT(converter.convert(8), Eq("VIII"));
}

TEST_F(RomanConvertTest, ConvertTensZero_RomansWhen4And9) {
    EXPECT_THAT(converter.convert(4), Eq("IV"));
    EXPECT_THAT(converter.convert(9), Eq("IX"));
}

TEST_F(RomanConvertTest, ConvertTensOne_RomansBiggerThan0LessThan4) {
    EXPECT_THAT(converter.convert(11), Eq("XI"));
    EXPECT_THAT(converter.convert(12), Eq("XII"));
    EXPECT_THAT(converter.convert(13), Eq("XIII"));
}

TEST_F(RomanConvertTest, ConvertTensOne_RomansBiggerThan5LessThan9) {
    EXPECT_THAT(converter.convert(16), Eq("XVI"));
    EXPECT_THAT(converter.convert(17), Eq("XVII"));
    EXPECT_THAT(converter.convert(18), Eq("XVIII"));
}

TEST_F(RomanConvertTest, ConvertTensOne_RomansWhen4And9) {
    EXPECT_THAT(converter.convert(41), Eq("XLI"));
    EXPECT_THAT(converter.convert(91), Eq("XCI"));
}

TEST_F(RomanConvertTest, ConvertTensOne_RomansWhenHas5) {
    EXPECT_THAT(converter.convert(45), Eq("XLV"));
    EXPECT_THAT(converter.convert(95), Eq("XCV"));
}

TEST_F(RomanConvertTest, ConvertTensAny_RomansAny) {
    EXPECT_THAT(converter.convert(122), Eq("CXXII"));
    EXPECT_THAT(converter.convert(152), Eq("CLII"));
    EXPECT_THAT(converter.convert(196), Eq("CXCVI"));
    EXPECT_THAT(converter.convert(247), Eq("CCXLVII"));
    EXPECT_THAT(converter.convert(298), Eq("CCXCVIII"));
}

// tests from tdd example
TEST_F(RomanConvertTest, CanConvertPositiveDigits) 
{
    EXPECT_THAT(converter.convert(1), Eq("I"));
    EXPECT_THAT(converter.convert(2), Eq("II"));
    EXPECT_THAT(converter.convert(3), Eq("III"));
    EXPECT_THAT(converter.convert(10), Eq("X"));
    EXPECT_THAT(converter.convert(11), Eq("XI"));
    EXPECT_THAT(converter.convert(12), Eq("XII"));
    EXPECT_THAT(converter.convert(13), Eq("XIII"));
    EXPECT_THAT(converter.convert(20), Eq("XX"));
    EXPECT_THAT(converter.convert(23), Eq("XXIII"));
    EXPECT_THAT(converter.convert(5), Eq("V"));

    EXPECT_THAT(converter.convert(50), Eq("L"));
    EXPECT_THAT(converter.convert(80), Eq("LXXX"));
    EXPECT_THAT(converter.convert(100), Eq("C"));
    EXPECT_THAT(converter.convert(288), Eq("CCLXXXVIII"));

    EXPECT_THAT(converter.convert(6), Eq("VI"));
    EXPECT_THAT(converter.convert(7), Eq("VII"));
    EXPECT_THAT(converter.convert(8), Eq("VIII"));

    EXPECT_THAT(converter.convert(4), Eq("IV"));

    EXPECT_THAT(converter.convert(5), Eq("V"));
    EXPECT_THAT(converter.convert(10), Eq("X"));
    EXPECT_THAT(converter.convert(11), Eq("XI"));
    EXPECT_THAT(converter.convert(12), Eq("XII"));
    EXPECT_THAT(converter.convert(13), Eq("XIII"));
    EXPECT_THAT(converter.convert(20), Eq("XX"));
    EXPECT_THAT(converter.convert(50), Eq("L"));
    EXPECT_THAT(converter.convert(80), Eq("LXXX"));
    EXPECT_THAT(converter.convert(100), Eq("C"));
    EXPECT_THAT(converter.convert(288), Eq("CCLXXXVIII"));
    EXPECT_THAT(converter.convert(2999), Eq("MMCMXCIX"));
    EXPECT_THAT(converter.convert(3447), Eq("MMMCDXLVII"));
    EXPECT_THAT(converter.convert(1513), Eq("MDXIII"));
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

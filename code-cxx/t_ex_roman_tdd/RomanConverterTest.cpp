//
// Appendix 2
// Code Kata: Roman Numeral Converter
//

#include "gmock/gmock.h"

using namespace ::testing;
using namespace std;

string convert(unsigned int arabic) 
{
    // What if we consider the Roman representation of four as a single digit,
    // despite  that it requires two of our alphabet’s characters (IV)?

    const auto arabicToRomanConversions = {
        make_pair(1000u, "M"), 
        make_pair(900u, "CM"), 
        make_pair(500u, "D"), 
        make_pair(400u, "CD"), 
        make_pair(100u, "C"),
        make_pair(90u, "XC"), 
        make_pair(50u, "L"), 
        make_pair(40u, "XL"), 
        make_pair(10u, "X"), 
        make_pair(9u, "IX"), 
        make_pair(5u, "V"), 
        make_pair(4u, "IV"), 
        make_pair(1u, "I") 
    };

    string roman{""};

    // while (arabic >= 10)
    // {
    //     roman += "X";
    //     arabic -= 10;
    // }
    //
    // while (arabic >= 1)
    // {
    //     roman += "I";
    //     arabic -= 1;
    // }

    for( auto arabicToRoman : arabicToRomanConversions )
    {
        while( arabic >= arabicToRoman.first )
        {
            roman += arabicToRoman.second;
            arabic -= arabicToRoman.first;
        }
    }

    return roman;
}

TEST(RomanConverter, CanConvertPositiveDigits) 
{
    EXPECT_THAT(convert(1), Eq("I"));
    EXPECT_THAT(convert(2), Eq("II"));
    EXPECT_THAT(convert(3), Eq("III"));
    EXPECT_THAT(convert(10), Eq("X"));
    EXPECT_THAT(convert(11), Eq("XI"));
    EXPECT_THAT(convert(12), Eq("XII"));
    EXPECT_THAT(convert(13), Eq("XIII"));
    EXPECT_THAT(convert(20), Eq("XX"));
    EXPECT_THAT(convert(23), Eq("XXIII"));
    EXPECT_THAT(convert(5), Eq("V"));

    EXPECT_THAT(convert(50), Eq("L"));
    EXPECT_THAT(convert(80), Eq("LXXX"));
    EXPECT_THAT(convert(100), Eq("C"));
    EXPECT_THAT(convert(288), Eq("CCLXXXVIII"));

    EXPECT_THAT(convert(6), Eq("VI"));
    EXPECT_THAT(convert(7), Eq("VII"));
    EXPECT_THAT(convert(8), Eq("VIII"));

    EXPECT_THAT(convert(4), Eq("IV"));

    EXPECT_THAT(convert(5), Eq("V"));
    EXPECT_THAT(convert(10), Eq("X"));
    EXPECT_THAT(convert(11), Eq("XI"));
    EXPECT_THAT(convert(12), Eq("XII"));
    EXPECT_THAT(convert(13), Eq("XIII"));
    EXPECT_THAT(convert(20), Eq("XX"));
    EXPECT_THAT(convert(50), Eq("L"));
    EXPECT_THAT(convert(80), Eq("LXXX"));
    EXPECT_THAT(convert(100), Eq("C"));
    EXPECT_THAT(convert(288), Eq("CCLXXXVIII"));
    EXPECT_THAT(convert(2999), Eq("MMCMXCIX"));
    EXPECT_THAT(convert(3447), Eq("MMMCDXLVII"));
    EXPECT_THAT(convert(1513), Eq("MDXIII"));
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

#include "gmock/gmock.h"
#include "soundex.h"

using namespace testing;

class SoundexEncoding : public Test
{
public:
    Soundex soundex;
};

TEST_F(SoundexEncoding, RetainsSoleLetterOfOneLetterWord)
{
    ASSERT_THAT(soundex.encode("A"), Eq("A000"));
}

// ??
TEST_F(SoundexEncoding, PadsWithZeros_ToEnsureThreeDigits)
{
    ASSERT_THAT(soundex.encode("I"), Eq("I000"));
}

TEST_F(SoundexEncoding, ReplaceConsonantsWithAppropriateDigits)
{
    ASSERT_THAT(soundex.encode("Ax"), Eq("A200"));
}
 
// [ RUN      ] SoundexEncoding.ReplacesMultipleConsonantsWithDigits
// soundextest.cpp:30: Failure
// Value of: soundex.encode("Acdl")
// Expected: is equal to 0x44ce95 pointing to "A234"
//   Actual: "A23400" (of type std::string)
// [  FAILED  ] SoundexEncoding.ReplacesMultipleConsonantsWithDigits (0 ms)

TEST_F(SoundexEncoding, ReplacesMultipleConsonantsWithDigits) 
{
  ASSERT_THAT(soundex.encode("Acdl"), Eq("A234"));
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

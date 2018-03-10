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

// To ignore unrecognized characters.
TEST_F(SoundexEncoding, IgnoresNonAlphabetics) {
   ASSERT_THAT(soundex.encode("A#"), Eq("A000"));
}

TEST_F(SoundexEncoding, ReplacesMultipleConsonantsWithDigits) {
   ASSERT_THAT(soundex.encode("Acdl"), Eq("A234"));
}

TEST_F(SoundexEncoding, LimitsLengthToFourCharacters) {
   ASSERT_THAT(soundex.encode("Dcdlb").length(), Eq(4u));
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

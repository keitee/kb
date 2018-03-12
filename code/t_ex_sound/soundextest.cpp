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

TEST_F(SoundexEncoding, IgnoresVowelLikeLetters)
{
    ASSERT_THAT(soundex.encode("Baeiouhycdl"), Eq("B234"));

    // We change the core algorithm outlined in encode to include uppercasing
    // the head of the word, which we expect to be only a single character.

    ASSERT_THAT(soundex.encode("BaAeEiIoOuUhHyYcdl"), Eq("B234"));
}

TEST_F(SoundexEncoding, CombinesDuplicateEncodings)
{
    // preconditions
    ASSERT_THAT(soundex.encodeDigit('b'), soundex.encodeDigit('f'));
    ASSERT_THAT(soundex.encodeDigit('c'), soundex.encodeDigit('g'));
    ASSERT_THAT(soundex.encodeDigit('d'), soundex.encodeDigit('t'));

    ASSERT_THAT(soundex.encode("Abfcgdt"), Eq("A123"));
}

// When we started the work in this section, we were trying to write a test to
// handle the scenario where the second letter duplicates the first letter. That
// triggered us to make our algorithm case-insensitive. We can now return to our
// original goal and write this test:

TEST_F(SoundexEncoding, UppercasesFirstLetter) {
   ASSERT_THAT(soundex.encode("abcd"), StartsWith("A"));
}

TEST_F(SoundexEncoding, IgnoresCaseWhenEncodingConsonants)
{
    ASSERT_THAT(soundex.encode("BCDL"), soundex.encode("bcdl"));
    ASSERT_THAT(soundex.encode("BCDL"), soundex.encode("Bcdl"));
}

TEST_F(SoundexEncoding, CombinesDuplicateCodesWhenSecondDuplicatesFirst)
{
    ASSERT_THAT(soundex.encode("Bbcd"), Eq("B230"));
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

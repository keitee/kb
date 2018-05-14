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

// for rule #1
TEST_F(SoundexEncoding, IgnoresVowelLikeLetters)
{
    ASSERT_THAT(soundex.encode("Baeiouhycdl"), Eq("B234"));

    // We change the core algorithm outlined in encode to include uppercasing
    // the head of the word, which we expect to be only a single character.

    ASSERT_THAT(soundex.encode("BaAeEiIoOuUhHyYcdl"), Eq("B234"));
}

// for rule #3
// c2/31/Soundex.h
// add it to the encodede string only if the current is different from the
// previous.

TEST_F(SoundexEncoding, CombinesDuplicateEncodings)
{
    // Let’s add a series of precondition assertions to help readers make 
    // the connection, rule #3
    
    ASSERT_THAT(soundex.encodeDigit('b'), soundex.encodeDigit('f'));
    ASSERT_THAT(soundex.encodeDigit('c'), soundex.encodeDigit('g'));
    ASSERT_THAT(soundex.encodeDigit('d'), soundex.encodeDigit('t'));

    ASSERT_THAT(soundex.encode("Abfcgdt"), Eq("A123"));
}

// for rule #3 where the second letter duplicates the first. To do this, have 
// case handling in place.

TEST_F(SoundexEncoding, UppercasesFirstLetter) {
   ASSERT_THAT(soundex.encode("abcd"), StartsWith("A"));
}

TEST_F(SoundexEncoding, IgnoresCaseWhenEncodingConsonants)
{
    ASSERT_THAT(soundex.encode("BCDL"), soundex.encode("bcdl"));
    ASSERT_THAT(soundex.encode("BCDL"), soundex.encode("Bcdl"));
}

// c2/36/SoundexTest.cpp
// note that "Bb" encodes to "B" as per rule #3
// To do this, have to encode whole word but not just tail.
// c2/38/Soundex.h passes

TEST_F(SoundexEncoding, CombinesDuplicateCodesWhenSecondDuplicatesFirst)
{
    ASSERT_THAT(soundex.encode("Bbcd"), Eq("B230"));
}

// What about vowels? Rule #3 also states that otherwise-duplicate encodings
// separated by a vowel (not h or w) get coded twice.
//
// Value of: soundex.encode("Jbob")
// Expected: is equal to 0x453645 pointing to "J110"
//   Actual: "J100" (of type std::string)
//
// since the second b is not added to encoding since it fails on condition check
// on lastDigit(). Do like when input "Jbb"
//
// So make them added to encoding when they are separated by vowels as rule 3
// said.


TEST_F(SoundexEncoding, DoesNotCombineDuplicateEncodingsSeparatedByVowels)
{
    ASSERT_THAT(soundex.encode("Jbob"), Eq("J110"));
    ASSERT_THAT(soundex.encode("Jbwb"), Eq("J100"));
    ASSERT_THAT(soundex.encode("Jbhb"), Eq("J100"));
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

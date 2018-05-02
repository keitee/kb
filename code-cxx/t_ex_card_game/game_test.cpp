#include "gmock/gmock.h"
#include "game.h"

using namespace testing;

class CardPlayerTest: public Test
{
public:
    CardPlayer player{"test player"};
};

TEST_F(CardPlayerTest, WhenNoCardPopulated) {
    ASSERT_THAT(player.HasMoreCards(), Eq(false));
}

TEST_F(CardPlayerTest, WhenPopulated) {
    player.PopulateCards(1);
    ASSERT_THAT(player.HasMoreCards(), Eq(true));
}

TEST_F(CardPlayerTest, GetCard) {
    player.PopulateCards(1);
    ASSERT_THAT(player.GetCard(), Eq(12));
}

TEST_F(CardPlayerTest, GetScore) {
    player.SaveCardToScore(Card{3});
    player.SaveCardToScore(Card{4});
    player.SaveCardToScore(Card{5});
    ASSERT_THAT(player.GetScore(), Eq(3));
}

class CardGameTest: public Test
{
public:
    CardGame game;
};

TEST_F(CardGameTest, PlayOneGame) {
    game.Play();
    game.AnnounceWinner();
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

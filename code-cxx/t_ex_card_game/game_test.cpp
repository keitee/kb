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
    player.PopulateCards();
    ASSERT_THAT(player.HasMoreCards(), Eq(true));
}

TEST_F(CardPlayerTest, GetScore) {
    player.SaveCardToScore(Card{3});
    player.SaveCardToScore(Card{4});
    player.SaveCardToScore(Card{5});
    ASSERT_THAT(player.GetScore(), Eq(3));
}


/////////////////////////////////////////////////////////////////////////////
// CardGameTest

class CardGameTest: public Test
{
public:
    CardGame game;
};

// TEST_F(CardGameTest, DISABLED_PopulatePlayers) {
TEST_F(CardGameTest, PopulatePlayers) {
    ASSERT_THAT(game.PopulatePlayer_(4), Eq(4));
}

// TEST_F(CardGameTest, DISABLED_PlayTwoPlayeGame) {
TEST_F(CardGameTest, PlayTwoPlayeGame) {
    game.PopulatePlayer_(2);
    game.Play();
    game.AnnounceWinner();
}

TEST_F(CardGameTest, DISABLED_PlayForPlayerGame) {
// TEST_F(CardGameTest, PlayForPlayerGame) {
    game.PopulatePlayer_(4);
    game.Play();
    game.AnnounceWinner();
}


// class CardGameSPTest: public Test
// {
// public:
//     CardGameSP game;
// };
// 
// TEST_F(CardGameSPTest, PopulatePlayers) {
// // TEST_F(CardGameSPTest, DISABLED_PopulatePlayers) {
//     ASSERT_THAT(game.PopulatePlayer_(4), Eq(4));
// }
// 
// TEST_F(CardGameSPTest, PlayTwoPlayeGame) {
// // TEST_F(CardGameSPTest, DISABLED_PlayTwoPlayeGame) {
//     game.PopulatePlayer_(2);
//     game.Play();
//     game.AnnounceWinner();
// }
// 
// TEST_F(CardGameSPTest, PlayForPlayerGame) {
// // TEST_F(CardGameSPTest, DISABLED_PlayForPlayerGame) {
//     game.PopulatePlayer_(4);
//     game.Play();
//     game.AnnounceWinner();
// }
// 
// TEST_F(CardGameSPTest, PlayEightPlayerGame) {
// // TEST_F(CardGameSPTest, DISABLED_PlayForPlayerGame) {
//     game.PopulatePlayer_(8);
//     game.Play();
//     game.AnnounceWinner();
// }

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

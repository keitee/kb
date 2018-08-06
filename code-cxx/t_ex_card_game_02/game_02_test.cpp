#include "gmock/gmock.h"
#include "game_02.h"

using namespace testing;

// Player

class CardPlayer: public Test
{
  public:
    Player player{"one"};
};

TEST_F(CardPlayer, HandelWhenNoCardToPlay) {

  ASSERT_THAT(player.GetPlayCard(), false);
}

TEST_F(CardPlayer, RunNormalPath) {

  player.SetupPlayCard({1,2,3,4,5});

  ASSERT_THAT(player.GetPlayCard(), 5);
  ASSERT_THAT(player.GetPlayCard(), 4);

  player.SaveWonCard(5);
  player.SaveWonCard(4);

  ASSERT_THAT(player.GetTotalPoints(), 2);
}


// Game

class CardGame: public Test
{
  public:
    Game game;
};

TEST_F(CardGame, PouplatePlayers)
{
  game.PouplatePlayers(2);
}

// when uses arguments
//
// TEST_F(CardGame, PlayGameUseFixedCards)
// {
//   one.SetupPlayCard({11,  7,  2,  8,  5,  9, 11, 10,  4,  1,  9,  4}); 
//   two.SetupPlayCard({8, 10, 12,  5,  3, 12,  9, 10,  8,  1,  8, 11}); 
// 
//   game.PlayGame(one, two);
// 
//   // player 1 and 2 have the same points, 5.
//   game.DeclareWinner(one, two);
// }
//
// TEST_F(CardGame, PlayGameUseRandomCards)
// {
//   game.PrepareCards(one, two);
//   game.PlayGame(one, two);
//   game.DeclareWinner(one, two);
// }


// when uses "extended version"
//
// [ RUN      ] CardGame.PlayGameUseFor2
// dtor: 00
// dtor: 01
// player [00] play cards: 11,  7,  2,  8,  5,  9, 11, 10,  4,  1,  9,  4,
// player [01] play cards:  8, 10, 12,  5,  3, 12,  9, 10,  8,  1,  8, 11,
// =={ Move
// dtor: 01
// dtor: 01
// ==} Move
// running_players_:
// finished_players_:
// player [00] play cards:
// player [00] won cards:  9, 11,  5,  8, 11,
// player [01] play cards:
// player [01] won cards: 11,  8, 12, 12, 10,
// dtor: 00
// dtor: 01
// [       OK ] CardGame.PlayGameUseFor2 (1 ms)
//
//
// when uses "super bonus"
//
// [ RUN      ] CardGame.PlayGameUseFor2
// dtor: 00
// dtor: 01
// player [00] play cards: 11,  7,  2,  8,  5,  9, 11, 10,  4,  1,  9,  4,
// player [01] play cards:  8, 10, 12,  5,  3, 12,  9, 10,  8,  1,  8, 11,
// dtor: 01
// dtor: 01
// running_players_:
// finished_players_:
// player [00] play cards:
// player [00] won cards:  9,  1, 11, 10,  5,  8, 11,
// player [01] play cards:
// player [01] won cards: 11,  8, 12, 12, 10,
// dtor: 00
// dtor: 01
// [       OK ] CardGame.PlayGameUseFor2 (2 ms)


TEST_F(CardGame, PlayGameUseFor2)
{
  game.PouplatePlayers(2);
  game.PlayGame();
  game.PrintPlayers();
}

// TEST_F(CardGame, PlayGameFor4)
// {
//   game.PouplatePlayers(4);
//   game.PlayGame();
//   game.PrintPlayers();
// }

// TEST_F(CardGame, PlayGameFor5)
// {
//   game.PouplatePlayers(5);
//   game.PlayGame();
//   game.PrintPlayers();
// }

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

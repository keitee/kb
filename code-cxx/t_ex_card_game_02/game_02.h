#ifndef GAME_02_H_
#define GAME_02_H_

/*

Date: 2018.08.06

Basic

Questions about requirements:

1. Which card does win when there have the same number on them?

2. What type of cards?
  Assumes cards which have only numbers. So use simple integers and this makes
  no need to use Card class.

  Assumes that 12 cards are given to a player.
  Assumes that cards numbers are in [1,12] but not 0.

3. Card numbers?
  Card numbers are [1,12] out of [1,12] or are [1,12] out of bigger pool such as
  [1,24]?


4. Top-most card? 
  Should use the order of the given cards and use simply one for each turn or
  should sort them to pick the biggest from the given card?

  Assume that it uses the given order and no sorting. 


Extended

5. Other points from the previous
  1. Choose players for every turn.
  Okay.

  2. When a card is the same, both players score.
  Assume that discard both when both are the same.

  3. When lost for a turn, put that back in the cards to play.
  Do not put it back.

*/

#include <algorithm>
#include <vector>
#include <random>

using namespace std;

const int NUMBER_OF_CARDS_PER_PLAYER = 12;

using Card = int;

class Player
{
  public:
    Player(const string name): name_(name) {}
    ~Player()
    {
      cout << "dtor: " << name_ << endl;
    }

    void SetupPlayCard(const vector<Card> &cards)
    {
      play_cards_ = cards;
    }

    // return 0 if there is no card to play
    int GetPlayCard()
    {
      Card card{0};

      if (!play_cards_.empty())
      {
        card = play_cards_.back();
        play_cards_.pop_back();
      }

      return card;
    }

    void SaveWonCard(Card card)
    {
      won_cards_.push_back(card);
    }

    int GetTotalPoints() const
    {
      return won_cards_.size();
    }

    void ShowPlayCards() const
    {
      cout << "player [" << name_ << "] play cards: ";
      for (auto e : play_cards_)
        cout << setw(2) << e << ", ";
      cout << endl;
    }

    void ShowWonCards() const
    {
      cout << "player [" << name_ << "] won cards: ";
      for (auto e : won_cards_)
        cout << setw(2) << e << ", ";
      cout << endl;
    }

  private:
    string name_;
    vector<Card> play_cards_{};
    vector<Card> won_cards_{};
};

class CardGenerator
{
  public:
    int operator()()
    {
      return udist(dre);
    }

  private:
    static default_random_engine dre;
    static uniform_int_distribution<unsigned> udist;
};

default_random_engine CardGenerator::dre;
uniform_int_distribution<unsigned> CardGenerator::udist{1, NUMBER_OF_CARDS_PER_PLAYER};


// note:
//
// From the given text:
// There should be some Game class managing the simulation. The name isn't that
// important, but it should contain the players and the functions for each phase
// of the game. Here the candidate's ability in OOP is tested.
// 
// Member variables replace the need for repeated arguments to functions. The
// functions should all be passed the players, scores, and cards as arguments,
// but instead, the players should be part of the instance.
//
// Or have players arguments which can be mocks and increase testibility of game
// class? If not, have to make private area `public` temporaily to test private
// functions and when done, make it private back again?


class Game
{
  public:
    Game() 
    {
      running_players_.reserve(10);
      finished_players_.reserve(10);
    }

  public:
    void PouplatePlayers(const int num_players)
    {
      string name{};

      for (int i = 0; i < num_players; ++i)
      {
        name = "0" + to_string(i);
        running_players_.push_back(Player(name));
      }

      for (auto &e : running_players_)
      {
        vector<Card> cards;
        generate_n(back_inserter(cards), NUMBER_OF_CARDS_PER_PLAYER, card_gen_);
        e.SetupPlayCard(cards);
        e.ShowPlayCards();
      }
    }

    // note:
    // which is different from the previous.
    //
    // have to run running player and finished players to find the winner since
    // there may a player which still have cards to play and is not finished but
    // has more won cards than else? For example, 5 players. 
    
    void DeclareWinner()
    {
    }

    void PlayGame()
    {
      while (RunningPlayers_())
      {
        auto players = SelectPlayersToPlay_();
        PlayTurn_(players.first, players.second);
        MoveFinishedPlayers_();
      }
    }

    void PrintPlayers()
    {
      cout << "running_players_: " << endl;
      for (auto &e : running_players_)
      {
        e.ShowPlayCards();
        e.ShowWonCards();
      }

      cout << "finished_players_: " << endl;
      for (auto &e : finished_players_)
      {
        e.ShowPlayCards();
        e.ShowWonCards();
      }
    }

  private:
    size_t GetRandomPlayerIndex_()
    {
      return rand() % running_players_.size();
    }

    // note: 
    std::pair<Player &, Player &> SelectPlayersToPlay_()
    {
      size_t first_player = GetRandomPlayerIndex_();
      size_t second_player = GetRandomPlayerIndex_();
      while (first_player == second_player)
        second_player = GetRandomPlayerIndex_();

      return pair<Player &, Player &>
        (running_players_[first_player], running_players_[second_player]);
    }

    // void PlayTurn_(Player &one, Player &two)
    // {
    //   Card card_one_{};
    //   Card card_two_{};
    //
    //   for(card_one_ = one.GetPlayCard(), card_two_ = two.GetPlayCard(); 
    //       card_one_ && card_two_;
    //       card_one_ = one.GetPlayCard(), card_two_ = two.GetPlayCard()) 
    //   {
    //     if (card_one_ < card_two_)
    //       two.SaveWonCard(card_two_);
    //     else if(card_two_ < card_one_)
    //       one.SaveWonCard(card_one_);
    //     else
    //     {
    //       // do nothing which means discard both when both are the same.
    //     }
    //   }
    // }

    void PlayTurn_(Player &one, Player &two)
    {
      vector<Card> draw_cards{};

      Card card_one_{};
      Card card_two_{};

      for(card_one_ = one.GetPlayCard(), card_two_ = two.GetPlayCard(); 
          card_one_ && card_two_;
          card_one_ = one.GetPlayCard(), card_two_ = two.GetPlayCard()) 
      {
        if (card_one_ < card_two_)
        {
          two.SaveWonCard(card_two_);

          for (auto e: draw_cards)
            two.SaveWonCard(e);

          draw_cards.clear();
        }
        else if(card_two_ < card_one_)
        {
          one.SaveWonCard(card_one_);

          for (auto e: draw_cards)
            two.SaveWonCard(e);

          draw_cards.clear();
        }
        else
        {
          // for super bonus
          draw_cards.push_back(card_one_);
          continue;
        }
      }
    }

    bool RunningPlayers_()
    {
      if (running_players_.size() == 0 || running_players_.size() ==1)
        return false;

      return true;
    }

    void MoveFinishedPlayers_()
    {
      // cout << "=={ Move " << endl;

      for (auto it = running_players_.begin();
            it != running_players_.end();)
      {
        if (!(it->GetPlayCard()))
        {
          finished_players_.push_back(std::move(*it));
          it = running_players_.erase(it);
        }
        else
          ++it;
      }

      // cout << "==} Move " << endl;
    }


    // Player one_;
    // Player two_;

    vector<Player> running_players_;
    vector<Player> finished_players_;
    CardGenerator card_gen_;
};

#endif // GAME_02_H_

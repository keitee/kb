//
// https://dev.to/mortoray/interview-question-a-two-player-card-game-67i
//
// * design problem
//
// I introduce the question with a preamble about being focused on the design
// and structure of the code.
//
// this is a two player card game
//
// the game starts with a deck of cards
//
// the cards are dealt out to both players
//
// on each turn:
// both players turn over their top-most card
//
// the player with the higher valued card takes the cards and puts them in their
// scoring pile (scoring 1 point per card) 
// note: this may bear question about a lower value card
// 
// this continues until the players have no cards left
// 
// the player with the highest score wins 
//
// * Candidates must identify and resolve issues in the requirements.
//
//  for example:
//    What type of cards? only numbers from 1 to 12? Shapes such as heart?
//    How many cards ecah player can have to play?
//    What if a value of cards are the same when turning them over?
//
//
// TODO: first version
//
// Based on this description I'm expected the person to write code that
// simulates the game: writes out who wins from one round of play.
//
// I'll expect the main sequence to be written. I want to see cards dealt out,
// the turns being taken, and the winner declared.
//
//
// NOTE: do not over-design and top-down
//
// The only definite pattern I've seen is a negative one with over-design:
// trying to plan for everything and writing extensive notes. 
//
// to think at a higher level, about what they might actually need.  it's best
// to think top-down and ignore details until you need them. That is, don't
// design all aspects of a theoretical Deck class as they may not be used.  It's
// okay to sketch out classes, placing some variables, but the details should be
// sparse.
//
// The importance of top-down design is echoed in processes like test-driven
// development and the concept of YAGNI "You aren't gonna need it".
//
//
// NOTE: decision on questions made adove and naming.
//
// Regardless of the choice of integer or class, it's important I understand why
// this decision is being made. In particular, I expect the candidate to tell me
// why they've chosen one or the other. The code should also be clear. A
// variable like int[] array is not helpful (yes, this happened once). Name
// things for their logical value, such as deck.
//
//
// NOTE: common mistake
// A common mistake is the failure to identify a Player type.
//
// vector<int> player1_cards, player2_cards;
// int player1_score, player2_score;
//
// A series of matching variable names, like player1_* and player2_*, is a good
// indication that you should have a Player type.
// 
//
// NOTE: arguments vs member variables
//
// There should be some Game class managing the simulation. The name isn't that
// important, but it should contain the players and the functions for each phase
// of the game. Here the candidate's ability in OOP is tested.
// 
// Member variables replace the need for repeated arguments to functions. The
// functions should all be passed the players, scores, and cards as arguments,
// but instead, the players should be part of the instance.
//
//
// NOTE:
// How much I need to say, and how explicit it needs to be, reflects a lot on
// the skills of the interviewee.
//
// NOTE:
// Comparison to natural language seems fair, some people seem to speak fluently
// and the code flows consistently and cleanly. Others seem to be stuttering,
// forgetting words, translating in their brain, and the code is less eloquent.
//
//
// TODO: extended questions
// If the interviewee has solved the base question and we have at least 10
// minutes left I'll add the next requirement:
//
// By rough estimation about half of the people make it to this question. That
// makes it a suitable criterion for filtering: I tend not to pass people that
// can't solve this extended question.
//
// The change to N players introduces a few new problems:
//
// * Dealing cards can no longer be done with an if-else to choose the player.
// I'm kind of surprised at how many people do not know how to use modulus % to
// select items from an array in a loop.
//
// * Picking the highest card from a list is no longer an if-else. It challenges
// some people as they know how to get the maximum value, but not always the
// index of that card.
//
// * The loop to decide if the game is over is now non-trivial, especially as
// players may not have the same number of cards. I noticed that better
// programmers naturally create a helper function, or otherwise avoid complex
// conditions in a loop.
//
// 1. How can support multiple players? And selectd two player play until
// there's no card left.
//
// 2. How can support multiple players? And select two player for every turn and
// this makes that players may not have the same number of cards
//
// NOTE: Which one to choose?
//
// The programmer's knowledge of the language seems to play a significant role
// here. Individuals that are comfortable in their language have an easier time
// doing these changes. It has been especially true of the candidates that know
// Python well, as there are great constructs to make this easier.
//
//
// TODO: Super Bonus Question
//
// It's a bit more challenging than the previous requirements: "don't assume the
// cards have unique values". The details are:
//
// * Remove the assumption that the cards are unique (that is, go to a standard
// deck)
//
// * If players have the same valued card, they draw an additional card, repeat
// until one has a higher card
//
// * The person with the highest card takes all the cards, scoring one each Only
// the players that tie continue drawing cards (1+ players may sit out on the
// additional draws) 
//
// * Only one person has made it this far, setting the gold standard for this
// interview question.
//
// Only the time pressure
// 
// What I like about this question is that it lacks any trickery or random
// algorithmic knowledge. The pacing seems to work well: interviewees manage
// varying degrees of the problem. Given the difficulties I've seen people have,
// it seems to test a good cross-section of abilities.
// 
// As interview practice, try coding the answer. Pay attention to which bits
// cause you trouble, and watch how long it takes you.


#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <memory>

using Card = uint32_t;
using Score = uint32_t;

template<typename T>
void PRINT_ELEMENTS(const T &coll, const std::string &optstr = "")
{
  size_t count{};

  std::cout << optstr;

  for (const auto &e : coll)
  {
    std:: cout << e << " ";
    ++count;
  }

  std::cout << "(" << count << ")" << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
// CardPlayer

class CardPlayer
{
  public:
    CardPlayer() = delete;
    CardPlayer(const std::string &name) 
      : name_(name) {}

    ~CardPlayer() 
    {
      PRINT_ELEMENTS(play_cards_, name_+ ":play cards : dtor: ");
      PRINT_ELEMENTS(score_cards_, name_+ ":score cards: dtor: ");
    }

    // assume that:
    // * it's descending sorted to have the biggest card from the beginning.
    //   remove `descending` since vector is expensive to take it from front.
    // * card number are from 0 to X

    void PopulateCards()
    {
      generate_n(back_inserter(play_cards_), NUMBER_OF_CARDS_PER_PLAYER, CardSequenceRandom());
      sort(play_cards_.begin(), play_cards_.end());
      PRINT_ELEMENTS(play_cards_, name_+ ": ");
    }

    bool HasMoreCards() const
    {
      return !play_cards_.empty();
    }

    // assumes that it's called only when a player has a card to play
    Card GetCard()
    {
      Card card{};

      card = play_cards_.back();
      play_cards_.pop_back();

      return card;
    }

    void SaveCardToScore(const Card card)
    {
      score_cards_.push_back(card);
    }

    void SaveCardToPlay(const Card card)
    {
      play_cards_.push_back(card);
    }

    Score GetScore() const
    {
      return score_cards_.size();
    }

    std::string GetName() const
    {
      return name_;
    }

  private:
    static const uint32_t NUMBER_OF_CARDS_PER_PLAYER;
    static const uint32_t NUMBER_OF_CARDS_PER_DECK;

    class CardSequenceRandom
    {
      public:
        int operator() () {
          return udist(dre);
        }

      private:
        static std::default_random_engine dre;
        static std::uniform_int_distribution<uint32_t> udist;
    };

    std::string name_;
    std::vector<Card> play_cards_{};
    std::vector<Card> score_cards_{};
};

const uint32_t CardPlayer::NUMBER_OF_CARDS_PER_DECK{24};
const uint32_t CardPlayer::NUMBER_OF_CARDS_PER_PLAYER{10};

std::default_random_engine 
  CardPlayer::CardSequenceRandom::dre;

std::uniform_int_distribution<uint32_t> 
  CardPlayer::CardSequenceRandom::udist{0, NUMBER_OF_CARDS_PER_DECK};


/////////////////////////////////////////////////////////////////////////////
// CardGameUseVector

class CardGame
{
  public:
    CardGame() 
    {
      // NOTE: to aviod reallocations
      running_players_.reserve(10);
    }

    // deals players that has cards to play so no check is needed to see if
    // it has cards to play
    bool HasRunningPlayers()
    {
      // NOTE that should cover case when there is no running players
      //
      // this code works well until have 2 players. However, as soon as moves to
      // 4 players, shows the issue.
      //
      // if(running_players_.size() == 0 || running_players_.size() % 2)
      // 
      // because it's random to pick up players, some gets finished earlier than
      // others and this makes the number of players odd sometimes. So have to
      // remove a check on odd number of players.
      
      // NOTE: 
      //
      // if(running_players_.size() == 0)
      //
      // starts to see hang with > 2 players sometimes but not always. Have to
      // change since as the reason above, can see odd number of running players
      // and this cause hang on rand() % 1. Have to end the play when there is
      // only one running player that still have cards to play.
      
      if(running_players_.size() == 0 || running_players_.size() == 1)
        return false;

      return true;
    }

    void Play()
    {
      Card first_card{}, second_card{};

      // NOTE: this causes two ctor/dtors
      CardPlayer first_player{"first"};
      CardPlayer second_player{"second"};

      while(HasRunningPlayers())
      {
        ++number_of_plays_;

        auto selected_players = GetRunningPlayers_();

        // copy players
        first_player = running_players_[selected_players.first];
        second_player = running_players_[selected_players.second];

        first_card = first_player.GetCard();
        second_card = second_player.GetCard();

        // std::cout << "Play (" << number_of_plays_ << ") : " << 
        //   selected_players.first << " {" << std::setw(2) << first_card << "}, " << 
        //   selected_players.second << " {" << std::setw(2) << second_card << "}" << std::endl;

        // NOTE: when there are same value items, it goes either to firsr or
        // second since GetRandomIndex_() uses random to select index so first
        // and second cannot be fixed. So depending on which one becomes first
        // card can go one of them so score result can be different from the
        // expected. 
        //
        // (first_card > second_card ? 
        //   first_player.SaveCardToScore(first_card) 
        //   : second_player.SaveCardToScore(second_card));
        //
        // This means that handling when the value is the same is not optional
        // and have to have decision on that. 
        
        // NOTE: when not see the same value, put back the one which is not
        // scored since GetCard() removes it from the vector. Otherwise, lost
        // them. so now the number of cards that all players started with should
        // match with the sum of the number of scores and the number of cards
        // which is not played at the end of run. 
        
        if(first_card == second_card)
        {
          second_player.SaveCardToScore(second_card);
          first_player.SaveCardToScore(first_card); 
        }
        else if(first_card > second_card)
        {
          first_player.SaveCardToScore(first_card); 
          second_player.SaveCardToPlay(second_card); 
        }
        else
        {
          first_player.SaveCardToPlay(first_card); 
          second_player.SaveCardToScore(second_card);
        }

        // copy players back
        running_players_[selected_players.first] = first_player;
        running_players_[selected_players.second] = second_player;

        MoveFinishedPlayers_();
      }
    }

    void AnnounceWinner() const
    {
      Score winner_score = std::numeric_limits<uint32_t>::min();
      std::string winner_name;

      for(const auto &e : finished_players_)
      {
        if ( winner_score < e.GetScore())
        {
          winner_score = e.GetScore();
          winner_name = e.GetName();
        }
      }

      std::cout << "=================================" << std::endl;
      std::cout << "play stat: " << std::endl;
      std::cout << "plays : " << number_of_plays_ << std::endl;
      std::cout << "winner: " << winner_name << " got " 
        << winner_score << " scores" << std::endl;
    }

  // note that make it public in order to use it from test. one done that make
  // it private again?
  //
  // private:
  public:
    size_t number_of_players_{};
    size_t number_of_plays_{};
    std::vector<CardPlayer> running_players_;
    std::vector<CardPlayer> finished_players_;

    size_t PopulatePlayer_(size_t number_of_players)
    {
        std::string player_name;

        for(size_t i = 0; i < number_of_players; ++i)
        {
            player_name = "player " + std::to_string(i);

            // NOTE: why dose it involve copy ctor?
            running_players_.push_back(CardPlayer(player_name));
        }

        // to have only 0 or 1 of test_seed
        for(auto &e : running_players_)
          e.PopulateCards();

        number_of_players_ = number_of_players;

        return running_players_.size();
    } 

    // get random index within number of running players
    size_t GetRandomIndex_()
    {
        return rand() % running_players_.size();
    }

    std::pair<size_t, size_t> GetRunningPlayers_()
    {
      // choose two players in random
      size_t index_first_player = GetRandomIndex_();
      size_t index_second_player = GetRandomIndex_();
      while (index_second_player == index_first_player)
        index_second_player = GetRandomIndex_();

      return 
        std::pair<size_t, size_t>(index_first_player, index_second_player);
    }

    // check and move players which do not have cards to play
    void MoveFinishedPlayers_()
    {
      for (auto it = running_players_.begin(); it != running_players_.end();)
      {
        if (!it->HasMoreCards())
        {
          std::cout << "MoveFinishedPlayers_: move " 
            << it->GetName() << std::endl;

          // NOTE: copy
          // finished_players_.push_back(*it);
          
          finished_players_.push_back(std::move(*it));

          it = running_players_.erase(it);
        }
        else
          ++it;
      }
    }
};


/////////////////////////////////////////////////////////////////////////////
// CardGameUseSharedPointer

class CardGameSP
{
  public:
    CardGameSP() 
    {
    }

    bool HasRunningPlayers()
    {
      if(running_players_.size() == 0 || running_players_.size() == 1)
        return false;

      return true;
    }

    void Play()
    {
      Card first_card{}, second_card{};

      std::shared_ptr<CardPlayer> first_player{};
      std::shared_ptr<CardPlayer> second_player{};

      while(HasRunningPlayers())
      {
        ++number_of_plays_;

        auto selected_players = GetRunningPlayers_();

        // copy players
        first_player = running_players_[selected_players.first];
        second_player = running_players_[selected_players.second];

        first_card = first_player->GetCard();
        second_card = second_player->GetCard();

        // std::cout << "Play (" << number_of_plays_ << ") : " << 
        //   selected_players.first << " {" << std::setw(2) << first_card << "}, " << 
        //   selected_players.second << " {" << std::setw(2) << second_card << "}" << std::endl;

        if(first_card == second_card)
        {
          second_player->SaveCardToScore(second_card);
          first_player->SaveCardToScore(first_card); 
        }
        else if(first_card > second_card)
        {
          first_player->SaveCardToScore(first_card); 
          second_player->SaveCardToPlay(second_card); 
        }
        else
        {
          first_player->SaveCardToPlay(first_card); 
          second_player->SaveCardToScore(second_card);
        }

        MoveFinishedPlayers_();
      }
    }

    void AnnounceWinner() const
    {
      Score winner_score = std::numeric_limits<uint32_t>::min();
      std::string winner_name;

      for(const auto &e : finished_players_)
      {
        if ( winner_score < e->GetScore())
        {
          winner_score = e->GetScore();
          winner_name = e->GetName();
        }
      }

      std::cout << "=================================" << std::endl;
      std::cout << "play stat: " << std::endl;
      std::cout << "plays : " << number_of_plays_ << std::endl;
      std::cout << "winner: " << winner_name << " got " 
        << winner_score << " scores" << std::endl;
    }

  // note that make it public in order to use it from test. one done that make
  // it private again?
  //
  // private:
  public:
    size_t number_of_players_{};
    size_t number_of_plays_{};
    std::vector<std::shared_ptr<CardPlayer>> running_players_;
    std::vector<std::shared_ptr<CardPlayer>> finished_players_;

    size_t PopulatePlayer_(size_t number_of_players)
    {
        std::string player_name;

        for(size_t i = 0; i < number_of_players; ++i)
        {
            player_name = "player " + std::to_string(i);

            running_players_.push_back(std::make_shared<CardPlayer>(player_name));
        }

        for(auto &e : running_players_)
          e->PopulateCards();

        number_of_players_ = number_of_players;

        return running_players_.size();
    } 

    // get random index within number of running players
    size_t GetRandomIndex_()
    {
        return rand() % running_players_.size();
    }


    std::pair<size_t, size_t> GetRunningPlayers_()
    {
      // choose two players in random
      size_t index_first_player = GetRandomIndex_();
      size_t index_second_player = GetRandomIndex_();
      while (index_second_player == index_first_player)
        index_second_player = GetRandomIndex_();

      return 
        std::pair<size_t, size_t>(index_first_player, index_second_player);
    }

    // check and move players which do not have cards to play
    void MoveFinishedPlayers_()
    {
      for (auto it = running_players_.begin(); it != running_players_.end();)
      {
        if (!(*it)->HasMoreCards())
        {
          std::cout << "MoveFinishedPlayers_: move " 
            << (*it)->GetName() << std::endl;

          finished_players_.push_back(std::move(*it));

          it = running_players_.erase(it);
        }
        else
          ++it;
      }
    }
};


// compare vector vs shared_ptr version
//
// [ RUN      ] CardGameTest.PlayTwoPlayeGame
// kit:vector copy ctor
// kit:vector copy ctor
// player 0:dtor: (0)
// kit:vector copy ctor
// kit:vector copy ctor
// player 1:dtor: (0)
// player 0: 0 0 1 1 9 9 12 13 16 20 (10)
// player 1: 2 10 10 13 14 16 17 17 21 23 (10)
// Play: 1 {23}, 0 {20}
// Play: 1 {21}, 0 {16}
// Play: 0 {13}, 1 {17}
// Play: 1 {17}, 0 {12}
// Play: 1 {16}, 0 { 9}
// Play: 1 {14}, 0 { 9}
// Play: 0 { 1}, 1 {13}
// Play: 0 { 1}, 1 {10}
// Play: 1 {10}, 0 { 0}
// Play: 0 { 0}, 1 { 2}
// MoveFinishedPlayers_: move player 0
// kit:vector copy ctor
// kit:vector copy ctor
// player 1:dtor: 23 21 17 17 16 14 13 10 10 2 (10)
// MoveFinishedPlayers_: move player 1
// kit:vector copy ctor
// kit:vector copy ctor
// kit:vector copy ctor
// kit:vector copy ctor
// player 0:dtor: (0)
// player 1:dtor: 23 21 17 17 16 14 13 10 10 2 (10)
// player 1:dtor: 23 21 17 17 16 14 13 10 10 2 (10)
// player 0:dtor: (0)
// winner player 1 got 10 scores
// player 0:dtor: (0)
// player 1:dtor: 23 21 17 17 16 14 13 10 10 2 (10)
// [       OK ] CardGameTest.PlayTwoPlayeGame (4 ms)
//
//
// [ RUN      ] CardGameSPTest.PlayTwoPlayeGame
// player 0: 1 6 9 11 12 13 19 19 22 23 (10)
// player 1: 0 3 15 17 18 18 20 21 22 24 (10)
// Play: 1 {24}, 0 {23}
// Play: 0 {22}, 1 {22}
// Play: 1 {21}, 0 {19}
// Play: 1 {20}, 0 {19}
// Play: 1 {18}, 0 {13}
// Play: 1 {18}, 0 {12}
// Play: 0 {11}, 1 {17}
// Play: 0 { 9}, 1 {15}
// Play: 0 { 6}, 1 { 3}
// Play: 0 { 1}, 1 { 0}
// MoveFinishedPlayers_: move player 0
// MoveFinishedPlayers_: move player 1
// winner player 1 got 8 scores
// player 0:dtor: 22 6 1 (3)
// player 1:dtor: 24 22 21 20 18 18 17 15 (8)
// [       OK ] CardGameSPTest.PlayTwoPlayeGame (0 ms)


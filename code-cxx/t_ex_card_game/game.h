//
// https://dev.to/mortoray/interview-question-a-two-player-card-game-67i
//
// * design problem
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
// Only one person has made it this far, setting the gold standard for this
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

using Card = uint32_t;
using Score = uint32_t;

class CardPlayer
{
  public:
    CardPlayer() = delete;
    CardPlayer(const std::string &name) : name_(name) {}

    // assume that:
    // * it's descending sorted to have the biggest card from the beginning.
    //   remove `descending` since vector is expensive to take it from front.
    // * 9 cards
    // * no duplicates
    // * card number are from 1 to X. 0 means invalid value

    void PopulateCards(int seed)
    {
      if( seed == 1 )
        play_cards_ = std::vector<Card>{2, 3, 5, 6, 7, 9, 10, 11, 12};
      // cards = std::vector<Card>{12, 11, 10, 9, 7, 6, 5, 3, 2};
      else if( seed == 0 )
        play_cards_ = std::vector<Card>{1, 4, 8, 13, 14, 15, 16, 17, 18};
      // cards = std::vector<Card>{18, 17, 16, 15, 14, 13, 8, 4, 1};

      // generate_n( back_inserter(cards, NUMBER_OF_CARDS_PER_PLAYER, CardSequence()));
      // std::vector<int> ivec;
      // std::generate_n( std::back_inserter(ivec, 12, std::rand));
      // std::cout << name_ << " has " << cards.size() << std::endl;
    }

    bool HasMoreCards() const
    {
      return !play_cards_.empty();
    }

    Card GetCard()
    {
      // card{0} means a invalid card
      Card card{};

      if(HasMoreCards())
      {
        card = play_cards_.back();
        play_cards_.pop_back();
      }

      return card;
    }

    void SaveCardToScore(const Card card)
    {
      if(card)
        score_cards_.push_back(card);
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
    const uint32_t NUMBER_OF_CARDS_PER_PLAYER{10};
    const uint32_t NUMBER_OF_CARDS_PER_DECK{24};

    class CardSequence
    {
      public:
        int operator() () {
          return rand() % 24;
        }
    };

    std::string name_;
    std::vector<Card> play_cards_{};
    std::vector<Card> score_cards_{};
};


class CardGame
{
  public:
    CardGame() 
    {
      left_player.PopulateCards(0);
      right_player.PopulateCards(1);
    }

    void Play()
    {
      Card card_from_right{}, card_from_left{};

      while( right_player.HasMoreCards() )
      {
        card_from_right = right_player.GetCard();
        card_from_left = left_player.GetCard();

        (card_from_left > card_from_right ? 
            left_player.SaveCardToScore(card_from_left) : right_player.SaveCardToScore(card_from_right));
      }
    }

    void AnnounceWinner() const
    {
      Score score_from_right = right_player.GetScore();
      Score score_from_left = left_player.GetScore();

      (score_from_right > score_from_left ? 
        PrintWinner_(right_player) : PrintWinner_(left_player));
    }

  private:
    // left and right side player
    CardPlayer left_player{"LSP"};
    CardPlayer right_player{"RSP"};

    std::vector<CardPlayer> running_players_;
    std::vector<CardPlayer> finished_players_;

    std::pair<CardPlayer&, CardPlayer&> GetRunningPlayers_()
    {
      static default_random_engine dre;
      // can use array size?
      static uniform_int_distribution<size_t> dist(0, 4);
    }

    void PrintWinner_(const CardPlayer &player) const
    {
      std::cout << "winner " << player.GetName() << " got " 
        << player.GetScore() << " scores" << std::endl;
    }
};


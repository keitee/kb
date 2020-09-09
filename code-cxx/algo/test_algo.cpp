#include <algorithm>
#include <bitset>
#include <cstring>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp
// (gdb) b AlgoList_Divide_Test::TestBody()

using namespace std;
using namespace testing;

/*
={=========================================================================
*/
template <typename T>
void PRINT_ELEMENTS(T &coll, const string optstr = "")
{
  size_t count{};
  cout << optstr;

  for (const auto &elem : coll)
  {
    std::cout << std::setw(2) << elem << " ";
    ++count;
  }

  std::cout << "(" << count << ")" << std::endl;
}

template <typename T>
void PRINT_M_ELEMENTS(T &coll, const string optstr = "")
{
  size_t count{};
  cout << optstr;

  for (const auto &elem : coll)
  {
    cout << "(" << elem.first << ", " << elem.second << ") ";
    ++count;
  }

  cout << "(" << count << ")" << endl;
}

/*
={=========================================================================
algo-squeeze cxx-bitset

Delete all c from string s. From C prog language, p47. All these are example
where postfix is required.

void squeeze(char s[], int c);

*/

namespace algosqueeze
{
  // ansic version
  void squeeze1(char s[], int c)
  {
    int i, j;

    for (i = j = 0; s[i] != '\0'; i++)
      if (s[i] != c)
        s[j++] = s[i];

    s[j] = '\0';
  }

  // use one var
  void squeeze2(char s[], int c)
  {
    char *scan;

    for (scan = s; *scan; scan++)
    {
      // if not matches: copy s[current] = s[sacn] and increase s
      if (*scan != c)
        *s++ = *scan;
    }

    *s = *scan;
  }

  void squeeze3(char s[], int c)
  {
    // do not use `run` variable since use s for that
    char *pos = s;

    while (*s)
    {
      if (*s != c)
        *pos++ = *s;

      ++s;
    }

    // set the end
    *pos = '\0';
  }

  // 2020.06. as seen above, can use only single variable.
  void squeeze4(char s[], int c)
  {
    char *end       = s;
    const char *run = s;

    for (; *run != '\0'; ++run)
    {
      // when current char is different from c, copy current to where end points
      // and increase it. when differnet, do not increase end.
      if (c != *run)
      {
        *end++ = *run;
      }
    }

    // when the loop ends, put it a string end.
    *end = '\0';
  }
} // namespace algosqueeze

TEST(AlgoSqueeze, remove_single_char)
{
  using namespace algosqueeze;

  std::vector<std::function<void(char *, int)>> coll{squeeze1,
                                                     squeeze2,
                                                     squeeze3,
                                                     squeeze4};

  for (const auto &f : coll)
  {
    char arr[]          = "this is a program to squeeze";
    const char result[] = "this is a program to squz";

    f(arr, 'e');

    // std::cout << arr << std::endl;

    EXPECT_THAT(strcmp(arr, result), 0);
  }
}

/*
ansic, exercise 2-4.

Write an alternate version of squeeze(s1,s2) that deletes each character in the
string s1 that matches 'any' character in the string s2.

void squeeze2(char s[], char t[]);

*/

namespace algosqueeze_mutiple
{
  void squeeze1(char s[], char t[])
  {
    int i, j, k;
    for (k = 0; t[k] != '\0'; k++)
    {
      for (i = j = 0; s[i] != '\0'; i++) // { squeeze1()
        if (s[i] != t[k])
          s[j++] = s[i];
      s[j] = '\0'; // }
    }
  }

  // mine
  void squeeze2(char s[], char t[])
  {
    char *run, *write;

    for (; *t; t++)
    {
      for (write = run = s; *run; run++)
      {
        if (*run != *t)
          *write++ = *run;
      }

      *write = '\0';
    }
  }

  // simple way would be that loops t[] for each char of s[]. O(n2)
  // so if suppose it's ASCII char, may use loopup table and get O(n) but use
  // more space.

  // Cannot use bset[(*t-'a')] since this assumes input is all lower characters.
  // For example, for space char(32) index becomes -65 (32-97) and makes a core.

  void squeeze3(char s[], char t[])
  {
    // assuems ASCII *cxx-vector-bool*
    std::vector<bool> lookup(256);

    // fills up the table
    while (*t)
    {
      lookup[*t] = true;
      ++t;
    }

    char *pos;

    for (pos = s; *s; ++s)
    {
      // not in the lookup, that is, not shown in t[]
      if (lookup[*s] == false)
        *pos++ = *s;
    }

    // since *s shuld be '\0'
    *pos = *s;
  }
} // namespace algosqueeze_mutiple

TEST(AlgoSqueeze, remove_multiple_chars)
{
  using namespace algosqueeze_mutiple;

  std::vector<std::function<void(char *, char *)>> coll{squeeze1,
                                                        squeeze2,
                                                        squeeze3};

  for (const auto &f : coll)
  {
    char arr[]          = "hello world";
    char filter[]       = "ol";
    const char result[] = "he wrd";

    f(arr, filter);

    // std::cout << arr << std::endl;

    EXPECT_THAT(strcmp(arr, result), 0);
  }
}

/*
={=========================================================================
algo-two-player-card-game

https://dev.to/mortoray/interview-question-a-two-player-card-game-67i

I've been asking interviewees, over at interviewing.io, to write a simulation
of a two-player card game. It's an engaging question, revealing a lot about
their abilities. It doesn't involve any trickery nor random algorithm
knowledge. It's suitable for programmers of any skill level and works in all
common languages.

I'd like to share the question and some of the insights I've had.


Two Player Card Game

* design problem

I introduce the question with a preamble about being focused on the design
and structure of the code.

this is a two player card game

the game starts with a deck of cards

the cards are dealt out to both players

on each turn:
both players turn over their top-most card

the player with the higher valued card takes the cards and puts them in their
scoring pile (scoring 1 point per card) 
note: this may bear question about a lower value card
 
this continues until the players have no cards left
 
the player with the highest score wins 

NOTE: Candidates must identify and resolve issues in the requirements.

 for example:

  What type of cards? only numbers from 1 to 12? Shapes such as heart?
  How many cards ecah player can have to play?
  What if a value of cards are the same when turning them over?
  Unique cards?

TODO: first version

Based on this description I'm expected the person to write code that
simulates the game: writes out who wins from one round of play.

I'll expect the main sequence to be written. I want to see cards dealt out,
the turns being taken, and the winner declared.


Design

The interviewee asking questions is good, but not necessarily revealing. I
expect some feedback that they've at least understood the problem. Beyond that
I'm flexible, having seen several different styles of solving the problem. 

NOTE: do not over-design and top-down

The only definite pattern I've seen is a negative one with over-design: trying
to plan for everything and writing extensive notes. If people are taking too
long here I prompt them to start writing code.

*cxx-tdd*

The importance of top-down design is echoed in processes like test-driven
development and the concept of YAGNI "You aren't gonna need it".

The complexity of this problem doesn't require much prework. Beyond coming up
with a couple of classes, like Player and Game, possibly Card, you can start
writing code. The best approach, used by many well-performing candidates, is
outlining the structure in actual code. Code is often the best pseudo-code:
just leave out details and take short-cuts as you're planning.


The Cards

Several candidates start working from the bottom up: taking a deep dive into
what defines a card, or creating a Deck class. I ask them why they are
building these classes, as a hint it's not a good approach. I encourage them
to think at a higher level, about what they might actually need.

Especially in a time-limited scenario, it's best to think top-down and ignore
details until you need them. That is, don't design all aspects of a
theoretical Deck class as they may not be used. It's okay to sketch out
classes, placing some variables, but the details should be sparse.

I avoid giving too much direction at this point; 

  being able to structure a problem in code is an essential skill of coding. 
    
I reserve my comments to when I feel it's going very wrong and the approach
will not let them finish the task. If they feel confident and making progress,
I try not to enforce an ideal view, instead I let them go with it.

One critical question that should arise, probably during design, and at the
latest when comparing cards, is what type of cards we're dealing with. Not
everyone is familiar with standard playing cards, which is fine; they ask what
type of cards these are. Others assume suited cards and ask how to compare
suits. In all cases I simplify the requirement to just be a set of numbered
cards, from 1 to N. 

*TN* With this requirement it's acceptable to not have a Card class at all,
instead just using an integer. 

Though, there's nothing wrong with a trivial Card type either.

NOTE: decision on questions made adove and naming.

Regardless of the choice of integer or class, it's important I understand why
this decision is being made. In particular, I expect the candidate to tell me
why they've chosen one or the other. The code should also be clear. A variable
like int[] array is not helpful (yes, this happened once). Name things for
their logical value, such as deck.


The game progress

I mentioned `the programming should be top-down.` After the initial design
period, and some code sketching, I'll expect the main sequence to be written.
I want to see cards dealt out, the turns being taken, and the winner declared.

Starting with a rough structure then filling in details is good. My experience
so far is that people who don't start at the high-level have a hard time
finishing the problem.

defn play_game = -> {
    deal_cards()

    take_turns()

    declare_winner()
}

Given the simplicity of the problem, it's also acceptable code each of these
directly in the function, provided it can be refactored into functions
afterwards. Whether the interviewee initiates this refactoring, or I have
prompt for it, seems to reflect on their experience level.

The importance of `top-down design is echoed in processes like test-driven`
development and the concept of YAGNI "You aren't gonna need it".


NOTE: Common issues

From here it's a matter of filling in details. This question has a lot of
small things to take care of, none of which are tricky, which is good for an
interview.


The Player class

A common mistake is the failure to identify a Player type. We end up with this
pattern of coding:

vector<int> player1_cards, player2_cards;
int player1_score, player2_score;

A series of matching variable names, like player1_* and player2_*, is 
`a good indication that you should have a Player type.`

Most candidates come to this conclusion their own, either starting with a
Player class directly, or refactoring it. Others require some prompting but
usually understand quickly. A select few just didn't comprehend and needed
explicit instruction.

Prompting people, without just giving away an answer, is difficult. In this
situation, I like to try things like; "look at those variables starting with
player1 and player2. Do you see a pattern there?", "Is there some structure
you can use `to abstract this?"`, or "Couldn't you group this better somehow?".


NOTE: Arguments and Member variables

`There should be some Game class managing the simulation.` 

The name isn't that important, but it should contain the players and the
functions for each phase of the game. Here the candidate's ability in OOP is
tested.

`Member variables replace the need for repeated arguments to functions.` 
The functions should all be passed the players, scores, and cards as
arguments, but instead, the players should be part of the instance. Some
people immediately do it this way, others require a bit of prompting.

The player details are almost always best as member variables, but there is
some variation on how to handle the initial deck and dealing.

Again, prompting appropriately is not easy. I try things like "Is it necessary
to pass the players to all the functions?", "Is it possible to avoid this
redundancy?", or more directly, "These feel like the players belong to the
class." Honestly, I don't remember the specific things I've said, but the key
is to start general, almost mysterious. 

  How much I need to say, and how explicit it needs to be, reflects a lot on the
  skills of the interviewee.


Magic numbers and details

There aren't too many involved in the code, but I don't like seeing them. For
instance, a loop that runs from 1..52 to create cards uses a magic number.
There's also a 2 that comes up when adding to the score: it's a bit more
subtle, but relevant for the extended questions I have planned.

It may seem like a trivial detail, but I've noticed that better programmers
are attuned to such things. They will create a constant, use an argument to a
constructor (possibly with default), or just say, "Yeah, this isn't good, I
should clean it up after."

By this time I usually have a good idea of how well the person knows the
programming language. 

NOTE: Comparison to natural language seems fair, some people seem to speak
fluently and the code flows consistently and cleanly. Others seem to be
stuttering, forgetting words, translating in their brain, and the code is less
eloquent.

Though people that use advanced syntax fair well, people have also completed
the question using nothing but basic imperative syntax as well. Real struggles
though aren't acceptable: the candidate gets to pick the language, so there's
an absolute expectation of basic knowledge.


NOTE: Extended Questions

If the interviewee has solved the base question and we have at least 10
minutes left I'll add the next requirement:


Extend the game to support more than two players

How many changes this requires depends on the initial approach they've taken.
If they didn't come up with a Player class, I'll request it. I'll also request
refactoring into functions if they are a bit sparse. The code needs to be in a
good state. Otherwise, this new requirement is too daunting.

By rough estimation about half of the people make it to this question. That
makes it a suitable criterion for filtering: I tend not to pass people that
can't solve this extended question. I make allowance for new grads, but not
for people with industry experience. I often won't ask this question if I
estimate they won't finish it, or sometimes I propose to extend just one
function. I prefer to end the interview early than add additional stress.


More Issues

The change to N players introduces a few new problems:

Dealing cards can no longer be done with an if-else to choose the player. I'm
kind of surprised at how many people do not know how to use modulus % to
select items from an array in a loop. 

Picking the highest card from a list is no longer an if-else. It challenges
some people as they know how to get the maximum value, but not always the
index of that card. 

The loop to decide if the game is over is now non-trivial, especially as
players may not have the same number of cards. 

  o Choose two players in every turn or all players in every turn?
    Assuming from "may not have the same number of cards", choose two in every
    turn.

  o When picks up a card and found the biggest, discards the rest


I noticed that better programmers naturally create a helper function, or
otherwise avoid complex conditions in a loop.  The programmer's knowledge of
the language seems to play a significant role here.  Individuals that are
comfortable in their language have an easier time doing these changes. It has
been especially true of the candidates that know Python well, as there are
great constructs to make this easier.

Completing this question is a positive indication that the interviewee is
indeed a good programmer or at least a good coder. I don't even care if they
had minor issues in the refactoring, but usually, they don't. It's a curious
pattern, the people that make it this far tend not to have difficulties with
this question.

The choosen decisions:
1. Choose players for every turn.
2. When a card is the same, both players score.
3. When lost for a turn, put that back in the cards to play.


Super Bonus Question

Should the interviewee manage to add N players support, I still have one
additional change. It's a bit more challenging than the previous requirements:
"don't assume the cards have unique values". The details are:

Remove the assumption that the cards are unique (that is, go to a standard
    deck) If players have the same valued card, they draw an additional card,
       repeat until one has a higher card The person with the highest card
       takes all the cards, scoring one each Only the players that tie
       continue drawing cards (1+ players may sit out on the additional draws)
  Only one person has made it this far, setting the gold standard for this
  interview question.

Only the time pressure

What I like about this question is that it lacks any trickery or random
algorithmic knowledge. The pacing seems to work well: interviewees manage
varying degrees of the problem. Given the difficulties I've seen people have,
        it seems to test a good cross-section of abilities.

As interview practice, try coding the answer. Pay attention to which bits
cause you trouble, and watch how long it takes you.
 
*/

namespace algo_two_player_card_game
{
  using Card = uint32_t;

  class CardPlayer
  {
  public:
    CardPlayer() = delete;
    CardPlayer(std::string const &name)
        : name_(name)
    {}

    void prepare_cards()
    {
      std::generate_n(back_inserter(cards_),
                      NUM_OF_CARDS_PER_PLAYER,
                      CardRandom());
    }

    bool has_card() { return !cards_.empty(); }

    // assums that client call it when is has a card to return
    Card get_card()
    {
      auto card = cards_.back();
      cards_.pop_back();
      return card;
    }

    void score_card(Card card) { scores_.push_back(card); }

    uint32_t get_score() { return scores_.size(); }

    std::string get_name() { return name_; }

    void print_cards()
    {
      PRINT_ELEMENTS(cards_, name_ + ":play  cards: ");
      PRINT_ELEMENTS(scores_, name_ + ":score cards: ");
    }

  private:
    std::string name_;
    std::vector<Card> cards_;
    std::vector<Card> scores_;

    static const uint32_t NUM_OF_CARDS_PER_PLAYER;
    static const uint32_t MAX_NUM_OF_CARDS;

    class CardRandom
    {
    public:
      int operator()() { return udist(dre); }

    private:
      static std::default_random_engine dre;
      static std::uniform_int_distribution<Card> udist;
    };
  };

  // *cxx-static*

  const uint32_t CardPlayer::NUM_OF_CARDS_PER_PLAYER{10};
  const uint32_t CardPlayer::MAX_NUM_OF_CARDS{24};

  std::default_random_engine CardPlayer::CardRandom::dre;

  std::uniform_int_distribution<Card> CardPlayer::CardRandom::udist{
    1, MAX_NUM_OF_CARDS};

  // simple

  class CardGame_1
  {
  public:
    CardGame_1()
        : lplayer("player1")
        , rplayer("player2")
    {
      lplayer.prepare_cards();
      rplayer.prepare_cards();
    }

    // when has the same card, score them to both

    void play()
    {
      while (lplayer.has_card())
      {
        auto lcard = lplayer.get_card();
        auto rcard = rplayer.get_card();

        if (lcard < rcard)
        {
          rplayer.score_card(rcard);
        }
        else if (lcard > rcard)
        {
          lplayer.score_card(lcard);
        }
        else
        {
          lplayer.score_card(lcard);
          rplayer.score_card(lcard);
        }
      }
    }

    void announce()
    {
      std::string winner{};
      uint32_t score{};

      lplayer.print_cards();
      rplayer.print_cards();

      if (lplayer.get_score() < rplayer.get_score())
      {
        winner = rplayer.get_name();
        score  = rplayer.get_score();
      }
      else
      {
        winner = lplayer.get_name();
        score  = lplayer.get_score();
      }

      std::cout << "=================================" << std::endl;
      std::cout << "winner: " << winner << " got " << score << " scores"
                << std::endl;
    }

  private:
    CardPlayer lplayer;
    CardPlayer rplayer;
  };

} // namespace algo_two_player_card_game

TEST(AlgoTwoPlayerCardGame, Player)
{
  using namespace algo_two_player_card_game;

  {
    CardPlayer player1("player 1");
    EXPECT_THAT(player1.has_card(), false);
  }

  {
    CardPlayer player1("player 1");
    player1.prepare_cards();
    EXPECT_THAT(player1.has_card(), true);
  }

  {
    CardPlayer player1("player 1");
    player1.score_card(Card{1});
    player1.score_card(Card{2});
    player1.score_card(Card{3});
    EXPECT_THAT(player1.get_score(), 3);
  }

  {
    CardPlayer player1("player 1");

    player1.prepare_cards();
    player1.print_cards();

    player1.get_card();
    player1.get_card();
    player1.get_card();

    player1.score_card(player1.get_card());
    player1.score_card(player1.get_card());
    player1.score_card(player1.get_card());

    player1.print_cards();

    EXPECT_THAT(player1.get_score(), 3);
  }
}

TEST(AlgoTwoPlayerCardGame, Game_1)
{
  using namespace algo_two_player_card_game;

  CardGame_1 game;

  game.play();
  game.announce();
}

namespace algo_two_player_card_game
{
  // extended

  class CardGame_2
  {
  public:
    CardGame_2() = delete;

    CardGame_2(size_t number_of_players)
    {
      // to avoid reallocations

      running_players_.reserve(MAX_NUM_OF_PLAYERS);
      finished_players_.reserve(MAX_NUM_OF_PLAYERS);

      prepare_players_(number_of_players);
    }

    // o when has the same card, score them to both
    // o do not put a card back when lost. if dose, will have more turns to
    // finish.

    // NOTE: when not see the same value, put back the one which is not
    // scored since GetCard() removes it from the vector. Otherwise, lost
    // them. so now the number of cards that all players started with should
    // match with the sum of the number of scores and the number of cards
    // which is not played at the end of run.

    void play()
    {
      Card lcard{};
      Card rcard{};

      while (has_running_players_())
      {
        auto selected = get_players_to_play_();

        // to avoid ctors and copying them back to vector once used.

        auto &lplayer = running_players_[selected.first];
        auto &rplayer = running_players_[selected.second];

        // cards remvoed from players
        lcard = lplayer.get_card();
        rcard = rplayer.get_card();

        if (lcard < rcard)
        {
          rplayer.score_card(rcard);
        }
        else if (lcard > rcard)
        {
          lplayer.score_card(lcard);
        }
        else
        {
          lplayer.score_card(lcard);
          rplayer.score_card(lcard);
        }

        move_finished_players_();
      }
    }

    // copy version to show and this leads to use shared_ptr
    //
    // void Play()
    // {
    //   Card first_card{}, second_card{};

    //   // NOTE: this causes two ctor/dtors
    //   CardPlayer first_player{"first"};
    //   CardPlayer second_player{"second"};

    //   while(HasRunningPlayers())
    //   {
    //     ++number_of_plays_;

    //     auto selected_players = GetRunningPlayers_();

    //     // copy players
    //     first_player = running_players_[selected_players.first];
    //     second_player = running_players_[selected_players.second];

    //     first_card = first_player.GetCard();
    //     second_card = second_player.GetCard();

    //     if(first_card == second_card)
    //     {
    //       second_player.SaveCardToScore(second_card);
    //       first_player.SaveCardToScore(first_card);
    //     }
    //     else if(first_card > second_card)
    //     {
    //       first_player.SaveCardToScore(first_card);
    //       second_player.SaveCardToPlay(second_card);
    //     }
    //     else
    //     {
    //       first_player.SaveCardToPlay(first_card);
    //       second_player.SaveCardToScore(second_card);
    //     }

    //     // copy players back
    //     running_players_[selected_players.first] = first_player;
    //     running_players_[selected_players.second] = second_player;

    //     MoveFinishedPlayers_();
    //   }
    // }

    void announce()
    {
      std::string winner{};
      uint32_t score{};

      // consider const later

      for (auto &e : finished_players_)
      {
        if (score < e.get_score())
        {
          score  = e.get_score();
          winner = e.get_name();
        }
      }

      std::cout << "=================================" << std::endl;
      std::cout << "winner: " << winner << " got " << score << " scores"
                << std::endl;
    }

  private:
    static const uint32_t MAX_NUM_OF_PLAYERS;

    std::vector<CardPlayer> running_players_;
    std::vector<CardPlayer> finished_players_;

    void prepare_players_(size_t number)
    {
      std::string name{};

      for (size_t i = 0; i < number; ++i)
      {
        name        = "player " + std::to_string(i);
        auto player = CardPlayer(name);
        player.prepare_cards();
        running_players_.push_back(player);
      }
    }

    // deals players that has cards to play so no check is needed to see if
    // it has cards to play
    //
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
    //
    // if(running_players_.size() == 0)
    //
    // starts to see hang with > 2 players sometimes but not always. Have to
    // change since as the reason above, can see odd number of running players
    // and this cause hang on rand() % 1. Have to end the play when there is
    // only one running player that still have cards to play.

    bool has_running_players_()
    {
      // if (running_players_.size() == 0 ||
      //     running_players_.size() == 1)
      //   return false;
      // return true;

      return running_players_.size() < 2 ? false : true;
    }

    size_t get_player_index_() { return rand() % running_players_.size(); }

    // since running_players_ has only players that has cards to play

    std::pair<size_t, size_t> get_players_to_play_()
    {
      auto lplayer = get_player_index_();
      auto rplayer = get_player_index_();

      // until find the other to play
      while (lplayer == rplayer)
        rplayer = get_player_index_();

      return make_pair(lplayer, rplayer);
    }

    // see how cxx-move and *cxx-iter-invalid* used
    void move_finished_players_()
    {
      for (auto it = running_players_.begin(); it != running_players_.end();
           // nothing
      )
      {
        // if do not cards
        if (!it->has_card())
        {
          finished_players_.push_back(std::move(*it));
          it = running_players_.erase(it);
        }
        else
          ++it;
      }
    }
  };

  const uint32_t CardGame_2::MAX_NUM_OF_PLAYERS{10};

} // namespace algo_two_player_card_game

TEST(AlgoTwoPlayerCardGame, Game_2)
{
  using namespace algo_two_player_card_game;

  {
    CardGame_2 game(2);

    game.play();
    game.announce();
  }
  {
    CardGame_2 game(4);

    game.play();
    game.announce();
  }
  {
    CardGame_2 game(3);

    game.play();
    game.announce();
  }
}

namespace algo_two_player_card_game
{
  // extended and shared_ptr
  //
  // note: not much benefit of using shared_ptr since already removed to use
  // copy overhead in play() by using reference.

  class CardGame_3
  {
  public:
    CardGame_3() = delete;

    CardGame_3(size_t number_of_players)
    {
      prepare_players_(number_of_players);
    }

    // o when has the same card, score them to both
    // o do not put a card back when lost. if dose, will have more turns to
    // finish.

    // NOTE: when not see the same value, put back the one which is not
    // scored since GetCard() removes it from the vector. Otherwise, lost
    // them. so now the number of cards that all players started with should
    // match with the sum of the number of scores and the number of cards
    // which is not played at the end of run.

    void play()
    {
      Card lcard{};
      Card rcard{};

      while (has_running_players_())
      {
        auto selected = get_players_to_play_();

        // to avoid ctors and copying them back to vector once used.

        auto &lplayer = running_players_[selected.first];
        auto &rplayer = running_players_[selected.second];

        // cards remvoed from players
        // note:
        lcard = lplayer->get_card();
        rcard = rplayer->get_card();

        if (lcard < rcard)
        {
          rplayer->score_card(rcard);
        }
        else if (lcard > rcard)
        {
          lplayer->score_card(lcard);
        }
        else
        {
          lplayer->score_card(lcard);
          rplayer->score_card(lcard);
        }

        move_finished_players_();
      }
    }

    void announce()
    {
      std::string winner{};
      uint32_t score{};

      // consider const later

      for (auto &e : finished_players_)
      {
        // note:
        if (score < e->get_score())
        {
          score  = e->get_score();
          winner = e->get_name();
        }
      }

      std::cout << "=================================" << std::endl;
      std::cout << "winner: " << winner << " got " << score << " scores"
                << std::endl;
    }

  private:
    static const uint32_t MAX_NUM_OF_PLAYERS;

    // note:
    std::vector<std::shared_ptr<CardPlayer>> running_players_;
    std::vector<std::shared_ptr<CardPlayer>> finished_players_;

    void prepare_players_(size_t number)
    {
      std::string name{};

      for (size_t i = 0; i < number; ++i)
      {
        name = "player " + std::to_string(i);

        // note: to use shared_ptr
        auto player = make_shared<CardPlayer>(name);
        player->prepare_cards();

        running_players_.push_back(player);
      }
    }

    bool has_running_players_()
    {
      return running_players_.size() < 2 ? false : true;
    }

    size_t get_player_index_() { return rand() % running_players_.size(); }

    // since running_players_ has only players that has cards to play

    std::pair<size_t, size_t> get_players_to_play_()
    {
      auto lplayer = get_player_index_();
      auto rplayer = get_player_index_();

      // until find the other to play
      while (lplayer == rplayer)
        rplayer = get_player_index_();

      return make_pair(lplayer, rplayer);
    }

    // see how cxx-move and *cxx-iter-invalid* used
    void move_finished_players_()
    {
      for (auto it = running_players_.begin(); it != running_players_.end();
           // nothing
      )
      {
        // if do not cards
        // note:
        if (!(*it)->has_card())
        {
          finished_players_.push_back(std::move(*it));
          it = running_players_.erase(it);
        }
        else
          ++it;
      }
    }
  };

  const uint32_t CardGame_3::MAX_NUM_OF_PLAYERS{10};

} // namespace algo_two_player_card_game

TEST(AlgoTwoPlayerCardGame, Game_3)
{
  using namespace algo_two_player_card_game;

  {
    CardGame_3 game(2);

    game.play();
    game.announce();
  }
  {
    CardGame_3 game(4);

    game.play();
    game.announce();
  }
  {
    CardGame_3 game(3);

    game.play();
    game.announce();
  }
}

/*
={=========================================================================
algo-swap: swap without a temporary
*/
namespace algoswap
{
  void swap_1(int &a, int &b)
  {
    a = a + b;
    b = a - b; // b = a
    a = a - b; // a = b
  }

  // cxx-xor
  // X XOR  X  = 0
  // X XOR  0  = X
  // X XOR  1  = ~X    // X XOR (~0) = ~X
  // X XOR ~X  = 1
  //
  // 00000000000000000000111111000000 // x
  // 00000000000000000000111111000000 // x^0
  // 11111111111111111111000000111111 // x^(~0) but not x^1
  //
  // x =  1010; y = 0011;          // before
  // x =  1001 =  1010  ^ 0011     // x = x^y
  // y =  1010 = [1001] ^ 0011     // y = x^y, y = (x^y)^y = (x^0) = x
  // x = [0011] =  1001 ^ [1010]   // x = x^y, x = (x^y)^x = (y^0) = y
  // x = 0011; y = 1010;           // after

  void swap_2(int &a, int &b)
  {
    a = a ^ b;
    b = a ^ b; // b = a^b = (a^b)^b = a^0 = a
    a = a ^ b; // a = a^b = (a^b)^a = b^0 = b
  }
} // namespace algoswap

TEST(AlgoSwap, check_swap)
{
  using namespace algoswap;

  {
    int a = 9, b = 4;

    swap_1(a, b);

    EXPECT_THAT(a, Eq(4));
    EXPECT_THAT(b, Eq(9));
  }

  {
    int a = 9, b = 4;

    swap_2(a, b);

    EXPECT_THAT(a, Eq(4));
    EXPECT_THAT(b, Eq(9));
  }
}

/*
={=========================================================================
algo-occurance find a number which is seen odd times cxx-xor

ex. input {2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};
    sorted 2 2 4 4 4 4 6 6 8 8 10 10 12 12 12 (15)
    answer is 12

Amazon phone interview question. 02/05/2013

You are going to be passed as input an array with an interesting property: the
array contains non-negative numbers that appear an even number of times
throughout the array, save one, that appears an odd number of times.

Your code should accept the array as input and return the number that appears
an odd number of times as output.

[1, 3, 1]                                             // returns 3
[2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4]    // returns 12
[1, 0, 1, 0, 1, 4, 4, 0, 3, 7, 0, 3, 7]               // returns 1

[1, 3, 1]
0001, 0011, 0001
0001 ^ 0011 ^ 0001
(0001 ^ 0001) ^ 0011 = 0000 ^ 0011 = 0011 (3)

Uses two *cxx-xor* `XOR properties` and uses that xor is transitive. If do xor
even times, becomes 0.

00 0      Y xor Y = 0  (2 times)
01 1      0 xor Y = Y  (3 times)
10 1      Y xor Y = 0  (4 times)
11 0      0 xor Y = Y  (5 times)
 

algo-leetcode-136. Single Number, Easy

Given a non-empty array of integers, every element appears twice except for
one. Find that single one.

Your algorithm should have a linear runtime complexity. Could you implement it
`without using extra memory`?

Example 1:
Input: [2,2,1]
Output: 1

Example 2:
Input: [4,1,2,1,2]
Output: 4

when use cxx-xor:

Runtime: 16 ms, faster than 96.66% of C++ online submissions for Single
Number.

Memory Usage: 9.7 MB, less than 65.99% of C++ online submissions for Single
Number.

*/

namespace algooccurance
{
  // cxx-xor
  unsigned int find_number_odd_times_1(const vector<unsigned int> &input)
  {
    unsigned int result{};

    for (const auto e : input)
      result ^= e;

    return result;
  }

  unsigned int find_number_odd_times_2(const vector<unsigned int> &input)
  {
    map<size_t, size_t> imap{};

    // put them into a map<key, count>
    for (const auto e : input)
      ++imap[e];

    auto pos_return =
      find_if(imap.cbegin(), imap.cend(), [](const pair<size_t, size_t> &e) {
        // odd times
        if (e.second % 2)
          return true;

        return false;
      });

    return (pos_return != imap.cend()) ? pos_return->first : 0;
  }

  unsigned int find_number_odd_times_3(const vector<unsigned int> &input)
  {
    size_t result{};
    multiset<size_t> imset{input.cbegin(), input.cend()};

    // put them into a map<key, count>
    for (const auto e : imset)
    {
      if (imset.count(e) % 2)
      {
        result = e;
        break;
      }
    }

    return result;
  }
} // namespace algooccurance

TEST(AlgoOccurance, check_seen_odd_times)
{
  using namespace algooccurance;

  std::vector<std::function<unsigned int(const std::vector<unsigned int>)>>
    coll{find_number_odd_times_1,
         find_number_odd_times_2,
         find_number_odd_times_3};

  {
    // 2 2 4 4 4 4 6 6 8 8 10 10 12 12 12 (15)
    const std::vector<unsigned int>
      input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};

    for (const auto &f : coll)
    {
      EXPECT_THAT(f(input), 12);
    }
  }

  {
    // find_if() returns the first match. so remove '4' to make it the first odd
    // num of sequence:
    // 2 2 {4 4 4} 6 6 8 8 10 10 {12 12 12} (15)

    const std::vector<unsigned int>
      input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2};

    // note that xor version do not pick the first sequence.
    EXPECT_THAT(coll[0](input), 8);

    EXPECT_THAT(coll[1](input), 4);
    EXPECT_THAT(coll[2](input), 4);
  }
}

/*
={=========================================================================
algo-occurance

When input a, and b are 0 <= a <= b <= 100,000,000), write a code to find
out how many k integer appears.

e.g.,
input: 11, 12, 13, 14, 15, k = 1

k appears 6 times.

EXPECT_THAT(count_occurance_1(const std::vector<int> coll, 1), 6);

*/

namespace algooccurance
{
  int count_occurance_1(const std::vector<int> &input, int key)
  {
    std::map<char, int> lookup{};

    // builds lookup table using input
    for (auto e : input)
    {
      auto value = std::to_string(e);

      for (auto c : value)
        ++lookup[c];
    }

    // since the key is char but not int
    auto ret = lookup.find(key + '0');

    if (ret != lookup.end())
      return ret->second;

    // not found
    return 0;
  }

  // can use lookup table approach. the key should be [0,9] and can reduce
  // lookup table size?
  int count_occurance_2(const std::vector<int> &input, int key)
  {
    // all inited as 0
    int lookup[256]{};

    // builds lookup table using input
    for (auto e : input)
    {
      auto value = std::to_string(e);

      for (auto c : value)
        ++lookup[(int)c];
    }

    return lookup[key + '0'];
  }

  // best so far
  int count_occurance_3(const std::vector<int> &input, int key)
  {
    int digit{}, count{};

    for (auto value : input)
    {
      // as algo-itoa
      for (; value;)
      {
        digit = value % 10;

        if (digit == key)
          ++count;

        value /= 10;
      }
    }

    return count;
  }
} // namespace algooccurance

TEST(AlgoOccurance, check_count_key)
{
  using namespace algooccurance;

  std::vector<std::function<int(const std::vector<int> &, int)>> imps{
    count_occurance_1,
    count_occurance_2,
    count_occurance_3};

  {
    std::vector<int> coll{11, 12, 13, 14, 15};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(coll, 1), 6);
      EXPECT_THAT(f(coll, 0), 0);
    }
  }
}

/*
={=========================================================================
algo-occurance

48. The most frequent element in a range

Write a function that, given a range, returns the most frequent element and the
number of times it appears in the range. If more than one element appears the
same maximum number of times then the function should return all the elements.

For instance, for the range {1,1,3,5,8,13,3,5,8,8,5}, it should return {5, 3}
and {8, 3}.

std::vector<pair<int, int>> f(std::vector<int> const &coll);


algo-leetcode-191

169. Majority Element, Easy

Given an array of size n, find the majority element. The majority element is the
element that appears more than ⌊ n/2 ⌋ times.

You may assume that the array is non-empty and the majority element always exist
in the array.

Example 1:
Input: [3,2,3]
Output: 3

Example 2:
Input: [2,2,1,1,1,2,2]
Output: 2

*/

namespace algooccurance
{
  // 2020.07.02
  std::vector<pair<int, int>> f1(const std::vector<int> &coll)
  {
    // builds count map<value, count>
    std::map<int, int> cmap{};

    for (auto e : coll)
    {
      ++cmap[e];
    }

    // build occurance multi-map<count, pair<value,count>>
    std::multimap<int, std::pair<int, int>> omap{};

    for (auto e : cmap)
    {
      std::pair<int, int> value{e.first, e.second};
      omap.emplace(std::make_pair(e.second, value));
    }

    // since multimap is sorted by count, the last is the biggest; most occurred
    // one.
    std::vector<std::pair<int, int>> ret{};

    if (omap.size())
    {
      auto found = omap.rbegin()->first;

      for (auto it = omap.equal_range(found).first;
           it != omap.equal_range(found).second;
           ++it)
        ret.emplace_back(it->second);
    }

    return ret;
  }

  // from "U48_Text"
  // may merge loop1 and loop2 into one.
  template <typename T>
  std::vector<std::pair<T, std::size_t>> f2(const std::vector<T> &input)
  {
    // loop1. builds a occurance table
    std::map<T, std::size_t> counts{};

    for (const auto &e : input)
      ++counts[e];

    // loop2. find the max (occurrance)
    //
    // *cxx-algo-max-element* *cxx-algo-minmax*
    // op is used to compare two elements:
    // op(elem1,elem2)
    // It should return true when the first element is less than the second
    // element.
    auto most = std::max_element(counts.cbegin(),
                                 counts.cend(),
                                 [](const std::pair<T, std::size_t> &e1,
                                    const std::pair<T, std::size_t> &e2) {
                                   return e1.second < e2.second;
                                 });

    // loop3. find elements which are the same as `most` occurrances
    std::vector<std::pair<T, size_t>> result{};

    std::copy_if(counts.cbegin(),
                 counts.cend(),
                 std::back_inserter(result),
                 [&](const auto &e) { return e.second == most->second; });

    return result;
  }

  std::vector<pair<int, int>> f3(std::vector<int> const &coll)
  {
    // loop1. 100 and input value range is [0,100)
    std::vector<int> bit_set(100);

    // first loop to count occurance of the input

    for (auto e : coll)
      ++bit_set[e];

    // loop2. to find the most frequent
    //
    // why? becuase to use single loop to find max and to find elements that
    // shown max times if there's only one max item then no need to have two
    // loop like here
    //
    // can do in a single loop?

    int current_element{};
    int current_max{};
    std::vector<pair<int, int>> result{};

    for (size_t i = 0; i < bit_set.size(); ++i)
    {
      if (current_max < bit_set[i])
      {
        // reset return coll
        result.clear();

        // update current set
        current_max     = bit_set[i];
        current_element = i;
      }
      else if (current_max == bit_set[i])
      {
        // push current to the return coll
        result.push_back(make_pair(current_element, current_max));

        // update current set
        current_max     = bit_set[i];
        current_element = i;
      }
    }

    // have to push the current which is the last and is not pushed in the for
    // loop
    if (current_element)
    {
      result.push_back(make_pair(current_element, current_max));
    }

    return result;
  }

  // take same approach as f2.
  int majority_1(const std::vector<int> &input)
  {
    std::map<int, size_t> omap{};

    for (const auto e : input)
      ++omap[e];

    // returns iterator
    auto most = std::max_element(omap.cbegin(),
                                 omap.cend(),
                                 [](const std::pair<int, std::size_t> &e1,
                                    const std::pair<int, std::size_t> &e2) {
                                   return e1.second < e2.second;
                                 });

    return most->first;
  }

  // take same approach as f2 and do max_element manually
  int majority_2(const std::vector<int> &input)
  {
    std::map<int, size_t> omap{};

    for (const auto e : input)
      ++omap[e];

    // std::max_element()
    {
      int cvalue{};
      size_t cmost{std::numeric_limits<size_t>::min()};

      for (const auto e : omap)
      {
        if (e.second > cmost)
        {
          cvalue = e.first;
          cmost  = e.second;
        }
      }

      return cvalue;
    }
  }

  // take same approach as f2 and do max_element manually
  // single loop
  int majority_3(const std::vector<int> &input)
  {
    std::map<int, size_t> omap{};
    int cvalue{};
    size_t cmost{std::numeric_limits<size_t>::min()};

    for (const auto e : input)
    {
      ++omap[e];

      if (omap[e] > cmost)
      {
        cvalue = e;
        cmost  = omap[e];
      }
    }

    return cvalue;
  }

  // can use std::unordered_map since do not need sorted coll.
  // int majority_4(const std::vector<int> &input);

  // Approach 6: Boyer-Moore Voting Algorithm
  //
  // Intuition
  // If we had some way of counting instances of the majority element as +1 and
  // instances of any other element as -1, summing them would make it obvious
  // that the majority element is indeed the majority element.
  //
  // Algorithm
  // Essentially, what Boyer-Moore does is look for a suffix suf of nums
  // where suf[0] is the majority element in that suffix.
  //
  // To do this, we maintain a count, which is incremented whenever we see an
  // instance of our current candidate for majority element and decremented
  // whenever we see anything else. Whenever count equals 0, we effectively
  // forget about everything in nums up to the current index and consider the
  // current number as the candidate for majority element.
  //
  // It is not immediately obvious why we can get away with forgetting prefixes
  // of nums - consider the following examples (pipes are inserted to separate
  // runs of nonzero count).
  //
  // [7, 7, 5, 7, 5, 1 | 5, 7 | 5, 5, 7, 7 | 7, 7, 7, 7]
  //
  // Here, the 7 at index 0 is selected to be the first candidate for majority
  // element. count will eventually reach 0 after index 5 is processed, so the 5
  // at index 6 will be the next candidate. In this case, 7 is the true majority
  // element, so by disregarding this prefix, we are ignoring an equal number of
  // majority and minority elements - therefore, 7 will still be the majority
  // element in the suffix formed by throwing away the first prefix.
  //
  // [7, 7, 5, 7, 5, 1 | 5, 7 | 5, 5, 7, 7 | 5, 5, 5, 5]
  //
  // Now, the majority element is 5 (we changed the last run of the array from
  // 7s to 5s), but our first candidate is still 7. In this case, our candidate
  // is not the true majority element, but we still cannot discard more majority
  // elements than minority elements (this would imply that count could reach -1
  // before we reassign candidate, which is obviously false).
  //
  // Therefore, given that it is impossible (in both cases) to discard more
  // majority elements than minority elements, we are safe in discarding the
  // prefix and attempting to recursively solve the majority element problem for
  // the suffix. Eventually, a suffix will be found for which count does not hit
  // 0, and the majority element of that suffix will necessarily be the same as
  // the majority element of the overall array.

  int majority_4(const std::vector<int> &input)
  {
    // candidate
    int suffix{};
    int count{};

    for (auto e : input)
    {
      if (count == 0)
        suffix = e;

      e == suffix ? ++count : --count;
    }

    return suffix;
  }

  // from discussion:
  //
  // One line solution in Python
  //
  // NOTICE that the majority element always exist in the array,so that the
  // middle always is the answer
  //
  // again, this is key:
  //
  // The problem states there must be a majority element; it has to show up >
  // n/2 times NOT just n/2. Something like [1, 1, 1, 1, 2, 3, 4, 5] is an
  // invalid input.
  //
  // Hey guys, please READ and UNDERSTAND the problem statement before making
  // comments complaining about how this doesn't work for certain test cases.
  // The problem states there must be a majority element; it has to show up >n/2
  // times NOT just n/2. Something like [1, 1, 1, 1, 2, 3, 4, 5] is an invalid
  // input.

  int majority_5(const std::vector<int> &input)
  {
    std::vector<int> coll{input};

    std::sort(coll.begin(), coll.end());
    return coll[coll.size() / 2];
  }
} // namespace algooccurance

TEST(AlgoOccurance, check_most_seen)
{
  using namespace algooccurance;

  // do not use vector of functions due to template function.
  // 1 1 3 3 5 5 5 8 8 8 13 (11)
  {
    std::vector<int> coll{1, 1, 3, 5, 8, 13, 3, 5, 8, 8, 5};
    auto result = f1(coll);
    EXPECT_THAT(result, ElementsAre(make_pair(5, 3), make_pair(8, 3)));
  }

  {
    std::vector<int> coll{1, 1, 3, 5, 8, 13, 3, 5, 8, 8, 5};
    auto result = f2(coll);
    EXPECT_THAT(result, ElementsAre(make_pair(5, 3), make_pair(8, 3)));
  }

  {
    std::vector<int> coll{1, 1, 3, 5, 8, 13, 3, 5, 8, 8, 5};
    auto result = f3(coll);
    EXPECT_THAT(result, ElementsAre(make_pair(5, 3), make_pair(8, 3)));
  }

  // majority cases
  {
    std::vector<int> coll{3, 2, 3};
    std::vector<std::function<int(const std::vector<int> &)>> imps{majority_1,
                                                                   majority_2,
                                                                   majority_3,
                                                                   majority_4};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(coll), 3);
    }
  }

  // majority cases
  {
    std::vector<int> coll{1, 1, 1, 1, 2, 3, 4, 5};
    std::vector<std::function<int(const std::vector<int> &)>> imps{majority_1,
                                                                   majority_2,
                                                                   majority_3,
                                                                   majority_4};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(coll), 1);
    }
  }

  // majority cases
  {
    std::vector<int> coll{7, 7, 5, 7, 5, 1, 5, 7, 5, 5, 7, 7, 7, 7, 7, 7};
    std::vector<std::function<int(const std::vector<int> &)>> imps{majority_1,
                                                                   majority_2,
                                                                   majority_3,
                                                                   majority_4,
                                                                   majority_5};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(coll), 7);
    }
  }

  // majority cases, invalid input
  {
    std::vector<int> coll{1, 1, 1, 1, 2, 3, 4, 5};
    std::vector<std::function<int(const std::vector<int> &)>> imps{majority_5};

    {
      EXPECT_THAT(imps[0](coll), Ne(1));
    }
  }
}

/*
={=========================================================================
algo-occurance
find the longest sequence of input char array.
1. firstly, O(n) for time, O(1) for space
2. secondly, can do better than O(n)?

Follow-ups:

Q: How to handle when there are many with the same length?
A: Currently use > which means it will have the first. If use >= then will
have the last. If want to keep all which has the same length then can use
container to keep those?

Q: Is there any other way to reduce time complexity?
A: Yes, there is

 1 2 3 4 5 6 7 8 9 10 11 12
 <-------> <-------->
           x        y

Suppose that the length of the first sequence is 5 and then see the current
index plus 5, which is 11.

* if [11] is the same as the current char then this second sequence might be
  longer one so *not skip* and see all chars. 

* if [11] is not the same as the current char then means a different sequence
  which cannot be longer than the sequnce of current char. So skip to 11 and
  starts search from there.

* HOWEVER, as with "AAAA|BCCC|CCCCC" case, cannot skip the second since need to
  know the start of C sequnce. 

So NO WAY to find chances to skip to reduce O(n) YET!
 
*/

namespace algooccurance
{
  MATCHER_P(NotEqPair, expected, "")
  {
    return arg.first != expected.first ||
           (int)arg.second != (int)expected.second;
  }

  // if can do without meeting O(n), O(1) then can use occurnace map in a single
  // loop.
  // std::pair<char, size_t> find_longest_1(const std::string &input)
  // {
  // }

  // single loop and no extra memory
  std::pair<char, size_t> find_longest_2(const std::string &input)
  {
    // current char and occurrance
    char cchar{};
    std::size_t coccur{std::numeric_limits<std::size_t>::min()};

    // saved char and occurrance
    char schar{};
    std::size_t soccur{std::numeric_limits<std::size_t>::min()};

    for (const auto e : input)
    {
      // can use cxx-xor if you fancy
      if (e != cchar)
      {
        if (coccur > soccur)
        {
          schar  = cchar;
          soccur = coccur;
        }

        cchar  = e;
        coccur = 1;
      }
      else
        ++coccur;
    }

    // to cover cases of {{"AAAAAAAAAAAA"}, {'A', 12}}
    if (coccur > soccur)
    {
      schar  = cchar;
      soccur = coccur;
    }

    return std::make_pair(schar, soccur);
  }

  // take "if" into the loop and keep updating "saved" but not only when see
  // different char.
  std::pair<char, size_t> find_longest_3(const std::string &input)
  {
    // current char and occurrance
    char cchar{};
    std::size_t coccur{std::numeric_limits<std::size_t>::min()};

    // saved char and occurrance
    char schar{};
    std::size_t soccur{std::numeric_limits<std::size_t>::min()};

    for (const auto e : input)
    {
      if (e != cchar)
      {
        cchar  = e;
        coccur = 1;
      }
      else
        ++coccur;

      if (coccur > soccur)
      {
        schar  = cchar;
        soccur = coccur;
      }
    }

    return std::make_pair(schar, soccur);
  }

  // Can do better than O(n)? Can skip some chars in searching a sequence.
  // not successful.
  pair<char, size_t> find_longest_4(const string &input)
  {
    char current_char{}, longest_char{};
    size_t current_occurance{}, longest_occurance{};

    size_t input_size = input.size();

    // take the first
    current_char      = input[0];
    current_occurance = 1;

    // *TN*
    // should kepp ++i here. Otherwise, would have no increase of i and
    // compare the same input in a loop.
    for (size_t i = 1; i < input_size; ++i)
    {
      // if see the different char. use XOR and looks fancy?
      if (current_char ^ input[i])
      {
        // save the previous sequence if it's the longest so far
        if (current_occurance > longest_occurance)
        {
          longest_occurance = current_occurance;
          longest_char      = current_char;
        }

        // see i and i + (current longest sequence-1) and skip them in
        // between if they are different
        //
        // if they are the same, don't skip so don't change i
        size_t check_skip = i + (current_occurance - 1);

        // *TN*
        // Have to have this check. Otherwise, access to out of range
        // of input.
        if (check_skip > input_size)
          break;

        // if they are different
        if (input[i] ^ input[check_skip])
        {
          // cout << "skipped : " << current_occurance-1 << endl;
          i += current_occurance - 1;
        }

        // reset and str a search again
        current_char      = input[i];
        current_occurance = 1;
      }
      // if see the same before
      else
        ++current_occurance;
    }

    // cout << "longest char      : " << longest_char << endl;
    // cout << "longest occurance : " << longest_occurance << endl;

    return pair<char, size_t>(longest_char, longest_occurance);
  }
} // namespace algooccurance

TEST(AlgoOccurance, check_longest_sequence)
{
  using namespace algooccurance;

  {
    std::vector<std::pair<std::string, std::pair<char, size_t>>> inputs{
      {{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"}, {'F', 18}},
      {{"AAABBCCCCDDD"}, {'C', 4}},
      {{"AAAAAAAAAAAA"}, {'A', 12}},
      {{"AAAABCBBBBBCCCCDDD"}, {'B', 5}}};

    std::vector<std::function<std::pair<char, size_t>(const std::string &)>>
      imps{find_longest_2, find_longest_3};

    for (const auto &f : imps)
    {
      for (const auto &i : inputs)
        EXPECT_THAT(f(i.first), i.second);
    }
  }

  {
    auto func = find_longest_4;

    // fails
    const std::string input_1{"AAAAAAAAAAAA"};
    EXPECT_THAT(func(input_1), NotEqPair(std::make_pair('A', 12)));

    // fails, return {A, 4}
    const std::string input_2{"AAAABCCCCCCCC"};
    EXPECT_THAT(func(input_2), NotEqPair(std::make_pair('C', 8)));
  }
}

/*
={=========================================================================
algo-unique

From Cracking the coding interview, p172,

Implement an algorithm to determine if a string has all unique characters. What
if you cannot use additional data structures?

bool f(const char *str);
return true if a string has all unique chars

o. Space? assume ASCII 256 chars
  ask for clarity. Since it's only for alphabet uppercase then use 32
  bset and reduce space requirement.

o. One simple optimization. return false if the length of input string is greater
  than the number of uniques chars in the set; e.g., ASCII, 256

  if( sizeString > 256 ) return false;

o. cstring or std::string?

o. time O(n) and space O(1)

*/

namespace algounique
{
  bool check_unique_1(const char *input)
  {
    bool table[256]{};

    for (; *input; ++input)
    {
      if (table[(int)*input])
        return false;

      table[(int)*input] = true;
    }

    return true;
  }

  bool check_unique_2(const char *input)
  {
    std::bitset<256> table{};

    for (; *input; ++input)
    {
      if (table[*input])
        return false;

      table[*input] = true;
    }

    return true;
  }

  // if use std::set. find() is the second loop
  bool check_unique_3(const char *input)
  {
    std::set<char> table{};

    for (; *input; ++input)
    {
      if (table.find(*input) != table.end())
        return false;

      table.insert(*input);
    }

    return true;
  }

  // if use std::string. find() is the second loop
  bool check_unique_4(const char *input)
  {
    std::string table{};

    for (; *input; ++input)
    {
      if (table.find(*input) != std::string::npos)
        return false;

      table += *input;
    }

    return true;
  }
} // namespace algounique

TEST(AlgoUnique, check_unique_lookup)
{
  using namespace algounique;

  {
    // pair<input, expected result>
    std::vector<std::pair<std::string, bool>> inputs{
      {"abcdefghijklmnopqa", false},
      {"abcdefghijklmnopqr", true}};

    std::vector<std::function<bool(const char *)>> imps{check_unique_1,
                                                        check_unique_2,
                                                        check_unique_3,
                                                        check_unique_4};

    for (const auto &f : imps)
    {
      for (const auto &i : inputs)
        EXPECT_THAT(f(i.first.c_str()), i.second);
    }
  }
}

/*
={=========================================================================
algo-unique

first unique byte *ex-interview*

Given an eventually very large stream of bytes, find the first unique byte in
this stream.

unsigned char f(const vector<unsigned char> &input);

see words; evntually, byte, first, unique

o time O(n) space O(1)
o Do not check input.
o Each input value are less than 256 and the # of input are less than unit
 max.
o What does "first" mean? first unique byte seen or first unique byte in
 input order?

Not byte seen first since every byte is seen first at first time and then may be
repeats later in the stream. The problem is that it can be shown in later of a
stream.
 
*/

namespace algounique
{
  unsigned char check_first_unique_1(const vector<unsigned char> &input)
  {
    // note:
    // input order depends on the input size. Here, the order is one for that
    // byte. NO.

    size_t occurance[256] = {}, order[256] = {};

    // build occurance and order
    size_t input_order{};

    for (const auto e : input)
    {
      ++occurance[e];
      // here order starts from 1
      order[e] = ++input_order;
    }

    // find the first byte *cxx-limits*
    unsigned char saved_input{};

    // const unsigned int UINT_MAX_ORDER = ~((unsigned int)0);
    // unsigned int saved_order = UINT_MAX_ORDER;
    size_t saved_order{numeric_limits<size_t>::max()};

    for (auto i = 0; i < 256; ++i)
    {
      if ((occurance[i] == 1) && (order[i] < saved_order))
      {
        // *TN* i refers to input value
        saved_input = i;
        saved_order = order[i];
      }
    }

    // // o. to print a char to int, have to define a var as int or to defind a
    // // char and to use +saved_input trick.
    // // http://www.cs.technion.ac.il/users/yechiel/c++-faq/print-char-or-ptr-as-number.html
    // cout << "saved input : " << +saved_input << endl;
    // cout << "saved order : " << saved_order << endl;

    return saved_input;
  }

  // same but use single structure

  unsigned char check_first_unique_2(const vector<unsigned char> &input)
  {
    // pair<occurance, index> and 255. size init but not list init
    std::vector<pair<size_t, size_t>> lookup_table(255);

    // first pass, build table with occurance and input index
    size_t input_index{};

    for (const auto e : input)
    {
      auto &entry = lookup_table[e];
      entry.first += 1;
      entry.second = input_index;

      ++input_index;
    }

    // second pass, find first and unique input byte
    unsigned char current_byte{};
    size_t current_order = numeric_limits<size_t>::max();

    unsigned char current_index{};

    for (const auto e : lookup_table)
    {
      if (e.first == 1 && e.second < current_order)
      {
        current_byte  = current_index;
        current_order = e.second;
      }

      ++current_index;
    }

    return current_byte;
  }

  // 2020.06
  // don't need to keep order as _1 and _2. No, see below.
  unsigned char check_first_unique_3(const std::vector<unsigned char> &input)
  {
    std::size_t table[256]{};

    for (auto e : input)
    {
      ++table[e];
    }

    for (size_t it = 0; it < sizeof table; ++it)
    {
      if (table[it] == 1)
        return it;
    }

    // how can we express "not found"?
    return 0;
  }
} // namespace algounique

TEST(AlgoUnique, check_first_unique)
{
  using namespace algounique;

  {
    // pair<input, expected result>
    const std::vector<std::pair<std::vector<unsigned char>, unsigned char>>
      inputs{
        // clang-format off
        {{20, 11, 23, 33, 34, 54, 44, 38, 215, 126, 101, 20, 11, 20, 54, 54, 44, 38, 38, 215, 126, 215, 23}, 33}
        // clang-format on
      };

    const std::vector<
      std::function<unsigned char(const std::vector<unsigned char> &)>>
      imps{check_first_unique_1, check_first_unique_2, check_first_unique_3};

    for (const auto &f : imps)
    {
      for (const auto &i : inputs)
        EXPECT_THAT(f(i.first), i.second);
    }
  }

  {
    // expects 2
    const std::vector<unsigned char> coll1{6, 6, 2, 3, 3, 1, 1, 5, 4, 4};

    // expects 5
    const std::vector<unsigned char> coll2{6, 6, 5, 3, 3, 1, 1, 2, 4, 4};

    EXPECT_THAT(check_first_unique_2(coll1), 2);
    EXPECT_THAT(check_first_unique_2(coll2), 5);

    EXPECT_THAT(check_first_unique_3(coll1), 2);
    // fails
    // EXPECT_THAT(check_first_unique_3(coll2), 5);
  }
}

/*
={=========================================================================
algo-unique

codility, absolute distinct count of this array
https://codility.com/train/AbsDistinct 

A non-empty zero-indexed array A consisting of N numbers is given. The
absolute distinct count of this array is the number of distinct absolute
values among the elements of the array.

For example, consider array A such that:

A[0] = -5    A[1] = -3    A[2] = -1
A[3] =  0    A[4] =  3    A[5] =  6

The absolute distinct count of this array is 5, because there are 5 distinct
absolute values among the elements of this array, namely 0, 1, 3, 5 and 6.

Write a function:

int absDistinct(int A[], int N);

that, given a non-empty zero-indexed array A consisting of N numbers, returns
  absolute distinct count of array A.

For example, given array A such that:

A[0] = -5    A[1] = -3    A[2] = -1
A[3] =  0    A[4] =  3    A[5] =  6

the function should return 5, as explained above.

Assume that:

N is an integer within the range [1..100,000]; each element of array A is an
integer within the range [-2,147,483,648..2,147,483,647]; array A is sorted in
non-decreasing order.

Complexity:

expected worst-case time complexity is O(N);
expected worst-case space complexity is O(N), beyond input storage (not
    counting the storage required for input arguments).

`Elements of input arrays can be modified.`


Test score: 100% 100 out of 100 points 
Detected time complexity: O(N) or O(N*log(N))
  

{Q} How to implement it using c? 

2014.02. How about running the modified binary search that use abs comparison on
each item? May be nlogn? That may be the same as set version?

2014.04.12. 
cannot use binary search since when move index from the start, no way to set
bottom and top.

i = 0, bs(1, n-1)
i = 1, bs(2, n-1)    // missing 0th.
...

2018.06.21
cannot see how to do it in C but why should when there is a better way?
 
*/

namespace algounique
{
  bool absLessThan(int a, int b) { return abs(a) < abs(b); }
  bool absEqual(int a, int b) { return abs(a) == abs(b); }

  int distinct_count_1(const vector<int> &A)
  {
    if (!A.size())
      return -1;

    vector<int> ivec;
    int count = 0;

    for (size_t i = 0; i < A.size(); ++i)
      ivec.push_back(A[i]);

    sort(ivec.begin(), ivec.end(), absLessThan);
    auto it_end_unique = unique(ivec.begin(), ivec.end(), absEqual);

    auto it_begin = ivec.begin();

    while (it_begin != it_end_unique)
    {
      ++it_begin;
      ++count;
    }

    return count;
  }

  // improve _1. possible to not use additional space for coll since the problem
  // description says that can modify input. However, still more than O(n) for
  // time.
  int distinct_count_2(const std::vector<int> &input)
  {
    // input check
    if (!input.size())
      return -1;

    // loop1. copy input since need to have collection which is modifiable.
    std::vector<int> coll{input};

    // loop2. abs sort
    std::sort(coll.begin(), coll.end(), [](int e1, int e2) {
      return std::abs(e1) < std::abs(e2);
    });

    // loop3.
    auto unique_end = std::unique(coll.begin(), coll.end(), [](int e1, int e2) {
      return std::abs(e1) == std::abs(e2);
    });

    return std::distance(coll.begin(), unique_end);
  }

  int distinct_count_3(const vector<int> &A)
  {
    // write your code in C++98
    int size = A.size();

    if (!size)
      return -1;

    std::set<int> iset;

    for (int i = 0; i < size; i++)
      iset.insert(abs(A[i]));

    return iset.size();
  }

  // use std::set
  int distinct_count_4(const std::vector<int> &input)
  {
    // the desciption says input is non-empty.
    // if (!input.size())
    //   return -1;

    std::set<int> coll{};

    // loop1.
    for (const auto e : input)
      coll.insert(std::abs(e));

    return coll.size();
  }

  // use std::unordered_set
  int distinct_count_5(const std::vector<int> &input)
  {
    std::unordered_set<int> coll{};

    // loop1.
    for (const auto e : input)
      coll.emplace(std::abs(e));

    return coll.size();
  }
} // namespace algounique

TEST(AlgoUnique, check_count_unique)
{
  using namespace algounique;

  {
    // pair<input, expected result>
    const std::vector<std::pair<std::vector<int>, int>> inputs{
      {{-5, -3, -1, 0, 3, 6}, 5}};

    const std::vector<std::function<int(const std::vector<int> &)>> imps{
      distinct_count_1,
      distinct_count_2,
      distinct_count_3,
      distinct_count_4};

    for (const auto &f : imps)
    {
      for (const auto &i : inputs)
        EXPECT_THAT(f(i.first), i.second);
    }
  }
}

/*
={=========================================================================
algo-unique

217. Contains Duplicate, Easy

Given an array of integers, find if the array contains any duplicates.

Your function should return true if any value appears at least twice in the
array, and it should return false if every element is distinct.

Example 1:

Input: [1,2,3,1]
Output: true
Example 2:

Input: [1,2,3,4]
Output: false
Example 3:

Input: [1,1,1,3,3,4,3,2,4,2]
Output: true

*/

namespace leetcode_easy_217
{
  // cannot use lookup table using input value since "integer" is too big.
  //
  // cxx-unordered
  // Assuming that the hashing strategy is well chosen and well implemented, you
  // can guarantee `amortized constant time` for insertions, deletions, and
  // element search

  // return true when there are duplicates
  //
  // Runtime: 60 ms, faster than 92.40% of C++ online submissions for Contains Duplicate.
  // Memory Usage: 20.2 MB, less than 45.77% of C++ online submissions for Contains Duplicate.

  bool check_duplicates_1(const std::vector<int> &nums)
  {
    std::unordered_set<int> coll{};

    for (const auto e : nums)
    {
      auto ret = coll.insert(e);

      if (ret.second == false)
        return true;
    }

    return false;
  }

  // use size from the discussion
  //
  // Runtime: 68 ms, faster than 72.85% of C++ online submissions for Contains Duplicate.
  // Memory Usage: 20.4 MB, less than 37.42% of C++ online submissions for Contains Duplicate.

  bool check_duplicates_2(const std::vector<int> &nums)
  {
    std::unordered_set<int> coll{nums.begin(), nums.end()};

    return (nums.size() != coll.size()) ? true : false;
  }
} // namespace leetcode_easy_217

TEST(AlgoUnique, check_unique_integer)
{
  using namespace leetcode_easy_217;

  auto imps = {check_duplicates_1, check_duplicates_2};

  for (const auto f : imps)
  {
    EXPECT_THAT(f({1, 2, 3, 1}), true);
    EXPECT_THAT(f({1, 2, 3, 4}), false);
    EXPECT_THAT(f({1, 1, 1, 3, 3, 4, 3, 2, 4, 2}), true);
  }
}

/*
={=========================================================================
algo-unique

219. Contains Duplicate II

Given an array of integers and an integer k, find out whether there are two
distinct indices i and j in the array such that nums[i] = nums[j] and the
absolute difference between i and j is at most k.

Example 1:

Input: nums = [1,2,3,1], k = 3
Output: true

Example 2:

Input: nums = [1,0,1,1], k = 1
Output: true

Example 3:

Input: nums = [1,2,3,1,2,3], k = 2
Output: false

*/

namespace leetcode_easy_219
{
  // fails on EXPECT_THAT(f({99, 99}, 2), true); since "at most"
  bool contain_duplicates_1(const std::vector<int> &nums, int k)
  {
    int first = 0;
    int last  = k;

    while (last < (int)nums.size())
    {
      if (nums[first] == nums[last])
        return true;

      ++first;
      last = first + k;
    }

    return false;
  }

  // Time Limit Exceeded when the input is huge
  bool contain_duplicates_2(const std::vector<int> &nums, int k)
  {
    auto loop = nums.size();

    for (int run = 0; run < (int)loop; ++run)
    {
      for (int scan = run + 1; (scan < (int)loop) && ((scan - run) <= k);
           ++scan)
      {
        if (nums[run] == nums[scan])
          return true;
      }
    }

    return false;
  }

  // from the discussion.
  // [Java] Linear Time Solution using HashMap
  // https://leetcode.com/problems/contains-duplicate-ii/discuss/824527/Java-Linear-Time-Solution-using-HashMap
  //
  // class Solution {
  //     public boolean containsNearbyDuplicate(int[] nums, int k) {
  //         Map<Integer, Integer> map = new HashMap();
  //         for(int i = 0; i < nums.length; i++) {
  //             if(map.containsKey(nums[i]) && Math.abs(i - map.get(nums[i])) <= k)
  //                 return true;
  //             map.put(nums[i], i);
  //         }
  //         return false;
  //     }
  // }
  //
  // tried it in c++
  //
  // Runtime: 52 ms, faster than 75.15% of C++ online submissions for Contains Duplicate II.
  // Memory Usage: 16.4 MB, less than 73.48% of C++ online submissions for Contains Duplicate II.

  bool contain_duplicates_3(const std::vector<int> &nums, int k)
  {
    auto loop = nums.size();

    // pair<value, index>
    std::unordered_multimap<int, int> coll{};

    for (int i = 0; i < (int)loop; ++i)
    {
      auto size  = coll.count(nums[i]);
      auto first = coll.find(nums[i]);

      // found and scan "equal_range"
      for (int scan = 0; scan < (int)size; ++scan)
      {
        if (std::abs(first->second - i) <= k)
          return true;

        ++first;
      }

      // if not found, insert it
      coll.insert(std::make_pair(nums[i], i));
    }

    return false;
  }
} // namespace leetcode_easy_219

TEST(AlgoUnique, unique_integer_in_range)
{
  using namespace leetcode_easy_219;

  // auto f = contain_duplicates_1;
  auto f = contain_duplicates_3;

  EXPECT_THAT(f({1, 2, 3, 1}, 3), true);
  EXPECT_THAT(f({1, 0, 1, 1}, 1), true);
  EXPECT_THAT(f({1, 2, 3, 1, 2, 3}, 2), false);

  EXPECT_THAT(f({99, 99}, 2), true);
}

/*
={=========================================================================
algo-reverse

From Cracking the coding interview, p173,

Write a function in C/C++ which reverse a null-terminated string.

void reverse(char* str);

*TN*
But do you need to reverse the string in the first place? With rbegin and
rend, any member functions or algorithms that operate on iterator ranges can
be used on the reverse version of the string. And if you want to search
through the string, you can use rfind to do what find does but starting from
the end of the string and moving backward. For large strings, or large numbers
of strings, reversing can be expensive, so avoid it if you can.

o use in/out parameter
o cstring but efficient
o strlen()-1 since array indexing is always [0, size-1], or [0,size) and in
  this code, [begin, end] but not [begin, end)

*/

namespace algoreverse
{
  // ansic, p62
  void reverse_string_1(char *input)
  {
    int str{}, end{};
    char temp{};

    for (end = strlen(input) - 1; str < end; ++str, --end)
    {
      // swap(str, end)
      temp = input[str], input[str] = input[end], input[end] = temp;
    }
  }

  // c style. assumces input is c-string since it's c version.
  void reverse_string_2(char *input)
  {
    // check input
    if (!input || !strlen(input))
      return;

    char *start{&input[0]};
    char *end{&input[strlen(input) - 1]};

    for (; start < end; ++start, --end)
      std::swap(*start, *end);
  }

  // if use std::string and still c-version is better
  std::string reverse_string_3(const std::string &input)
  {
    return std::string{input.crbegin(), input.crend()};
  }

  void reverse_string_4(std::string &input)
  {
    std::reverse(input.begin(), input.end());
  }
} // namespace algoreverse

TEST(AlgoReverse, check_string)
{
  using namespace algoreverse;

  {
    const std::vector<std::function<void(char *)>> imps{reverse_string_1,
                                                        reverse_string_2};

    for (const auto &f : imps)
    {
      char input[] = "REVERSE IT";
      f(input);
      EXPECT_THAT(input, StrEq("TI ESREVER"));
    }
  }

  {
    const std::vector<std::function<std::string(const std::string &)>> imps{
      reverse_string_3};

    for (const auto &f : imps)
    {
      std::string input{"REVERSE IT"};
      EXPECT_THAT(f(input), "TI ESREVER");
    }
  }

  {
    auto f = reverse_string_4;

    {
      std::string input{"REVERSE IT"};
      f(input);
      EXPECT_THAT(input, "TI ESREVER");
    }
  }
}

// Write a program to reverse a string with all its duplicates removed. Only
// the last instance of a character in the reverse string has to appear. Also,
// the following conditions are to be satisfied: Assume only Capital Letters.
//
// o. assume that input is ASCII and is all upper case chars. so can use
// unsigned int to check if it's a duplicate or not. if needs more range to
// cover then need to use something else.
//
// o. from the net

namespace algoreverse
{
  std::string reverse_string_6(const std::string &input)
  {
    std::string sout{};
    unsigned int bappeared{};

    // remove duplicates from input
    for (size_t i = 0, size = input.size(); i < size; ++i)
    {
      // only if not appeared before. use bitwise
      if (!(bappeared & (1 << (input[i] - 'A'))))
      {
        sout += input[i];
        bappeared |= (1 << (input[i] - 'A'));
      }
    }

    // sout: JTVAKISHL

    // return reverse;
    return std::string{sout.crbegin(), sout.crend()};
  }
} // namespace algoreverse

TEST(AlgoReverse, check_string_no_duplicates)
{
  using namespace algoreverse;

  auto func = reverse_string_6;

  std::string input{"JTVAKAVISHAAAL"};
  EXPECT_THAT(func(input), Eq("LHSIKAVTJ"));
}

/*
={=========================================================================
algo-anagram

From Cracking the coding interview, p174,

Given two strings, write a method to decide if one is a anagram(permutation) of
the other. 

bool f(string one, string two);

from STL

Testing for Unordered Equality

Both forms return whether the elements in the range [beg1,end1) are a
permutation of the elements in the range starting with beg2; that is, whether
they return equal elements in whatever order.

o. Space? assume ASCII 256 chars
  ask for clarity. Since it's only for alphabet uppercase then use 32
  bset and reduce space requirement.

o. One simple optimization. return false if the length of input strings
  are different

o. cstring or std::string?

o. time O(n) and space O(1)

*/

namespace algoanagram
{
  bool anagram_1(string one, string two)
  {
    // quick optimisation.
    if (one.size() != two.size())
      return false;

    std::sort(one.begin(), one.end());
    std::sort(two.begin(), two.end());

    return one == two ? true : false;
  }

  bool anagram_2(string one, string two)
  {
    // quick optimisation.
    if (one.size() != two.size())
      return false;

    std::bitset<256> table{};

    for (const auto e : one)
      table.set(e);

    for (const auto e : two)
    {
      if (!table[e])
        return false;
    }

    return true;
  }

  // Necessary since don't seem to meeet "anagram" definition.
  // To pass when there are duplicates in the input:
  //  1. remove duplicates
  //  2. move size check after removing duplicates.

  bool anagram_3(string one, string two)
  {
    bitset<256> bset{};

    auto one_end_unique = unique(one.begin(), one.end());
    one.erase(one_end_unique, one.end());

    auto two_end_unique = unique(two.begin(), two.end());
    two.erase(two_end_unique, two.end());

    if (one.size() != two.size())
      return false;

    for (const auto &e : one)
      bset[e] = 1;

    for (const auto &e : two)
    {
      if (!bset[e])
      {
        return false;
      }
    }

    return true;
  }
} // namespace algoanagram

TEST(AlgoAnagram, check_inputs)
{
  using namespace algoanagram;

  {
    const std::vector<std::function<bool(std::string, std::string)>> imps{
      anagram_1,
      anagram_2};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f("PARK", "APRK"), true);
      EXPECT_THAT(f("PARK", "APRKPARK"), false);
      EXPECT_THAT(f("PARK", "CARK"), false);
      EXPECT_THAT(f("PARK", "PAAA"), false);
    }
  }
}

/*
={=========================================================================
algo-palindrome algo-leetcode-9 9. Palindrome Number, Easy

Determine whether an integer is a palindrome. An integer is a palindrome when it
reads the same backward as forward.

bool is_palindrome(int input);

Example 1:

Input: 121
Output: true

Example 2:

Input: -121
Output: false

Explanation: From left to right, it reads -121. From right to left, it becomes
121-. Therefore it is not a palindrome.

Example 3:

Input: 10
Output: false

Explanation: Reads 01 from right to left. Therefore it is not a palindrome.

Follow up:
Coud you solve it without converting the integer to a string?

*/

namespace algopalindrome
{
  // do algo-atoi and algo-itoa at the same time and no need to reverse.
  //
  // Runtime: 108 ms, faster than 97.64% of C++ online submissions for
  // Palindrome Number.
  //
  // Memory Usage: 73 MB, less than 63.19% of C++ online submissions for
  // Palindrome Number.

  bool palindrome_1(int input)
  {
    int result{};
    int remains{};

    int value = input;

    // quick optimisation.
    int sign{};
    sign = (input < 0) ? -1 : 1;
    if (sign < 0)
      return false;

    while (value)
    {
      // as itoa(), get right-most value
      remains = value % 10;

      // as atoi(), reverse it
      result = remains + 10 * result;

      // as itoa(), reduce value
      value /= 10;
      // cout << "value: " << value << ", result: " << result << ", remains: " << remains << endl;
    }

    return result == input ? true : false;
  }

  // use more space but saves on the loop.
  bool palindrome_2(int input)
  {
    std::string coll{std::to_string(input)};

    size_t start{0};
    size_t end{coll.size() - 1};

    for (; start < end; ++start, --end)
      if (coll[start] != coll[end])
        return false;

    return true;
  }
} // namespace algopalindrome

TEST(AlgoPalindrome, check_integer_input)
{
  using namespace algopalindrome;

  {
    const std::vector<std::function<bool(int)>> imps{palindrome_1,
                                                     palindrome_2};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(121), true);
      EXPECT_THAT(f(-121), false);
      EXPECT_THAT(f(10), false);
      EXPECT_THAT(f(0), true);
      EXPECT_THAT(f(0000000123), false);
      EXPECT_THAT(f(1230000000), false);
      EXPECT_THAT(f(1234554321), true);
    }
  }
}

/*
={=========================================================================
algo-palindrome-string algo-leetcode-19

125. Valid Palindrome, Easy

Given a string, determine if it is a palindrome, considering only alphanumeric
characters and ignoring cases.

bool f(string s);

Note: For the purpose of this problem, we define empty string as valid palindrome.

Example 1:
Input: "A man, a plan, a canal: Panama"
Output: true

Example 2:
Input: "race a car"
Output: false
 
*/

namespace algo_palindrome
{
  // Runtime: 16 ms, faster than 17.06% of C++ online submissions for Valid
  // Palindrome.
  //
  // Memory Usage: 9 MB, less than 89.62% of C++ online submissions for Valid
  // Palindrome.

  // use iterator like pointer
  bool palindrome_string_1(const std::string &s)
  {
    if (s.empty())
      return true;

    auto begin = s.begin();
    auto end   = --s.end();

    while (begin < end)
    {
      if (!isalnum(*begin))
      {
        ++begin;
        continue;
      }

      if (!isalnum(*end))
      {
        --end;
        continue;
      }

      if (toupper(*begin) != toupper(*end))
        return false;

      ++begin;
      --end;
    }

    return true;
  }

  // use index
  bool palindrome_string_2(const std::string &s)
  {
    int begin{};
    int end{(int)s.size() - 1};

    while (begin < end)
    {
      if (!isalnum(s[begin]))
      {
        ++begin;
        continue;
      }

      if (!isalnum(s[end]))
      {
        --end;
        continue;
      }

      if (toupper(s[begin]) != toupper(s[end]))
        return false;

      ++begin;
      --end;
    }

    return true;
  }

  // 2020.07
  bool palindrome_string_3(const std::string &input)
  {
    // Note: For the purpose of this problem, we define empty string as valid
    // palindrome.
    if (input.empty())
      return true;

    size_t start{0};
    size_t end{input.size() - 1};

    // this causes the wrong result.
    // for (start, end; start < end; ++start, --end)

    for (; start < end;)
    {
      // not isspace() since should ignore others.
      if (!std::isalnum(input[start]))
      {
        ++start;
        continue;
      }

      if (!std::isalnum(input[end]))
      {
        --end;
        continue;
      }

      if (std::tolower(input[start]) != std::tolower(input[end]))
      {
        return false;
      }

      ++start;
      --end;
    }

    return true;
  }

  bool palindrome_string_4(const std::string &s)
  {
    if (s.empty())
      return true;

    auto begin = s.begin();
    auto end   = --s.end();

    while (begin < end)
    {
      while (begin < s.end() && !isalnum(*begin))
        ++begin;

      while (end > s.begin() && !isalnum(*end))
        --end;

      if (begin < end && toupper(*begin) != toupper(*end))
        return false;

      ++begin;
      --end;
    }

    return true;
  }

  // from discussion *py-code*
  // # trick 1: save re.sub() result to s itself
  // # trick 2: palindrome is the same when reversed
  //
  // class Solution_isPalindrome:
  //     def answer(self, s):
  //         s = re.sub(r'\W', '', s).upper()
  //         return s == s[::-1]
  bool palindrome_string_5(const std::string &s)
  {
    std::string coll{};

    // remove non alnum chars from the input
    for (auto e : s)
    {
      if (std::isalnum(e))
        coll.push_back(std::tolower(e));
    }

    std::string reversed{coll.rbegin(), coll.rend()};

    return coll == reversed ? true : false;
  }
} // namespace algo_palindrome

TEST(AlgoPalindrome, check_string_input)
{
  using namespace algo_palindrome;

  {
    const std::vector<std::function<bool(const std::string &)>> imps{
      palindrome_string_1,
      palindrome_string_2,
      palindrome_string_3,
      palindrome_string_4,
      palindrome_string_5};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(""), true);
      EXPECT_THAT(f("121"), true);
      EXPECT_THAT(f("A man, a plan, a canal: Panama"), true);
      EXPECT_THAT(f("race a car"), false);
      EXPECT_THAT(f("0P"), false);

      // 3 spaces
      EXPECT_THAT(f("   "), true);
      //               "12345678 12345678"
      EXPECT_THAT(f("        a        "), true);
      EXPECT_THAT(f("a                "), true);
      EXPECT_THAT(f("                a"), true);
      EXPECT_THAT(f(".,"), true);
      EXPECT_THAT(f("s"), true);
    }
  }
}

/*
={=========================================================================
algo-palindrome-longest 

The Modern C++ Challenge
28. Longest palindromic substring

Write a function that, given an input string, locates and returns the longest
sequence in the string that is a palindrome. If multiple palindromes of the same
length exist, the first one should be returned.

std::string longest_palindrome(const std::string text);

// from wikipedia

In computer science, the longest palindromic substring or longest *symmetric*
factor problem is the problem of finding a maximum-length contiguous substring
of a given string that is also a palindrome. 

For example, the longest palindromic substring of "bananas" is "anana". The
longest palindromic substring is not guaranteed to be unique; for example, in
the string "abracadabra", there is no palindromic substring with length greater
than three, but there are two palindromic substrings with length three, namely,
"aca" and "ada". In some applications it may be necessary to return all maximal
palindromic substrings (that is, all substrings that are themselves palindromes
and cannot be extended to larger palindromic substrings) rather than returning
only one substring or returning the maximum length of a palindromic substring.

{
   using namespace std::string_literals;
   assert(longest_palindrome("sahararahnide") == "hararah");
   assert(longest_palindrome("level") == "level");
   assert(longest_palindrome("s") == "s");
}

Unlike number version, the difference is "string" and "largest"

*/

// namespace U28_2018_12_03
namespace algopalindrome
{
  // palindrome is *symmetric* while moving i(center) from input text.
  // so center start from 0th and for every char of input:
  // o scan to right and left from that
  // o keep update current symetric substr and save it if that's longer
  // o return the input when there's no finding such as single input
  //
  // However, this fails on
  // EXPECT_THAT(f("ss"), "ss");

  std::string palindrome_longest_1(const std::string &text)
  {
    auto length = text.size();
    int lidx{}, ridx{};
    char current_char{};
    string current{}, saved{};

    // moving i
    for (decltype(length) i = 0; i < length; ++i)
    {
      // whenever moves i, start search again
      current.clear();
      current_char = text[i];

      current.push_back(current_char);

      for (lidx = i - 1, ridx = i + 1; lidx >= 0 && ridx < (int)length;
           --lidx, ++ridx)
      {
        if (text[lidx] == text[ridx])
        {
          current.insert(0, 1, text[lidx]);
          current.push_back(text[lidx]);
          if (saved.length() < current.length())
            saved = current;
        }
        else
        {
          // exit as soon as break symmetric condition
          break;
        }
      }
    }

    // to pass the case when single input char
    if (saved.length() < current.length())
      saved = current;

    return saved;
  }
} // namespace algopalindrome

// The simplest solution to this problem is to try a brute-force approach,
// checking if each substring is a palindrome. However, this means we need to
// check C(N, 2) substrings (where N is the number of characters in the string),
// and the time complexity would be O(N^3).
//
//
// The complexity could be reduced to O(N^2) by storing results of sub problems.
// To do so we need a table of Boolean values, of size, where
//
// the key idea:
// the element at [i, j] indicates whether the substring from position i to j is
// a palindrome.
//
// We start by initializing all elements [i,i] with true (one-character
// palindromes) and all the elements [i,i+i] with true for all consecutive two
// identical characters (for two-character palindromes). We then go on to
// inspect substrings greater than two characters, setting the element at [i,j]
// to true if the element at [i+i,j-1] is true and the characters on the
// positions i and j in the string are also equal. Along the way, we retain the
// start position and length of the longest palindromic substring in order to
// extract it after finishing computing the table.
//
//       0 1 2 3 4
//       l e v e l
// 0, l  o       *
// 1, e    o   *
// 2, v      o x
// 3, e        o
// 4, l          o
//
// i = 0, maxLen = 5
//
// why chekc "table[(i + 1)*len + j - 1]"? To see substr is symetric. ex, to see
// [0, 4] is symetic, check substr(1,3) is symetric, that is [1, 3]

namespace algopalindrome
{
  // from text code
  std::string palindrome_longest_2(std::string str)
  {
    size_t const len    = str.size();
    size_t longestBegin = 0;
    size_t maxLen       = 1;

    std::vector<bool> table(len * len, false);

    // one-character palindromes:
    // We start by initializing all elements [i,i] with true and set diagonal
    // elements of the table

    for (size_t i = 0; i < len; i++)
    {
      table[i * len + i] = true;
    }

    // two-character palindromes:
    // and all the elements [i,i+i] with true for all consecutive two identical
    // characters. We then go on to inspect
    //
    // why "len -1"? since needs two chars to inspect
    // 0 1 2 3 4, len is 5
    // l e v e 1

    for (size_t i = 0; i < len - 1; ++i)
    {
      if (str[i] == str[i + 1])
      {
        table[i * len + i + 1] = true;
        if (maxLen < 2)
        {
          longestBegin = i;
          maxLen       = 2;
        }
      }
    }

    // k = 3 which means see three elements range
    //
    // We then go on to inspect substrings greater than two characters, setting
    // the element at [i,j] to true if the element at [i+1,j-1] is true and the
    // characters on the positions i and j in the string are also equal.
    //
    // Along the way, we retain the start position and length of the longest
    // palindromic substring in order to extract it after finishing computing
    // the table.

    for (size_t k = 3; k <= len; k++)
    {
      for (size_t i = 0; i < len - k + 1; i++)
      {
        size_t j = i + k - 1;

        cout << "k: " << k << ", i: " << i << ", j: " << j << ", table["
             << ((i + 1) * len + j - 1) << "]:" << table[(i + 1) * len + j - 1]
             << endl;

        // table[(i + 1) * len + j - 1] is to mark the previous palindrome
        // substring.
        //
        // table[i * len + j] = true; is to set current substring

        if (str[i] == str[j] && table[(i + 1) * len + j - 1])
        {
          table[i * len + j] = true;

          cout << "k: " << k << ", i: " << i << ", j: " << j << ", table["
               << ((i + 1) * len + j - 1)
               << "]:" << table[(i + 1) * len + j - 1] << ", table["
               << i * len + j << "] = true" << endl;

          if (maxLen < k)
          {
            longestBegin = i;
            maxLen       = k;
          }
        } // if end
      }
    }

    return std::string(str.substr(longestBegin, maxLen));
  }
} // namespace algopalindrome

// TEST(U28, 2018_12_03)
TEST(AlgoPalindrome, check_longest)
{
  using namespace algopalindrome;

  {
    auto f = palindrome_longest_2;

    EXPECT_THAT(f("sahararahnide"), "hararah");

    //       0 1 2 3 4
    //       l e v e l
    // 0, l  o       *
    // 1, e    o   *
    // 2, v      o x
    // 3, e        o
    // 4, l          o
    //
    // k: 3, i: 0, j: 2, table[6]:1
    // k: 3, i: 1, j: 3, table[12]:1
    // k: 3, i: 1, j: 3, table[12]:1, table[8] = true
    // k: 3, i: 2, j: 4, table[18]:1
    // k: 4, i: 0, j: 3, table[7]:0
    // k: 4, i: 1, j: 4, table[13]:0
    // k: 5, i: 0, j: 4, table[8]:1
    // k: 5, i: 0, j: 4, table[8]:1, table[4] = true

    EXPECT_THAT(f("level"), "level");

    EXPECT_THAT(f("ss"), "ss");
    EXPECT_THAT(f("s"), "s");
  }
}

/*
={=========================================================================
algo-grade

F: 60 > score
D: 70 >
C: 80 >
B: 90 >
A: 100 >

plus '+' for 8, 9 and '-' for 0,1,2 for each grade

vector<string> f(const vector<int> scores);

*/

namespace algograde
{
  vector<string> grade_1(const vector<int> scores)
  {
    const vector<string> grade_table{"F", "D", "C", "B", "A"};
    vector<string> result;
    string grade{};

    for (auto e : scores)
    {
      // handled here if e < 60 so `band` below never be a minus value
      if (e < 60)
        result.push_back(grade_table[0]);
      else
      {
        grade.clear();

        auto band = (e - 50) / 10;

        grade = grade_table[band];

        if (band > 0)
        {
          int plus_or_minus = e % 10;
          if (plus_or_minus < 3)
            grade += "-";
          else if (plus_or_minus > 7)
            grade += "+";
        }

        result.push_back(grade);
      }
    }

    return result;
  }

  // 2020.07. For F, no need to have +/-
  std::vector<std::string> grade_2(const std::vector<int> scores)
  {
    const std::string table{"FFFFFFDCBA"};
    std::vector<std::string> result{};

    for (const auto e : scores)
    {
      std::string grade{};

      auto band = e / 10;

      grade.append(1, table[band]);

      if (band > 5)
      {
        auto remain = e % 10;

        if (remain > 7)
          grade.append("+");
        else if (remain < 3)
          grade.append("-");
      }

      result.push_back(grade);
    }

    return result;
  }

  // CodeComplete 18.4 Stair-Step Access Tables.
  // use loop rather than using index of table as aboves
  //    60    70    80    90    100
  //    |     |     |     |*****|
  //                 *****
  //  F    D     C     B     A

  std::string grade_3(int score)
  {
    const std::vector<int> band{60, 70, 80, 90, 100};
    const std::vector<std::string> grade{"F", "D", "C", "B", "A"};

    auto max_grade = grade.size() - 1;
    size_t grade_index{};
    std::string student_grade{"A"};

    // grade "A" means that not found yet and no need to check grade[max] since
    // if index gets that value, means "A" anyway.

    while ((student_grade == "A") && (grade_index < max_grade))
    {
      if (score < band[grade_index])
        student_grade = grade[grade_index];

      ++grade_index;
    }

    return student_grade;
  }
} // namespace algograde

TEST(AlgoGrade, check_imps)
{
  using namespace algograde;

  {
    // pair<input, expected result>
    const std::vector<std::pair<std::vector<int>, std::vector<std::string>>>
      inputs{{{42, 54, 60, 62, 66, 68, 71, 73, 78, 89, 98},
              {"F", "F", "D-", "D-", "D", "D+", "C-", "C", "C+", "B+", "A+"}}};

    const std::vector<
      std::function<std::vector<std::string>(const std::vector<int>)>>
      imps{grade_1, grade_2};

    for (const auto &f : imps)
    {
      for (const auto &i : inputs)
        EXPECT_THAT(f(i.first), i.second);
    }
  }

  // since grade_3 do not support +/-
  {
    const std::vector<int> inputs{42, 54, 60, 62, 66, 68, 71, 73, 78, 89, 98};
    const std::vector<std::string>
      expected{"F", "F", "D", "D", "D", "D", "C", "C", "C", "B", "A"};
    std::vector<std::string> results{};

    for (const auto &score : inputs)
    {
      results.push_back(grade_3(score));
    }

    EXPECT_THAT(results, expected);
  }
}

/*
={=========================================================================
algo-intersect find if rectangles intersect

*ex-interview* From NDS CF office interview. 

Implement the intersects method contained within the rectangle class below. The
method should return true if the supplied rectangle intersects with the
internally represented rectangle.

from ANSIC 130. Handles point rather than each value.

assumption about bot/and.

   +--------+ top(x2, y2)      +--------+ bot(x2, y2)
   |        |                  |        |
   |        |                  |        |
   +--------+                  +--------+
bot(x1,y1)                  top(x1,y1)


since depending on how Rect is defined, checking can be different in
is_point_in_rect().

this assumes bot < point < top and if use top/bot way, it fails

bool is_point_in_rect(const Point &point, const Rect &rect)
{
   return ((rect.bot_.x_ <= point.x_) && (point.x_ <= rect.top_.x_)) &&
       ((rect.bot_.y_ <= point.y_) && (point.y_ <= rect.top_.y_));
}

this affect also the order of arguments to Rect(). However, not much point of
supporting no-assumption case. No, when tried it again, spend hours why the
check code fails depending on how Rect() is defined.

NOTE: need to ask about how RECT is defined.
 
*/

namespace algointersect
{
  namespace first
  {
    struct Point
    {
      // not a default ctor so have to define it
      Point(const int x, const int y)
          : x_(x)
          , y_(y)
      {}
      int x_{};
      int y_{};
    };

    struct Rect
    {
      // not a default ctor so have to define it
      // Rect(bot, top);
      Rect(const Point &a, const Point &b)
          : bot_(a)
          , top_(b)
      {}
      Point bot_;
      Point top_;
    };

    // bot.x <= x <= top.x AND bot.y <= y <= top.y
    // should be AND
    //
    // support BOTH cases which Rect defines.

    bool is_point_in_rect(const Point &point, const Rect &rect)
    {
      return ((rect.top_.x_ >= point.x_ && rect.bot_.x_ <= point.x_) ||
              (rect.bot_.x_ >= point.x_ && rect.top_.x_ <= point.y_)) &&
             ((rect.top_.y_ >= point.y_ && rect.bot_.y_ <= point.y_) ||
              (rect.bot_.y_ >= point.y_ && rect.top_.y_ <= point.y_));
    }

    bool is_intersected(const Rect &r1, const Rect &r2)
    {
      return is_point_in_rect(r1.top_, r2) || is_point_in_rect(r1.bot_, r2);
    }
  } // namespace first

  namespace second
  {
    struct Point
    {
      explicit Point(int xvalue, int yvalue)
          : x{xvalue}
          , y{yvalue}
      {}
      int x;
      int y;
    };

    // to support both rect definition: bot/top and top/bot, can use point1 and
    // point2 than top/bot name.
    struct Rect
    {
      explicit Rect(const Point &t, const Point &b)
          : top{t}
          , bot{b}
      {}
      Point top;
      Point bot;
    };

    // return true if point falls into the rect
    bool is_point_in_rect(const Point &point, const Rect &rect)
    {
      // std::cout << "point {" << point.x << ", " << point.y << "}, rect{bot{"
      //           << rect.bot.x << ", " << rect.bot.y << "}, top{" << rect.top.x
      //           << ", " << rect.top.y << "}}" << std::endl;

      // to support both rect definition: bot/top and top/bot.
      if ((((rect.bot.x <= point.x) && (point.x <= rect.top.x)) &&
           ((rect.bot.y <= point.y) && (point.y <= rect.top.y))) ||
          (((rect.top.x <= point.x) && (point.x <= rect.bot.x)) &&
           ((rect.top.y <= point.y) && (point.y <= rect.bot.y))))
      {
        // std::cout << "return true\n";
        return true;
      }
      else
      {
        // std::cout << "return false\n";
        return false;
      }
    }

    bool is_intersect(const Rect &r1, const Rect &r2)
    {
      if (is_point_in_rect(r1.bot, r2) || is_point_in_rect(r1.top, r2))
        return true;
      else
        return false;
    }
  } // namespace second
} // namespace algointersect

TEST(AlgoIntersect, check_imps)
{
  {
    using namespace algointersect::first;

    auto f = is_intersected;

    {
      // bot/top
      Rect a(Point(10, 10), Point(20, 20));
      Rect b(Point(15, 15), Point(25, 25));
      EXPECT_THAT(f(a, b), Eq(true));
    }

    {
      // top/bot
      Rect a(Point(20, 20), Point(10, 10));
      Rect b(Point(25, 25), Point(15, 15));
      EXPECT_THAT(f(a, b), Eq(true));
    }

    {
      // bot/top, inclues the same point
      Rect a(Point(10, 10), Point(20, 20));
      Rect b(Point(20, 20), Point(25, 25));
      EXPECT_THAT(f(a, b), Eq(true));
    }

    {
      // bot/top, inclues the same point
      Rect a(Point(10, 10), Point(20, 20));
      Rect b(Point(25, 25), Point(35, 35));
      EXPECT_THAT(f(a, b), Eq(false));
    }
  }

  {
    using namespace algointersect::second;

    auto f = is_intersect;

    {
      // bot/top
      Rect a(Point(10, 10), Point(20, 20));
      Rect b(Point(15, 15), Point(25, 25));
      EXPECT_THAT(f(a, b), Eq(true));
    }

    {
      // top/bot
      Rect a(Point(20, 20), Point(10, 10));
      Rect b(Point(25, 25), Point(15, 15));
      EXPECT_THAT(f(a, b), Eq(true));
    }

    {
      // bot/top, inclues the same point
      Rect a(Point(10, 10), Point(20, 20));
      Rect b(Point(20, 20), Point(25, 25));
      EXPECT_THAT(f(a, b), Eq(true));
    }

    {
      // bot/top, inclues the same point
      Rect a(Point(10, 10), Point(20, 20));
      Rect b(Point(25, 25), Point(35, 35));
      EXPECT_THAT(f(a, b), Eq(false));
    }
  }
}

/*
={=========================================================================
algo-roman-to-roman

std::string to_roman(unsigned int arabic);

Roman numerals chart

1   5   10  50  100   500   1000
I   V   X   L   C     D     M

1   I     1
2   II    1+1
3   III   1+1+1
4   IV    5-1
5   V     5
6   VI    5+1
7   VII   5+1+1
8   VIII  5+1+1+1
9   IX    10-1
10  X     10

11  XI      10+1
12  XII     10+1+1
13  XIII    10+1+1+1
14  XIV     10-1+5
15  XV      10+5
16  XVI     10+5+1
17  XVII    10+5+1+1
18  XVIII   10+5+1+1+1
19  XIX     10-1+10
20  XX      10+10

21  XXI     10+10+1
22  XXII    10+10+1+1
23  XXIII   10+10+1+1+1
24  XXIV    10+10-1+5
25  XXV     10+10+5
26  XXVI    10+10+5+1
27  XXVII   10+10+5+1+1
28  XXVIII  10+10+5+1+1+1
29  XXIX    10+10-1+10
30  XXX     10+10+10

31  XXXI    10+10+10+1
32  XXXII   10+10+10+1+1
33  XXXIII  10+10+10+1+1+1
34  XXXIV   10+10+10-1+5
35  XXXV    10+10+10+5
36  XXXVI   10+10+10+5+1
37  XXXVII  10+10+10+5+1+1
38  XXXVIII 10+10+10+5+1+1+1
39  XXXIX   10+10+10-1+10
40  XL      50-10

41  XLI     50-10+1
42  XLII    50-10+1+1
43  XLIII   50-10+1+1+1
44  XLIV    50-10-1+5
45  XLV     50-10+5
46  XLVI    50-10+5+1
47  XLVII   50-10+5+1+1
48  XLVIII  50-10+5+1+1+1
49  XLIX    50-10-1+10
50  L       50

51  LI      50+1
52  LII     50+1+1
53  LIII    50+1+1+1
54  LIV     50-1+5
55  LV      50+5
56  LVI     50+5+1
57  LVII    50+5+1+1
58  LVIII   50+5+1+1+1
59  LIX     50-1+10
60  LX      50+10
...

Convert Arabic number to Roman number

1. the roman number has fixed mappings:

1       : I
5       : V
10      : X
50      : L
100     : C
500     : D
1000    : M

2. As itoa(), use loop, %, and / to get a digit to convert:

    tens    2   1   0
            X   X   X
    digit   D   D   D

3. Divide 0-9 into sub-groups to get:

* 0 < digit < 4:

    tens = 0:
        1       (1, 10**0)      I
        2       (2, 10**0)      II
        3       (3, 10**0)      III

    tens = 1:
        10      (1, 10**1)      X
        20      (2, 10**1)      XX
        30      (3, 10**1)      XXX

    tens = 2:
        100     (1, 10**2)      C
        200     (2, 10**2)      CC
        300     (3, 10**2)      CCC
    ...

    To use the same function as 5 < digit < 9 case:

    tens = 0:
                        (base, repeat, roman to repeat)
        1, 0+1          (0*10**0, 1, 10**0)     I
        2, 0+1          (0*10**0, 2, 10**0)     II
        3, 0+1          (0*10**0, 3, 10**0)     III

* 5 < digit < 9:

    tens = 0:
                        (base, repeat, roman to repeat)
        6, 5+1          (5*10**0, 1, 10**0)     VI
        7, 5+2          (5*10**0, 2, 10**0)     VII
        8, 5+3          (5*10**0, 3, 10**0)     VIII

    tens = 1:
        60, 50+10       (5*10**1, 1, 10**1)     LX
        70, 50+20       (5*10**1, 2, 10**1)     LXX
        89, 50+30       (5*10**1, 3, 10**1)     LXXX

    tens = 2:
        600, 500+100    (5*10**1, 1, 10**1)     DC
        700, 500+200    (5*10**1, 2, 10**1)     DCC
        890, 500+300    (5*10**1, 3, 10**1)     DCCC
    ...

* 4 or 9

    tens = 0:
        4, 5-1          (10**0 + (4+1)*10**0)   IV
        9, 10-1         (10**0 + (9+1)*10**0)   IX

    tens = 1:
        40, 50-10       (10**1 + (4+1)*10**1)   XL
        90, 100-10      (10**1 + (9+1)*10**1)   XC
    ...

* 5

    tens = 0:
        5,              (10**0*5)

    tens = 1:
        50              (10**1*5)

    tens = 2:
        500             (10**2*5)

*/

namespace algoroman
{
  class RomanConvert
  {
  public:
    // 1. tried to use input/output string arg like
    //
    //      void convert(uint32_t, std::string &)
    //
    // and use public string member in the fixture class.
    // However, since there are many asserts in a test, have to reset
    // this string member before using the next assert. dropped.
    //
    // 2. uses one single string and add at the front whenever converted a
    // letter. By this, no need to reverse the result as itoa() does but
    // there will be a downside; relocation and performance. Howerver, since
    // it is not a long string, do not think they matter here.

    std::string convert(uint32_t number) const
    {
      std::string converted;
      uint32_t tens{0}, digit{0};

      // for fixed roman letters
      // the code works without this to cover fixed romans since the loop
      // below will find it eventually. However, it uses a map already and
      // may save some time.

      auto it = roman_table.find(number);
      if (it != roman_table.cend())
        return it->second;

      for (; number;)
      {
        digit = number % 10;

        if (digit < 4)
        {
          padWithFoundRoman(digit, tens, 0, converted);
        }
        else if (digit > 5 && digit < 9)
        {
          padWithFoundRoman(digit, tens, 5, converted);
        }
        else if (!(digit % 5))
        {
          findFiveBaseRoman(digit, tens, converted);
        }
        else if (digit == 4 || digit == 9)
        {
          findSubstractRoman(digit, tens, converted);
        }

        ++tens;

        number /= 10;
      }

      return converted;
    }

  private:
    const std::map<uint32_t, std::string> roman_table{{0, ""},
                                                      {1, "I"},
                                                      {5, "V"},
                                                      {10, "X"},
                                                      {50, "L"},
                                                      {100, "C"},
                                                      {500, "D"},
                                                      {1000, "M"}};

    void padWithFoundRoman(uint32_t number,
                           uint32_t tens,
                           uint32_t fixed,
                           std::string &padded) const
    {
      std::string result;

      // find the letter to pad
      auto it = roman_table.find(pow(10, tens));
      if (it == roman_table.cend())
        std::cout << "roman_table(" << pow(10, tens) << ") not found"
                  << std::endl;

      std::string letter = it->second;

      // find the base
      it = roman_table.find(fixed * pow(10, tens));
      if (it == roman_table.cend())
        std::cout << "roman_table(" << fixed * pow(10, tens) << ") not found"
                  << std::endl;

      std::string base = it->second;

      // add the base once
      result += base;

      // add the pad
      for (uint32_t count = number - fixed; count; --count)
        result += letter;

      padded.insert(0, result);
    }

    void findSubstractRoman(uint32_t number,
                            uint32_t tens,
                            std::string &converted) const
    {
      std::string padded;

      // find the letter in substract form
      auto it = roman_table.find(pow(10, tens));
      if (it == roman_table.cend())
        std::cout << "roman_table(" << pow(10, tens) << ") not found"
                  << std::endl;

      std::string letter = it->second;

      // find the base
      it = roman_table.find((number + 1) * pow(10, tens));
      if (it == roman_table.cend())
        std::cout << "roman_table(" << (number + 1) * pow(10, tens)
                  << ") not found" << std::endl;

      std::string base = it->second;

      converted.insert(0, letter + base);
    }

    void findFiveBaseRoman(uint32_t number,
                           uint32_t tens,
                           std::string &converted) const
    {
      // find the letter in substract form
      auto it = roman_table.find(number * pow(10, tens));
      if (it == roman_table.cend())
        std::cout << "roman_table(" << pow(10, tens) << ") not found"
                  << std::endl;

      converted.insert(0, it->second);
    }
  };

  // 1   5   10  50  100   500   1000
  // I   V   X   L   C     D     M
  // from TDD book
  std::string to_roman_1(unsigned int value)
  {
    std::string result{};

    // // OK
    // const std::vector<std::pair<int, std::string>> table = {
    //   std::make_pair(1000, "M"),
    //   std::make_pair(900, "CM"),
    //   std::make_pair(500, "D"),
    //   std::make_pair(400, "CD"),
    //   std::make_pair(100, "C"),
    //   std::make_pair(90, "XC"),
    //   std::make_pair(50, "L"),
    //   std::make_pair(40, "XL"),
    //   std::make_pair(10, "X"),
    //   std::make_pair(9, "IX"),
    //   std::make_pair(5, "V"),
    //   std::make_pair(4, "IV"),
    //   std::make_pair(1, "I")};

    // // OK
    // const auto table = {
    //   std::make_pair(1000, "M"),
    //   std::make_pair(900, "CM"),
    //   std::make_pair(500, "D"),
    //   std::make_pair(400, "CD"),
    //   std::make_pair(100, "C"),
    //   std::make_pair(90, "XC"),
    //   std::make_pair(50, "L"),
    //   std::make_pair(40, "XL"),
    //   std::make_pair(10, "X"),
    //   std::make_pair(9, "IX"),
    //   std::make_pair(5, "V"),
    //   std::make_pair(4, "IV"),
    //   std::make_pair(1, "I")};

    // error: direct-list-initialization of ‘auto’ requires exactly one element [-fpermissive]
    // const auto roman_table{
    //   make_pair(1000u, "M"),
    //     make_pair(900u, "CM"),
    //     make_pair(500u, "D"),
    //     make_pair(400u, "CD"),
    //     make_pair(100u, "C"),
    //     make_pair(90u, "XC"),
    //     make_pair(50u, "L"),
    //     make_pair(40u, "XL"),
    //     make_pair(10u, "X"),
    //     make_pair(9u, "IX"),
    //     make_pair(5u, "V"),
    //     make_pair(4u, "IV"),
    //     make_pair(1u, "I")
    // };

    // // error: unable to deduce ‘const std::initializer_list<const auto>’ from ‘{{1000, "M"},
    // const auto table = {{1000, "M"},
    //                     {900, "CM"},
    //                     {500, "D"},
    //                     {400, "CD"},
    //                     {100, "C"},
    //                     {90, "XC"},
    //                     {50, "L"},
    //                     {40, "XL"},
    //                     {10, "X"},
    //                     {9, "IX"},
    //                     {5, "V"},
    //                     {4, "IV"},
    //                     {1, "I"}};

    // OK
    const std::initializer_list<std::pair<unsigned int, std::string>> table = {
      {1000, "M"},
      {900, "CM"},
      {500, "D"},
      {400, "CD"},
      {100, "C"},
      {90, "XC"},
      {50, "L"},
      {40, "XL"},
      {10, "X"},
      {9, "IX"},
      {5, "V"},
      {4, "IV"},
      {1, "I"}};

    for (const auto &e : table)
    {
      while (value >= e.first)
      {
        value -= e.first;
        result.append(e.second);
      }
    }

    // if comment out "return result", crashes.
    // From standard 6.6.3/2
    // A return statement without an expression can be used only in functions
    // that do not return a value, that is, a function with the return type
    // void, a constructor (12.1), or a destructor (12.4). A return statement
    // with an expression of non-void type can be used only in functions
    // returning a value; the value of the expression is returned to the caller
    // of the function. The expression is implicitly converted to the return
    // type of the function in which it appears. A return statement can involve
    // the construction and copy of a temporary object (12.2). Flowing off the
    // end of a function is equivalent to a return with no value; this results
    // in undefined behavior in a value-returning function.

    return result;
  }

} // namespace algoroman

TEST(AlgoRoman, check_to_roman)
{
  using namespace algoroman;

  {
    RomanConvert converter;

    EXPECT_THAT(converter.convert(1), Eq("I"));
    EXPECT_THAT(converter.convert(2), Eq("II"));
    EXPECT_THAT(converter.convert(3), Eq("III"));
    EXPECT_THAT(converter.convert(4), Eq("IV"));
    EXPECT_THAT(converter.convert(5), Eq("V"));
    EXPECT_THAT(converter.convert(6), Eq("VI"));
    EXPECT_THAT(converter.convert(7), Eq("VII"));
    EXPECT_THAT(converter.convert(8), Eq("VIII"));
    EXPECT_THAT(converter.convert(9), Eq("IX"));
    EXPECT_THAT(converter.convert(10), Eq("X"));
    EXPECT_THAT(converter.convert(11), Eq("XI"));
    EXPECT_THAT(converter.convert(12), Eq("XII"));
    EXPECT_THAT(converter.convert(13), Eq("XIII"));
    EXPECT_THAT(converter.convert(16), Eq("XVI"));
    EXPECT_THAT(converter.convert(17), Eq("XVII"));
    EXPECT_THAT(converter.convert(18), Eq("XVIII"));
    EXPECT_THAT(converter.convert(20), Eq("XX"));
    EXPECT_THAT(converter.convert(23), Eq("XXIII"));
    EXPECT_THAT(converter.convert(41), Eq("XLI"));
    EXPECT_THAT(converter.convert(45), Eq("XLV"));
    EXPECT_THAT(converter.convert(50), Eq("L"));
    EXPECT_THAT(converter.convert(80), Eq("LXXX"));
    EXPECT_THAT(converter.convert(91), Eq("XCI"));
    EXPECT_THAT(converter.convert(95), Eq("XCV"));
    EXPECT_THAT(converter.convert(100), Eq("C"));
    EXPECT_THAT(converter.convert(122), Eq("CXXII"));
    EXPECT_THAT(converter.convert(152), Eq("CLII"));
    EXPECT_THAT(converter.convert(196), Eq("CXCVI"));
    EXPECT_THAT(converter.convert(247), Eq("CCXLVII"));
    EXPECT_THAT(converter.convert(288), Eq("CCLXXXVIII"));
    EXPECT_THAT(converter.convert(298), Eq("CCXCVIII"));
    EXPECT_THAT(converter.convert(500), Eq("D"));
    EXPECT_THAT(converter.convert(1000), Eq("M"));
    EXPECT_THAT(converter.convert(1513), Eq("MDXIII"));
    EXPECT_THAT(converter.convert(2999), Eq("MMCMXCIX"));
    EXPECT_THAT(converter.convert(3447), Eq("MMMCDXLVII"));
  }

  {
    const std::vector<std::function<std::string(unsigned int)>> imps{
      to_roman_1};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(1), Eq("I"));
      EXPECT_THAT(f(2), Eq("II"));
      EXPECT_THAT(f(3), Eq("III"));
      EXPECT_THAT(f(4), Eq("IV"));
      EXPECT_THAT(f(5), Eq("V"));
      EXPECT_THAT(f(6), Eq("VI"));
      EXPECT_THAT(f(7), Eq("VII"));
      EXPECT_THAT(f(8), Eq("VIII"));
      EXPECT_THAT(f(9), Eq("IX"));
      EXPECT_THAT(f(10), Eq("X"));
      EXPECT_THAT(f(11), Eq("XI"));
      EXPECT_THAT(f(12), Eq("XII"));
      EXPECT_THAT(f(13), Eq("XIII"));
      EXPECT_THAT(f(16), Eq("XVI"));
      EXPECT_THAT(f(17), Eq("XVII"));
      EXPECT_THAT(f(18), Eq("XVIII"));
      EXPECT_THAT(f(20), Eq("XX"));
      EXPECT_THAT(f(23), Eq("XXIII"));
      EXPECT_THAT(f(41), Eq("XLI"));
      EXPECT_THAT(f(45), Eq("XLV"));
      EXPECT_THAT(f(50), Eq("L"));
      EXPECT_THAT(f(80), Eq("LXXX"));
      EXPECT_THAT(f(91), Eq("XCI"));
      EXPECT_THAT(f(95), Eq("XCV"));
      EXPECT_THAT(f(100), Eq("C"));
      EXPECT_THAT(f(122), Eq("CXXII"));
      EXPECT_THAT(f(152), Eq("CLII"));
      EXPECT_THAT(f(196), Eq("CXCVI"));
      EXPECT_THAT(f(247), Eq("CCXLVII"));
      EXPECT_THAT(f(288), Eq("CCLXXXVIII"));
      EXPECT_THAT(f(298), Eq("CCXCVIII"));
      EXPECT_THAT(f(500), Eq("D"));
      EXPECT_THAT(f(1000), Eq("M"));
      EXPECT_THAT(f(1513), Eq("MDXIII"));
      EXPECT_THAT(f(2999), Eq("MMCMXCIX"));
      EXPECT_THAT(f(3447), Eq("MMMCDXLVII"));
    }
  }
}

/*
={=========================================================================
algo-roman-to-integer

algo-leetcode-4
13. Roman to Integer, Easy

Roman numerals are represented by seven different symbols: I, V, X, L, C, D and M.

Symbol       Value
I             1
V             5
X             10
L             50
C             100
D             500
M             1000

For example, two is written as II in Roman numeral, just two one's added
together. Twelve is written as, XII, which is simply X + II. The number twenty
seven is written as XXVII, which is XX + V + II.

Roman numerals are usually written largest to smallest from left to right.
However, the numeral for four is not IIII. Instead, the number four is written
as IV. Because the one is before the five we subtract it making four. The same
principle applies to the number nine, which is written as IX. There are six
instances where subtraction is used:

I can be placed before V (5) and X (10) to make 4 and 9. 
X can be placed before L (50) and C (100) to make 40 and 90. 
C can be placed before D (500) and M (1000) to make 400 and 900.

Given a roman numeral, convert it to an integer. Input is guaranteed to be
within the range from 1 to 3999.

see algo-roman for integer to roman conversion which is rated as:

12. Integer to Roman, Medium

*/

namespace algoroman
{
  // Runtime: 52 ms, faster than 97.00% of C++ online submissions for Roman to
  // Integer.
  //
  // Memory Usage: 30.7 MB, less than 96.08% of C++ online submissions for Roman
  // to Integer.

  int to_int_1(const std::string &input)
  {
    // do not use this from since "M" is deduced to "const char*" but want to
    // use it as string so that can use size() on it. Or can add size member in
    // the table.
    //
    // const auto table = {
    //   make_pair("M", 1000),
    //   make_pair("CM", 900)
    // };

    // for each char of the table, see if there is matched substr using running
    // start position and string size. contine doing so until there is no match
    // and if there is no match, move on to the next char.

    const initializer_list<pair<std::string, int>> table = {{"M", 1000},
                                                            {"CM", 900},
                                                            {"D", 500},
                                                            {"CD", 400},
                                                            {"C", 100},
                                                            {"XC", 90},
                                                            {"L", 50},
                                                            {"XL", 40},
                                                            {"X", 10},
                                                            {"IX", 9},
                                                            {"V", 5},
                                                            {"IV", 4},
                                                            {"I", 1}};

    size_t run{};
    int result{};

    for (const auto &e : table)
    {
      while ((run < input.size()) &&
             (input.substr(run, e.first.size()) == e.first))
      {
        // cout << "first: " << e.first << endl;
        result += e.second;
        run += e.first.size();
      }
    }
    // cout << "result: " << result << endl;

    return result;
  }

  int to_int_2(const std::string &input)
  {
    const auto table = {make_pair(std::string("M"), 1000),
                        make_pair(std::string("CM"), 900),
                        make_pair(std::string("D"), 500),
                        make_pair(std::string("CD"), 400),
                        make_pair(std::string("C"), 100),
                        make_pair(std::string("XC"), 90),
                        make_pair(std::string("L"), 50),
                        make_pair(std::string("XL"), 40),
                        make_pair(std::string("X"), 10),
                        make_pair(std::string("IX"), 9),
                        make_pair(std::string("V"), 5),
                        make_pair(std::string("IV"), 4),
                        make_pair(std::string("I"), 1)};

    int result{};
    size_t start{};

    for (auto const &e : table)
    {
      while (input.find(e.first, start) == start)
      {
        start += e.first.size();
        result += e.second;
      }
    }

    return result;
  }

  int to_int_3(const std::string &roman)
  {
    const std::vector<std::pair<std::string, int>> table = {{"M", 1000},
                                                            {"CM", 900},
                                                            {"D", 500},
                                                            {"CD", 400},
                                                            {"C", 100},
                                                            {"XC", 90},
                                                            {"L", 50},
                                                            {"XL", 40},
                                                            {"X", 10},
                                                            {"IX", 9},
                                                            {"V", 5},
                                                            {"IV", 4},
                                                            {"I", 1}};
    size_t run{};
    int value{};

    for (const auto &e : table)
    {
      while (0 == roman.compare(run, e.first.size(), e.first.c_str()))
      {
        run += e.first.size();
        value += e.second;
      }
    }

    return value;
  }
} // namespace algoroman

TEST(AlgoRoman, check_to_int)
{
  using namespace algoroman;

  {
    const std::vector<std::function<int(const std::string &)>> imps{to_int_1,
                                                                    to_int_2,
                                                                    to_int_3};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f("I"), 1);
      EXPECT_THAT(f("II"), 2);
      EXPECT_THAT(f("III"), 3);
      EXPECT_THAT(f("IV"), 4);
      EXPECT_THAT(f("V"), 5);
      EXPECT_THAT(f("VI"), 6);
      EXPECT_THAT(f("VII"), 7);
      EXPECT_THAT(f("VIII"), 8);
      EXPECT_THAT(f("IX"), 9);
      EXPECT_THAT(f("X"), 10);
      EXPECT_THAT(f("XI"), 11);
      EXPECT_THAT(f("XII"), 12);
      EXPECT_THAT(f("XIII"), 13);
      EXPECT_THAT(f("XVI"), 16);
      EXPECT_THAT(f("XVII"), 17);
      EXPECT_THAT(f("XVIII"), 18);
      EXPECT_THAT(f("XX"), 20);
      EXPECT_THAT(f("XXIII"), 23);
      EXPECT_THAT(f("XLI"), 41);
      EXPECT_THAT(f("XLV"), 45);
      EXPECT_THAT(f("L"), 50);
      EXPECT_THAT(f("LXXX"), 80);
      EXPECT_THAT(f("XCI"), 91);
      EXPECT_THAT(f("XCV"), 95);
      EXPECT_THAT(f("C"), 100);
      EXPECT_THAT(f("CXXII"), 122);
      EXPECT_THAT(f("CLII"), 152);
      EXPECT_THAT(f("CXCVI"), 196);
      EXPECT_THAT(f("CCXLVII"), 247);
      EXPECT_THAT(f("CCLXXXVIII"), 288);
      EXPECT_THAT(f("CCXCVIII"), 298);
      EXPECT_THAT(f("D"), 500);
      EXPECT_THAT(f("M"), 1000);
      EXPECT_THAT(f("MDXIII"), 1513);
      EXPECT_THAT(f("MMCMXCIX"), 2999);
      EXPECT_THAT(f("MMMCDXLVII"), 3447);
    }
  }
}

/*
={=========================================================================
algo-recursion-hanoi

The recursion is `divide-and-conquer` as it reduce the large problem to one or
more problems of a similar nature but a smaller size. 

Must determine the `stopping-rule`, the smallest case because there must be some
way that the recursion stops.


the-tower-of-hanoi

The idea is to concentrate not on the first step, but rather on the hardest
step: moving the bottom disk because condition is that only one disk can be
moved at a time and the bottom, largest, one can never be on top of any other.

Assume that there are 64 disks:

note that disk is disk num, disk num 0, disk num 1, ...

Move(disk, start, finish, temp);

Move(64, 1, 3, 2)
{
  // moves 63th from 1 to 2
  Move(63, 1, 2, 3);

  // moves 64th from 1 to 3 since there is no 63th on top of it
  printf("move disk 64 from tower 1 to 3.\n");

  // moves 63th from 2 to 3
  Move(63, 2, 3, 1);
}

The stopping rule is when there is no disk to be moved, there is nothing to do.


This is a tool to visualize recursive call in which node represents recursion
call. The time requirement is the total number of nodes, vertices, in a
recursion tree since traverse all nodes and the space(stack space) is the
depth of tree, not the number of nodes.

                                                          Move(3, 1,3,2) ()

                             Move(2, 1,2,3) ()                             ...

         Move(1, 1,3,2) ()                      Move(1, 3,2,1) ()          

Move(0, 1,2,3) ()  Move(0, 2,3,1) ()   Move(0, 3,1,2) ()  Move(0, 1,2,3) ()


disks are 0, 1, 2, 3

----(1) Move(3,1,3,2)
------(2) Move(2,1,2,3)                {
--------(3) Move(1,1,3,2)
----------(4) Move(0,1,2,3)
                move 0 disk, 1 -> 2
              move 1 disk, 1 -> 3
----------(4) Move(0,2,3,1)
                move 0 disk, 2 -> 3

            move 2 disk, 1 -> 2

--------(3) Move(1,3,2,1)
----------(4) Move(0,3,1,2)
                move 0 disk, 3 -> 1
              move 1 disk, 3 -> 2
----------(4) Move(0,1,2,3)
                move 0 disk, 1 -> 2    }

          move 3 disk, 1 -> 3

------(2) Move(2,2,3,1)
--------(3) Move(1,2,1,3)
----------(4) Move(0,2,3,1)
                move 0 disk, 2 -> 3
              move 1 disk, 2 -> 1
----------(4) Move(0,3,1,2)
                move 0 disk, 3 -> 1
            move 2 disk, 2 -> 3
--------(3) Move(1,1,3,2)
----------(4) Move(0,1,2,3)
                move 0 disk, 1 -> 2
              move 1 disk, 1 -> 3
----------(4) Move(0,2,3,1)
                move 0 disk, 2 -> 3
[       OK ] (3 ms)


The number of non-leafs, that is the number of moves for 64 is 2^64-1. This
is about 5x10^11 years where 2x10^10 is 20 billion years

>>> 2**64
18446744073709551616L

>>> 2**24
16777216

when DISK is 24:
...
stop recursion count #25
move #2 disk: 1 -> 3
move #0 disk: 2 -> 1
stop recursion count #25
move #1 disk: 2 -> 3
move #0 disk: 1 -> 3
stop recursion count #25
[       OK ] (77854 ms)

*/

namespace algo_recursion_hanoi
{
  bool print_recursion{false};

  void print_recursion_depth(bool dash, unsigned int depth)
  {
    for (unsigned int i = 0; i <= depth; ++i)
    {
      if (dash)
        cout << "--";
      else
        cout << "  ";
    }

    if (dash)
      cout << "(" << depth << ") ";
    else
      cout << "      ";
  }

  void move_disk(int disknum, int start, int finish, int temp)
  {
    static int recursion_depth{};

    ++recursion_depth;

    if (print_recursion)
    {
      print_recursion_depth(true, recursion_depth);
      std::cout << "Move(" << disknum << "," << start << "," << finish << ","
                << temp << ")" << std::endl;
    }

    if (disknum > 0)
    {
      // move #disknum-1 to temp
      move_disk(disknum - 1, start, temp, finish);

      // move #disknum
      if (print_recursion)
      {
        print_recursion_depth(false, recursion_depth);
      }

      std::cout << "move #" << disknum << " disk: " << start << " -> " << finish
                << std::endl;

      // move #disknum-1 to finish
      move_disk(disknum - 1, temp, finish, start);
    }
    // stopping condition. no more recursion.
    else
    {
      // move #disknum
      if (print_recursion)
      {
        print_recursion_depth(false, recursion_depth);
      }

      std::cout << "move #" << disknum << " disk: " << start << " -> " << finish
                << std::endl;
      std::cout << "stop recursion count #" << recursion_depth << std::endl;
    }

    --recursion_depth;
  }
} // namespace algo_recursion_hanoi

TEST(AlgoRecursion, check_hanoi)
{
  using namespace algo_recursion_hanoi;

  const int DISKS{3};

  move_disk(DISKS, 1, 3, 2);
}

/*
={=========================================================================
algo-recursion-count-and-say algo-leetcode-12

38. Count and Say, Easy

The count-and-say sequence is the sequence of integers with the first five terms
as following:

1.     1
2.     11
3.     21
4.     1211
5.     111221

1 is read off as "one 1" or 11.

11 is read off as "two 1s" or 21.

21 is read off as "one 2, then one 1" or 1211.

Given an integer n where 1 ≤ n ≤ 30, generate the nth term of the count-and-say
sequence.

Note: Each term of the sequence of integers will be represented as a string.

Example 1:
Input: 1
Output: "1"

Example 2:
Input: 4
Output: "1211"

string count_and_say(int n);

note:

the problem is to make string output which is done by counting numbers and what
the number is for the previous. That is "count and what"

starts new counting when see different char.

*/

namespace leetcode_easy_012
{
  string count_string_1(string const &input)
  {
    char current_char{};
    size_t count{};
    string result{};

    for (auto ch : input)
    {
      if (ch == current_char)
        ++count;
      else
      {
        // to handle the first read
        if (count != 0)
          result += to_string(count) + current_char;

        current_char = ch;
        count        = 1;
      }
    }

    // to handle when input ends
    result += to_string(count) + current_char;

    // cout << "result: " << result << endl;

    return result;
  }

  // to remove handling of the first char read

  string count_string_2(string const &input)
  {
    char current_char = input[0];
    size_t count{1};
    string result{};

    for (size_t i = 1; i < input.size(); ++i)
    {
      if (input[i] == current_char)
        ++count;
      else
      {
        result += to_string(count) + current_char;
        current_char = input[i];
        count        = 1;
      }
    }

    // to handle then input ends
    result += to_string(count) + current_char;

    return result;
  }

  // Given an integer n where 1 ≤ n ≤ 30,

  string count_and_say_1(int n)
  {
    if (n == 1)
      return "1";

    auto result = count_and_say_1(n - 1);
    return count_string_1(result);
  }

  string count_and_say_2(int n)
  {
    if (n == 1)
      return "1";

    auto result = count_and_say_2(n - 1);
    return count_string_2(result);
  }

  // to make a single function
  //
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Count and
  // Say.
  //
  // Memory Usage: 9.1 MB, less than 35.44% of C++ online submissions for Count
  // and Say.

  string count_and_say_3(int n)
  {
    if (n == 1)
      return "1";

    auto input = count_and_say_3(n - 1);

    // return count_string_2(result);
    // string count_string_2(string const &input)
    {
      char current_char = input[0];
      size_t count{1};
      string result{};

      for (size_t i = 1; i < input.size(); ++i)
      {
        if (input[i] == current_char)
          ++count;
        else
        {
          result += to_string(count) + current_char;
          current_char = input[i];
          count        = 1;
        }
      }

      // to handle then input ends
      result += to_string(count) + current_char;

      return result;
    }
  }
} // namespace leetcode_easy_012

TEST(AlgoRecursion, check_count_and_say_1)
{
  using namespace leetcode_easy_012;

  {
    // 2th
    EXPECT_THAT(count_string_1("1"), "11");
    // 3th
    EXPECT_THAT(count_string_1("11"), "21");
    // 4th
    EXPECT_THAT(count_string_1("21"), "1211");
    // 5th
    EXPECT_THAT(count_string_1("1211"), "111221");
    // 6th
    EXPECT_THAT(count_string_1("111221"), "312211");

    EXPECT_THAT(count_and_say_1(2), "11");
    EXPECT_THAT(count_and_say_1(3), "21");
    EXPECT_THAT(count_and_say_1(4), "1211");
    EXPECT_THAT(count_and_say_1(5), "111221");
    EXPECT_THAT(count_and_say_1(6), "312211");

    EXPECT_THAT(count_and_say_3(2), "11");
    EXPECT_THAT(count_and_say_3(3), "21");
    EXPECT_THAT(count_and_say_3(4), "1211");
    EXPECT_THAT(count_and_say_3(5), "111221");
    EXPECT_THAT(count_and_say_3(6), "312211");
  }

  // up to "4462" long
  {
    for (int i = 1; i <= 30; ++i)
    {
      count_and_say_1(i).size();
      // cout << count_and_say(i).size() << endl;
    }

    for (int i = 1; i <= 30; ++i)
    {
      count_and_say_2(i).size();
    }
  }
}

namespace algo_recursion_count_and_say
{
  // wrong interpretation which supports
  // EXPECT_THAT(count_and_say(6), "2111121211");
  std::string build_cound_and_say_1(const std::string &result)
  {
    // special case since the rest will be covered the table below.
    if (result == "1")
      return "11";

    std::map<std::string, std::string> table{{"11", "21"},
                                             {"21", "1211"},
                                             {"12", "1112"}};

    std::string build{};

    for (size_t pos = 0; pos < result.size();)
    {
      // note that the second is `length`. (start, length). gets two chars
      // always.
      std::string token = result.substr(pos, 2);

      try
      {
        build += table.at(token);
        pos += token.size();
      } catch (const std::exception &e)
      {
        std::cout << "except " << e.what() << std::endl;
      }
    }

    return build;
  }

  std::string count_and_say_1(int value)
  {
    // stopping rule
    if (value == 1)
      return "1";
    else
    {
      std::string result = count_and_say_1(value - 1);

      // build string count and say of the given result
      return build_cound_and_say_1(result);
    }
  }

  // supports
  // EXPECT_THAT(count_and_say_3(6), "312211");
  std::string build_cound_and_say_2(const std::string &result)
  {
    // special case since the rest will be covered the table below.
    if (result == "1")
      return "11";

    std::string build{};
    size_t current_count{};
    char current_char{};

    for (auto e : result)
    {
      // when see a different char
      if (e != current_char)
      {
        if (current_char)
        {
          // build token out of current char and count
          build += std::to_string(current_count);
          build += current_char;
        }

        current_char  = e;
        current_count = 1;
      }
      // when see the same char
      else
        ++current_count;
    }

    // build token out of current char and count
    build += std::to_string(current_count);
    build += current_char;

    return build;
  }

  std::string count_and_say_2(int value)
  {
    // stopping rule
    if (value == 1)
      return "1";
    else
    {
      std::string result = count_and_say_2(value - 1);

      // build string count and say of the given result
      return build_cound_and_say_2(result);
    }
  }
} // namespace algo_recursion_count_and_say

TEST(AlgoRecursion, check_count_and_say_2)
{
  using namespace algo_recursion_count_and_say;

  {
    auto f = build_cound_and_say_1;

    EXPECT_THAT(f("11"), "21");
    EXPECT_THAT(f("21"), "1211");
    EXPECT_THAT(f("12"), "1112");
    EXPECT_THAT(f("1211"), "111221");
    EXPECT_THAT(f("111221"), "2111121211");
  }

  {
    auto f = count_and_say_1;

    EXPECT_THAT(f(1), "1");
    EXPECT_THAT(f(2), "11");
    EXPECT_THAT(f(3), "21");
    EXPECT_THAT(f(4), "1211");
    EXPECT_THAT(f(5), "111221");
    EXPECT_THAT(f(6), "2111121211");
  }

  {
    auto f = build_cound_and_say_2;

    EXPECT_THAT(f("11"), "21");
    EXPECT_THAT(f("21"), "1211");
    EXPECT_THAT(f("12"), "1112");
    EXPECT_THAT(f("1211"), "111221");
    EXPECT_THAT(f("111221"), "312211");
  }

  {
    auto f = count_and_say_2;

    EXPECT_THAT(f(1), "1");
    EXPECT_THAT(f(2), "11");
    EXPECT_THAT(f(3), "21");
    EXPECT_THAT(f(4), "1211");
    EXPECT_THAT(f(5), "111221");
    EXPECT_THAT(f(6), "312211");
  }
}

/*
={=========================================================================
algo-recursion-fibonacci

The Fibonacci Sequence is the series of numbers:

0, 1, 1, 2, 3, 5, 8, 13, 21, 34, ...

The definition is:

F0 = 0.
F1 = 1.
Fn = Fn-1 + Fn-2 for n >= 2

int fibonacci(int n)
{
  if( n <= 0 )
    return 0;
  else if( n==1 )
    return 1;
  else
    return fibonacci(n-1) + fibonacci(n-2);
}

Far more wasteful example than factorial example, the calculation of f7, when
use recursive because when see recursion tree, figure 3.18, DSAPD, after f5 is
calculated on the way to f6 in the left side of tre, it will be lost and
unavailable when it is later needed to get f7 in the right side of tree. Have
to calculate it again. O(2^n) tree

The interative version is:

current   one back(fn-1)   two back(fn-2)
2       = 1              + 0
3       = 2              + 1
4       = 3              + 2
5       = 4              + 3
6       = 5              + 4
...
*/

namespace algo_recursion_fibonacci
{
  // n is nth fibonacci term

  int fibonacci_1(int n)
  {
    if (n <= 0)
      return 0;
    else if (n == 1)
      return 1;
    else
      return fibonacci_1(n - 1) + fibonacci_1(n - 2);
  }

  int fibonacci_2(int n)
  {
    int twoback{}; // f(n-2)
    int oneback{}; // f(n-1)
    int current{};

    if (n <= 0)
      return 0;
    else if (n == 1)
      return 1;
    else
    {
      // back from current
      twoback = 0;
      oneback = 1;

      for (int i = 2; i <= n; ++i)
      {
        current = twoback + oneback;

        // for next f
        twoback = oneback;
        oneback = current;
      }
    }

    return current;
  }
} // namespace algo_recursion_fibonacci

TEST(AlgoRecursion, check_fibonacci)
{
  using namespace algo_recursion_fibonacci;

  EXPECT_THAT(fibonacci_1(4), 3);
  EXPECT_THAT(fibonacci_1(5), 5);
  EXPECT_THAT(fibonacci_1(6), 8);
  EXPECT_THAT(fibonacci_1(7), 13);

  EXPECT_THAT(fibonacci_2(4), 3);
  EXPECT_THAT(fibonacci_2(5), 5);
  EXPECT_THAT(fibonacci_2(6), 8);
  EXPECT_THAT(fibonacci_2(7), 13);
}

/*
={=========================================================================
algo-recursion-climbing-stairs algo-leetcode-18

70. Climbing Stairs, Easy

You are climbing a stair case. It takes n steps to reach to the top.

Each time you can either climb 1 or 2 steps. In how many distinct ways can you
climb to the top?

Note: Given n will be a positive integer.

Example 1:
Input: n = 2
Output: 2
Explanation: There are two ways to climb to the top.
1. 1 step + 1 step
2. 2 steps

Example 2:
Input: n = 3
Output: 3
Explanation: There are three ways to climb to the top.
1. 1 step + 1 step + 1 step
2. 1 step + 2 steps
3. 2 steps + 1 step
 

Approach 1: Brute Force

In this brute force approach we take all possible step combinations i.e. 1 and
2, at every step. At every step we are calling the function climbStairs for step
1 and 2, and return the sum of returned values of both functions.

int climb_stairs(int start, int end);

climbStairs(i, n) = ("i+1",n) + climbStairs("i+2", n)

where i defines the current step and n defines the destination step.

Time complexity : O(2^n)

since the size of recursion tree will be 2^n

N = 2
          (0,2)

    (1,2)       (2,2)
                ret 1
(2,2)  (3,2) 
ret 1  ret 0

return 1 means found and return 0 means not found.

Space complexity : O(n) The depth of the recursion tree can go upto n.

NOTE:
1. need to know whether it reaches to "n" or not.
2. return value if num of way to reach "n"
 
*/

namespace leetcode_easy_018
{
  int climb_stairs(int start, int end)
  {
    // stopping condition
    if (start == end)
      return 1;
    else if (start > end)
      return 0;

    return climb_stairs(start + 1, end) + climb_stairs(start + 2, end);
  }

  int climbStairs_1(int n) { return climb_stairs(0, n); }
} // namespace leetcode_easy_018

TEST(AlgoRecusrion, check_climb_stairs_1)
{
  using namespace leetcode_easy_018;
  auto func = climbStairs_1;

  EXPECT_THAT(func(2), 2);
  EXPECT_THAT(func(3), 3);
  EXPECT_THAT(func(4), 5);
  EXPECT_THAT(func(30), 1346269);
}

/*
Approach 2: Recursion with memory

In the previous approach we are redundantly calculating the result for every
step. Instead, we can store the result at each step in memo array and directly
returning the result from the memo array whenever that function is called again.

In this way we are *pruning* recursion tree with the help of memo array and
reducing the size of recursion tree upto n.

(Like fibonacci problem, the right part of recursion tree uses the same
calculation which are calculated already but calculate them again since they are
lost. So can keep them and use it then better performance)

Time complexity : O(n). Size of recursion tree can go upto nn.
Space complexity : O(n). The depth of recursion tree can go upto nn. 

NOTE:
1. add checking memo value in the stopping condition which removes recursion
calls or prunes tree.

2. memo[index] means the num of ways to reach to the end from that index. so no
need to call the function to get the same result.
 
*/

namespace leetcode_easy_018
{
  int climb_stairs(int start, int end, std::vector<int> &memo)
  {
    // stopping condition
    if (start == end)
      return 1;
    else if (start > end)
      return 0;
    else if (memo[start])
      return memo[start];

    memo[start] =
      climb_stairs(start + 1, end, memo) + climb_stairs(start + 2, end, memo);

    return memo[start];
  }

  int climbStairs_2(int n)
  {
    std::vector<int> memo(n + 1, 0);

    return climb_stairs(0, n, memo);
  }
} // namespace leetcode_easy_018

TEST(AlgoRecusrion, check_climb_stairs_2)
{
  using namespace leetcode_easy_018;

  auto func = climbStairs_2;

  EXPECT_THAT(func(2), 2);
  EXPECT_THAT(func(3), 3);
  EXPECT_THAT(func(4), 5);
  EXPECT_THAT(func(30), 1346269);
}

/*
Approach 3: Dynamic Programming

As we can see this problem can be broken into subproblems, and it contains the
optimal substructure property i.e. its optimal solution can be constructed
efficiently from optimal solutions of its subproblems, we can use dynamic
programming to solve this problem.

One can reach ith step in one of the two ways:

Taking a single step from (i-1) th step.

Taking two step from (i−2) th step.

(since it is about way to reach to n but not number of steps)

So, the total number of ways to reach i th is equal to sum of ways of reaching
(i−1)th step and ways of reaching (i-2)th step.  

Let dp[i] denotes the number of ways to reach on i th step:

dp[i]=dp[i-1]+dp[i-2]


Approach 4: Fibonacci Number

In the above approach we have used dp array where dp[i]=dp[i-1]+dp[i-2]. It
can be easily analysed that dp[i] is nothing but ith fibonacci number.

means the dp value sequence. this is fibonacci sequence:

0, 1, 1, 2, 3, 5, 8, 13, 21, 34, ...

dp value sequence:

0, 1, 2, 3, 5, 8, 13, 21, 34, ...

Now we just have to find n th number of the fibonacci series having 1 and 2
their first and second term respectively, i.e. Fib(1)=1 and Fib(2)=2.


  int fibonacci_2(int n)
  {
    int twoback{};  // f(n-2)
    int oneback{};  // f(n-1)
    int current{};

    if (n <= 0)
      return 0;
    else if (n == 1)
      return 1;
    else
    {
      // back from current
      twoback = 0;
      oneback = 1;

      for (int i = 2; i <= n; ++i)
      {
        current = twoback + oneback;

        // for next f
        twoback = oneback;
        oneback = current;
      }
    }

    return current;
  }
*/

// to show value sequence is fibonacci series
TEST(AlgoRecusrion, check_climb_stairs_value_sqquence)
{
  using namespace leetcode_easy_018;

  auto func = climbStairs_2;

  EXPECT_THAT(func(0), 1);
  EXPECT_THAT(func(1), 1);
  EXPECT_THAT(func(2), 2);
  EXPECT_THAT(func(3), 3);
  EXPECT_THAT(func(4), 5);
  EXPECT_THAT(func(5), 8);
  EXPECT_THAT(func(6), 13);
  EXPECT_THAT(func(7), 21);
  EXPECT_THAT(func(8), 34);
}

namespace leetcode_easy_018
{
  int climbStairs_3(int n)
  {
    // base cases
    if (n <= 0)
      return 0;
    if (n == 1)
      return 1;
    if (n == 2)
      return 2;

    int one_step_before  = 2; // when n == 2
    int two_steps_before = 1; // when n == 1
    int all_ways         = 0;

    // starts from n == 3
    for (int i = 3; i <= n; i++)
    {
      all_ways         = one_step_before + two_steps_before;
      two_steps_before = one_step_before;
      one_step_before  = all_ways;
    }

    return all_ways;
  };
} // namespace leetcode_easy_018

TEST(AlgoRecusrion, check_climb_stairs_3)
{
  using namespace leetcode_easy_018;

  auto func = climbStairs_3;

  EXPECT_THAT(func(2), 2);
  EXPECT_THAT(func(3), 3);
  EXPECT_THAT(func(4), 5);
  EXPECT_THAT(func(30), 1346269);
}

/*
see time difference between recursion and iterative version

[ RUN      ] AlgoRecusrion.check_climb_stairs_1
[       OK ] AlgoRecusrion.check_climb_stairs_1 (10 ms)
[ RUN      ] AlgoRecusrion.check_climb_stairs_2
[       OK ] AlgoRecusrion.check_climb_stairs_2 (0 ms)
[ RUN      ] AlgoRecusrion.check_climb_stairs_3
[       OK ] AlgoRecusrion.check_climb_stairs_3 (0 ms)

[ RUN      ] LeetCode.Easy_018_ClimbStairs_1
[       OK ] LeetCode.Easy_018_ClimbStairs_1 (52 ms)
[ RUN      ] LeetCode.Easy_018_ClimbStairs_2
[       OK ] LeetCode.Easy_018_ClimbStairs_2 (0 ms)
[ RUN      ] LeetCode.Easy_018_ClimbStairs_3
[       OK ] LeetCode.Easy_018_ClimbStairs_3 (0 ms)
*/

/*
={=========================================================================
algo-recursion-factorial

4! = 4 × 3 × 2 × 1 = 24
 
*/

namespace algo_recursion_factorial
{
  // CPR 227
  int factorial_1(int value)
  {
    // CodeComplete 440, #19
    if (value > 1)
      return factorial_1(value - 1) * value;

    return 1;
  }

  // CodeComplete 397, #17.2,
  // Don’t use recursion for factorials or Fibonacci numbers

  int factorial_2(int value)
  {
    int result{1};

    // for (int i = 1; i <= value; ++i)

    for (int i = 2; i <= value; ++i)
    {
      result *= i;
    }

    return result;
  }

  int factorial_3(int value)
  {
    int result{1};

    for (; 0 < value; --value)
    {
      result *= value;
    }

    return result;
  }
} // namespace algo_recursion_factorial

// not good idea to use factorial to see performance difference due to tail
// recursion since number gets bigger quickly

TEST(AlgoRecursion, factorial)
{
  using namespace algo_recursion_factorial;

  EXPECT_THAT(factorial_1(5), 120);
  EXPECT_THAT(factorial_1(10), 3628800);

  EXPECT_THAT(factorial_2(5), 120);
  EXPECT_THAT(factorial_2(10), 3628800);

  EXPECT_THAT(factorial_3(5), 120);
  EXPECT_THAT(factorial_3(10), 3628800);
}

/*
={=========================================================================
algo-recursion-factorial

172. Factorial Trailing Zeroes

Given an integer n, return the number of trailing zeroes in n!.

Example 1:

Input: 3
Output: 0
Explanation: 3! = 6, no trailing zero.
Example 2:

Input: 5
Output: 1
Explanation: 5! = 120, one trailing zero.

Note: Your solution should be in logarithmic time complexity.
 
*/

namespace algo_recursion_factorial
{
  // ok, use itoa way
  int get_trailing_zero_1(int value)
  {
    int zeros{};

    while (value)
    {
      auto remainder = value % 10;
      if (remainder != 0)
        break;

      value = value / 10;
      ++zeros;
    }

    return zeros;
  }

  // combine factorial and trailing zero.
  int get_trailing_zero_2(int value)
  {
    int fact{1};
    int zeros{};

    for (int i = 2; i <= value; ++i)
    {
      fact *= i;
    }

    while (fact)
    {
      // auto remainder = fact % 10;
      // if (remainder != 0)
      //   break;

      if ((fact % 10) != 0)
        break;

      fact = fact / 10;
      ++zeros;
    }

    return zeros;
  }

  // Runtime: 0 ms, faster than 100.00% of C++ online submissions for Factorial Trailing Zeroes.
  // Memory Usage: 5.9 MB, less than 62.36% of C++ online submissions for Factorial Trailing Zeroes.

  int get_trailing_zero_3(int value)
  {
    int zeros{};

    while (value >= 5)
    {
      zeros += value / 5;
      value = value / 5;
    }

    return zeros;
  }
} // namespace algo_recursion_factorial

TEST(AlgoRecursion, factorial_overflow)
{
  using namespace algo_recursion_factorial;

  // Ok. EXPECT_THAT(factorial_1(5), 120);
  {
    auto f   = get_trailing_zero_1;
    auto ret = f(120);
    EXPECT_THAT(ret, 1);
  }

  // Ok. EXPECT_THAT(factorial_1(10), 3628800);
  {
    auto f   = get_trailing_zero_1;
    auto ret = f(3628800);
    EXPECT_THAT(ret, 2);
  }

  // Ok.
  {
    auto f   = get_trailing_zero_2;
    auto ret = f(5);
    EXPECT_THAT(ret, 1);
  }

  // Ok.
  {
    auto f   = get_trailing_zero_2;
    auto ret = f(10);
    EXPECT_THAT(ret, 2);
  }

  // However, "int get_trailing_zero_2(int value)" cause runtime overflow error:
  //
  // Line 9: Char 12: runtime error: signed integer overflow:
  // 2432902008176640000 * 21 cannot be represented in type 'long long'
  // (solution.cpp)
  //
  // changed to "long long" but still see the error. to see why.
  //
  // long long factorial_2(int value)
  // {
  //   long long result{1};
  //
  //   for (int i = 2; i <= value; ++i)
  //   {
  //     result *= i;
  //   }
  //
  //   return result;
  // }

  {
    EXPECT_THAT(std::numeric_limits<int>::max(), 2147483647);
  }

  // 9223372036854775807
  // 2432902008176640000
  {
    EXPECT_THAT(std::numeric_limits<long long>::max(), 9223372036854775807);
    // EXPECT_THAT(factorial_2_2(20), 2432902008176640000);
  }

  // Line 9: Char 12: runtime error: signed integer overflow:
  // 2432902008176640000 * 21 cannot be represented in type 'long long'
  // (solution.cpp)

  // NOTE: so "getting factorial" is not a way since it gets too big quickly.

  // see when there are 5, it gets "0" and also see that when value 25, has 6
  // zeros but not 5. need to do on "remiander" as well.
  //
  // 1*2*3*4*5
  // 6*7*8*9*10
  // ...
  //
  // >>> *5
  // 120
  // >>> 120*6
  // 720
  // >>> 120*6*7*8*9*10
  // 3628800
  // >>> 120*6*7*8*9*10*11*12*13*14*15
  // 1307674368000
  // >>> 120*6*7*8*9*10*11*12*13*14*15*16*17*18*19*20
  // 2432902008176640000
  // >>> 120*6*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25
  // 15511210043330985984000000L

  // Ok
  {
    auto f   = get_trailing_zero_3;
    auto ret = f(10);
    EXPECT_THAT(ret, 2);
  }

  {
    auto f   = get_trailing_zero_3;
    auto ret = f(25);
    EXPECT_THAT(ret, 6);
  }
}

// ={=========================================================================
// algo-recursion-maze algo-maze

using MazePoint = std::pair<int, int>;

// 1. input and point(row, col) which is the same as point(y, x) but
// not point(x, y). This has to do with array access.
//
// 2. If it's valid point which are not checked against to traveled points, call
// find_path for every possible move from a point and then this move is cheked
// against to the traveled points. So supporting diagonal move needs more
// recursive calls. Therefore, it use the traveled path to exit the call for
// points that it already traveled; to move forward, to move back when see the
// dead end, and naturally to prevent circular path.
//
// See again that calls find_path() for every possible move.
//
// 3. To support range from (1,1) to (N,N), used padding to the input data:
//
// {2,2,2,2,2,2},
// {2,0,0,0,0,0},
// {2,1,1,1,1,0},
// {2,0,0,0,0,0},
// {2,0,1,1,1,1},
// {2,2,0,0,0,0}
//
// the result of run:
//
// (1, 2) (1, 3) (1, 4) (1, 5) (2, 5) (3, 5) (3, 4) (3, 3) (3, 2) (3, 1) (4, 1) (5, 1) (5, 2) (5, 3) (5, 4) (5, 5) (16)
//
//             [ 10578] | find_path(1, 1) {
//             [ 10578] |   find_path(1, 2) {
//             [ 10578] |     find_path(1, 1) {
//   12.428 us [ 10578] |     } /* find_path */
//             [ 10578] |     find_path(1, 3) {
//             [ 10578] |       find_path(1, 2) {
//   10.787 us [ 10578] |       } /* find_path */
//             [ 10578] |       find_path(1, 4) {
//             [ 10578] |         find_path(1, 3) {
//   13.404 us [ 10578] |         } /* find_path */
//             [ 10578] |         find_path(1, 5) {
//             [ 10578] |           find_path(1, 4) {
//   12.767 us [ 10578] |           } /* find_path */
//             [ 10578] |           find_path(2, 5) {
//             [ 10578] |             find_path(1, 5) {
//   85.506 us [ 10578] |             } /* find_path */
//             [ 10578] |             find_path(3, 5) {
//             [ 10578] |               find_path(3, 4) {
//             [ 10578] |                 find_path(3, 3) {
//             [ 10578] |                   find_path(3, 2) {
//             [ 10578] |                     find_path(3, 1) {
//             [ 10578] |                       find_path(3, 2) {
//   28.249 us [ 10578] |                       } /* find_path */
//             [ 10578] |                       find_path(4, 1) {
//             [ 10578] |                         find_path(3, 1) {
//  182.678 us [ 10578] |                         } /* find_path */
//             [ 10578] |                         find_path(5, 1) {
//             [ 10578] |                           find_path(5, 2) {
//             [ 10578] |                             find_path(5, 1) {
//   82.954 us [ 10578] |                             } /* find_path */
//             [ 10578] |                             find_path(5, 3) {
//             [ 10578] |                               find_path(5, 2) {
//   17.163 us [ 10578] |                               } /* find_path */
//             [ 10578] |                               find_path(5, 4) {
//             [ 10578] |                                 find_path(5, 3) {
//   19.277 us [ 10578] |                                 } /* find_path */
//             [ 10578] |                                 find_path(5, 5) {
//   19.241 us [ 10578] |                                 } /* find_path */
//  116.223 us [ 10578] |                               } /* find_path */
//  213.627 us [ 10578] |                             } /* find_path */
//  391.920 us [ 10578] |                           } /* find_path */
//  967.572 us [ 10578] |                         } /* find_path */
//    2.857 ms [ 10578] |                       } /* find_path */
//    3.996 ms [ 10578] |                     } /* find_path */
//    4.060 ms [ 10578] |                   } /* find_path */
//    4.126 ms [ 10578] |                 } /* find_path */
//    4.220 ms [ 10578] |               } /* find_path */
//    4.287 ms [ 10578] |             } /* find_path */
//    5.416 ms [ 10578] |           } /* find_path */
//    6.411 ms [ 10578] |         } /* find_path */
//    6.485 ms [ 10578] |       } /* find_path */
//    6.559 ms [ 10578] |     } /* find_path */
//    6.708 ms [ 10578] |   } /* find_path */
//    7.233 ms [ 10578] | } /* find_path */

struct Maze
{
  std::vector<std::vector<int>> input;

  // std::array<std::array<int, 11>, 11> input;
  // std::array<std::array<int, 6>, 6> input;

  // set of visited points
  std::set<MazePoint> visited_points{};

  // array of path points
  std::vector<MazePoint> path_points{};

  Maze(int row, int col)
      : row_(row)
      , col_(col)
  {}

  bool AlreadyTried(MazePoint position)
  {
    return visited_points.find(position) == visited_points.end() ? false : true;
  }

  bool FoundTheExit(MazePoint position)
  {
    return position == MazePoint(row_ - 1, col_ - 1) ? true : false;
  }

  void RememberPosition(MazePoint position)
  {
    auto result = visited_points.insert(position);
    if (!result.second)
    {
      cout << "RememberPosition: founds duplicates" << endl;
      cout << "RememberPosition: (" << position.first << ", " << position.second
           << ")" << endl;
      PRINT_M_ELEMENTS(visited_points);
    }
  }

  // if cannot move, return the input position
  MazePoint GetPositionToMoveLeft(MazePoint position)
  {
    MazePoint point_to_move{position.first, position.second - 1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else
    {
      return position;
    }
  }

  MazePoint GetPositionToMoveRight(MazePoint position)
  {
    MazePoint point_to_move{position.first, position.second + 1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else
    {
      return position;
    }
  }

  MazePoint GetPositionToMoveUp(MazePoint position)
  {
    MazePoint point_to_move{position.first - 1, position.second};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else
    {
      return position;
    }
  }

  MazePoint GetPositionToMoveDown(MazePoint position)
  {
    MazePoint point_to_move{position.first + 1, position.second};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else
    {
      return position;
    }
  }

  MazePoint GetPositionToDiagRightUp(MazePoint position)
  {
    MazePoint point_to_move{position.first - 1, position.second + 1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else
    {
      return position;
    }
  }

  MazePoint GetPositionToDiagRightDown(MazePoint position)
  {
    MazePoint point_to_move{position.first + 1, position.second + 1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else
    {
      return position;
    }
  }

  MazePoint GetPositionToDiagLeftUp(MazePoint position)
  {
    MazePoint point_to_move{position.first - 1, position.second - 1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else
    {
      return position;
    }
  }

  MazePoint GetPositionToDiagLeftDown(MazePoint position)
  {
    MazePoint point_to_move{position.first + 1, position.second - 1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else
    {
      return position;
    }
  }

private:
  bool ValidMazePoint(MazePoint position)
  {
    if ((0 <= position.first && position.first < row_) &&
        (0 <= position.second && position.second < col_) &&
        input[position.first][position.second] != 1)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  int row_{};
  int col_{};
};

// To use uftrace log, changed it from
// bool find_path(Maze &maze, MazePoint position)

bool find_path(Maze &maze, int row, int col)
{
  MazePoint position{row, col};

  // to prevent circular path and going backwards and this make it search
  // forward.
  if (maze.AlreadyTried(position))
  {
    return false;
  }

  // found a path which is stop condition
  if (maze.FoundTheExit(position))
  {
    return true;
  }

  // remember a position tried
  maze.RememberPosition(position);

  MazePoint new_position{};

  // left
  if ((new_position = maze.GetPositionToMoveLeft(position)) != position)
  {
    // cout << "l:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "left:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // right
  if ((new_position = maze.GetPositionToMoveRight(position)) != position)
  {
    // cout << "r:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "right:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // up
  if ((new_position = maze.GetPositionToMoveUp(position)) != position)
  {
    // cout << "u:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "up:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // down
  if ((new_position = maze.GetPositionToMoveDown(position)) != position)
  {
    // cout << "d:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "down:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag right up
  if ((new_position = maze.GetPositionToDiagRightUp(position)) != position)
  {
    // cout << "dru:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:rup:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag right down
  if ((new_position = maze.GetPositionToDiagRightDown(position)) != position)
  {
    // cout << "drd:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:rdown:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag left up
  if ((new_position = maze.GetPositionToDiagLeftUp(position)) != position)
  {
    // cout << "dlu:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:lup:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag left down
  if ((new_position = maze.GetPositionToDiagLeftDown(position)) != position)
  {
    // cout << "dld:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:ldown:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  return false;
}

TEST(AlgoMaze, ExerciseInterfaces)
{
  Maze maze(5, 5);
  maze.input = {{0, 0, 0, 0, 0},
                {1, 1, 1, 1, 0},
                {0, 0, 0, 0, 0},
                {0, 1, 1, 1, 1},
                {2, 0, 0, 0, 0}};

  maze.RememberPosition(MazePoint(1, 1));
  maze.RememberPosition(MazePoint(1, 2));

  EXPECT_THAT(maze.AlreadyTried(MazePoint(1, 1)), true);
  EXPECT_THAT(maze.AlreadyTried(MazePoint(1, 3)), false);

  EXPECT_THAT(maze.FoundTheExit(MazePoint(4, 5)), false);
  EXPECT_THAT(maze.FoundTheExit(MazePoint(5, 5)), false);

  // not able to move
  EXPECT_THAT(maze.GetPositionToMoveLeft(MazePoint(0, 0)), Eq(MazePoint(0, 0)));

  // able to move
  EXPECT_THAT(maze.GetPositionToMoveLeft(MazePoint(0, 1)), Eq(MazePoint(0, 0)));
}

TEST(DISABLED_AlgoMaze, Array5x5)
{
  Maze maze(5, 5);
  maze.input = {{0, 0, 0, 0, 0},
                {1, 1, 1, 1, 0},
                {0, 0, 0, 0, 0},
                {0, 1, 1, 1, 1},
                {2, 0, 0, 0, 0}};

  // use start point (0, 0) rather then (1, 1).
  find_path(maze, 0, 0);
  PRINT_M_ELEMENTS(maze.path_points);
}

// TEST(DISABLED_Maze, Array10x10)
TEST(AlgoMaze, Array10x10)
{
  // when not support diagonal move
  // Maze maze(10, 10);
  // maze.input = {
  //   {0,1,0,1,0,1,0,0,0,1},
  //   {0,1,0,1,0,1,1,1,0,1},
  //   {0,0,0,0,0,1,0,0,0,1},
  //   {0,1,0,1,1,1,0,1,1,1},
  //   {0,1,2,1,0,0,0,0,0,1},
  //   {1,1,0,1,0,1,1,1,1,1},
  //   {0,1,0,0,0,0,0,1,0,1},
  //   {0,1,1,1,0,1,1,1,0,1},
  //   {0,0,0,0,0,0,0,0,0,1},
  //   {1,1,1,1,1,1,1,1,0,0}
  // };

  Maze maze(10, 10);
  maze.input = {{0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
                {0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
                {0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
                {0, 1, 0, 1, 1, 1, 0, 1, 1, 1},
                {0, 1, 2, 1, 0, 0, 0, 0, 0, 1},
                {1, 1, 0, 1, 0, 1, 1, 1, 1, 1},
                {0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
                {0, 1, 1, 1, 0, 1, 1, 1, 0, 1},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 0}};

  find_path(maze, 0, 0);
  PRINT_M_ELEMENTS(maze.path_points);
}

TEST(DISABLED_AlgoMaze, Array15x15)
{
  Maze maze(15, 15);
  maze.input = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

  find_path(maze, 0, 0);
  PRINT_M_ELEMENTS(maze.path_points);
}

// This input select the path which do have have "2" in. How to support this
// requirement?
//
// * change the traveled path to have the value and to keep only traveled points
// so far. So need to remove point when find_path() returns false so that
// traveled path only have points up to where it runs into
//
// * changed the end condition to see if the traveled path has the "2". if not
// return false so that can try other paths.

TEST(DISABLED_AlgoMaze, Array20x20)
{
  Maze maze(20, 20);
  maze.input = {

    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}};

  find_path(maze, 0, 0);
  PRINT_M_ELEMENTS(maze.path_points);
}

/*
={=========================================================================
algo-bits-count-bits count same bits between two integers

191. Number of 1 Bits, Easy

Write a function that takes an unsigned integer and return the number of '1'
bits it has (also known as the Hamming weight).

Example 1:

Input: 00000000000000000000000000001011
Output: 3

Explanation: 
The input binary string 00000000000000000000000000001011 has a total of three
'1' bits.

Example 2:

Input: 00000000000000000000000010000000
Output: 1

Explanation: 
The input binary string 00000000000000000000000010000000 has a total of one '1'
bit.

Example 3:

Input: 11111111111111111111111111111101
Output: 31

Explanation: 
The input binary string 11111111111111111111111111111101 has a total of thirty
one '1' bits.

int get_hamming_weight(uint32_t n);
 
Note:

Note that in some languages such as Java, there is no unsigned integer type. In
this case, the input will be given as signed integer type and should not affect
your implementation, as the internal binary representation of the integer is the
same whether it is signed or unsigned. 

In Java, the compiler represents the signed integers using 2's complement
notation. Therefore, in Example 3 above the input represents the signed integer
-3.

*/

namespace algo_bit
{
  // as with itoa
  //
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Number of
  // 1 Bits.
  //
  // Memory Usage: 8.1 MB, less than 70.42% of C++ online submissions for Number
  // of 1 Bits.
  int get_hamming_weight_1(uint32_t n)
  {
    int count{};

    while (n)
    {
      if (n % 2)
        count++;

      n /= 2;
    }

    return count;
  }

  int get_hamming_weight_2(uint32_t n)
  {
    int set_count{};

    while (n)
    {
      if (n & 0x1)
        ++set_count;

      n >>= 1;
    }

    return set_count;
  }

  // page 51. exercise 2-9. In a two's complement number system, x &= (x-1)
  // deletes the rightmost 1-bit in x. Explain why. Use this observation to write
  // a 'faster' version of bitcount.
  //
  // Answer:
  //
  // If x is odd, then (x-1) has the same bit representation as x except that the
  // rightmost 1-bit becomes a 0. In this case, (x & (x-1)) == (x-1).
  //
  // x = 5: 5(101) & 4(100) = 100  // 101 -> 100 by having rightmost 1 to 0
  //
  // If x is even, the end result of anding(&) x and x-1 has the rightmost 1 of x to 0.
  //
  // x = 4: 4(100) & 3(11)  = 0    // 100 -> 0   by having rightmost 1 to 0
  //          ^ rightmost 1
  //
  // x = 6: 6(110) & 5(101) = 100  // 110 -> 100 by having rightmost 1 to 0
  //           ^ rightmost 1
  //
  // x = 8: 8(1000) & 7(111) = 0   // 1000 -> 0  by having rightmost 1 to 0
  //
  // 000   0     All even numbers has tailing 0s and it becomes 1 when minus 1
  // 001   1
  // 010   2
  // 011   3
  // 100   4
  // 101   5
  // 110   6
  // 111   7
  // ...
  //
  // note: This is about careful observation but not a mechanism of borrowing a
  // carry for example. For both odd and even case, has the effect of having
  // rightmost 1 to 0. So clear 1 from x one by one and no need to check on if
  // to count bits.
  //
  // note: And(&) is faster than shift operation? Yes and also there is no `if`
  // in the loop.

  int get_hamming_weight_3(uint32_t n)
  {
    int count{};

    for (; n; n &= (n - 1))
      count++;

    return count;
  }

  // known as "compute parity"
  // return 0 if there even number of 1
  // return 1 if there odd number of 1

  bool get_parity_1(uint32_t n)
  {
    uint32_t count{};

    while (n)
    {
      if (n & 0x1)
        ++count;

      n >>= 1;
    }

    return ((count % 2) == 0 ? 0 : 1);
  }

  // performance improvement? cxx-xor
  // see algo-occurance find a number seen odd times cxx-xor
  bool get_parity_2(uint32_t n)
  {
    uint32_t result{};

    while (n)
    {
      result = (n & 0x1) ^ result;

      n >>= 1;
    }

    return result;
  }
} // namespace algo_bit

TEST(AlgoBit, check_hamming_weight)
{
  using namespace algo_bit;

  {
    std::vector<std::function<int(unsigned int)>> imps{get_hamming_weight_1,
                                                       get_hamming_weight_2,
                                                       get_hamming_weight_3};

    for (const auto &f : imps)
    {
      // Input: 00000000000000000000000000001011, 11
      EXPECT_THAT(f(11), 3);

      // Input: 00000000000000000000000010000000, 128
      EXPECT_THAT(f(128), 1);

      // Input: 11111111111111111111111111111101, 4294967293
      EXPECT_THAT(f(4294967293), 31);
    }
  }

  // https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
  // 6.59 Other Built-in Functions Provided by GCC
  // Built-in Function: int __builtin_popcount (unsigned int x)
  // Returns the number of 1-bits in x.
  // error: built-in function ‘__builtin_popcount’ must be directly called
  {
    EXPECT_THAT(__builtin_popcount(11), 3);
    EXPECT_THAT(__builtin_popcount(128), 1);
    EXPECT_THAT(__builtin_popcount(4294967293), 31);
  }

  {
    std::vector<std::function<int(unsigned int)>> imps{get_parity_1,
                                                       get_parity_2};

    for (const auto &f : imps)
    {
      // Input: 00000000000000000000000000001011, 11
      EXPECT_THAT(f(11), true);

      // Input: 00000000000000000000000010000001, 129
      EXPECT_THAT(f(129), false);

      // Input: 11111111111111111111111111111101, 4294967293
      EXPECT_THAT(f(4294967293), true);
    }
  }
}

/*

A = 35 = 10 0011
B =  9 =    1001

Ans = 2 because only counts bit positions which are valid position in both
integers.

From ansic, p50.
The function counts the number of 1 bits in its integer argument.

1. The key is not to use sizeof operator
2. unsigned int

*cxx-shift* Must use `unsigned` to do  `right-shift` in order to have
guaranteed 0 values.

3. use independent of type.

*/

namespace algo_bit
{
  // 2018.0619
  // 1. unsigned int
  int get_common_bits_1(uint32_t a, uint32_t b)
  {
    // get min and max
    auto input = minmax(a, b);

    // take min value
    unsigned int min = input.first;
    unsigned int max = input.second;

    // get position of the pivot
    unsigned int num_of_bits = sizeof(min) * 8;
    unsigned int pos_of_msb{};
    unsigned int pivot = min;

    for (unsigned int i = 0; i < num_of_bits; ++i)
    {
      if (pivot & 0x1)
        pos_of_msb = i;

      pivot >>= 1;
    }

    // // get mask value, mask max, and get xor'ed value
    // unsigned int mask_value{}, calculated_input{};

    // for (unsigned int i = 0; i <= pos_of_msb; ++i)
    //   mask_value |= (1 << i);

    // max = max & mask_value;

    unsigned int calculated_input = max ^ min;

    // get num of common bits
    unsigned int num_of_common_bits{};

    for (unsigned int i = 0; i <= pos_of_msb; ++i)
    {
      if (!(calculated_input & 0x1))
        ++num_of_common_bits;

      calculated_input >>= 1;
    }

    return num_of_common_bits;
  }

  // 2018.07
  int get_common_bits_2(uint32_t a, uint32_t b)
  {
    // get min and max
    auto input = minmax(a, b);

    // take min value
    unsigned int min = input.first;
    unsigned int max = input.second;

    unsigned int num_of_common_bits{};

    // no differetn when use for (; min && min >>= 1;)
    for (; min; min >>= 1, max >>= 1)
    {
      // same bit, 0 or 1 between two numbers
      if ((min & 0x1) == (max & 0x1u))
        ++num_of_common_bits;
    }

    return num_of_common_bits;
  }

  // 2019.03
  int get_common_bits_3(uint32_t a, uint32_t b)
  {
    uint32_t count{};

    for (; a && b; a >>= 1, b >>= 1)
    {
      if ((a & 0x1) == (b & 0x01))
        count++;
    }

    return count;
  }

  // 2020.07
  int get_common_bits_4(uint32_t x, uint32_t y)
  {
    int common_count{};

    while (x && y)
    {
      if ((x & 0x1) == (y & 0x1))
        ++common_count;

      x >>= 1;
      y >>= 1;
    }

    return common_count;
  }
} // namespace algo_bit

TEST(AlgoBit, check_common_bits)
{
  using namespace algo_bit;

  {
    std::vector<std::function<int(unsigned int, unsigned int)>> imps{
      get_common_bits_1,
      get_common_bits_2,
      get_common_bits_3,
      get_common_bits_4};

    for (const auto &f : imps)
    {
      //   9,   1001
      //  35, 100011,   mask, 15 (1111),  max, 3(0011)
      //                                    9,   1001
      //                                  xor,   1010
      //                                  ans, 2
      EXPECT_THAT(f(35, 9), 2);

      // 55 = 100111,  mask, 7 (0111),   max, 7(0111)
      //                                    5,    101
      //                                  xor, 2( 010)
      //                                  ans, 2
      EXPECT_THAT(f(55, 5), 2);
    }
  }
}

/*
={=========================================================================
algo-bits-reverse

190. Reverse Bits, Easy

Reverse bits of a given 32 bits unsigned integer.

uint32_t reverse_bits(uint32_t n);

Example 1:

Input: 00000010100101000001111010011100
Output: 00111001011110000010100101000000

Explanation: 
The input binary string 00000010100101000001111010011100 represents the unsigned
integer 43261596, so return 964176192 which its binary representation is
00111001011110000010100101000000.

Example 2:

Input: 11111111111111111111111111111101
Output: 10111111111111111111111111111111

Explanation: 
The input binary string 11111111111111111111111111111101 represents the unsigned
integer 4294967293, so return 3221225471.
 
note:
input string?

*/

namespace algo_bit
{
  // >>> bin(43261596)
  // '0b10100101000001111010011100'
  // >>> bin(964176192)
  // '0b111001011110000010100101000000'
  // >>> bin(15065253)
  // '0b111001011110000010100101'
  //
  // >>> bin(15065253) is correct when simply reversing bits. why different?
  // since input has leasing 0s.
  //
  // >>> bin(43261596)
  // '0b????.??10.1001.0100.0001.1110.1001.1100'
  // >>> bin(964176192)
  // '0b0011.1001.0111.1000.0010.1001.0100.0000'

  uint32_t reverse_bits_1(uint32_t n)
  {
    uint32_t result{};

    while (n)
    {
      result <<= 1;
      result |= (n & 0x1);

      n >>= 1;
    }

    return result;
  }

  // from online
  uint32_t reverse_bits_2(uint32_t n)
  {
    int reverse = 0;

    for (int i = 0; i < 32; i++)
    {
      reverse = (reverse << 1) | (n & 1);
      n >>= 1;
    }

    return reverse;
  }

  // revise reverse_bits_1()
  // Runtime: 4 ms, faster than 69.13% of C++ online submissions for Reverse Bits.
  // Memory Usage: 5.9 MB, less than 91.90% of C++ online submissions for Reverse Bits.
  uint32_t reverse_bits_3(uint32_t n)
  {
    uint32_t result{};

    for (size_t i = 0; i < 32; ++i)
    {
      // warning: operation on ‘result’ may be undefined [-Wsequence-point]
      // result = ((result <<= 1) | (n & 0x1));
      uint32_t temp = result <<= 1;
      result        = (temp | (n & 0x1));
      n >>= 1;
    }

    return result;
  }

  // NOTE: the key is it's "32" bits

  // from discussion:
  //
  // O(1) bit operation C++ solution (8ms), tworuler
  //
  // for 8 bit binary number abcdefgh, the process is as follow:
  // abcdefgh -> [efgh]abcd -> [gh][ef]cdab -> [hgfe]dcba

  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Reverse
  // Bits.
  //
  // Memory Usage: 8 MB, less than 79.46% of C++ online submissions for Reverse
  // Bits.
  //
  // exchange 16, 8, 4, 2, 1 bits.

  uint32_t reverse_bits_4(uint32_t n)
  {
    // abcdefgh -> efghabcd
    n = (n >> 16) | (n << 16);

    // efghabcd -> gh[ef]cd[ab]
    // ef00ab00 >> 00ef00ab, 00gh00cd << gh00cd00
    n = ((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8);

    // ghefcdab -> hgfedcba
    n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);

    // 1100 and 0011
    n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
    // 1010 and 0101
    n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);

    return n;
  }

  // NOTE: faster than for loop.
} // namespace algo_bit

TEST(AlgoBit, check_reverse_bits)
{
  using namespace algo_bit;

  {
    std::vector<std::function<int(unsigned int)>> imps{reverse_bits_2,
                                                       reverse_bits_3,
                                                       reverse_bits_4};

    for (const auto &f : imps)
    {

      EXPECT_THAT(f(43261596), 964176192);
      EXPECT_THAT(f(4294967293), 3221225471);
    }
  }
}

/*
={=========================================================================
algo-equi-index

codility: equilibrium index of a sequence

The equilibrium index of a sequence is an index such that the sum of elements
at lower indexes is equal to the sum of elements at higher indexes. For
example, in a sequence A:

A[0]=-7 A[1]=1 A[2]=5 A[3]=2 A[4]=-4 A[5]=3 A[6]=0    [-7, 1, 5, 2, -4, 3, 0]

3 is an equilibrium index, because: A[0]+A[1]+A[2]=A[4]+A[5]+A[6]

6 is also an equilibrium index, because: A[0]+A[1]+A[2]+A[3]+A[4]+A[5]=0 (The
sum of zero elements is zero) 

7 is not an equilibrium index - because it is not a valid index of sequence A.

If you still have doubts, here is a precise definition: 

The integer k is an equilibrium index of a sequence A[0],A[1]..,A[n-1] if and
only if 0<= k and sum(A[0..(k-1)])=sum(A[(k+1)..(n-1)]). Assume the sum of
zero elements is equal to zero.

Write a function

int equi(int A[], int n);

that, given a sequence, returns its equilibrium index (any) or -1 if no
equilibrium index exists. `Assume that the sequence may be very long.` 

The problem can be solved by using various approaches, the most common being
simply to follow the equilibrium definition:

// while moving index, continues to sum right and left sum.

int equi ( int A[], int n ) {

  int k, m, lsum, rsum; 

  for(k = 0; k < n; ++k) { 
    lsum = 0; rsum = 0;
    for(m = 0; m < k; ++m) lsum += A[m]; 
    for(m = k + 1; m < n; ++m) rsum += A[m];  
    if (lsum == rsum) return k;
  } 
  return -1; 
} 

Unfortunately, this approach has two disadvantages:

o it fails on large input data sets, since the time complexity is O(n2)

o it fails on large input values (for example if the input array contains
    values like MIN/MAX_INT) due to the arithmetic overflows

We can fix the first problem with a better algorithm, and the second problem
with a better data-type (for example, using long long type instead of int for
    sum computations). 
  
The key observation for better running time is to update the left/right sums
in constant time instead of recomputing them from the scratch. O(n)

      [0] [ ] [ ] ... [ ]
   ->     <------------->
   lsum   rsum

      [0] [1] [ ] ... [ ]
   ----->     <------------->
   lsum   rsum

      [0] [1] [2] [3] ... [ ]
   --------->     <------------->
   lsum   rsum

*/

namespace algo_equi_index
{
  // old. use total sum
  int equi_index_1(const int A[], int n)
  {
    int index{};

    long long total_sum{};
    for (index = 0; index < n; ++index)
      total_sum += A[index];

    long long right_sum{total_sum};
    long long left_sum{};

    for (index = 0; index < n; ++index)
    {
      if (index - 1 >= 0)
        left_sum += A[index - 1];

      right_sum = total_sum - left_sum - A[index];

      if (left_sum == right_sum)
        return index;
    }

    return -1;
  }

  // old. same as above but remove if by moving equal check
  int equi_index_2(const int A[], int n)
  {
    int index{};

    long long total_sum{};
    for (index = 0; index < n; ++index)
      total_sum += A[index];

    long long right_sum{total_sum};
    long long left_sum{};

    for (index = 0; index < n; ++index)
    {
      // if (index-1 >= 0)
      //   left_sum += A[index-1];

      right_sum = total_sum - left_sum - A[index];

      if (left_sum == right_sum)
        return index;

      left_sum += A[index];
    }

    return -1;
  }

  // old. do not use total_sum variable since rsum is total
  int equi_index_3(const int A[], int n)
  {
    if (!n || !A)
      return -1;

    long long rsum = 0, lsum = 0;

    for (int i = 0; i < n; i++)
      rsum += A[i];

    for (int i = 0; i < n; i++)
    {
      rsum -= A[i];

      if (lsum == rsum)
        return i;

      lsum += A[i];
    }

    return -1;
  }

  // 2020.07
  int equi_index_4(const int A[], int n)
  {
    long long rsum{}, lsum{};

    // gets the rsum
    for (int i = 0; i < n; ++i)
      rsum += A[i];

    for (int i = 0; i < n; ++i)
    {
      if ((i - 1) >= 0)
        lsum += A[i - 1];

      rsum -= A[i];

      if (lsum == rsum)
        return i;
    }

    // found no equi index.
    return -1;
  }

  // 2020.07
  int equi_index_5(const int A[], int n)
  {
    long long rsum{}, lsum{};

    // gets the rsum
    for (int i = 0; i < n; ++i)
      rsum += A[i];

    for (int i = 0; i < n; ++i)
    {
      rsum -= A[i];

      if (lsum == rsum)
        return i;

      lsum += A[i];
    }

    // found no equi index.
    return -1;
  }
} // namespace algo_equi_index

TEST(AlgoEquilbrium, check_equi_index)
{
  using namespace algo_equi_index;

  {
    // error: on define function<> when has array argument
    // std::vector<std::function<int(const int[], int)>> imps{equi_index_1};
    //
    // OK, same
    // std::initializer_list<std::function<int(const std::vector<int> &)>> imps = {
    //  check_perm_4};

    auto imps = {equi_index_1,
                 equi_index_2,
                 equi_index_3,
                 equi_index_4,
                 equi_index_5};

    const int coll[]{-7, 1, 5, 2, -4, 3, 0};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(coll, 7), 3);
    }
  }
}

/*
={=========================================================================
algo-equi-tape

https://codility.com/train/

When choose C++

A non-empty zero-indexed array A consisting of N integers is given. Array A
represents numbers on a tape.

Any integer P, such that 0 < P < N, splits this tape into two non-empty parts: 
A[0], A[1], ..., A[P - 1] and A[P], A[P + 1], ..., A[N - 1].

The difference between the two parts is the value of: 
|(A[0] + A[1] + ... + A[P - 1]) - (A[P] + A[P + 1] + ... + A[N - 1])|

In other words, it is the absolute difference between the sum of the first part
and the sum of the second part.

For example, consider array A such that:

  A[0] = 3    _ (split point)
  A[1] = 1    _
  A[2] = 2    _
  A[3] = 4    _
  A[4] = 3

We can split this tape in four places:

  P = 1, difference = |3 - 10| = 7
  P = 2, difference = |4 - 9 | = 5
  P = 3, difference = |6 - 7 | = 1
  P = 4, difference = |10 - 3| = 7

Write a function:

int solution(vector<int> &A);

that, given a non-empty zero-indexed array A of N integers, returns the
`minimal-difference`, but not index, that can be achieved.

For example, given:

  A[0] = 3
  A[1] = 1
  A[2] = 2
  A[3] = 4
  A[4] = 3

the function should return `minimal difference` 1 when P = 3, as explained above.

Assume that:

N is an integer within the range [2..100,000];
each element of array A is an integer within the range [-1,000..1,000].

Complexity:

expected worst-case time complexity is O(N);
expected worst-case space complexity is O(N), beyond input storage (not counting
    the storage required for input arguments).

Elements of input arrays can be modified.

you can also use includes, for example:
#include <algorithm>

note:
If calculate first and second half sum every time when moves array index, time
complexity would be O(N*N).

*/

namespace algo_equi_tape
{
  // model answer
  int equi_tape_1(std::vector<int> &A)
  {
    // write your code in C++98
    if (!A.size())
      return -1;

    long long sum = 0, rsum = 0, lsum = 0;
    int cmin = INT_MAX;

    for (size_t i = 0; i < A.size(); i++)
      sum += A[i];

    lsum = A[0];

    // it is okay to use (n-1)th? WHY?
    // because lsum is used after getting abs value so the last is not used in
    // that calcuation.

    for (size_t i = 1; i < A.size(); i++)
    {
      rsum = sum - lsum;

      // cmin = abs cause warning of possible loss since assign from long long to int.
      if (abs(lsum - rsum) < cmin)
        cmin = abs(lsum - rsum);

      lsum += A[i];
    }

    return cmin;
  }

  // 2020.07
  int equi_tape_2(std::vector<int> &A)
  {
    long long rsum{}, lsum{};
    int current_min{std::numeric_limits<int>::max()};

    // gets the rsum
    for (int i = 0; i < (int)A.size(); ++i)
      rsum += A[i];

    // NOTE: [0, N-2] since no need to process the last element
    // for (int i = 0; i < A.size(); ++i)

    for (int i = 0; i < (int)A.size() - 1; ++i)
    {
      lsum += A[i];
      rsum -= A[i];

      auto min = std::abs(lsum - rsum);

      if (current_min > min)
        current_min = min;
    }

    return current_min;
  }
} // namespace algo_equi_tape

TEST(AlgoEquilbrium, check_equi_tape)
{
  using namespace algo_equi_tape;

  {
    auto imps = {equi_tape_1, equi_tape_2};

    std::vector<int> coll{3, 2, 1, 4, 3};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(coll), 1);
    }
  }
}

/*
={=========================================================================
algo-water-volume

How much water between walls?
http://qandwhat.runkite.com/i-failed-a-twitter-interview/

Consider the following picture:
              _ _
7             7 7 _
6   _         # # 6
5   5         # # #
4   #       4 # # #
3 _ #     3 # # # #
2 2 #   2 # # # # #
1 # # 1 # # # # # #
  -----------------
  0 1 2 3 4 5 6 7 8

In this picture we have walls of different heights. This picture is
represented by an array of integers, where the value at each index is the
height of the wall. The picture above is represented with an array as
[2,5,1,2,3,4,7,7,6].

Now imagine it rains. How much water is going to be accumulated in puddles
between walls?

              _ _
7             7 7 _
6   _         # # 6
5   5 * * * * # # #
4   # * * * 4 # # #
3 _ # * * 3 # # # #
2 2 # * 2 # # # # #
1 # # 1 # # # # # #
  -----------------
  0 1 2 3 4 5 6 7 8

We count volume in square blocks of 1X1. So in the picture above, everything
to the left of index 1 spills out. Water to the right of index 7 also spills
out. We are left with a puddle between 1 and 6 and the volume is 10. 


The first thing I tried to do was to figure out how much water we would have
at any given index. This stroke a resemblance with Calculus and integrals, so
I immediately remembered that looking for local maximums could be of use. And
indeed, in the picture above, the water above index 2 is bounded by the
smaller of the two surrounding maximums at index 1 and 6. 

I was thinking out loud: "What if we found all the local maximums, and filled
in water between them. Would that work?"

"Yeah, that should work" replied Justin.

So I went ahead and coded this solution. Then Justin asked me for a bunch of
test cases which I provided. All the test cases we talked about seemed to
work.

"Do you have questions for me?" Justin asked. "How did I do?" "Reasonably
well. Your solution does 2 passes, but there is a more interesting one that
does only 1"


The flaw:

The second I hung up I realized my solution was wrong. Think about this input:

[2, 5, 1, 3, 1, 2, 1, 7, 7, 6]

My solution solved between the local maximums and looked like this: 

7               # # _
6   _           # # #
5   #           # # #
4   #           # # #
3 _ # * #       # # #
2 # # * # * # * # # #
1 # # 1 # # # # # # #
  --------------- ---
  0 1 2 3 4 5 6 7 7 8

But the result should have been one puddle between the two taller towers: 

7               # # _
6   _           # # #
5   # * * * * * # # #
4   # * * * * * # # #
3 _ # * # * * * # # #
2 # # * # * # * # # #
1 # # 1 # # # # # # #
  --------------- ---
  0 1 2 3 4 5 6 7 7 8

Now I ask myself: what have I learned from this? Realistically - not much. I
am upset that the interviewer didn't ask me the right questions to guide me
towards the right train of thought. I don't know why Justin told me "this
should work," when my solution in fact didn't. I know that this should have
come up in the test cases he asked for, but since I missed the flaw when
coming up with the algorithm, I didn't think of testing for it. 


2014.04.12. Initial thought by drawings is that

o only have waters when there is down-and-up, i.e., 5-1-2. 
o there are as many as down-and-ups in a array. 
o when goes down including the start from 0 index, set the hightest.
o when goes up, check with the previous heightest to see which is less than, 
  then there is water up to the less on hight.

This has the same flaw as the author's initial try has since only get the part.


two-pass solution

If we traverse the list from left to right, the amount of water at each index is
at most the largest value we have discovered so far. That means that 

"if we knew that there is something larger or equal to it somewhere on the
right" we would know exactly how much water we can hold without spilling. 

Same goes for traversing in the opposite direction: if we know we have found
something larger on the left than the largest thing on the right, we can safely
fill up water.

With this in mind, one solution would be to first find the absolute maximum
value, traverse from the left to the maximum, and then traverse from the right
to the maximum. This solution does 2 passes: one to find the maximum, and the
other is split into two subtraversals.

This approach is to find the max in the list and have two traversals to the
max. 

left ...              max ... right
sum towards to max -> *
                        <- sum towards to max

So first pass to find the max in the list and second pass to sum waters.


The solution in one pass (shown in the gist) avoids finding the maximum value
by moving two pointers from the opposite ends of the array towards each other.
If the largest value found to the left of the left pointer is smaller than the
largest value found to the right of the right pointer, then move the left
pointer one index to the right. Otherwise move the right pointer one index to
the left. Repeat until the two pointers intersect. This is a wordy
explanation, but the code is really simple.

class Ideone
{
  public static void main (String[] args) throws java.lang.Exception
  {
    int[] myIntArray = {2, 5, 1, 2, 3, 4, 7, 7, 6};
    System.out.println(calculateVolume(myIntArray));
  }

  public static int calculateVolume(int[] land) {
    int leftMax = 0;
    int rightMax = 0;
    int left = 0;
    int right = land.length - 1;
    int volume = 0;

    while(left < right) {

      // update current max for left and right.
      if(land[left] > leftMax) {
        leftMax = land[left];
      }
      if(land[right] > rightMax) {
        rightMax = land[right];
      }

      // decide to which direction it start to sum. Should from the lesser. If
      // equals, from right but do not matter when start from left.


      if(leftMax >= rightMax) {
        volume += rightMax - land[right];
        right--;
      } else {
        volume += leftMax - land[left];
        left++;
      }
    }

    return volume;
  }
}

*/

namespace algo_water_volume
{
  // two pass
  //
  // To contain water, the other side should be at least >= than the height of
  // the side. Having max pos and moving towards to that make sure that and
  // there is no need to worry about spilling water and simply do sum along the
  // way.
  //
  // One pass removes the step to get the max since while moving right and left
  // towards bigger one, the max remains at the end and do the same for one
  // pass.

  // old
  int water_volume_1(vector<int> const &A)
  {
    int max_value{};
    size_t max_index{};
    size_t volume{};

    // get the index of max
    for (size_t i = 0; i < A.size(); ++i)
    {
      if (max_value < A[i])
      {
        max_value = A[i];
        max_index = i;
      }
    }

    // from left to the max
    int max_current{std::numeric_limits<int>::min()};

    for (size_t i = 0; i < max_index; ++i)
    {
      if (max_current <= A[i])
        max_current = A[i];
      else
        volume += max_current - A[i];
    }

    // from right to the max
    max_current = std::numeric_limits<int>::min();

    for (size_t i = A.size() - 1; i > max_index; --i)
    {
      if (max_current <= A[i])
        max_current = A[i];
      else
        volume += max_current - A[i];
    }

    return volume;
  }

  // 2020.07
  int water_volume_2(std::vector<int> const &A)
  {
    // get the max in the input. the first max if there are multiples and not a
    // problem when getting sum of left and right traversal since when max ==
    // value in right traversal, do nothing.
    auto max_it    = std::max_element(A.cbegin(), A.cend());
    auto max_value = *max_it;
    auto max_pos   = std::distance(A.cbegin(), max_it);

    int max_current{};
    int sum_left{};
    int sum_right{};

    // left subtraversal
    max_current = 0;

    for (int left = 0; left < max_pos; ++left)
    {
      if (A[left] > max_current)
        max_current = A[left];
      else if ((A[left] < max_current) && (max_current <= max_value))
      {
        sum_left += (max_current - A[left]);
      }
    }

    // right subtraversal
    max_current = 0;

    for (int right = A.size() - 1; right > max_pos; --right)
    {
      if (A[right] > max_current)
        max_current = A[right];
      else if ((A[right] < max_current) && (max_current <= max_value))
      {
        sum_right += (max_current - A[right]);
      }
    }

    return sum_left + sum_right;
  }

  // one pass. old
  int water_volume_3(vector<int> const &A)
  {
    int right_max{std::numeric_limits<int>::min()};
    int left_max{std::numeric_limits<int>::min()};

    size_t left_index{0};
    size_t right_index{A.size() - 1};

    size_t volume{};

    while (left_index < right_index)
    {
      left_max  = max(left_max, A[left_index]);
      right_max = max(right_max, A[right_index]);

      // cout << "li: " << left_index << ", ri: " << right_index
      //   << ", lm: " << left_max << ", rm: " << right_max << endl;

      // move left bar to right
      // note: should have "equal" case on either if. otherwise, loops goes
      // infinitive.

      if (left_max <= right_max)
      {
        if (A[left_index] < left_max)
        {
          volume += left_max - A[left_index];
          // cout << "vol += " << left_max << " - "
          //   << A[left_index] << endl;
        }

        left_index++;
      }

      // move right bar to left
      if (left_max > right_max)
      {
        if (A[right_index] < right_max)
        {
          volume += right_max - A[right_index];
          // cout << "vol += " << right_max << " - "
          //   << A[right_index] << endl;
        }

        right_index--;
      }
    }

    return volume;
  }

  // one pass. old
  // simpler since there is no "if" checks on max() and getting sum since there
  // is no changes when ?max is same as A[?]

  int water_volume_4(vector<int> const &A)
  {
    size_t lindex{};
    size_t rindex{A.size() - 1};
    int lmax{};
    int rmax{};
    int volume{};

    while (lindex < rindex)
    {
      if (A[lindex] < A[rindex])
      {
        lmax = max(lmax, A[lindex]);
        volume += (lmax - A[lindex]);
        ++lindex;
      }
      else
      {
        rmax = max(rmax, A[rindex]);
        volume += (rmax - A[rindex]);
        --rindex;
      }
    }

    return volume;
  }

  // one pass. 2020.07
  int water_volume_5(std::vector<int> const &A)
  {
    int sum_left{};
    int sum_right{};

    size_t run_left{};
    size_t run_right{};

    int max_left{};
    int max_right{};

    for (run_left = 0, run_right = A.size() - 1; run_left < run_right;)
    {
      // when left is smaller, move left
      if (A[run_left] < A[run_right])
      {
        if (A[run_left] > max_left)
          max_left = A[run_left];
        else if (A[run_left] < max_left)
        {
          sum_left += (max_left - A[run_left]);
        }

        ++run_left;
      }

      // when right is smaller, move right to left.
      // NOTE: should have "=" on if to make for loop ends although do not
      // affect sum math. otherwise, the loof do not end.

      if (A[run_right] <= A[run_left])
      {
        if (A[run_right] > max_right)
          max_right = A[run_right];
        else if (A[run_right] < max_right)
        {
          sum_right += (max_right - A[run_right]);
        }

        --run_right;
      }
    }

    return sum_left + sum_right;
  }
} // namespace algo_water_volume

TEST(AlgoWaterVolume, check_water_volume)
{
  using namespace algo_water_volume;

  {
    auto imps = {water_volume_1,
                 water_volume_2,
                 water_volume_3,
                 water_volume_4,
                 water_volume_5};

    for (const auto &f : imps)
    {
      // 4+3+2+1 = 10
      {
        std::vector<int> coll{2, 5, 1, 2, 3, 4, 7, 7, 6};
        EXPECT_THAT(f(coll), 10);
      }

      // 4+3+2+1 = 10, when 5 is max and there are multiple 5
      {
        std::vector<int> coll{2, 5, 1, 2, 3, 4, 5, 5, 4};
        EXPECT_THAT(f(coll), 10);
      }

      {
        std::vector<int> coll{2, 5, 1, 3, 1, 2, 1, 7, 7, 6};
        EXPECT_THAT(f(coll), 17);
      }
      {
        std::vector<int>
          coll{2, 5, 4, 3, 4, 7, 6, 5, 4, 5, 7, 9, 5, 4, 3, 4, 5, 6};
        EXPECT_THAT(f(coll), 21);
      }
    }
  }
}

/*
={=========================================================================
algo-frog-jump

codility, frog jump

A small frog wants to get to the other side of the road. The frog is currently
located at position X and wants to get to a position greater than or equal to
Y. The small frog always jumps a fixed distance, D.

Count the minimal number of jumps that the small frog must perform to reach
its target.

Write a function:

int solution(int X, int Y, int D); 

that, given three integers X, Y and D, returns the minimal number of jumps
from position X to a position equal to or greater than Y.

For example, given:

  X = 10
  Y = 85
  D = 30

the function should return 3, because the frog will be positioned as follows:

  after the first jump, at position 10 + 30 = 40
  after the second jump, at position 10 + 30 + 30 = 70
  after the third jump, at position 10 + 30 + 30 + 30 = 100

Assume that:

X, Y and D are integers within the range [1..1,000,000,000];
X <= Y.

Complexity:

expected worst-case time complexity is O(1);
expected worst-case space complexity is O(1).

note: key conditions are: X and Y is > 0. X <= Y.

*/

namespace algo_frog_jump
{
  // brute force but this is not O(1) since loop may be long if Y  is big
  // enough.
  int frog_jump_1(int X, int Y, int D)
  {
    int count{};

    for (long long i = X; i < Y; i += D)
      ++count;

    return count;
  }

  // model
  int frog_jump_2(int X, int Y, int D)
  {
    int quotient  = (Y - X) / D;
    int remainder = (Y - X) % D;

    return remainder ? quotient + 1 : quotient;
  }

  // score: 100 of 100. Detected time complexity:O(1)
  //
  // X==Y : no jump
  // X<Y  : ----------------------
  //         X         Y   D
  //
  //    (Y-X)/D == 0. needs one jump.
  //    (Y-X)/D > 0. needs more jump.
  //       -----------------------
  //         X         Y
  //              D   D
  //       (Y-X)%D == 0. fall exactly on Y.
  //       (Y-X)%D != 0. +1 jump.
  //
  // Lesson learned. Read the question carefully such as 'greater or equal', 'X <=
  // Y', and O(1).

  // There are three cases:
  //
  //                   Y    Y   Y
  // -------------- | ----- | ----- | ----------------------
  //                       jumps == X + D*jump;

  // 2014.12
  int frog_jump_3(int X, int Y, int D)
  {
    if (X > Y || D == 0)
      return -1;

    int diff = (Y - X);
    int jump = diff / D;

    if ((diff % D) == 0)
      return jump;
    else
      return jump + 1;
  }

  // 2020.07. same as _2
  int frog_jump_4(int X, int Y, int D)
  {
    auto diff{Y - X};
    auto quotient{diff / D};
    auto remainder{diff % D};

    return quotient + (remainder ? 1 : 0);
  }
} // namespace algo_frog_jump

TEST(AlgoFrogJump, check_frog_jump)
{
  using namespace algo_frog_jump;

  {
    auto imps = {frog_jump_1, frog_jump_2, frog_jump_3, frog_jump_4};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(10, 85, 30), 3);
      EXPECT_THAT(f(10, 10, 30), 0);
    }
  }
}

/*
={=========================================================================
algo-minmax

{
  // bits/predefined_ops.h
  // this is a bit different from cxx-less but do the same.

  struct _Iter_less_iter
  {
    template<typename _Iterator1, typename _Iterator2>
      bool
      operator()(_Iterator1 __it1, _Iterator2 __it2) const
      { return *__it1 < *__it2; }
  };

  // bits/stl_algo.h

  **
   *  @brief  Return the maximum element in a range.
   *  @ingroup sorting_algorithms
   *  @param  __first  Start of range.
   *  @param  __last   End of range.
   *  @return  Iterator referencing the first instance of the largest value.

  template<typename _ForwardIterator>
    inline _ForwardIterator
    max_element(_ForwardIterator __first, _ForwardIterator __last)
    {
      return _GLIBCXX_STD_A::__max_element(__first, __last,
        __gnu_cxx::__ops::__iter_less_iter());
    }

  template<typename _ForwardIterator, typename _Compare>
    _ForwardIterator
    __max_element(_ForwardIterator __first, _ForwardIterator __last,
      _Compare __comp)
    {
      if (__first == __last) return __first;
      _ForwardIterator __result = __first;
      while (++__first != __last)
        if (__comp(__result, __first))
          __result = __first;
      return __result;
    }
}

*/

namespace algo_min_max
{
  bool AbsLess(int elem1, int elem2) { return abs(elem1) < abs(elem2); }
} // namespace algo_min_max

// copied from cxx-min-cxx-max
TEST(AlgoMinMax, check_stl_imps)
{
  using namespace algo_min_max;

  {
    std::deque<int> coll{2, 3, 4, 5, 6, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6};

    // NOTE: does it matter since we want max value but not index of max
    // element? yes if want to know the pos of it
    //
    // If more than one minimum or maximum element exists, min_element() and
    // max_element() return `the first` found; minmax_element() returns the first
    // minimum but the last maximum element, so max_element() and minmax_element()
    // don’t yield the same maximum element.

    EXPECT_THAT(*max_element(coll.begin(), coll.end()), 6);
    EXPECT_THAT(*min_element(coll.begin(), coll.end()), -3);

    // get the first
    EXPECT_THAT(distance(coll.begin(), max_element(coll.begin(), coll.end())),
                4);

    // return iterator pair
    // Note also that minmax_element() yields `the last maximum`, so the distance
    // 9.
    auto minmax = minmax_element(coll.begin(), coll.end());
    EXPECT_THAT(*(minmax.first), -3); // first minimum
    EXPECT_THAT(*(minmax.second), 6); // last maximum

    // last maximum is 6 which is the last element so minmax returns the last
    // max.
    EXPECT_THAT(distance(coll.begin(), minmax.second), coll.size() - 1);

    // see difference
    EXPECT_THAT(distance(minmax.first, minmax.second), 9);
    EXPECT_THAT(distance(min_element(coll.begin(), coll.end()),
                         max_element(coll.begin(), coll.end())),
                -1);

    // min/max of absolute values
    EXPECT_THAT(*min_element(coll.begin(), coll.end(), AbsLess), 0);
    EXPECT_THAT(*max_element(coll.begin(), coll.end(), AbsLess), 6);
  }
}

namespace algo_min_max
{
  struct my_less_1
  {
    template <typename _Iterator1, typename _Iterator2>
    bool operator()(_Iterator1 it1, _Iterator2 it2)
    {
      // see *cxx-pair-comparison*
      return *it1 < *it2;
    }
  };

  // use single iterator type
  struct my_less_2
  {
    template <typename _Iterator1>
    bool operator()(_Iterator1 it1, _Iterator1 it2)
    {
      // see *cxx-pair-comparison*
      return *it1 < *it2;
    }
  };

  // should make it template structure and if not, see:
  // error: ‘algopad::my_less_2’ is not a template
  //  template <typename _Iterator, typename _Compare = my_less_2<_Iterator>>

  template <typename _Iterator1>
  struct my_less_3
  {
    bool operator()(_Iterator1 it1, _Iterator1 it2)
    {
      // see *cxx-pair-comparison*
      return *it1 < *it2;
    }
  };

  // implement own min/max_element()
  // max_element(coll.begin(), coll.end();
  //
  // 1. how to define value_type of _Iterator? no need to define var for current
  // max value and comp function. All we need are iterators
  // 2. see comp is a copy

  template <typename _Iterator, typename _Compare>
  _Iterator my_max_element_1(_Iterator it1, _Iterator it2, _Compare comp)
  {
    // need to handle when there is only one element. otherwise, would access
    // to end which is error.
    if (it1 == it2)
      return it1;

    _Iterator result = it1;

    for (++it1; it1 != it2; ++it1)
    {
      // if "current < *it", that is get new max
      if (comp(result, it1))
        result = it1;
    }

    return result;
  }

  // "first and last" var naming seems better.
  template <typename _Iterator, typename _Compare>
  _Iterator
  my_max_element_2(_Iterator __first, _Iterator __last, _Compare __comp)
  {
    // if thre is only one
    if (__first == __last)
      return __first;

    _Iterator __result = __first;

    // if *__result < *__first
    while (++__first != __last)
      if (__comp(__result, __first))
        __result = __first;

    return __result;
  }

  // "first and last" var naming seems better. default comp
  template <typename _Iterator, typename _Compare = my_less_3<_Iterator>>
  _Iterator my_max_element_3(_Iterator __first,
                             _Iterator __last,
                             _Compare __comp = _Compare())
  {
    // if thre is only one
    if (__first == __last)
      return __first;

    _Iterator __result = __first;

    // if *__result < *__first
    while (++__first != __last)
      if (__comp(__result, __first))
        __result = __first;

    return __result;
  }

  template <typename _Iterator, typename _Compare>
  _Iterator
  my_min_element_2(_Iterator __first, _Iterator __last, _Compare __comp)
  {
    if (__first == __last)
      return __first;

    _Iterator __result = __first;

    while (++__first != __last)
      if (!__comp(__result, __first))
        __result = __first;

    return __result;
  }

  // same but "if (__comp(__first, __result))" which seems better
  template <typename _Iterator, typename _Compare>
  _Iterator
  my_min_element_3(_Iterator __first, _Iterator __last, _Compare __comp)
  {
    if (__first == __last)
      return __first;

    _Iterator __result = __first;

    while (++__first != __last)
      if (__comp(__first, __result))
        __result = __first;

    return __result;
  }

  // std-minmax
  // If more than one minimum or maximum element exists, min_element() and
  // max_element() return `the first` found;
  //
  // minmax_element() returns the first minimum but the last maximum element, so
  // max_element() and minmax_element() don’t yield the same maximum element.

  template <typename _Iterator>
  std::pair<_Iterator, _Iterator> my_minmax_element_1(_Iterator __first,
                                                      _Iterator __last)
  {
    if (__first == __last)
      return std::make_pair(__first, __last);

    _Iterator __max = __first;
    _Iterator __min = __first;

    while (++__first != __last)
    {
      // get max. add "=" to get the last max as std version
      if (*__max <= *__first)
        __max = __first;

      // get min
      if (*__first < *__min)
        __min = __first;
    }

    return std::make_pair(__min, __max);
  }
} // namespace algo_min_max

TEST(AlgoMinMax, check_own_imps)
{
  using namespace algo_min_max;

  // on std-map
  {
    // sorted by key
    std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

    auto e = std::max_element(counts.begin(), counts.end());
    EXPECT_THAT(*e, std::make_pair(13, 1));

    // ForwardIterator
    // max_element (ForwardIterator beg, ForwardIterator end, CompFunc op)
    // op is used to compare two elements:
    // op(elem1,elem2)
    // It should return true when the first element is less than the second
    // element.

    auto maxelem = std::max_element(
      std::begin(counts),
      std::end(counts),
      [](pair<int, size_t> const &e1, pair<int, size_t> const &e2) {
        return e1.second < e2.second;
      });

    EXPECT_THAT(*maxelem, std::make_pair(5, 3));
  }

  // multimap
  {
    // sorted by key and the order in the equal range are the order of input
    std::multimap<int, size_t> counts{{1, 2},
                                      {3, 9},
                                      {3, 8},
                                      {5, 3},
                                      {8, 3},
                                      {13, 2},
                                      {13, 4},
                                      {13, 12},
                                      {13, 1}};

    // for (auto &e : counts)
    //   cout << e.first << ", " << e.second << endl;

    // Q: how max_element() finds the max on the second?
    // see *cxx-pair-comparison*

    auto e = std::max_element(counts.begin(), counts.end());
    EXPECT_THAT(*e, std::make_pair(13, 12));
  }

  // max_element
  {
    // sorted by key
    std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

    {
      auto pos = my_max_element_1(counts.begin(), counts.end(), my_less_1());
      EXPECT_THAT(*pos, std::make_pair(13, 1));
    }

    {
      auto pos = my_max_element_2(counts.begin(), counts.end(), my_less_1());
      EXPECT_THAT(*pos, std::make_pair(13, 1));
    }
  }

  // max_element with default comp
  {
    // sorted by key
    std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

    {
      auto pos = my_max_element_3(counts.begin(), counts.end());
      EXPECT_THAT(*pos, std::make_pair(13, 1));
    }
  }

  // min_element
  {
    // sorted by key
    std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

    {
      auto pos = my_min_element_2(counts.begin(), counts.end(), my_less_1());
      EXPECT_THAT(*pos, std::make_pair(1, 2));
    }

    {
      auto pos = my_min_element_3(counts.begin(), counts.end(), my_less_1());
      EXPECT_THAT(*pos, std::make_pair(1, 2));
    }
  }

  // min_element and my_less_2
  {
    // sorted by key
    std::map<int, size_t> counts{{1, 2}, {3, 2}, {5, 3}, {8, 3}, {13, 1}};

    {
      auto pos = my_min_element_2(counts.begin(), counts.end(), my_less_2());
      EXPECT_THAT(*pos, std::make_pair(1, 2));
    }

    {
      auto pos = my_min_element_3(counts.begin(), counts.end(), my_less_2());
      EXPECT_THAT(*pos, std::make_pair(1, 2));
    }
  }

  {
    std::vector<int> coll{2, 3, 4, 5, 6, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6};

    // return iterator pair
    // Note also that minmax_element() yields `the last maximum`, so the distance
    // 9.
    auto minmax = my_minmax_element_1(coll.begin(), coll.end());
    EXPECT_THAT(*(minmax.first), -3);
    EXPECT_THAT(*(minmax.second), 6);
    EXPECT_THAT(std::distance(minmax.first, minmax.second), 9);
  }
}

/*
={=========================================================================
algo-find-missing

codility, find missing element. PermMissingElem

A zero-indexed array A consisting of N different integers is given. The array
contains integers in the range [1..(N + 1)], which means that exactly one
element is missing.

Your goal is to find that missing element.

Write a function:

int solution(int A[], int N);
int solution(const vector<int> &A);

that, given a zero-indexed array A, returns the value of the missing element.

For example, given array A such that:

  A[0] = 2
  A[1] = 3
  A[2] = 1
  A[3] = 5

the function should return 4, as it is the missing element.

Assume that:

N is an integer within the range [0..100,000]; the elements of A are all
distinct; each element of array A is an integer within the range [1..(N + 1)].

Complexity:

expected worst-case time complexity is O(N);
expected worst-case space complexity is O(1), beyond input storage (not
counting the storage required for input arguments).

Elements of input arrays can be modified.

fails when N=0.

score: 90 of 100
Detected time complexity: O(N)

empty list and single element 	0.020 s. 	WRONG ANSWER got 0 expected 1

This is about permutation. For example, {1,2,3,4,5} can have

{1,2,3,4} is missing 5
{2,3,4,5} is missing 1
{1,3,4,5} is missing 2

Reversely,
if N==3 inputs are given, then it's one of permutation of 4. [1,4]
if N==2 inputs are given, then it's one of permutation of 3. [1,3]
if N==1 inputs are given, then it's one of permutation of 2. [1,2]
if N==0 inputs are given, then it's one of permutation of 1. [1] so the
missing is always 1.


note on "different integers" so no duplicates on the input
 
*/

namespace algo_find_missing
{
  // removes one loop
  int find_missing_element_1(const std::vector<int> &A)
  {
    auto num = A.size();
    long long expected{}, input{};

    // use the fact that sum{1..N} is N(N+1)/2 and take cauiton on integer
    // division. so not n*((n+1)/2)
    {
      auto n   = A.size() + 1;
      expected = (n * (n + 1)) / 2;
    }

    for (size_t i = 0; i < num; ++i)
    {
      input += A[i];
    }

    return expected - input;
  }

  int find_missing_element_2(const std::vector<int> &A)
  {
    auto num = A.size();
    long long expected{}, input{};

    for (size_t i = 1; i <= num + 1; ++i)
    {
      expected += i;
    }

    for (size_t i = 0; i < num; ++i)
    {
      input += A[i];
    }

    return expected - input;
  }
} // namespace algo_find_missing

TEST(AlgoFindMissing, check_find_missing)
{
  using namespace algo_find_missing;

  {
    auto imps = {find_missing_element_1, find_missing_element_2};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f({2, 3, 1, 5}), 4);
      EXPECT_THAT(f({1, 2, 3, 4}), 5);
      EXPECT_THAT(f({2, 3, 4, 5}), 1);
      EXPECT_THAT(f({1, 3, 4, 5}), 2);
      EXPECT_THAT(f({}), 1);
    }
  }
}

/*
={=========================================================================
algo-find-missing-integer

TODO: DO NOT UNDERSTAND

Find the minimal positive integer not occurring in a given sequence.

Write a function:

int solution(int A[], int N); 

that, given a non-empty zero-indexed array A of N integers, returns the
  minimal positive integer that does not occur in A.

For example, given:

  A[0] = 1    
  A[1] = 3    
  A[2] = 6
  A[3] = 4    
  A[4] = 1    
  A[5] = 2

the function should return 5.

Assume that:

N is an integer within the range [1..100,000]; each element of array A is an
integer within the range [-2,147,483,648..2,147,483,647].

Complexity:

expected worst-case time complexity is O(N);
expected worst-case space complexity is O(N), beyond input storage (not
    counting the storage required for input arguments).

Elements of input arrays can be modified.

*/

namespace algo_find_missing_integer
{
  // 2018.06.27
  // 1. do not say about return value for errors
  // 2. Allocate lookup table for 2,147,xxx,xxx x 2? No and not meet O(n) space as
  // well.
  //
  // Since it's about `positive` minimal integer, inputs are 1..2,147,xxx,xxx.
  // Since N size array could have the max input value which is N.
  //
  // So can reduce input value range to 1...N
  //
  // What if input is not sequential such as "{100, 200, 300, 340}"? Not a valid
  // input since we are looking for `missing`, `not occurring` element. If
  // that's valid input then what are the missing element? So many and not a
  // valid input.
  //
  //
  // 2018.08.07
  // These assumptions seems wrong:
  //
  // Since it's about `positive` minimal integer, inputs are 1..2,147,xxx,xxx.
  // Since N size array could have the max input value which is N.
  //
  // So can reduce input value range to 1...N
  //
  // And what if input is {-2, -1, 0, 2, 3}?
  //
  // So not a good problem?
  //
  //
  // 2020.07
  // "So can reduce input value range to 1...N" is wrong. since think about
  // input {300,301,303}

  // this is based on 2018.06
  int find_missing_integer_1(const std::vector<int> &A)
  {
    // input check
    if (A.empty())
      return -1;

    // size is high and performance is high
    bool table[A.size()]{false};

    // set table on input
    for (auto e : A)
    {
      table[e] = true;
    }

    // assumes element values >= 1 so starts from 1
    for (size_t i = 1; i < A.size(); ++i)
    {
      if (table[i] == false)
        return i;
    }

    // when no missing integer found
    return -1;
  }

  // old. does it mean it actually passes? don't understand now.
  // O(N), 100%
  //
  // Use bool vector approach? The input element can be negative so ignore
  // negegative inputs.  However, the problem is input value can be too big to
  // have bool vector. how to solve?
  //
  // The key is whatever the input value is the aim to find the minimum positive
  // value which is missed. So have bool vector(N) and only consider inputs in 0 <
  // x <= N. Since even if there is no input in the specificed range then it
  // simply means that it misses the whole value of the range and need to get the
  // first false in the bool vector.
  //
  // If bool vector has all set then return N+1. ????
  int find_missing_integer_2(const vector<int> &A)
  {
    // input check
    if (A.empty())
      return -1;

    vector<bool> flags(A.size());

    for (unsigned int i = 0; i < A.size(); i++)
    {
      int value = A[i];

      if (value > 0 && value <= (int)A.size())
        flags[value - 1] = true;
    }

    for (unsigned int i = 0; i < flags.size(); i++)
      if (flags[i] == false)
        return i + 1;

    return A.size() + 1;
  }

  // what if input has {-1, 0, 1, 3}?

} // namespace algo_find_missing_integer

TEST(AlgoFindMissingInteger, check_find_missing_integer)
{
  using namespace algo_find_missing_integer;

  {
    auto imps = {find_missing_integer_1, find_missing_integer_2};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f({1, 3, 6, 4, 1, 2}), 5);
      EXPECT_THAT(f({}), -1);
    }
  }

  // wrong
  {
    auto f = find_missing_integer_2;
    EXPECT_THAT(f({300, 301, 303}), 1);
  }
}

/*
={=========================================================================
algo-find-perm

PermCheck https://codility.com/programmers/lessons/2

A non-empty zero-indexed array A consisting of N integers is given.

A permutation is a sequence containing 'each' element from 1 to N once, and
only once.

For example, array A such that:

    A[0] = 4
    A[1] = 1
    A[2] = 3
    A[3] = 2

is a permutation, but array A such that:

    A[0] = 4
    A[1] = 1
    A[2] = 3

is not a permutation.

The goal is to check whether array A is a permutation.

Write a function:

    int solution(int A[], int N); 

that, given a zero-indexed array A, returns 1 if array A is a permutation and
0 if it is not.

For example, given array A such that:

    A[0] = 4
    A[1] = 1
    A[2] = 3
    A[3] = 2

the function should return 1.

Given array A such that:

    A[0] = 4
    A[1] = 1
    A[2] = 3

the function should return 0.

Assume that:

N is an integer within the range [1..100,000]; each element of array A is an
integer within the range [1..1,000,000,000].

Complexity:

expected worst-case time complexity is O(N);
expected worst-case space complexity is O(N), beyond input storage (not
counting the storage required for input arguments).

Elements of input arrays can be modified.

note: there is no "duplicates" on the input so duplicates can happen.

*/

namespace algo_check_perm
{
  // similar to algo-find-missing
  //
  // actually it is:
  // bool check_perm_1(const std::vector<int> &A);

  // 2018.06.25
  //
  // fail since there are duplicates
  // EXPECT_THAT(f({4, 1, 3, 2, 1}), 0);
  // EXPECT_THAT(f({1, 4, 1}), 0);
  // fails
  // EXPECT_THAT(f({9,5,7,3,2,7,3,1,10,8}),0);
  int check_perm_1(const std::vector<int> &A)
  {
    //  int N = A.size();
    int total_sum{};
    int input_max{};

    for (auto e : A)
    {
      if (e > input_max)
        input_max = e;

      total_sum += e;
    }

    int perm_sum = (input_max * (input_max + 1)) / 2;

    // cout << "total: " << total_sum << ", perm: " << perm_sum << endl;

    return total_sum == perm_sum;
  }

  // based on old tries. N's permutation and it downs to algo-unique in the end
  // so if don't need to be defensive about input value, can return false as
  // soon as see duplicates.
  //
  // fails on:
  // extreme_min_max
  // single element with minimal/maximal value
  // large_range
  // sequence 1, 2, ..., N, N = ~100,000
  //
  // so input value should be in [1, A.size()]

  int check_perm_2(const vector<int> &A)
  {
    int count{};
    int input_max{};

    vector<bool> lookup(A.size() + 1);

    for (auto e : A)
    {
      if (e > input_max)
        input_max = e;

      if (lookup[e] == true)
        return false;
      else
      {
        lookup[e] = true;
        ++count;
      }
    }

    // cout << "input_max: " << input_max << ", perm: " << count << endl;

    return count == input_max;
  }

  // The count() in the return which is a loop can be removed as below since can
  // return 0 as soon as duplucates.
  //
  // 100% pass
  // Detected time complexity: O(N * log(N)) or O(N)

  int check_perm_3(const vector<int> &A)
  {
    if (!A.size())
      return 0;

    // default is false
    vector<bool> flag(A.size());

    for (unsigned int i = 0; i < A.size(); i++)
    {
      // note: -1 since permutation starts from 1 but index starts from 0
      // note: 'unsigned' to handle possible negative input which will be caught
      // below if statement.

      unsigned int value = A[i] - 1;

      // note: this covers values which are not in [1, N]
      if (value < A.size() && !flag[value])
        flag[value] = true;
      else
        return 0;
    }

    return 1;
  }

  // no duplicate
  // N, input max is A.size()
  // all values <= N, input max
  //
  // 100% pass
  // Detected time complexity: O(N * log(N)) or O(N)

  int check_perm_4(const vector<int> &A)
  {
    int count{};
    int input_max = A.size();

    vector<bool> lookup(input_max + 1);

    for (auto e : A)
    {
      if (e > input_max || lookup[e] == true)
        return false;
      else
      {
        lookup[e] = true;
        ++count;
      }
    }

    return count == input_max;
  }
} // namespace algo_check_perm

TEST(AlgoCheckPerm, check_permutation)
{
  using namespace algo_check_perm;

  {
    auto imps = {check_perm_4};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f({4, 1, 3, 2, 1}), 0);
      EXPECT_THAT(f({1, 4, 1}), 0);
      EXPECT_THAT(f({9, 5, 7, 3, 2, 7, 3, 1, 10, 8}), 0);
    }
  }
}

/*
={=========================================================================
algo-frog-river

FrogRiverOne 

A small frog wants to get to the other side of a river. The frog is currently
located at position 0, and wants to get to position X. Leaves fall from a tree
onto the surface of the river.

You are given a non-empty zero-indexed array A consisting of N integers
representing the falling leaves. A[K] represents the position where one leaf
falls at time K, measured in minutes.

The goal is to find the earliest time when the frog can jump to the other side
of the river. The frog can cross only when leaves appear at every position
across the river from 1 to X.

For example, you are given integer X = 5 and array A such that:

  A[0] = 1
  A[1] = 3
  A[2] = 1
  A[3] = 4
  A[4] = 2
  A[5] = 3
  A[6] = 5
  A[7] = 4

In minute 6, a leaf falls into position 5. This is the earliest time when
leaves appear in every position across the river.

Write a function:

    int solution(int X, int A[], int N); 

that, given a non-empty zero-indexed array A consisting of N integers and
  integer X, returns the earliest time when the frog can jump to the other
  side of the river.

If the frog is never able to jump to the other side of the river, the function
should return -1.

For example, given X = 5 and array A such that:

  A[0] = 1
  A[1] = 3
  A[2] = 1
  A[3] = 4
  A[4] = 2
  A[5] = 3
  A[6] = 5
  A[7] = 4

the function should return 6, as explained above. Assume that:

N and X are integers within the range [1..100,000];
each element of array A is an integer within the range [1..X].

note: suggest that all input are < N

Complexity:

expected worst-case time complexity is O(N);
expected worst-case space complexity is O(X), beyond input storage (not
    counting the storage required for input arguments).

Elements of input arrays can be modified.

*/

namespace algo_frog_river
{
  // old
  // good on using sum on X and input. can use single loop
  // but space O(A+1) but not O(X)
  int check_frog_river_1(int X, const std::vector<int> &A)
  {
    // input check
    if (A.empty())
      return -1;

    std::vector<bool> lookup(A.size() + 1);

    int target_sum = (X * (X + 1)) / 2;
    int input_sum{};

    for (size_t i = 0; i < A.size(); ++i)
    {
      // see if check
      if (A[i] <= X && lookup[A[i]] == false)
      {
        lookup[A[i]] = true;
        input_sum += A[i];

        if (target_sum == input_sum)
          return i;
      }
    }

    return -1;
  }

  // NOTE: BEST
  // make space O(X+1) due to "if check" and can make O(X)
  int check_frog_river_2(int X, const std::vector<int> &A)
  {
    // input check
    if (A.empty())
      return -1;

    std::vector<bool> lookup(X + 1);

    int target_sum = (X * (X + 1)) / 2;
    int input_sum{};

    for (size_t i = 0; i < A.size(); ++i)
    {
      if (A[i] <= X && lookup[A[i]] == false)
      {
        lookup[A[i]] = true;
        input_sum += A[i];

        if (target_sum == input_sum)
          return i;
      }
    }

    return -1;
  }

  // NOTE: BEST
  // Detected time complexity: O(N)
  // The key idea is that it is to use counting and to use counter to check if
  // receives all inputs, stones, to jump
  int check_frog_river_3(int X, const std::vector<int> &A)
  {
    // write your code in C++11
    if (A.empty() || !X)
      return -1;

    std::vector<bool> flags(X);
    int count = 0;

    for (unsigned int i = 0; i < A.size(); i++)
    {
      int value = A[i] - 1;

      if (value < X && flags[value] == false)
      {
        flags[value] = true;
        count++;
      }

      if (count == X)
        return i;
    }

    return -1;
  }

  // 2020.07
  //
  // each element of array A is an integer within the range [1..X].
  //
  // to pass: EXPECT_THAT(f(1, {1}), 0);
  // xidx = 0 is valid so have xidx with init value -1.
  //
  // space O(X+1) and can make O(X)

  int check_frog_river_4(int X, const std::vector<int> &A)
  {
    // when input is empty
    if (!A.size())
      return -1;

    int xidx{-1};
    bool table[X + 1]{false};

    // get the index of X and set up the lookup table
    for (size_t i = 0; i < A.size(); ++i)
    {
      if (A[i] == X)
        xidx = i;

      table[A[i]] = true;
    }

    // not found so not able to jump
    if (xidx == -1)
      return -1;

    // check if there are missing up to xidx; index of X
    for (int i = 1; i <= X; ++i)
      if (table[i] == false)
        return -1;

    return xidx;
  }
} // namespace algo_frog_river

TEST(AlgoFrogRiver, check_frog_river)
{
  using namespace algo_frog_river;

  {
    auto imps = {check_frog_river_1,
                 check_frog_river_2,
                 check_frog_river_3,
                 check_frog_river_4};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(5, {1, 3, 1, 4, 2, 3, 5, 4}), 6);
      EXPECT_THAT(f(1, {2, 3, 4, 5, 1, 3, 5, 4}), 4);
      EXPECT_THAT(f(5, {}), -1);
      EXPECT_THAT(f(5, {1}), -1);
      EXPECT_THAT(f(1, {2}), -1);
      EXPECT_THAT(f(1, {1}), 0);
    }
  }
}

/*
={=========================================================================
algo-max-counters

Calculate the values of counters after applying all alternating operations:
increase counter by 1; set value of all counters to current maximum.

You are given N counters, initially set to 0, and you have two possible
operations on them:

increase(X) − counter X is increased by 1,
max_counter − all counters are set to the maximum value of any counter.

A non-empty zero-indexed array A of M integers is given. This array represents
consecutive operations:

if A[K] = X, such that 1 ≤ X ≤ N, then operation K is increase(X),
if A[K] = N + 1 then operation K is max_counter.

For example, given integer N = 5 and array A such that:

    A[0] = 3
    A[1] = 4
    A[2] = 4
    A[3] = 6
    A[4] = 1
    A[5] = 4
    A[6] = 4

the values of the counters after each consecutive operation will be:
     1  2  3  4  5    counter #, X.
    (0, 0, 1, 0, 0)
    (0, 0, 1, 1, 0)
    (0, 0, 1, 2, 0)
    (2, 2, 2, 2, 2)
    (3, 2, 2, 2, 2)
    (3, 2, 2, 3, 2)
    (3, 2, 2, 4, 2)

The goal is to calculate the value of every counter after all operations.

Assume that the following declarations are given:

    struct Results {
      int * C;
      int L;
    }; 

Write a function:

struct Results solution(int N, int A[], int M); 

that, given an integer N and a non-empty zero-indexed array A consisting of M
  integers, returns a sequence of integers representing the values of the
  counters.

The sequence should be returned as:

a structure Results (in C), or
a vector of integers (in C++), or
a record Results (in Pascal), or
an array of integers (in any other programming language).

For example, given:

    A[0] = 3
    A[1] = 4
    A[2] = 4
    A[3] = 6
    A[4] = 1
    A[5] = 4
    A[6] = 4

the function should return [3, 2, 2, 4, 2], as explained above.

Assume that:

N and M are integers within the range [1..100,000];
each element of array A is an integer within the range [1..N + 1].

Complexity:

expected worst-case time complexity is O(N+M);
expected worst-case space complexity is O(N), beyond input storage (not counting
the storage required for input arguments).

Elements of input arrays can be modified.

*/

namespace algo_find_max_counters
{

  // The result is that 100% correctness and 40% performance.
  //
  // Therefore, can see that the problem is the max-all operation and as a worst
  // case, when there are N max-all operations this will be O(N*M) but target is
  // O(M+N). So the key is to find a way to have max-all effect without doing a
  // loop. How?
  std::vector<int> find_max_counters_1(int N, std::vector<int> A)
  {
    vector<int> counters(N, 0);

    int current_max = 0;

    for (size_t i = 0; i < A.size(); i++)
    {
      // set current max to all
      if (A[i] >= N + 1)
      {
        // find the max
        for (size_t j = 0; j < counters.size(); j++)
          if (counters[j] > current_max)
            current_max = counters[j];

        // do max-all
        for (size_t j = 0; j < counters.size(); j++)
          counters[j] = current_max;
      }
      // increment a counter
      else
        counters[A[i] - 1] += 1;
    }

    return counters;
  }

  // 2018.06.27. little better
  std::vector<int> find_max_counters_2(int N, vector<int> A)
  {
    vector<int> result(N, 0);
    int max{};

    for (size_t i = 0; i < A.size(); ++i)
    {
      if (A[i] == N + 1)
      {
        // fill_n(result, N, max);
        for (auto &e : result)
          e = max;
      }
      else if (A[i] >= 1 && A[i] <= N)
      {
        if (++result[A[i] - 1] > max)
          max = result[A[i] - 1];
      }
    }

    return result;
  }

  // To remove "max-all", use max-all value than actually updating of array.
  //
  // {3,4,4,6,1,4,4}
  //
  // 0   0 0 0 0
  // 0   0 1 0 0
  // 0   0 1 1 0
  // 0   0 1 2 0
  // -   - - - - take a snapshot of current max-all value
  // 2+1 0 1 2 0
  // 3   0 1 3 0
  // 3   0 1 4 0
  // then do lopp and update elements that are less than snapshot value so that
  // avoid run loop every max op.
  //
  // same as solution from online.
  // http://codility-lessons.blogspot.co.uk/2014/07/lesson-2maxcounters.html
  //
  //     (0, 0, 1, 0, 0)        (0, 0, 1, 0, 0)
  //     (0, 0, 1, 1, 0)        (0, 0, 1, 1, 0)
  //     (0, 0, 1, 2, 0)        (0, 0, 1, 2, 0)
  //     (2, 2, 2, 2, 2)        (-, -, -, -, -)   max=2, maxLastMaxOp = 2
  //     (3, 2, 2, 2, 2)        (3, 0, 1, 2, 0)
  //     (3, 2, 2, 3, 2)        (3, 0, 1, 3, 0)
  //     (3, 2, 2, 4, 2)        (3, 0, 1, 4, 0)
  //                            (3, 2, 2, 4, 2)   set maxLastMaxOp to all which are not increased since
  //                            last max-all operation.
  //
  // This approach use flags to know which is increased since the last max-all
  // operation and set maxLastMaxOp to all which are not increased since the
  // last max-all.
  //
  // The key 'observation' is that max-all sets the 'base' for following
  // increase operations.

  vector<int> find_max_counters_3(int N, vector<int> A)
  {
    vector<int> result(N, 0);
    int current_max{}, operation_max{};

    for (size_t i = 0; i < A.size(); ++i)
    {
      if (A[i] == N + 1)
      {
        operation_max = current_max;
      }
      else if (A[i] >= 1 && A[i] <= N)
      {
        if (result[A[i] - 1] < operation_max)
          result[A[i] - 1] = operation_max + 1;
        else
          result[A[i] - 1] += 1;

        if (result[A[i] - 1] > current_max)
          current_max = result[A[i] - 1];
      }
    }

    for (auto &e : result)
    {
      if (e < operation_max)
        e = operation_max;
    }

    return result;
  }

  // This approach removes the use of flags. As with the above observation,
  // max-all set the base that means any following increase should be based on
  // 'hidden' base. So if result[op] < maxLastMaxOp then result[op] =
  // maxLastMaxOp+1. Once done a loop, handle all which are not increased since
  // the last max-all by checking less than maxLastMaxOp.
  //
  // Verified 100% peformance mark.

  vector<int> find_max_counters_4(int N, vector<int> A)
  {
    // write your code in C++11
    vector<int> result(N, 0);

    int maxLast = 0, maxCurrent = 0;

    for (unsigned int i = 0; i < A.size(); i++)
    {
      int op = A[i];

      if (op == N + 1) // max-all op
        maxLast = maxCurrent;
      else // inc op
      {
        op--;

        if (result[op] < maxLast)
          result[op] = maxLast + 1;
        else
          result[op]++;

        if (result[op] > maxCurrent)
          maxCurrent = result[op];
      }
    }

    for (int i = 0; i < N; i++)
      if (result[i] < maxLast)
        result[i] = maxLast;

    return result;
  }

  // NOTE: _3 and _4 are the same and

} // namespace algo_find_max_counters

TEST(AlgoMaxCounters, check_find_max_counters)
{
  using namespace algo_find_max_counters;

  {
    auto imps = {find_max_counters_1,
                 find_max_counters_2,
                 find_max_counters_3,
                 find_max_counters_4};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(5, {3, 4, 4, 6, 1, 4, 4}), ElementsAre(3, 2, 2, 4, 2));
      EXPECT_THAT(f(5, {3, 4, 4, 6, 1, 4, 6}), ElementsAre(3, 3, 3, 3, 3));
      EXPECT_THAT(f(5, {3, 6, 6, 6, 6, 6, 6}), ElementsAre(1, 1, 1, 1, 1));
    }
  }
}

/*
={=========================================================================
algo-perfix-sum

Prefix sums

There is a simple yet powerful technique that allows for the fast calculation of
`sums of elements` in given slice (`contiguous` segments of array). Its main
idea uses prefix sums which are defined as the consecutive totals of the first
0, 1, 2, . . . , n elements of an array.

        a0            a1              a2                ...   an−1
p0 = 0  p1 = a0       p2 = a0 + a1    p3 = a0 + a1 + a2 ...   pn = a0 + a1 + ... + an−1
p0 = 0  p1 = p0 + a0  p2 = p1 + a1    p3 = p2 + a2      ...   pn = pn-1 + an−1

or
        a0            a1              a2                ...   an−1
        p0 = a0       p1 = p0 + a1    p2 = p1 + a2      ...   pn-1 = a0 + a1 + ... + an−1

We can easily calculate the prefix sums in O(n) time complexity. Notice that the
total pk equals pk−1 + ak−1, so each consecutive value can be calculated in a
constant time.


5.1: Counting prefix sums — O(n).
1 def prefix_sums(A):
2 n = len(A)
3 P = [0] * (n + 1)
4 for k in xrange(1, n + 1):
5 P[k] = P[k - 1] + A[k - 1]
6 return P

Similarly, we can calculate suffix sums, which are the totals of the k last
values. 

*/

namespace prefix_sum
{
  // build prefix sum, O(n)
  //   (1, 2, 3, 4, 5, 6)
  // (0,1, 3, 6,10,15,21)
  //
  //         a0            a1              a2                ...   an−1
  // p0 = 0  p1 = a0       p2 = a0 + a1    p3 = a0 + a1 + a2 ...   pn = a0 + a1 + ... + an−1
  // p0 = 0  p1 = p0 + a0  p2 = p1 + a1    p3 = p2 + a2      ...   pn = pn-1 + an−1

  std::vector<int> build_prefix_sum_1(const vector<int> &coll)
  {
    int size = coll.size() + 1;

    std::vector<int> sum(size);

    for (int i = 1; i < size; ++i)
    {
      sum[i] = sum[i - 1] + coll[i - 1];
    }

    return sum;
  }

  // no +1 and use the same size since p0 is always 0.
  std::vector<int> build_prefix_sum_2(const vector<int> &coll)
  {
    int size = coll.size();

    std::vector<int> sum{coll};

    for (int i = 1; i < size; ++i)
    {
      sum[i] += sum[i - 1];
    }

    return sum;
  }

  // int find_max_sub_array_1(std::vector<int> &nums) {}
} // namespace prefix_sum

TEST(AlgoPrefixSum, check_prefix_sum)
{
  using namespace prefix_sum;

  {
    EXPECT_THAT(build_prefix_sum_1({1, 2, 3, 4, 5, 6}),
                ElementsAre(0, 1, 3, 6, 10, 15, 21));

    EXPECT_THAT(build_prefix_sum_1({2, 3, 7, 5, 1, 3, 9}),
                ElementsAre(0, 2, 5, 12, 17, 18, 21, 30));
  }

  {
    EXPECT_THAT(build_prefix_sum_2({1, 2, 3, 4, 5, 6}),
                ElementsAre(1, 3, 6, 10, 15, 21));

    EXPECT_THAT(build_prefix_sum_2({2, 3, 7, 5, 1, 3, 9}),
                ElementsAre(2, 5, 12, 17, 18, 21, 30));
  }
}

/*

Using prefix (or suffix) sums allows us to calculate the total of any slice of
the array very quickly. For example, assume that you are asked about the totals
of m slices [x..y] such that 0 <= x <= y < n, where the total is the sum 
ax + ax+1 + . . . + ay−1 + ay.

The simplest approach is to iterate through the whole array for each result
separately; however, that requires O(nm) time. 

The better approach is to use prefix sums. If we calculate the prefix sums then
we can answer each question directly in constant time. Let’s subtract px from
the value py+1.

py+1          a0 a1 . . . ax−1  ax ax+1 ... ay−1 ay ay+1
px            a0 a1 . . . ax−1
py+1 - px                       ax ax+1 ... ay−1 ay


5.2: Total of one slice — O(1).
1 def count_total(P, x, y):
2 return P[y + 1] - P[x]

slice[2,4]

 | 0 | 1 | 2 | | 3 | 4 | 5 |
 |------->
 |--------------------->

*/

namespace prefix_sum
{
  //    0  1  2  3  4  5
  // p0 1  2  3  4  5  6
  //   {1, 2, 3, 4, 5, 6}
  // (0,1, 3,[6,10,15],21)
  //
  // 15-3 = 12

  int get_slice_sum(const vector<int> &sum, int first, int last)
  {
    return sum[last + 1] - sum[first];
  }
} // namespace prefix_sum

TEST(AlgoPrefixSum, check_sum_slice)
{
  using namespace prefix_sum;

  EXPECT_THAT(get_slice_sum(build_prefix_sum_1({1, 2, 3, 4, 5, 6}), 2, 4), 12);

  // (0,2,5,[12,17,18],21,30)
  // 18-5 = 13

  EXPECT_THAT(get_slice_sum(build_prefix_sum_1({2, 3, 7, 5, 1, 3, 9}), 2, 4),
              13);
}

/*
5.1. Exercise

Problem: 

You are given a non-empty, zero-indexed array A of n (1 <= n <= 100,000)
integers a0, a1, ..., an−1 (0 <= ai <= 1 000). 

This array represents number of mushrooms growing on the consecutive spots along
a road. You are also given integers k and m (0 <= k, m < n). A mushroom picker
is at spot number k on the road and should perform m moves. In one move she
moves to an adjacent spot. She collects all the mushrooms growing on spots she
visits. 

The goal is to calculate the maximum number of mushrooms that the mushroom
picker can collect in m moves.

For example, consider array A such that:

{2 3[7 5 1 3 9]}
 0 1 2 3 4 5 6

The mushroom picker starts at spot k = 4 and should perform m = 6 moves. She
might move to spots 3, 2, 3, 4, 5, 6 and thereby collect.  

value(index)

1(4) + 5(3) + 7(2) + 0(3) + 0(4) + 3(5) + 9(6) = 25 mushrooms. 

This is the maximal number of mushrooms she can collect.

note:
do not count mushroom that's already counted.

def mushrooms(A, k, m):

Solution O(m2): 

the best strategy is to move in one direction optionally followed by some moves
in the opposite direction. In other words, the mushroom picker should not change
direction more than once. With this observation we can find the simplest
solution.  Make the first p = 0, 1, 2, . . . ,m moves in one direction, then the
next m - p moves in the opposite direction. This is just a simple simulation of
the moves of the mushroom picker which requires O(m2) time.


Solution O(n+m): 

A better approach is to use prefix sums. If we make p moves in one direction, we
can calculate the maximal opposite location of the mushroom picker. The mushroom
picker collects all mushrooms between these extremes. We can calculate the total
number of collected mushrooms in constant time by using prefix sums.


That is:

To get [first, last] which is common for above solutions:

Make the first p = 0, 1, 2, . . . ,m moves in one direction, then the next m - p
moves in the opposite direction.

Then use prefix sum for each slice and get the max

*/

namespace prefix_sum
{
  // old
  // start: 4, moves: 6
  // loop(0, 5), left:  4, shift:  6, right: 10 -> 10 ->  6, xresult: 13, result: 13
  // loop(1, 5), left:  3, shift:  4, right:  8 ->  8 ->  6, xresult: 18, result: 18
  // loop(2, 5), left:  2, shift:  2, right:  6 ->  6 ->  6, xresult: 25, result: 25
  // loop(3, 5), left:  1, shift:  0, right:  4 ->  4 ->  4, xresult: 16, result: 25
  // loop(4, 5), left:  0, shift: -2, right:  2 ->  4 ->  4, xresult: 18, result: 25
  //
  // when move window to the left
  //          0    1   2   3   4   5   6   7   8   9   10
  //          2    3   7   5  [1   3   9   X   X   X   X]
  //          2    3   7  [5   1   3   9   X   X]  X   X
  //          2    3  [7   5   1   3   9]  X   X   X   X
  //          2   [3   7   5   1]  3   9   X   X   X   X
  //         [2    3   7   5   1]  3   9   X   X   X   X
  //
  // when move window to the right
  // [X   X   2    3   7   5   1]  3   9   X   X   X   X
  //     [X   2    3   7   5   1   3]  9   X   X   X   X
  //          2    3  [7   5   1   3   9]  X   X   X   X
  //
  // when used a bug
  // loop(0, 2), right:  4, shift:  6, left: -2 -> -2 ->  0, xresult: 18, result: 25
  // loop(1, 2), right:  5, shift:  4, left:  0 ->  0 ->  0, xresult: 21, result: 25
  //
  // start: 4, moves: 6
  // loop(0, 3), shift:  6, left: -2 -> -2 ->  0, right:  4, xresult: 18, result: 25
  // loop(1, 3), shift:  4, left:  0 ->  0 ->  0, right:  5, xresult: 21, result: 25
  // loop(2, 3), shift:  2, left:  2 ->  2 ->  2, right:  6, xresult: 25, result: 25
  //
  // from this observation, when shift to left to the start, right end get reduced
  // by 2 since uses "move" twice when goes to left and right again.
  //
  // After all, get possible max mushroom and moves windows which starts from
  // start pos and ends with start pos.
  //
  // start: 8, moves: 5
  // loop(0, 6), left:  8, shift:  5, right: 13 -> 13 -> 13, xresult: 21, result: 21
  // loop(1, 6), left:  7, shift:  3, right: 11 -> 11 -> 11, xresult: 27, result: 27
  // loop(2, 6), left:  6, shift:  1, right:  9 ->  9 ->  9, xresult: 23, result: 27
  // loop(3, 6), left:  5, shift: -1, right:  7 ->  8 ->  8, xresult: 26, result: 27
  // loop(4, 6), left:  4, shift: -3, right:  5 ->  8 ->  8, xresult: 30, result: 30
  // loop(5, 6), left:  3, shift: -5, right:  3 ->  8 ->  8, xresult: 32, result: 32
  //
  //   0   1   2  3  4  5  6   7 *8* 9 10 11 12 13 14
  //  13  12  11  2  4  6  8  10 [2  3  7  5  1  3] 9     (21)
  //  13  12  11  2  4  6  8 [10  2  3  7  5] 1  3  9     (27)
  //  13  12  11  2  4  6 [8  10  2  3] 7  5  1  3  9     (23)
  //  13  12  11  2  4 [6  8  10  2] 3  7  5  1  3  9     (26)
  //  13  12  11  2 [4  6  8  10  2] 3  7  5  1  3  9     (30)
  //  13  12  11 [2  4  6  8  10  2] 3  7  5  1  3  9     (32)
  //  ...
  //
  // start: 8, moves: 5
  // loop(0, 6), shift:  5, left:  3 ->  3 ->  3, right:  8, xresult: 32, result: 32
  // loop(1, 6), shift:  3, left:  5 ->  5 ->  5, right:  9, xresult: 29, result: 32
  // loop(2, 6), shift:  1, left:  7 ->  7 ->  7, right: 10, xresult: 22, result: 32
  // loop(3, 6), shift: -1, left:  9 ->  8 ->  8, right: 11, xresult: 17, result: 32
  // loop(4, 6), shift: -3, left: 11 ->  8 ->  8, right: 12, xresult: 18, result: 32
  // loop(5, 6), shift: -5, left: 13 ->  8 ->  8, right: 13, xresult: 21, result: 32
  int mushroom_picker_1(const vector<int> &A, int start, int moves)
  {
    int max_input_index = A.size() - 1;
    int result{};
    auto prefix_sum = build_prefix_sum_1(A);
    int loop_max{};

    // cout << "start: " << start << ", moves: " << moves << endl;
    loop_max = min(start, moves) + 1;
    for (int prefix_index = 0; prefix_index < loop_max; ++prefix_index)
    {
      int left_pos           = start - prefix_index;
      int shift_value        = (moves - 2 * prefix_index);
      int possible_right_pos = start + shift_value;

      // max? since right end cannot be less than start
      int max_on_possible_right_pos = max(start, possible_right_pos);

      // min? since right end cannot be greater than max index of input.
      int right_pos = min(max_input_index, max_on_possible_right_pos);

      // collect mushroon only once? count_total use range and counts only once.
      auto xresult = get_slice_sum(prefix_sum, left_pos, right_pos);
      result       = max(result, xresult);

      // cout << "loop(" << prefix_index << ", " << loop_max << ")"
      //   << ", left: " << setw(2) << left_pos
      //   << ", shift: " << setw(2) << shift_value
      //   << ", right: " << setw(2) << possible_right_pos << " -> "
      //   << setw(2) << max_on_possible_right_pos << " -> " << setw(2) << right_pos
      //   << ", xresult: " << xresult << ", result: " << result << endl;
    }

    // cout << "start: " << start << ", moves: " << moves << endl;

    // from codility code but looks wrong.
    // loop_max = min(moves+1, max_input_index-start);
    loop_max = min(moves, max_input_index - start) + 1;
    for (int prefix_index = 0; prefix_index < loop_max; ++prefix_index)
    {
      int right_pos = start + prefix_index;
      // left_pos = max(0, min(K, K-(M-2*prefix_index)));
      int shift_value       = (moves - 2 * prefix_index);
      int possible_left_pos = start - shift_value;

      // min? left end should be less than the start
      int min_on_possible_left_pos = min(start, possible_left_pos);

      // max? cannot not less than 0
      int left_pos = max(0, min_on_possible_left_pos);

      auto xresult = get_slice_sum(prefix_sum, left_pos, right_pos);
      result       = max(result, xresult);

      // cout << "loop(" << prefix_index << ", " << loop_max << ")"
      //   << ", shift: " << setw(2) << shift_value
      //   << ", left: " << setw(2) << possible_left_pos << " -> "
      //   << setw(2) << min_on_possible_left_pos << " -> " << setw(2) << left_pos
      //   << ", right: " << setw(2) << right_pos
      //   << ", xresult: " << xresult << ", result: " << result << endl;
    }

    return result;
  }

  // old
  int mushroom_picker_2(const vector<int> &A, int start, int moves)
  {
    int max_input_index = A.size() - 1;
    int num_loop{};
    int result{};

    auto prefix_sum = build_prefix_sum_1(A);

    // moves a window to the left
    // since `start` is actually index, it's sure to have elements in [0,start] so
    // use it directly
    num_loop = min(start, moves) + 1;
    for (int i = 0; i < num_loop; ++i)
    {
      int left_end    = start - i;
      int shift_value = (moves - 2 * i);
      int right_end   = start + shift_value;

      // right_end should be in [start, max input index]
      int right_contained      = min(max_input_index, right_end);
      int right_end_calculated = max(start, right_contained);

      int prefix_sum_result =
        get_slice_sum(prefix_sum, left_end, right_end_calculated);
      result = max(prefix_sum_result, result);
    }

    // moves a windows to the right
    // unlike `to the left` case, cannot use start and have to use max input
    // index.
    num_loop = min(moves, max_input_index - start) + 1;
    for (int i = 0; i < num_loop; ++i)
    {
      int right_end   = start + i;
      int shift_value = (moves - 2 * i);
      int left_end    = start - shift_value;

      // left_end should be in [0, start]
      int left_end_contained  = max(0, left_end);
      int left_end_calculated = min(start, left_end_contained);

      int prefix_sum_result =
        get_slice_sum(prefix_sum, left_end_calculated, right_end);
      result = max(prefix_sum_result, result);
    }

    return result;
  }

  // last is a last index of array. start, last, max_move are fixed.
  // this is key to get sub array, slice, [] which can get within max move from
  // start.
  std::pair<int, int>
  get_slice_index(int last, int start, int max_move, int current_move)
  {
    auto slice_last = std::min(start + current_move, last);

    // since moves right and moves left back so current_move uses twice.
    auto slice_first = std::max(start - (max_move - current_move * 2), 0);

    return {slice_first, slice_last};
  }

  // 2020.08
  //
  // slice {0, 4}
  // slice {0, 5}
  // slice {2, 6}
  // slice {3, 8}
  // slice {5, 9}
  // slice {7, 10}
  // slice {9, 11}
  // slice {11, 12}

  int mushroom_picker_3(const std::vector<int> &coll, int start, int moves)
  {
    int result{std::numeric_limits<int>::min()};
    int last = coll.size() - 1;

    // get prefix sum
    auto sum = build_prefix_sum_1(coll);

    for (int i = start, count = 0; count < moves && i < (int)coll.size();
         ++i, ++count)
    {
      // `start` is fixed
      auto slice = get_slice_index(last, start, moves, count);

      // cout << "slice {" << slice.first << ", " << slice.second << "}\n";

      auto slice_sum = get_slice_sum(sum, slice.first, slice.second);

      if (slice_sum > result)
        result = slice_sum;
    }

    return result;
  }
} // namespace prefix_sum

TEST(AlgoPrefixSum, check_mushroom_picker)
{
  using namespace prefix_sum;

  // auto imps = {mushroom_picker_1, mushroom_picker_2, mushroom_picker_3};
  auto imps = {mushroom_picker_3};

  for (const auto &f : imps)
  {
    EXPECT_THAT(f({2, 3, 7, 5, 1, 3, 9}, 4, 6), 25);

    EXPECT_THAT(f({13, 12, 11, 2, 4, 6, 8, 10, 2, 3, 7, 5, 1, 3, 9}, 8, 5), 32);
  }
}

/*
algo-leetcode-53

53. Maximum Subarray, Easy, algo-max-sub-array

Given an integer array nums, find the contiguous subarray (containing at least
one number) which has the largest sum and return its sum.

Example:

Input: [-2,1,-3,4,-1,2,1,-5,4],
Output: 6
Explanation: [4,-1,2,1] has the largest sum = 6.

Follow up:
If you have figured out the O(n) solution, try coding another solution using the
divide and conquer approach, which is more subtle.

int maxSubArray(vector<int>& nums) {}
*/

namespace prefix_sum
{
  // In discussion, by _LeetCode,  Last Edit: October 24, 2018 8:10 PM
  // for python
  //
  // for i in range(1, len(nums)):
  // if nums[i-1] > 0:
  // nums[i] += nums[i-1]
  // return max(nums)
  //
  // The key observation is:
  //
  // [i-1]   [i]
  //
  // if the previous, [i-1], is positive, then "sum" gets bigger whether or not
  // the current element is positive or negative.
  //
  // if runs the above code, then 6 is max sum
  //
  // input:-2  1 -3  4 -1  2  1 -5  4 (9)
  // coll :-2  1 -2  4  3 [5] 6  1  5 (9)
  //
  // [5]? since modify input during scanning and get the previous which is
  // modified.
  //
  // Runtime: 12 ms, faster than 98.45% of C++ online submissions for Maximum
  // Subarray.
  //
  // Memory Usage: 10.5 MB, less than 15.81% of C++ online submissions for
  // Maximum Subarray.

  int max_sub_array_1(std::vector<int> &input)
  {
    // for print purpose since it modifies coll.
    std::vector<int> coll{input};

    for (size_t i = 1; i < coll.size(); ++i)
    {
      if (coll[i - 1] > 0)
      {
        coll[i] += coll[i - 1];
      }
    }

    // PRINT_ELEMENTS(input, "input:");
    // PRINT_ELEMENTS(coll, "coll: ");

    return *std::max_element(coll.begin(), coll.end());
  }

  // https://www.geeksforgeeks.org/largest-sum-contiguous-subarray/
  //
  // Simple idea of the Kadane’s algorithm is to look for all positive
  // contiguous segments of the array (max_ending_here is used for this). And
  // keep track of *maximum sum* contiguous segment among all positive segments
  // (max_so_far is used for this). Each time we get a positive sum compare it
  // with max_so_far and update max_so_far if it is greater than max_so_far
  //
  // if runs the prefix-sum but do not allow negative value ,
  // then 6 is max sum
  //
  //    -2  1   -3 [4   -1  2   1]  -5  4
  // 0  -2  1   -2  2    1  3   4   -1  3   prefix sum
  // 0   0  1    0  4    3  5   6   1   5   prefix sum and do not allow negative
  //
  // note that do not use space for prefix sum and do sum while scanning.

  int max_sub_array_2(std::vector<int> &input)
  {
    int max_so_far{std::numeric_limits<int>::min()};
    int max_current{};

    for (auto e : input)
    {
      // same
      // max_current = e + max_current;
      // if (max_current < 0)
      //   max_current = 0;

      max_current = e + max_current;
      max_current = std::max(0, max_current);

      if (max_current > max_so_far)
        max_so_far = max_current;
    }

    return max_so_far;
  }

  // if make the previous value bigger? affect on next sum and will
  // be covered
  //
  // -2  100   -3    4    -1     2     1   -5    4
  //           97  101   100   102   103   98  102
  //
  //
  // This is about "sum" but not "sub array" How about returnning
  // "sub array"?
  //
  // -2  1   -3 [4   -1  2   1]  -5  4
  //         -2      3   5   6   1   5
  //         *       *
  //         *
  // "*" starts and max_element() is ends.
} // namespace prefix_sum

TEST(AlgoPrefixSum, check_max_sub_array)
{
  using namespace prefix_sum;

  auto imps = {max_sub_array_1, max_sub_array_2};

  for (const auto &f : imps)
  {
    {
      std::vector<int> coll{-2, 1, -3, 4, -1, 2, 1, -5, 4};
      EXPECT_THAT(f(coll), 6);
    }

    {
      std::vector<int> coll{-2, 100, -3, 4, -1, 2, 1, -5, 4};
      EXPECT_THAT(f(coll), 103);
    }
  }
}

// ={=========================================================================
// algo-passing-car

// in sum, find (0, X) pair from the input.
//
// if loop on input to find each (0, X) pair then cannot meet O(N)
//
// How to solve?
//
// the key is whenever see 1, it's counted more depending on preceding 0's
// number.
//
// {0, 1, 0, 1, 1}
//     *     *  *
//           *  *
// so sum is 5

int passing_car_0628_01(const vector<int> A)
{
  int pair_count{}, zero_count{};

  for (size_t i = 0; i < A.size(); ++i)
  {
    if (A[i] == 0)
      ++zero_count;
    else
    {
      pair_count += zero_count;
    }
  }

  return pair_count;
}

TEST(AlgoPassingCar, 0628_01)
{
  EXPECT_THAT(passing_car_0628_01({0, 1, 0, 1, 1}), 5);
}

// From http://codility-lessons.blogspot.co.uk/2014/07/lesson-3-passingcars.html.
//
// The idea is that
//
//    0 1 0 1 1
//    *------->
//        *--->

int passing_car_old_01(vector<int> &A)
{
  int count = 0, countEast = 0;

  for (int i = 0; i < (int)A.size(); i++)
  {
    if (A[i] == 0)
      countEast++;
    else
    {
      count += countEast;

      if (count > 1000000000)
        return -1;
    }
  }

  return count;
}

// ={=========================================================================
// algo-count-div

// Since time O(1), cannot use loop.
//
// How to solve?
//
// when A is divisible by K:
//    B-S = diff. diff/K + 1 is the number of integers that can be divisible by
//    K. +1 since diff do not include A.
//
// 6,7,8,9,10,11,12, K=2
//
// 12-6=6. 6/2=3. 3+1=4
//
// when A is not diviaible by K:
//    cannot use loop either to find S(start value). so have to find the next
//    K*x element in the input. To do that, if A%K != 0, then S = (A/K + 1)*K.
//
//    B-S = diff. diff/K + 1.

// peformance 100%, correctness 50%
int count_div_0628_01(int A, int B, int K)
{
  int start{}, result{};

  if (A % K == 0)
    start = A;
  else
    start = (A / K + 1) * K;

  return result = (B - start) / K + 1;
}

TEST(AlgoCountDiv, 0628_01)
{
  EXPECT_THAT(count_div_0628_01(6, 11, 2), 3);
}

// failed from the report
// EXPECT_THAT(count_div_0628_01(1, 1, 11), 0);
//
// failed from the report
// EXPECT_THAT(count_div_0628_03(1, 1, 11), 0);
//
// fails since 0/K and 0%K are 0. WHY 1? Since 0 is still divisible.
// EXPECT_THAT(count_div_0628_03(0, 1, 11), 1);
//
// why 1?
// EXPECT_THAT(count_div_0628_03(0, 0, 11), 1);
//
// fails
// EXPECT_THAT(count_div_0628_03(0, 14, 2), 8);
//
// after all, missed to handle:
// 1. end case which is 0 on both A and B
// 2. 0/K and 0%K are 0.

// 100% pass
int count_div_0628_03(int A, int B, int K)
{
  int start{}, result{};

  if (A % K == 0)
    start = A;
  else
    start = (A / K + 1) * K;

  if (B - start >= 0)
    result = (B - start) / K + 1;

  return result;
}

TEST(AlgoCountDiv, 0628_03)
{
  EXPECT_THAT(count_div_0628_03(6, 11, 2), 3);
  EXPECT_THAT(count_div_0628_03(1, 1, 11), 0);
  EXPECT_THAT(count_div_0628_03(0, 1, 11), 1);
  EXPECT_THAT(count_div_0628_03(10, 10, 5), 1);
  EXPECT_THAT(count_div_0628_03(10, 10, 7), 0);
  EXPECT_THAT(count_div_0628_03(10, 10, 20), 0);
  EXPECT_THAT(count_div_0628_03(0, 0, 11), 1);
  EXPECT_THAT(count_div_0628_03(0, 14, 2), 8);
}

// ={=========================================================================
// algo-count-identical-pairs

// {3,5,6,3,3,5}
//
// Why the answer is 4 but not 3? Use index and there are 4 pairs
//
// no mention of input value range and assumes 255, 0 <= A[x] <= 255
//
// the same as algo-passing_car

int count_identical_pairs_0629_01(const vector<int> &A)
{
  int count{};

  vector<pair<int, int>> map(256);

  for (auto e : A)
  {
    // not set before
    if (map[e].first == 0)
      map[e] = pair<int, int>(e, 0);

    // add count
    count += map[e].second;

    // update count
    map[e].second += 1;
  }

  return count;
}

TEST(AlgoCountIdenticalPairs, 0629_01)
{
  EXPECT_THAT(count_identical_pairs_0629_01({3, 5, 6, 3, 3, 5}), 4);
}

// ={=========================================================================
// algo-repairman

typedef long long lint;

int g_points[1000];
int g_weights[1000];

// the order of arg do not matter since it's abs. for example, abs(s-e) or abs(e-s)
#define ABS_DISTANCE(s, e) (abs(g_points[e] - g_points[s]))

lint sum_range(int start, int finish, lint start_distance = 0);
lint sum_range_with_start(int start,
                          int begin,
                          int end,
                          lint start_distance = 0);

// get weighted sum in (begin, end] from start offset. note that do not
// include start in its sum.
//
// * loop condition and distance math
// turns out have to have loop exit in the middle of loop as the orignial has.
// Other attemtps like these cause probblems:
//
// for (int i = begin + direction; i != end ; i += direction)
// cause to miss out the last in add so wrong result.
//
// for (int i = begin + direction; i != (end+1); i += direction)
// cause to seg fault when do the left direction, direction is -1.
//
// the proper is:
//
// for (int i = begin + direction; ; i += direction)
// {
//   start_distance += ABS_DISTANCE(i-direction, i);
//
//   if (i == end)
//    return weight_sum;
// }
//
// Why? In order to support both direction and not inclding the start, use
// clever way to do match. In particuler, the way `direction` is used.
//
// For example:
//
// sum_range(0,5)
//  not include start, 0.
//  direction = 1, i = 1, ABS(0,1), wi[1]
//  direction = 1, i = 2, ABS(1,2), wi[2]
//  direction = 1, i = 3, ABS(3,2), wi[3]
//  direction = 1, i = 4, ABS(4,3), wi[4]
//  direction = 1, i = 5, ABS(5,4), wi[5]
//  exit
//
// sum_range(5,0)
//  not include start, 5.
//  direction = -1, i = 4, ABS(5,4), wi[4]
//  direction = -1, i = 3, ABS(4,3), wi[3]
//  direction = -1, i = 2, ABS(3,2), wi[2]
//  direction = -1, i = 1, ABS(2,1), wi[1]
//  direction = -1, i = 0, ABS(1,0), wi[0]
//  exit
//
//
// running:
//
//  i   0    1    2     3     4     5
// wi   1    2    10    3     5     1
// -----+----+-----+-----+-----+-----+---
// xi   1    6    12    13    14    24
//      s--------------------------->   244
//      -----s
//      ---------------------------->   329
//      ----------s
//      ---------------------------->   246
//      ----------------s
//      ---------------------------->   196
//      ----------------------s
//      ---------------------------->   88
//      ...
//
// 86 is made of:
//
//                s-----*-----*
//      *----*----
//                            ------*
//
// where * denotes points used in sum.
//
//
// # DURATION     TID     FUNCTION
//             [   529] | search(0, 5) {
//
//                          /* starts from 0 to 5 */
//                          /* sum(start, begin, end, dis); */
//    0.665 us [   529] |   sum(0, 5, 0, 5) = 244;
//
//                          /* starts from 1 */
//                          /* sum(begin, end, dis); */
//             [   529] |   sum(1, 0, 5, 0) {
//    0.443 us [   529] |     sum(1, 0, 0, 0) = 5;
//    1.035 us [   529] |     sum(1, 5, 10, 5) = 329;
//    1.219 ms [   529] |   } = 334; /* sum */
//
//                          /* starts from 2 */
//             [   529] |   sum(2, 0, 5, 0) {
//    0.514 us [   529] |     sum(2, 0, 0, 0) = 23;
//    0.499 us [   529] |     sum(2, 5, 22, 5) = 223;
//    1.225 ms [   529] |   } = 246; /* sum */
//
//                          /* starts from 3 */
//             [   529] |   sum(3, 0, 5, 0) {
//    0.554 us [   529] |     sum(3, 0, 0, 0) = 36;
//    0.566 us [   529] |     sum(3, 5, 24, 5) = 160;
//    1.078 ms [   529] |   } = 196; /* sum */
//
//                          /* update minimum sum and save start point */
//
//                          /* starts from 4 */
//             [   529] |   sum(4, 0, 5, 0) {
//    0.493 us [   529] |     sum(4, 0, 0, 0) = 52;
//    0.431 us [   529] |     sum(4, 5, 26, 5) = 36;
//  190.644 us [   529] |   } = 88; /* sum */
//
//                          /* update minimum sum and save start point */
//
//                          /* search [0,5] ends and there are updates so search again [4,0] */
//
//             [   529] |   search(4, 0) {
//
//                          /* starts from 4 to 0 */
//    0.242 us [   529] |     sum(4, 0, 0, 0) = 52;
//
//             [   529] |     sum(3, 4, 0, 0) {
//    0.202 us [   529] |       sum(3, 4, 0, 4) = 5;
//    0.394 us [   529] |       sum(3, 0, 2, 0) = 62;
//  280.063 us [   529] |     } = 67; /* sum */
//
//             [   529] |     sum(2, 4, 0, 0) {
//    0.301 us [   529] |       sum(2, 4, 0, 4) = 13;
//    0.220 us [   529] |       sum(2, 0, 4, 0) = 35;
//   40.841 us [   529] |     } = 48; /* sum */
//
//                          /* update minimum sum and save start point */
//
//             [   529] |     sum(1, 4, 0, 0) {
//    0.217 us [   529] |       sum(1, 4, 0, 4) = 121;
//    0.211 us [   529] |       sum(1, 0, 16, 0) = 21;
//   37.859 us [   529] |     } = 142; /* sum */
//
//                          /* search [4,0] ends and there are updates so search again [2,4] */
//
//             [   529] |     search(2, 4) {
//    0.224 us [   529] |       sum(2, 4, 0, 4) = 13;
//
//             [   529] |       sum(3, 2, 4, 0) {
//    3.156 us [   529] |         sum(3, 2, 0, 2) = 10;
//    0.221 us [   529] |         sum(3, 4, 2, 4) = 15;
//   44.632 us [   529] |       } = 25; /* sum */
//
//    0.208 us [   529] |       sum(2, 4, 0, 4) = 13;
//
//                              /* search [2,4] ends
//                                 add time[2, end 4], time +=2, time 2
//                                 <13, 2> = sum(ind:2, e:4, time:0)
//                              */
//
//                              /* means that start = 2 for minimul sum ! */
//
//   71.569 us [   529] |     } = 13; /* search */
//
//                            /* still in search [4,0]
//                               ind was 2, add time[2, start 4], time +=2, time 4
//                             */
//
//    0.428 us [   529] |     sum(2, 0, 4, 0) = 35;
//
//                            /* search [4,0] ends
//                               add time[2, start 0], time +=11, time 15
//                               <48, 15> = sum(ind:2, e:0, time:4)
//                            */
//
//  955.074 us [   529] |   } = 48; /* search */
//
//                            /* still in search [0,5]
//                               ind was 4, add time[4, start 0], time +=13, time 28
//                             */
//
//    0.235 us [   529] |   sum(4, 5, 28, 5) = 38;
//
//                            /* search [0,5] ends
//                               add time[4, end 5], time +=10, time 38
//                               <86, 38> sum(ind:4, e:5, time:28)
//                            */
//
//    6.085 ms [   529] | } = 86; /* search */
//
//             [   529] | search(5, 0) {
//    0.239 us [   529] |   sum(5, 0, 0, 0) = 262;
//             [   529] |   sum(4, 5, 0, 0) {
//    0.206 us [   529] |     sum(4, 5, 0, 5) = 10;
//    0.223 us [   529] |     sum(4, 0, 20, 0) = 372;
//   49.363 us [   529] |   } = 382; /* sum */
//             [   529] |   sum(3, 5, 0, 0) {
//    0.300 us [   529] |     sum(3, 5, 0, 5) = 16;
//    0.295 us [   529] |     sum(3, 0, 22, 0) = 322;
//   50.948 us [   529] |   } = 338; /* sum */
//             [   529] |   sum(2, 5, 0, 0) {
//    0.287 us [   529] |     sum(2, 5, 0, 5) = 25;
//    0.305 us [   529] |     sum(2, 0, 24, 0) = 95;
//   52.524 us [   529] |   } = 120; /* sum */
//             [   529] |   sum(1, 5, 0, 0) {
//    0.254 us [   529] |     sum(1, 5, 0, 5) = 139;
//    0.249 us [   529] |     sum(1, 0, 36, 0) = 41;
//   42.398 us [   529] |   } = 180; /* sum */
//             [   529] |   search(2, 5) {
//    0.223 us [   529] |     sum(2, 5, 0, 5) = 25;
//             [   529] |     sum(3, 2, 5, 0) {
//    0.202 us [   529] |       sum(3, 2, 0, 2) = 10;
//    0.211 us [   529] |       sum(3, 5, 2, 5) = 28;
//   37.587 us [   529] |     } = 38; /* sum */
//             [   529] |     sum(4, 2, 5, 0) {
//    0.279 us [   529] |       sum(4, 2, 0, 2) = 23;
//    0.270 us [   529] |       sum(4, 5, 4, 5) = 14;
//   56.792 us [   529] |     } = 37; /* sum */
//    0.363 us [   529] |     sum(2, 5, 0, 5) = 25;
//  657.999 us [   529] |   } = 25; /* search */
//    0.286 us [   529] |   sum(2, 0, 24, 0) = 95;
//  954.739 us [   529] | } = 120; /* search */
//
//  86 = min(86, 120)
//
// Things to note:
//
// 1. Always calculate distance from the start point.
// 2. Do not count point which are already counted.
// 3. The start point do not included in the sum.

lint sum_range(int begin, int end, lint start_distance)
{
  if (begin == end)
    return 0;

  // forward or backward
  int direction = ((end > begin) ? 1 : -1);

  lint weight_sum = 0;

  for (int i = begin + direction;; i += direction)
  {
    start_distance += ABS_DISTANCE(i - direction, i);
    weight_sum += start_distance * (lint)g_weights[i];

    if (i == end)
      return weight_sum;
  }

  return weight_sum;
}

lint sum_range_with_start(int start, int begin, int end, lint start_distance)
{
  lint d1 = sum_range(start, begin, start_distance);

  // why 2? since used twice to make a turn
  lint d3 =
    sum_range(start, end, start_distance + 2 * ABS_DISTANCE(begin, start));

  return d1 + d3;
}

// search(start index, end index)
pair<lint, lint> search(int begin, int end)
{
  lint current_sum  = sum_range(begin, end, 0);
  int current_start = begin;

  if (begin == end)
    return make_pair(0, 0);

  int direction = ((end > begin) ? 1 : -1);

  lint calculated_sum      = 0;
  lint calculated_distance = 0;

  for (int start = begin + direction; start != end; start += direction)
  {
    lint range_sum = sum_range_with_start(start, begin, end);

    // found sum which is less than sum of [begin, end] and save start point.
    // so `ind` is saved start which shows the minimum sum so far.
    //
    // redundant check
    // if (range_sum < current_sum && abs(start - begin) > abs(current_start - begin))

    if (range_sum < current_sum)
    {

      current_sum   = range_sum;
      current_start = start;
    }
  }

  // when found start point which has lesser sum since current_start do not
  // change when there is no lesser sum found in the range.

  if (current_start != begin)
  {
    pair<lint, lint> result = search(current_start, begin);
    calculated_sum += result.first;
    calculated_distance += result.second + ABS_DISTANCE(current_start, begin);
  }

  calculated_sum += sum_range(current_start, end, calculated_distance);
  calculated_distance += ABS_DISTANCE(current_start, end);

  return make_pair(calculated_sum, calculated_distance);
}

TEST(AlgoRapairman, 0704)
{
  {
    vector<int> dcall{1, 6, 12, 13, 14, 24};
    vector<int> wcall{1, 2, 10, 3, 5, 1};

    for (size_t i = 0; i < dcall.size(); ++i)
    {
      g_points[i]  = dcall[i];
      g_weights[i] = wcall[i];
    }

    int N = dcall.size();

    pair<lint, lint> res1 = search(0, N - 1);
    pair<lint, lint> res2 = search(N - 1, 0);

    EXPECT_THAT(min(res1.first, res2.first), 86);
  }

  {
    vector<int>
      dcall{5, 34, 45, 49, 51, 52, 53, 56, 63, 81, 84, 88, 93, 99, 106};
    vector<int> wcall{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    for (size_t i = 0; i < dcall.size(); ++i)
    {
      g_points[i]  = dcall[i];
      g_weights[i] = wcall[i];
    }

    int N = dcall.size();

    pair<lint, lint> res1 = search(0, N - 1);
    pair<lint, lint> res2 = search(N - 1, 0);

    EXPECT_THAT(min(res1.first, res2.first), 630);
  }
}

/*
={=========================================================================
algo-conversion algo-atoi

* input type? digits only? no space?
* input size?
* what base? 10 or 2?
* sign support?

from ansic, p43.

when base is 10.

this is 'naive' implementation since no error handlings and return 0 when failed
to convert. compare to strtol

there is no check on the end of string input? '0' is not the same as 0(null) and
when see any other than numbers, for loops ends.

*/

namespace algo_conversion
{
  uint32_t atoi_navie(const char *str)
  {
    uint32_t value{0}, i{0};

    for (; str[i] >= '0' && str[i] <= '9'; ++i)
    {
      value = value * 10 + (str[i] - '0');
    }

    return value;
  }

  // use isdigit()

  uint32_t atoi_isdigit(const char *str)
  {
    uint32_t value{0}, i{0};

    for (; isdigit(str[i]); ++i)
    {
      value = value * 10 + (str[i] - '0');
    }

    return value;
  }

  // To support sign and leading space

  uint32_t atoi_sign(const char *str)
  {
    uint32_t value{0}, i{0}, sign{0};

    while (isspace(str[i]))
      ++i;

    // check sign but don't need to increase i
    sign = str[i] == '-' ? -1 : 1;

    // have to increase
    if (str[i] == '-' || str[i] == '+')
      ++i;

    for (; isdigit(str[i]); ++i)
    {
      value = value * 10 + (str[i] - '0');
    }

    return sign * value;
  }

  // use base 2. value is right but not representation

  // Chapter 7: String Fundamentals, 207
  // #!/usr/bin/python
  //
  // B = '1101'
  // I = 0
  //
  // while B != '':
  //     I = I*2 + (ord(B[0]) - ord('0'))
  //     B = B[1:]
  //
  // print(I)

  uint32_t atoi_binary(const char *str)
  {
    uint32_t value{0}, i{0};

    for (; str[i] >= '0' && str[i] <= '9'; ++i)
    {
      value = value * 2 + (str[i] - '0');
    }

    return value;
  }

  // use base 16. htoi
  //
  // From ansic, exercise 2-3.
  //
  // Write the function htoi(s), which converts a string of hexadecimal digits
  // (including an 'optional' 0x or 0X) into its equivalent integer value. The
  // allowable digits are 0 through 9, a through f, and A through F.
  //
  // isxdigit()
  //        checks for hexadecimal digits, that is, one of
  //        0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F.

  // previous try
  //
  // int htoi(char s[])
  // {
  //   int n, i = 0, v = 0;
  //
  //   // optional 0x or 0X
  //   if(s[0] == '0' && ( s[1] == 'x' || s[1] == 'X' ))
  //     i = 2;
  //
  //   // isxdigit()
  //   // checks for a hexadecimal digits, that is, one of
  //   // 0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F.
  //
  //   for(n = 0; isxdigit(s[i]); i++)
  //   {
  //     if( s[i] >= '0' && s[i] <= '9' )
  //       v = s[i] - '0';
  //     else if( s[i] >= 'a' && s[i] <= 'f' )
  //       v = s[i] - 'a' + 10;
  //     else
  //       v = s[i] - 'A' + 10;
  //
  //     n = n*16 + v;
  //   }
  //
  //   return n;
  // }

  uint32_t atoi_hex(const char *str)
  {
    uint32_t value{0}, i{0};
    const std::string hex{"0123456789abcdef"};

    for (; hex.find(std::tolower(str[i])) != std::string::npos; ++i)
    {
      value = value * 16 + hex.find(std::tolower(str[i]));
    }

    return value;
  }
} // namespace algo_conversion

TEST(AlgoConversion, check_to_integer)
{
  using namespace algo_conversion;

  EXPECT_THAT(atoi_navie("123"), Eq(123));
  EXPECT_THAT(atoi_isdigit("123"), Eq(123));

  EXPECT_THAT(atoi_sign("123"), Eq(123));
  EXPECT_THAT(atoi_sign("-123"), Eq(-123));

  EXPECT_THAT(atoi_binary("1101"), Eq(13));

  EXPECT_THAT(atoi_hex("1a"), Eq(26));
  EXPECT_THAT(atoi_hex("1A"), Eq(26));
}

/*
={=========================================================================
algo-conversion algo-itoa

* what base? 10 or 2?
* sign support?

from ansic, p43.

when base is 10.

there is no check on the end of string input? '0' is not the same as
0(null) and when see any other than numbers, for loops ends.
 
*/

namespace algoconversion
{
  // naive version
  std::string itoa_1(const int input)
  {
    int value{input};
    char letter{0};
    std::string result{};

    for (; value;)
    {
      // note: the order of statement does matter since it updtes "value"

      letter = '0' + (value % 10);
      result += letter;
      value /= 10;
    }

    // *algo-reverse*
    return std::string{result.crbegin(), result.crend()};
  }

  // no reverse since insert() inserts char at front which moves up the rest
  std::string itoa_2(const int input)
  {
    std::string result{};
    std::string temp{};
    char letter{0};
    int value{input};

    for (; value;)
    {
      letter = '0' + (value % 10);
      result.insert(result.begin(), 1, letter);
      value /= 10;
    }

    return result;
  }

} // namespace algoconversion

TEST(AlgoConversion, check_to_string)
{
  using namespace algoconversion;

  const std::vector<std::function<std::string(const int)>> imps{itoa_1, itoa_2};

  for (const auto &f : imps)
  {
    EXPECT_THAT(f(123), Eq("123"));
  }
}

/*
={=========================================================================
algo-conversion algo-leetcode-191

171. Excel Sheet Column Number, Easy

Given a column title as appear in an Excel sheet, return its corresponding
column number.

For example:

    A -> 1
    B -> 2
    C -> 3
    ...
    Z -> 26
    AA -> 27
    AB -> 28 
    ...

Example 1:
Input: "A"
Output: 1

Example 2:
Input: "AB"
Output: 28

Example 3:
Input: "ZY"
Output: 701


168. Excel Sheet Column Title, Easy

Given a positive integer, return its corresponding column title as appear in an
Excel sheet.

For example:

    1 -> A
    2 -> B
    3 -> C
    ...
    26 -> Z
    27 -> AA
    28 -> AB 
    ...

*/

namespace leetcode_easy_171
{
  // uses *algo-atoi* when base 26
  //
  // A  B  C  ... Z
  // 1  2  3      26
  //
  // AA AB AC ... AZ
  // 27 28 29 ... 52 (26 + 26)
  //
  // BA
  // 53
  //
  // assumes that input are uppercase
  //
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Excel
  // Sheet Column Number.
  //
  // Memory Usage: 8.2 MB, less than 27.21% of C++ online submissions for Excel
  // Sheet Column Number.

  int get_column_number_1(const std::string &title)
  {
    int result{};

    for (auto const e : title)
    {
      if (!isalpha(e))
        return 0;

      result = result * 26 + (e - 'A' + 1);
    }

    return result;
  }

  // as with atoi(), use 26 as base
  int get_column_number_2(const std::string &title)
  {
    auto loop{title.size()};
    int result{};

    for (size_t i = 0; i < loop; ++i)
    {
      result = result * 26 + (title[i] - 'A' + 1);
    }

    return result;
  }

  // base 10:
  //
  // 0  1   2   3   4   5   6   7   8   9
  // 10 11  12  13  14  15  16  17  18  19
  // ...
  //
  // 1  2   3   4   5   6   ... 25  26
  // A  B   C   D   E   F       Y   Z
  // % 26
  // 1  2   3   4   5   6   ... 25  0
  //
  // 27   28    29                  52
  // AA   AB    AC                  AZ
  // 1/1  1/2   1/3                 2/0
  //
  // so special handing when % is 0.
  //
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Excel
  // Sheet Column Title.
  //
  // Memory Usage: 8 MB, less than 84.11% of C++ online submissions for Excel
  // Sheet Column Title.

  std::string get_column_title_1(int n)
  {
    string result{};
    char ch{};
    int value{};

    while (n)
    {
      value = (n % 26);
      n     = value == 0 ? (n / 26) - 1 : n / 26;
      ch    = value == 0 ? 'Z' : 'A' + value - 1;

      // not use algo-reverse
      // result += ch;
      result.insert(result.begin(), 1, ch);
    }

    return result;
  }

  // as with itoa(). same as "get_column_title_1"
  std::string get_column_title_2(int number)
  {
    std::string result{};
    int remainder{};

    while (number)
    {
      remainder = (number % 26);

      if (remainder == 0)
      {
        result += ('Z');
        number = number / 26 - 1;
      }
      else
      {
        result += ('A' + remainder - 1);
        number = number / 26;
      }
    }

    return std::string{result.rbegin(), result.rend()};
  }

  // remove special handing by using "--n" and make it same as algo-itoa
  // from discussion:
  // Efficient C++ solution (easily transposable) by daedric

  std::string get_column_title_3(int number)
  {
    std::string result{};
    int remainder{};

    while (number)
    {
      --number;

      remainder = (number % 26);
      number    = number / 26;
      result += ('A' + remainder);
    }

    return std::string{result.rbegin(), result.rend()};
  }
} // namespace leetcode_easy_171

TEST(AlgoConversion, check_get_column_number)
{
  using namespace leetcode_easy_171;

  auto imps = {get_column_number_1, get_column_number_1};

  for (const auto &f : imps)
  {
    EXPECT_THAT(f("A"), 1);
    EXPECT_THAT(f("AB"), 28);
    EXPECT_THAT(f("ZY"), 701);
  }
}

TEST(AlgoConversion, check_get_column_title)
{
  using namespace leetcode_easy_171;

  auto imps = {get_column_title_1, get_column_title_2, get_column_title_3};

  for (const auto &f : imps)
  {
    EXPECT_THAT(f(1), "A");
    EXPECT_THAT(f(28), "AB");
    EXPECT_THAT(f(701), "ZY");
  }
}

// ={=========================================================================
// algo-list

// The stack and `simple-list` are essentially the same implementation in terms
// of contiguous and linked implementation.
//
// o ListEntry can be any type.
// o No remove() to remove entry at random position since it's expensive
// operation as with other contiguous implementation; contiguous stack.

// class List
// {
//   public:
//     void create();
//     void clear();
//     bool empty();
//     bool full();
//     int size();
//     // as push_back();
//     void push(ListEntry const& entry);
//     std::vector<ListEntry> snap();
// };

namespace algo_list_contiguous
{
  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept
        : row_(row)
        , col_(col)
    {}

    int row_{};
    int col_{};
  };

  // cxx-operator-overload
  bool operator==(ListEntry const &lhs, ListEntry const &rhs)
  {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_) ? true : false;
  }

  bool operator!=(ListEntry const &lhs, ListEntry const &rhs)
  {
    return !(lhs == rhs);
  }

  class List
  {
  public:
    explicit List()
        : count_(0)
    {}

    void clear() { count_ = 0; }

    bool empty() { return count_ == 0 ? true : false; }

    bool full() { return count_ >= MAX_ENTRY ? true : false; }

    int size() { return count_; }

    // as push_back();
    void push(ListEntry const &entry)
    {
      if (full())
        throw runtime_error("list is full");

      coll_[count_++] = entry;
    }

    std::vector<ListEntry> snap()
    {
      std::vector<ListEntry> coll;

      for (int i = 0; i < count_; ++i)
        coll.push_back(coll_[i]);

      return coll;
    }

  private:
    // cxx-static
    // error: invalid use of non-static data member ‘algo_list_contiguous::List::MAX_ENTRY’
    // const int MAX_ENTRY{5};

    static const int MAX_ENTRY{5};

    // count_ should be [0, 4] or [1, 5]? choose [1,5]
    int count_;

    ListEntry coll_[MAX_ENTRY];
  };
} // namespace algo_list_contiguous

TEST(AlgoList, ContiguousSimple)
{
  using namespace algo_list_contiguous;

  std::vector<ListEntry> values{ListEntry(1, 2),
                                ListEntry(2, 3),
                                ListEntry(3, 4),
                                ListEntry(4, 5),
                                ListEntry(5, 6)};

  List coll;

  for (auto &e : values)
    coll.push(e);

  EXPECT_THAT(coll.size(), 5);

  // requires cxx-operator-overload
  EXPECT_THAT(coll.snap(), values);

  EXPECT_THROW(coll.push(ListEntry(6, 7)), runtime_error);
}

namespace algo_list_linked
{
  // when node and entry are in a single structure and these can be different
  // structure such as ListEntry and ListNode

  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept
        : row_(row)
        , col_(col)
        , next_(nullptr)
    {}

    int row_{};
    int col_{};

    ListEntry *next_;
  };

  // cxx-operator-overload
  bool operator==(ListEntry const &lhs, ListEntry const &rhs)
  {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_) ? true : false;
  }

  bool operator!=(ListEntry const &lhs, ListEntry const &rhs)
  {
    return !(lhs == rhs);
  }

  class List
  {
  public:
    explicit List() noexcept
        : head_(nullptr)
    {}

    bool emptry() { return count_ == 0 ? true : false; }

    int size() { return count_; }

    // push_back()
    void push_old(ListEntry const &entry)
    {
      if (!head_)
        head_ = new ListEntry(entry);
      else
      {
        ListEntry *run = head_;

        // unlike clear(), snap(), run shall be before end() so that can
        // insert new one. Hence check run->next

        while (run->next_)
          run = run->next_;

        run->next_ = new ListEntry(entry);
      }

      ++count_;
    }

    // push_back()
    void push(ListEntry const &entry)
    {
      ListEntry *run{};

      // find node for insertion *algo-list-find-end*
      // works both when head_ is null and is not null

      for (run = head_; run && run->next_; run = run->next_)
        ;

      // first item
      if (!run)
        head_ = new ListEntry(entry);
      else
        run->next_ = new ListEntry(entry);

      ++count_;
    }

    void clear()
    {
      ListEntry *run = head_;
      ListEntry *prev{};

      while (run)
      {
        prev = run;
        run  = run->next_;
        free(prev);
        --count_;
      }

      head_ = run;
    }

    std::vector<ListEntry> snap()
    {
      ListEntry *run = head_;
      std::vector<ListEntry> coll;

      while (run)
      {
        // ok as well
        // coll.push_back(ListEntry(*run));
        coll.push_back(*run);
        run = run->next_;
      }

      return coll;
    }

  private:
    int count_{};

    // can use ListEntry head_; which changes member implementation

    ListEntry *head_;
  };

} // namespace algo_list_linked

TEST(AlgoList, LinkedSimple)
{
  using namespace algo_list_linked;

  std::vector<ListEntry> values{ListEntry(1, 2),
                                ListEntry(2, 3),
                                ListEntry(3, 4),
                                ListEntry(4, 5),
                                ListEntry(5, 6)};

  List coll;

  for (auto &e : values)
    coll.push(e);

  EXPECT_THAT(coll.size(), 5);

  coll.push(ListEntry(6, 7));
  EXPECT_THAT(coll.size(), 6);

  // requires cxx-operator-overload
  std::vector<ListEntry> expected{ListEntry(1, 2),
                                  ListEntry(2, 3),
                                  ListEntry(3, 4),
                                  ListEntry(4, 5),
                                  ListEntry(5, 6),
                                  ListEntry(6, 7)};

  EXPECT_THAT(coll.snap(), expected);

  coll.clear();
  EXPECT_THAT(coll.size(), 0);
}

// TEST(AlgoList, LinkedSimpleReverse)
// {
//   using namespace algo_list_linked;
//
//   {
//     auto values{
//       ListEntry(1,2),
//         ListEntry(2,3),
//         ListEntry(3,4),
//         ListEntry(4,5),
//         ListEntry(5,6)
//     };
//
//     List coll;
//
//     for (auto &e : values)
//       coll.push(e);
//
//     EXPECT_THAT(coll.size(), 5);
//
//     // now do not expect exception since there's no max
//     // EXPECT_THROW(coll.push(ListEntry(6,7)), runtime_error);
//
//     coll.push(ListEntry(6,7));
//     EXPECT_THAT(coll.size(), 6);
//
//     EXPECT_THAT(coll.snap(),
//         ElementsAre(
//           make_pair(1,2),
//           make_pair(2,3),
//           make_pair(3,4),
//           make_pair(4,5),
//           make_pair(5,6),
//           make_pair(6,7))
//         );
//
//     coll.reverse();
//
//     EXPECT_THAT(coll.snap(),
//         ElementsAre(
//           make_pair(6,7),
//           make_pair(5,6),
//           make_pair(4,5),
//           make_pair(3,4),
//           make_pair(2,3),
//           make_pair(1,2))
//         );
//
//     coll.clear();
//     EXPECT_THAT(coll.size(), 0);
//   }
// }

// In order to exercise Divide(), have to have access to list structure but do
// not see any practical way to do it through class interface. so make `head_`
// public. same as algo_list_linked

namespace algo_list_linked_divide
{
  // when node and entry are in a single structure and these can be different
  // structure such as ListEntry and ListNode

  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept
        : row_(row)
        , col_(col)
        , next_(nullptr)
    {}

    int row_{};
    int col_{};

    ListEntry *next_;
  };

  // cxx-operator-overload
  bool operator==(ListEntry const &lhs, ListEntry const &rhs)
  {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_) ? true : false;
  }

  bool operator!=(ListEntry const &lhs, ListEntry const &rhs)
  {
    return !(lhs == rhs);
  }

  class List
  {
  public:
    explicit List() noexcept
        : head_(nullptr)
    {}

    bool emptry() { return count_ == 0 ? true : false; }

    int size() { return count_; }

    // push_back()
    void push_old(ListEntry const &entry)
    {
      if (!head_)
        head_ = new ListEntry(entry);
      else
      {
        ListEntry *run = head_;

        // unlike clear(), snap(), run shall be before end() so that can
        // insert new one. Hence check run->next

        while (run->next_)
          run = run->next_;

        run->next_ = new ListEntry(entry);
      }

      ++count_;
    }

    // push_back()
    void push(ListEntry const &entry)
    {
      ListEntry *run{};

      // find node for insertion *algo-list-find-end*
      // works both when head_ is null and is not null

      for (run = head_; run && run->next_; run = run->next_)
        ;

      // first item
      if (!run)
        head_ = new ListEntry(entry);
      else
        run->next_ = new ListEntry(entry);

      ++count_;
    }

    void clear()
    {
      ListEntry *run = head_;
      ListEntry *prev{};

      while (run)
      {
        prev = run;
        run  = run->next_;
        free(prev);
        --count_;
      }

      head_ = run;
    }

    std::vector<ListEntry> snap()
    {
      ListEntry *run = head_;
      std::vector<ListEntry> coll;

      while (run)
      {
        // ok as well
        // coll.push_back(ListEntry(*run));
        coll.push_back(*run);
        run = run->next_;
      }

      return coll;
    }

  public:
    int count_{};

    // can use ListEntry head_; which changes member implementation
    ListEntry *head_;
  };

  // o slow and ffast starts from same place, begin()
  // o slow goes 1 and ffast goes 2
  // o this is implementation of {algo-list-tortoise-and-hare}

  bool detect_cycle_01(List const &list)
  {
    ListEntry *slow;
    ListEntry *fast;
    ListEntry *ffast;

    for (slow = fast = ffast = list.head_;
         slow && (fast = ffast->next_) && (ffast = fast->next_);)
    {
      if (slow == fast || slow == ffast)
        return true;

      slow = slow->next_;
    }

    return false;
  }

  // o use single fast
  // o see *cxx-for-while*
  //
  // algo-leetcode
  //
  // 141. Linked List Cycle
  //
  // Runtime: 12 ms, faster than 99.74% of C++ online submissions for Linked
  // List Cycle.
  //
  // Memory Usage: 9.7 MB, less than 73.40% of C++ online submissions for Linked
  // List Cycle.

  bool detect_cycle_02(List const &list)
  {
    ListEntry *slow;
    ListEntry *fast;

    for (slow = fast = list.head_;
         slow && (fast = fast->next_) && (fast = fast->next_);)
    {
      if (slow == fast)
        return true;

      slow = slow->next_;
    }

    return false;
  }

  bool detect_cycle_03(List const &list)
  {
    ListEntry *slow;
    ListEntry *fast;

    // do not check when head_ is null
    for (slow = list.head_, fast = slow->next_; fast && slow != fast;)
    {
      fast = fast->next_;

      if (fast)
      {
        fast = fast->next_;
        slow = slow->next_;
      }
    }

    // loop ends when fast is null or when there is cycle so need to check if it
    // was when there is a cycle

    return fast != slow ? false : true;
  }

} // namespace algo_list_linked_divide

TEST(AlgoList, DetectCycle)
{
  using namespace algo_list_linked_divide;

  {
    std::vector<ListEntry> values{ListEntry(1, 2),
                                  ListEntry(2, 3),
                                  ListEntry(3, 4),
                                  ListEntry(4, 5),
                                  ListEntry(5, 6)};

    List coll;

    for (auto &e : values)
      coll.push(e);

    EXPECT_THAT(detect_cycle_01(coll), false);
    EXPECT_THAT(detect_cycle_02(coll), false);
    EXPECT_THAT(detect_cycle_03(coll), false);
  }

  {
    std::vector<ListEntry> values{ListEntry(1, 2),
                                  ListEntry(2, 3),
                                  ListEntry(3, 4),
                                  ListEntry(4, 5),
                                  ListEntry(5, 6)};

    List coll;

    for (auto &e : values)
      coll.push(e);

    // to make a cycle
    ListEntry *current;
    ListEntry *next;

    current = coll.head_;

    // find end()-1
    for (next = current; next; next = next->next_)
    {
      current = next;
    }

    // makes a cycle
    current->next_ = coll.head_;

    EXPECT_THAT(detect_cycle_01(coll), true);
    EXPECT_THAT(detect_cycle_02(coll), true);
    EXPECT_THAT(detect_cycle_03(coll), true);
  }
}

namespace algo_list_linked_divide
{
  // o devide the first list in two as evenly as possible
  // o when the size of the first is odd, the divided first will have one more
  //   than the second
  // o as with detect_cycle_03(), slow starts from 0 and fast starts from 1.

  void divide_list_01(List &first, List &second)
  {
    ListEntry *slow;
    ListEntry *fast;

    // do not check if slow is null or not

    for (slow = first.head_, fast = slow->next_; fast;)
    {
      fast = fast->next_;

      if (fast)
      {
        fast = fast->next_;
        slow = slow->next_;
      }
    }

    // `slow` is the end of divided first list
    second.head_ = slow->next_;
    slow->next_  = nullptr;
  }

  // as with detect_cycle_01(), both starts from the same pos

  void divide_list_02(List &first, List &second)
  {
    ListEntry *slow;
    ListEntry *fast;

    for (slow = fast = first.head_;
         slow && (fast = fast->next_) && (fast = fast->next_);)
    {
      slow = slow->next_;
    }

    // `slow` is the end of divided first list
    second.head_ = slow->next_;
    slow->next_  = nullptr;
  }
} // namespace algo_list_linked_divide

TEST(AlgoList, Divide)
{
  using namespace algo_list_linked_divide;

  std::vector<ListEntry> values{ListEntry(1, 2),
                                ListEntry(2, 3),
                                ListEntry(3, 4),
                                ListEntry(4, 5),
                                ListEntry(5, 6),
                                ListEntry(6, 7)};

  {
    const auto func = divide_list_01;

    List coll1;

    for (auto &e : values)
      coll1.push(e);

    // requires cxx-operator-overload
    std::vector<ListEntry> divided_first{ListEntry(1, 2),
                                         ListEntry(2, 3),
                                         ListEntry(3, 4)};

    std::vector<ListEntry> divided_second{ListEntry(4, 5),
                                          ListEntry(5, 6),
                                          ListEntry(6, 7)};

    // note:
    // since divide_list_01() do not change `count_`, count of simple_list,
    // simple_second will not be correct after this.

    List coll2;

    func(coll1, coll2);

    EXPECT_THAT(coll1.snap(), divided_first);
    EXPECT_THAT(coll2.snap(), divided_second);

    coll1.clear();
    coll2.clear();
  }

  {
    const auto func = divide_list_02;

    List coll1;

    for (auto &e : values)
      coll1.push(e);

    // requires cxx-operator-overload
    std::vector<ListEntry> divided_first{ListEntry(1, 2),
                                         ListEntry(2, 3),
                                         ListEntry(3, 4)};

    std::vector<ListEntry> divided_second{ListEntry(4, 5),
                                          ListEntry(5, 6),
                                          ListEntry(6, 7)};

    // note:
    // since divide_list_01() do not change `count_`, count of simple_list,
    // simple_second will not be correct after this.

    List coll2;

    func(coll1, coll2);

    EXPECT_THAT(coll1.snap(), divided_first);
    EXPECT_THAT(coll2.snap(), divided_second);

    coll1.clear();
    coll2.clear();
  }

  // TEST(List, SpliceAndDivide)
  {
    std::list<int> coll{26, 33, 35, 29, 19, 12, 22};
    auto slow = coll.begin();
    auto fast = next(slow);

    for (; fast != coll.end();)
    {
      ++fast;

      if (fast != coll.end())
      {
        ++fast;
        ++slow;
      }
    }

    list<int> coll1;
    list<int> coll2;

    // due to open end of iterator, increase one more compared to C version.
    EXPECT_THAT(*slow, 29);
    ++slow;

    // c.splice(pos,c2, c2beg,c2end)
    // Moves all elements of the range [c2beg,c2end) in c2 in
    // front of pos of list c (c and c2 may be identical)

    coll1.splice(coll1.begin(), coll, coll.begin(), slow);
    coll2.splice(coll2.begin(), coll, slow, coll.end());

    EXPECT_THAT(coll1, ElementsAre(26, 33, 35, 29));
    EXPECT_THAT(coll2, ElementsAre(19, 12, 22));
  }
}

// namespace list_simple_linked_list_public_two
// {
//   // C version list from the reference
//
//   typedef int ListEntry;
//
//   typedef struct node {
//     int       key;
//     struct node *pnext;
//   } ListNode;
//
//   typedef struct {
//     ListNode *header;
//     int count;
//   } List;
//
//   void CombineList(List *first, List *last, List *result)
//   {
//     // handle when one of lists is empty
//     if( !first->header )
//     {
//       *result = *last;
//       return;
//     }
//     else if ( !last->header )
//     {
//       *result  = *first;
//       return;
//     }
//
//     // handle first comparison
//     ListNode *pfirst = first->header, *psecond = last->header;
//     ListNode *psorted;
//
//     if(pfirst->key < psecond->key)
//     {
//       result->header = pfirst;
//       pfirst = pfirst->pnext;
//     }
//     else
//     {
//       result->header = psecond;
//       psecond = psecond->pnext;
//     }
//
//     psorted = result->header;
//
//     // sort until finish one of lists because first and second is alreay sorted itself
//     while( pfirst && psecond )
//     {
//       if(pfirst->key < psecond->key)
//       {
//         psorted->pnext = pfirst;
//         psorted = pfirst;
//         pfirst = pfirst->pnext;
//       }
//       else
//       {
//         psorted->pnext = psecond;
//         psorted = psecond;
//         psecond = psecond->pnext;
//       }
//     }
//
//     // when one of lists are finished, simply append the other list to the sorted
//     if(!pfirst)
//       psorted->pnext = psecond;
//     else
//       psorted->pnext = pfirst;
//   }
// } // namespace

/*
={=========================================================================
algo-stack

class Stack
{
  public:
    void create();
    void clear();
    bool empty();
    bool full();
    int size();

    // as push_back();
    void push(ListEntry const& entry);
    void pop();
    ListEntry top();

    // as traverse()
    std::vector<ListEntry> snap();
};

*/

namespace algo_stack_contiguous
{
  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept
        : row_(row)
        , col_(col)
    {}

    int row_{};
    int col_{};
  };

  bool operator==(ListEntry const &lhs, ListEntry const &rhs)
  {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_) ? true : false;
  }

  bool operator!=(ListEntry const &lhs, ListEntry const &rhs)
  {
    return !(lhs == rhs);
  }

  class Stack
  {
  public:
    void create() { top_ = 0; }

    void clear() { top_ = 0; }

    bool empty() { return top_ == 0 ? true : false; }

    bool full() { return top_ >= MAX_ENTRY ? true : false; }

    int size() { return top_; }

    // as push_back();
    void push(ListEntry const &entry)
    {
      if (full())
        throw runtime_error("list is full");

      coll_[top_++] = entry;
    }

    void pop() { --top_; }

    ListEntry top()
    {
      if (empty())
        throw runtime_error("list is empty");

      return coll_[top_ - 1];
    }

    std::vector<ListEntry> snap()
    {
      std::vector<ListEntry> coll;

      for (int i = 0; i < top_; ++i)
        coll.push_back(coll_[i]);

      return coll;
    }

  private:
    // cxx-static
    // error: invalid use of non-static data member ‘algo_list_contiguous::List::MAX_ENTRY’
    // const int MAX_ENTRY{5};

    static const int MAX_ENTRY{5};

    // top_ is next positon to push
    int top_;

    ListEntry coll_[MAX_ENTRY];
  };
} // namespace algo_stack_contiguous

TEST(AlgoStack, check_contiguous_simple)
{
  using namespace algo_stack_contiguous;

  std::vector<ListEntry> values{ListEntry(1, 2),
                                ListEntry(2, 3),
                                ListEntry(3, 4),
                                ListEntry(4, 5),
                                ListEntry(5, 6)};

  Stack coll;
  coll.create();

  for (auto &e : values)
    coll.push(e);

  EXPECT_THAT(coll.size(), 5);
  EXPECT_THROW(coll.push(ListEntry(6, 7)), runtime_error);

  EXPECT_THAT(coll.top(), ListEntry(5, 6));

  coll.pop();
  coll.pop();
  coll.pop();

  EXPECT_THAT(coll.size(), 2);

  // requires cxx-operator-overload
  auto expected = {ListEntry(1, 2), ListEntry(2, 3)};

  EXPECT_THAT(coll.snap(), expected);
}

/*
={=========================================================================
algo-leetcode-155

155. Min Stack, Easy

Design a stack that supports push, pop, top, and retrieving the minimum element
in constant time.

push(x) -- Push element x onto stack.
pop() -- Removes the element on top of the stack.
top() -- Get the top element.
getMin() -- Retrieve the minimum element in the stack.

Example:

MinStack minStack = new MinStack();
minStack.push(-2);
minStack.push(0);
minStack.push(-3);
minStack.getMin();   --> Returns -3.
minStack.pop();
minStack.top();      --> Returns 0.
minStack.getMin();   --> Returns -2.

*/

namespace leetcode_easy_155
{
  // 1. can keep the min while doing push but the problem is when doing pop. if
  // search the min whenever doing pop, cannot meet the condition:
  // "retrieving the minimum element in constant time"

  // 2. Python single stack O(1) all operations, by destinynitsed
  //
  // uses "extra space" to keep the last min value along with element to push
  // and read the min value from the fixed pos. each last min value is value
  // when each is pushed.
  //
  // Python code is clearer
  //
  // class MinStack:
  //
  //     def __init__(self):
  //         self.stack = []
  //
  //     def push(self, x):
  //         if self.stack:
  // 			      self.stack.append(min(self.stack[-2], x))
  //         else:
  //             self.stack.append(x)
  //         self.stack.append(x)
  //
  //
  //     def pop(self):
  //         if self.stack:
  //             self.stack.pop()
  //             self.stack.pop()
  //
  //     def top(self):
  //         if self.stack:
  //             return self.stack[-1]
  //
  //     def getMin(self):
  //         if self.stack:
  //             return self.stack[-2]
  //
  // For example, input -2, 0, 3 makes
  // [-2, -2, -2, 0, -3, -3]
  //  ^^      ^^     ^^      where ^^ are the min values.

  class MinStack_2
  {
  private:
    std::vector<int> _coll;

  public:
    bool empty() { return _coll.empty(); }

    void push(int elem)
    {
      if (empty())
      {
        _coll.emplace_back(elem);
      }
      else
      {
        // so get the min value and push it
        _coll.emplace_back(std::min(*(_coll.end() - 2), elem));
      }

      // and push the element.
      _coll.emplace_back(elem);
    }

    void pop()
    {
      if (empty())
        throw std::runtime_error("stack is empty");

      _coll.pop_back();
      _coll.pop_back();
    }

    int &top()
    {
      if (empty())
        throw std::runtime_error("stack is empty");

      return _coll.back();
    }

    int getMin()
    {
      if (empty())
        throw std::runtime_error("stack is empty");

      return *(_coll.end() - 2);
    }
  };

  // 3. Java accepted solution using one stack, by sometimescrazy
  // saves space by adding previous min only when min changes then 2.
  //
  // For example, input -2, 0, 3 makes
  //
  // MinStack_2: [-2, -2, -2, 0, -3, -3]
  // MinStatk_3: [max, -2, 0, -2, -3]

  class MinStack_3
  {
  private:
    std::vector<int> _coll;
    int _min{std::numeric_limits<int>::max()};

  public:
    bool empty() { return _coll.empty(); }

    // do not save the min value if do not see elem which is less than current
    // min value. so need to save the previous min value when see the lesser.
    // here save the previous before updating it.
    //
    // _coll.emplace_back(_min);

    void push(int elem)
    {
      if (elem <= _min)
      {
        _coll.emplace_back(_min);
        _min = elem;
      }

      _coll.emplace_back(elem);
    }

    void pop()
    {
      if (empty())
        throw runtime_error("stack is empty");

      if (_coll.back() == _min)
      {
        _coll.pop_back();

        // restore min with the previous min
        _min = _coll.back();
      }

      _coll.pop_back();
    }

    int &top()
    {
      if (empty())
        throw runtime_error("stack is empty");

      return _coll.back();
    }

    int getMin()
    {
      if (empty())
        throw runtime_error("stack is empty");

      return _min;
    }
  };
} // namespace leetcode_easy_155

TEST(AlgoStack, check_min_stack)
{
  using namespace leetcode_easy_155;

  {
    MinStack_2 ms;

    ms.push(-2);
    ms.push(0);
    ms.push(-3);

    EXPECT_THAT(ms.getMin(), -3);

    ms.pop();
    EXPECT_THAT(ms.top(), 0);

    EXPECT_THAT(ms.getMin(), -2);
  }

  {
    MinStack_2 ms;

    ms.push(2147483646);
    ms.push(2147483646);
    ms.push(2147483646);
    ms.top();
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483646);
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483646);
    ms.pop();

    ms.push(2147483647);
    ms.top();
    EXPECT_THAT(ms.getMin(), 2147483647);
    ms.push(-2147483648);
    ms.top();
    EXPECT_THAT(ms.getMin(), -2147483648);
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483647);
  }

  {
    MinStack_3 ms;

    ms.push(-2);
    ms.push(0);
    ms.push(-3);

    EXPECT_THAT(ms.getMin(), -3);

    ms.pop();
    EXPECT_THAT(ms.top(), 0);

    EXPECT_THAT(ms.getMin(), -2);
  }

  {
    MinStack_3 ms;

    ms.push(2147483646);
    ms.push(2147483646);
    ms.push(2147483646);
    ms.top();
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483646);
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483646);
    ms.pop();

    ms.push(2147483647);
    ms.top();
    EXPECT_THAT(ms.getMin(), 2147483647);
    ms.push(-2147483648);
    ms.top();
    EXPECT_THAT(ms.getMin(), -2147483648);
    ms.pop();
    EXPECT_THAT(ms.getMin(), 2147483647);
  }
}

/*
={=========================================================================
algo-sort-insertion

[sorted][unsorted]

reference code, ascending sort
o the unsorted < the last sorted to start scanning.
o the unsorted > the current to to stop scanning.

vs. bubble sort

move bigger to the right, so ascending order. the bubble sort find a final
position of an element and insertion sort do not.

template <typename Cont>
void bubble_sort(Cont& cont) {
  for (int i = 0; i < cont.size(); i++) 
  {
    for (int j = i + 1; j < cont.size(); j++) 
    {
      if (cont[i] > cont[j]) 
      {
        cont.swap(i, j);
      }
    }
  }
}
 
*/

namespace algo_sort_insertion
{
  // old
  void sort_insertion_1(std::vector<int> &coll)
  {
    // start from 1 since one entry is always sorted.
    int size = (int)coll.size();

    for (int unsorted_index = 1; unsorted_index < size; ++unsorted_index)
    {
      int sorted_index = unsorted_index - 1;

      // pick the first from `unsorted`. if it is less than the last of the
      // sorted, then have to place it in the sorted area.

      // if (coll[unsorted_index] < coll[sorted_index])
      if (coll[sorted_index] > coll[unsorted_index])
      {
        int unsorted_entry = coll[unsorted_index];
        int current_index  = sorted_index;

        // find the place of picked unsorted in the sorted until see that
        // sorted[current-1] < the entry in question by:
        //
        // move the last sorted down one which makes a space, and check the one
        // before the current, current-1 since already checked the current.
        //
        // must have "index == 0" check since have a check on current-1 to see
        // whether or not contine searching.

        for (; 0 <= current_index; --current_index)
        {
          // move one entry down
          coll[current_index + 1] = coll[current_index];

          if (current_index == 0 || coll[current_index - 1] < unsorted_entry)
            break;
        }

        coll[current_index] = unsorted_entry;
      }
    }
  }

  // Unlike 1:
  // o removes to check `current-1` from the current in searching the place in the
  // sorted and then no need to check on -1 index.
  //
  // o the difference is this. 01 compare the sorted and the unsorted to start
  // scanning the place of the unsorted and use "current-1", one ahead, to check.
  // 02 instead, scanning from current regardless.

  void sort_insertion_2(std::vector<int> &coll)
  {
    int size = (int)coll.size();

    for (int unsorted_index = 1; unsorted_index < size; ++unsorted_index)
    {
      int sorted_index = unsorted_index - 1;

      if (coll[unsorted_index] < coll[sorted_index])
      {
        int unsorted_entry = coll[unsorted_index];
        int current_index  = sorted_index;

        // diffs from 01
        for (; 0 <= current_index; --current_index)
        {
          if (coll[current_index] < unsorted_entry)
            break;

          coll[current_index + 1] = coll[current_index];
        }

        coll[current_index + 1] = unsorted_entry;
      }
    }
  }

  // 2020.07. similar approach
  void sort_insertion_3(std::vector<int> &coll)
  {
    // note: "-1" since the loop "+1" look ahead and if don't this, result has
    // one item shited to right in the result.
    // also use "sorted" index
    for (size_t sorted = 0; sorted < coll.size() - 1; ++sorted)
    {
      // if (coll[sorted] <= coll[sorted+1]) then already sorted, so no need to
      // swap. if not, swap it until it found a place
      if (coll[sorted] > coll[sorted + 1])
      {
        std::swap(coll[sorted], coll[sorted + 1]);

        // note: no need to keep searching to [0]. break if not.
        for (size_t search = sorted; search > 0; --search)
        {
          if (coll[search - 1] > coll[search])
            std::swap(coll[search - 1], coll[search]);
          else
            break;
        }
      }
    }
  }

  // old. use "unsorted" index, two for loop, and swap
  void sort_insertion_4(std::vector<int> &coll)
  {
    auto size = coll.size();

    for (size_t unsorted = 1; unsorted < size; ++unsorted)
    {
      for (size_t search = unsorted;
           search && (coll[search - 1] > coll[search]);
           --search)
      {
        std::swap(coll[search - 1], coll[search]);
      }
    }
  }

  void sort_insertion_5(vector<int> &coll)
  {
    size_t sorted{0}, unsorted{0};
    int target{0}, source{0};

    for (unsorted = 1; unsorted < coll.size();
         ++unsorted, sorted = unsorted - 1)
    {
      // ascending order
      for (target = sorted, source = unsorted;
           target >= 0 && coll[target] > coll[source];
           --target, --source)
      {
        std::swap(coll[target], coll[source]);
      }
    }
  }

  // *cxx-template-default-arguemnts*
  // cxx-greater
  // std::greater(x,y) { return __x > __y; }
  template <typename T, typename F = std::greater<T>>
  void sort_insertion_6(vector<T> &coll, F f = F())
  {
    size_t sorted{0}, unsorted{0};
    int target{0}, source{0};

    for (unsorted = 1; unsorted < coll.size();
         ++unsorted, sorted = unsorted - 1)
    {
      // ascending order
      for (target = sorted, source = unsorted;
           target >= 0 && f(coll[target], coll[source]);
           --target, --source)
      {
        std::swap(coll[target], coll[source]);
      }
    }
  }

  // From Programming Pearl 11.1
  //
  // for i = [1, n)
  //  // invariant: x[0..i-1] is sorted
  //  // shift x[i] down to its proper place in x[0..i]
  //
  // From the swap version which do two operations, move down elements and put the
  // saved back when loop ends as 01/02 version. So one update rather than doing
  // it every time when use swap(). so runs faster?
  //
  // Still "move down" so not sure how faster it will be. need to measure

  void sort_insertion_7(vector<int> &coll)
  {
    int size = (int)coll.size();

    for (int unsorted_index = 1; unsorted_index < size; ++unsorted_index)
    {
      int unsorted      = coll[unsorted_index];
      int current_index = unsorted_index;

      for (; 0 < current_index && unsorted < coll[current_index - 1];
           --current_index)
      {
        // swap current and current-1
        // swap(coll[current_index], coll[current_index-1]);
        coll[current_index] = coll[current_index - 1];
      }

      coll[current_index] = unsorted;
    }
  }

  // https://www.codeproject.com/Articles/854127/Top-Beautiful-Cplusplus-std-Algorithms-Examples
  // cxx-algo-rotate
  void sort_insertion_8(vector<int> &coll)
  {
    auto first = coll.begin();

    for (auto run = first; run != coll.end(); ++run)
      std::rotate(std::upper_bound(first, run, *run), run, next(run));
  }

  // 33  2 31 34  1  4 (6)
  //  1 33 31 34  2  4 (6)
  //  1  2 33 34 31  4 (6)
  //  1  2  4 34 33 31 (6)
  //  1  2  4 31 34 33 (6)
  //  1  2  4 31 33 34 (6)
  //  1  2  4 31 33 34 (6)
  //
  // "i" is sorted index and [sorted] gets bigger over iteration.
  //
  // coll[i] keeps changing then storing it to compare. that each iteration
  // finds the sorted element. not able to stop/break in "swap loop"

  void bubble_sort(vector<int> &cont)
  {
    PRINT_ELEMENTS(cont);

    for (int i = 0; i < (int)cont.size(); i++)
    {
      for (int j = i + 1; j < (int)cont.size(); j++)
      {
        if (cont[i] > cont[j])
        {
          std::swap(cont[i], cont[j]);
        }
      }

      PRINT_ELEMENTS(cont);
    }
  }
} // namespace algo_sort_insertion

TEST(AlgoSort, check_insertion)
{
  using namespace algo_sort_insertion;

  {
    auto imps = {sort_insertion_1,
                 sort_insertion_2,
                 sort_insertion_3,
                 sort_insertion_4,
                 sort_insertion_5,
                 // sort_insertion_6,
                 sort_insertion_7};

    for (const auto &f : imps)
    {
      std::vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};
      f(coll);
      EXPECT_THAT(
        coll,
        ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
    }
  }

  {
    std::vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};
    sort_insertion_6(coll);
    EXPECT_THAT(
      coll,
      ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }

  {
    std::vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};
    sort_insertion_6(coll, std::less<int>());
    EXPECT_THAT(
      coll,
      ElementsAreArray({33, 31, 30, 29, 17, 15, 13, 12, 10, 6, 5, 3, 2}));
  }

  {
    std::vector<int> coll{33, 2, 31, 34, 1, 4};
    bubble_sort(coll);
    EXPECT_THAT(coll, ElementsAreArray({1, 2, 4, 31, 33, 34}));
  }
}

/*
={=========================================================================
algo-partition algo-sort-insert

algo-partition which uses the same grouping trick as algo-sort-insert

Re-arrange the portfolio between (begin, end)  in such a way that all the stocks
with quantity <= maxQuantity precede all those with quantity > maxQuantity

Return the iterator to the first element with quantity > maxQuantity

[ <= max ][ > max]

*/

namespace algo_partition
{
  using PortfolioIterator = std::vector<unsigned int>::iterator;

  // maintain the order of input elements when grouping.
  // 2N space and 2N time(2 pass)
  PortfolioIterator my_partition_1(PortfolioIterator begin,
                                   PortfolioIterator end,
                                   unsigned int maxQuantity)
  {
    // implement me
    std::vector<unsigned int> PortfolioGreater, PortfolioLesser;
    PortfolioIterator run = begin;

    for (; run != end; ++run)
    {
      // greater
      // if( run->quantity > maxQuantity )
      if (*run > maxQuantity)
        PortfolioGreater.push_back(*run);
      else
        PortfolioLesser.push_back(*run);
    }

    run = begin;
    for (const auto &elem : PortfolioLesser)
    {
      *run++ = elem;
    }
    begin = run;

    for (const auto &elem : PortfolioGreater)
    {
      *run++ = elem;
    }

    return begin;
  }

  // NOT working. less space but still 2 pass
  PortfolioIterator my_partition_2(PortfolioIterator begin,
                                   PortfolioIterator end,
                                   unsigned int max_quanity)
  {
    // how to get T of coll such as algo-remove? here, assumes that we know T
    vector<unsigned int> coll;

    PortfolioIterator start = begin;
    PortfolioIterator current{};

    // one pass to filter <=

    for (; start != end; ++start)
    {
      // not use push_back() since void push_back()
      if (*start <= max_quanity)
        current = coll.insert(coll.end(), *start);
    }

    start = begin;

    // second pass to filter >

    for (; start != end; ++start)
    {
      if (*start > max_quanity)
        coll.push_back(*start);
    }

    // copy it back
    copy(coll.begin(), coll.end(), begin);

    // *cxx-vector-reallocation* *cxx-iter-invalidated*
    // *cxx-iter-singular* means invalidated iterator since there is no guarantee
    // that current is valid after second pass push_back due to relocation
    //
    // /usr/include/c++/6/debug/safe_iterator.h:298:
    // Error: attempt to increment a singular iterator.
    //
    // Objects involved in the operation:
    //     iterator "this" @ 0x0x7ffdeb5ea9a0 {
    //       type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<unsigned int*, std::__cxx1998::vector<unsigned int, std::allocator<unsigned int> > >,
    //          std::__debug::vector<unsigned int, std::allocator<unsigned int> > > (mutable iterator);
    //       state = singular;
    //       references sequence with type 'std::__debug::vector<unsigned int, std::allocator<unsigned int> >' @ 0x0x7ffdeb5eaa00
    //     }
    // Aborted
    //
    // return ++current;

    return current;
  }

  namespace algo_code
  {
    // /usr/include/c++/4.9.2/bits/stl_algo.h

    /**
     *  @brief Move elements for which a predicate is true to the beginning
     *         of a sequence.
     *  @ingroup mutating_algorithms
     *  @param  __first   A forward iterator.
     *  @param  __last    A forward iterator.
     *  @param  __pred    A predicate functor.
     *  @return  An iterator @p middle such that @p __pred(i) is true for each
     *  iterator @p i in the range @p [__first,middle) and false for each @p i
     *  in the range @p [middle,__last).
     *
     *  @p __pred must not modify its operand. @p partition() does not preserve
     *  the relative ordering of elements in each group, use
     *  @p stable_partition() if this is needed.
     */
    template <typename _ForwardIterator, typename _Predicate>
    inline _ForwardIterator partition(_ForwardIterator __first,
                                      _ForwardIterator __last,
                                      _Predicate __pred)
    {
      return std::__partition(__first,
                              __last,
                              __pred,
                              std::__iterator_category(__first));
    }

    /// This is a helper function...
    template <typename _ForwardIterator, typename _Predicate>
    _ForwardIterator __partition(_ForwardIterator __first,
                                 _ForwardIterator __last,
                                 _Predicate __pred,
                                 forward_iterator_tag)
    {
      if (__first == __last)
        return __first;

      while (__pred(*__first))
        if (++__first == __last)
          return __first;

      _ForwardIterator __next = __first;

      while (++__next != __last)
        if (__pred(*__next))
        {
          std::iter_swap(__first, __next);
          ++__first;
        }

      return __first;
    }
  } // namespace algo_code

  // same as std::partition
  template <typename _Iterator, typename _Compare>
  _Iterator my_partition_3(_Iterator begin, _Iterator end, _Compare comp)
  {
    if (begin == end)
      return end;

    // skip elements that are already matched and return an iterator of the
    // first unmatched. return end if all are matched.
    while (comp(*begin))
      if (++begin == end)
        return begin;

    // okay, found the unmatched from the loop above.
    _Iterator run = begin;

    while (++run != end)
    {
      if (comp(*run))
      {
        // note: not use "=" since it's not *cxx-algo-remove*
        std::iter_swap(run, begin);
        ++begin;
      }
    }

    return begin;
  }

  // since it do swap for elements that are already matched and are not needed
  // to swap, std::partition seems better.
  PortfolioIterator my_partition_x(PortfolioIterator begin,
                                   PortfolioIterator end,
                                   unsigned int max)
  {
    PortfolioIterator eog{begin};  // end of the group
    PortfolioIterator scan{begin}; // scan

    for (; scan != end; ++scan)
    {
      // found an item whicn is <= max. swap and incr eog.
      if (*scan <= max)
        std::iter_swap(eog++, scan);
    }

    return eog;
  }
} // namespace algo_partition

TEST(AlgoPartition, check_partition)
{
  using namespace algo_partition;

  // [ groups where f() returns true][ groups where f() returns false]
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto pos1 = std::partition(coll.begin(), coll.end(), [](int e) {
      return e % 2 == 0;
    });

    EXPECT_THAT(coll, ElementsAre(8, 2, 6, 4, 5, 3, 7, 1, 9));

    // there are four even numbers
    EXPECT_THAT(std::distance(coll.begin(), pos1), 4);
  }

  // if use stable version, it keeps the original order
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto pos1 = std::stable_partition(coll.begin(), coll.end(), [](int e) {
      return e % 2 == 0;
    });

    EXPECT_THAT(coll, ElementsAre(2, 4, 6, 8, 1, 3, 5, 7, 9));

    // there are four even numbers
    EXPECT_THAT(std::distance(coll.begin(), pos1), 4);
  }

  // use std::partition and the order do not kept.
  {
    std::vector<unsigned int> coll{43, 6,  11, 42, 29, 23, 21, 19, 34, 37,
                                   48, 24, 15, 20, 13, 26, 41, 30, 6,  23};

    auto pos1 =
      std::partition(coll.begin(), coll.end(), [](int e) { return e < 25; });

    // EXPECT_THAT(coll,
    //     ElementsAreArray({6,  11, 23, 21, 19, 24, 15, 20, 13, 6,
    //       23, 43, 42, 29, 34, 37, 48, 26, 41, 30}));

    EXPECT_THAT(std::distance(coll.begin(), pos1), 11);
  }

  // the order of the unmatched group is different but the distance should be
  // the same. so use std::distance().
  {
    auto imps = {my_partition_x};

    for (const auto &f : imps)
    {
      vector<unsigned int> coll{43, 6,  11, 42, 29, 23, 21, 19, 34, 37,
                                48, 24, 15, 20, 13, 26, 41, 30, 6,  23};

      auto ret = f(coll.begin(), coll.end(), 25);

      EXPECT_THAT(std::distance(coll.begin(), ret), 11);
    }
  }

  {
    std::vector<unsigned int> coll{43, 6,  11, 42, 29, 23, 21, 19, 34, 37,
                                   48, 24, 15, 20, 13, 26, 41, 30, 6,  23};

    auto ret = my_partition_3(coll.begin(), coll.end(), [](unsigned int value) {
      return value <= 25;
    });

    EXPECT_THAT(std::distance(coll.begin(), ret), 11);
  }
}

/*
={=========================================================================
algo-partition algo-remove
*/

namespace algo_remove
{
  // same as std::partition() but [unmatched][matched]

  namespace algo_code
  {
    // /usr/include/c++/4.9/bits/predefined_ops.h

    // check if when predicate is called, what arg iter refers to equals to the
    // value.

    template <typename _Value>
    struct _Iter_equals_val
    {
      _Value &_M_value;

      _Iter_equals_val(_Value &__value)
          : _M_value(__value)
      {}

      template <typename _Iterator>
      bool operator()(_Iterator __it)
      {
        return *__it == _M_value;
      }
    };

    template <typename _Value>
    inline _Iter_equals_val<_Value> __iter_equals_val(_Value &__val)
    {
      return _Iter_equals_val<_Value>(__val);
    }

    // /usr/include/c++/4.9/bits/stl_algo.h

    /**
     *  @brief Remove elements from a sequence.
     *  @ingroup mutating_algorithms
     *  @param  __first  An input iterator.
     *  @param  __last   An input iterator.
     *  @param  __value  The value to be removed.
     *  @return   An iterator designating the end of the resulting sequence.
     *
     *  All elements equal to @p __value are removed from the range
     *  @p [__first,__last).
     *
     *  remove() is stable, so the relative order of elements that are
     *  not removed is unchanged.
     *
     *  Elements between the end of the resulting sequence and @p __last
     *  are still present, but their value is unspecified.
     */
    template <typename _ForwardIterator, typename _Tp>
    inline _ForwardIterator remove(_ForwardIterator __first,
                                   _ForwardIterator __last,
                                   const _Tp &__value)
    {
      return std::__remove_if(__first,
                              __last,
                              __gnu_cxx::__ops::__iter_equals_val(__value));
    }

    template <typename _ForwardIterator, typename _Predicate>
    _ForwardIterator __remove_if(_ForwardIterator __first,
                                 _ForwardIterator __last,
                                 _Predicate __pred)
    {
      __first = std::__find_if(__first, __last, __pred);
      if (__first == __last)
        return __first;

      _ForwardIterator __result = __first;
      ++__first;
      for (; __first != __last; ++__first)
        if (!__pred(__first))
        {
          // note
          // this is `assign` but not `swap` which make a difference to own
          // remove does.

          *__result = _GLIBCXX_MOVE(*__first);
          ++__result;
        }
      return __result;
    }

    // /usr/include/c++/4.9/bits/predefined_ops.h
    template <typename _Predicate>
    struct _Iter_pred
    {
      _Predicate _M_pred;

      _Iter_pred(_Predicate __pred)
          : _M_pred(__pred)
      {}

      template <typename _Iterator>
      bool operator()(_Iterator __it)
      {
        return bool(_M_pred(*__it));
      }
    };

    template <typename _Predicate>
    inline _Iter_pred<_Predicate> __pred_iter(_Predicate __pred)
    {
      return _Iter_pred<_Predicate>(__pred);
    }

    template <typename _ForwardIterator, typename _Predicate>
    inline _ForwardIterator remove_if(_ForwardIterator __first,
                                      _ForwardIterator __last,
                                      _Predicate __pred)
    {
      return std::__remove_if(__first,
                              __last,
                              __gnu_cxx::__ops::__pred_iter(__pred));
    }
  } // namespace algo_code

  using _Iterator = std::vector<int>::iterator;

  // same as std::partition
  // use std::swap and do not use "begin++ or run++"; do not use side-effect
  _Iterator my_remove_1(_Iterator begin, _Iterator end, int value)
  {
    if (begin == end)
      return end;

    // skip elements which are not matched.
    while (*begin != value) // !comp
      if (++begin == end)
        return begin;

    _Iterator run = begin;

    while (++run != end)
    {
      if (*run != value) // !comp
      {
        std::iter_swap(run, begin);
        ++begin;
      }
    }

    return begin;
  }

  // use "="
  _Iterator my_remove_2(_Iterator begin, _Iterator end, int value)
  {
    if (begin == end)
      return end;

    // skip elements which are not matched.
    while (*begin != value)
      if (++begin == end)
        return begin;

    _Iterator run = begin;

    while (++run != end)
    {
      if (*run != value)
      {
        *begin = *run;
        ++begin;
      }
    }

    return begin;
  }

  // 1. do the same as algo-partition but from the end.
  // 2. therefore, do not care about the order of unmatched group.

  template <typename _Iterator, typename _T>
  _Iterator my_remove_3(_Iterator __begin, _Iterator __end, _T __value)
  {
    _Iterator run             = __end - 1;
    _Iterator start_of_remove = __end;

    for (; run > __begin; --run)
    {
      // swap only when element has the same value and swap is necessary
      if ((*run == __value) && (run != start_of_remove - 1))
        swap(*run, *(--start_of_remove));
    }

    // swap only when element has the same value and swap is necessary
    if ((*run == __value) && (run != start_of_remove - 1))
      swap(*run, *(--start_of_remove));

    return start_of_remove;
  }
} // namespace algo_remove

TEST(AlgoRemove, check_remove)
{
  using namespace algo_remove;

  {
    auto imps = {my_remove_1};

    for (const auto &f : imps)
    {
      std::vector<int> coll{1, 2, 3, 4, 5, 6, 2, 7, 2, 8, 2, 9};

      auto end = f(coll.begin(), coll.end(), 2);

      EXPECT_THAT(std::distance(end, coll.end()), 4);

      // use std::swap so different order
      EXPECT_THAT(coll, ElementsAreArray({1, 3, 4, 5, 6, 7, 8, 9, 2, 2, 2, 2}));

      coll.erase(end, coll.end());
      EXPECT_THAT(coll, ElementsAre(1, 3, 4, 5, 6, 7, 8, 9));
    }
  }

  {
    auto imps = {my_remove_2};

    for (const auto &f : imps)
    {
      std::vector<int> coll{1, 2, 3, 4, 5, 6, 2, 7, 2, 8, 2, 9};

      auto end = f(coll.begin(), coll.end(), 2);

      EXPECT_THAT(std::distance(end, coll.end()), 4);

      // use "="
      EXPECT_THAT(coll, ElementsAreArray({1, 3, 4, 5, 6, 7, 8, 9, 2, 8, 2, 9}));

      coll.erase(end, coll.end());
      EXPECT_THAT(coll, ElementsAre(1, 3, 4, 5, 6, 7, 8, 9));
    }
  }

  {
    {
      std::vector<int> coll{1, 2, 3, 4, 5, 6, 2, 7, 2, 8, 2, 9};

      auto end = my_remove_3(coll.begin(), coll.end(), 2);

      EXPECT_THAT(std::distance(end, coll.end()), 4);

      coll.erase(end, coll.end());
      EXPECT_THAT(coll, ElementsAreArray({1, 9, 3, 4, 5, 6, 8, 7}));
    }
  }
}

/*
={=========================================================================
algo-partition algo-unique

[matched(unique)][unmatched]
                  ^dest

use the dest which is the end of the matched group


1. std::unique() is not perferct

The first form removes from the range [beg,end) all elements that are equal
to `the previous elements.`

Thus, only when the elements in the sequence are *sorted*, or at least when
all elements of the same value are adjacent, it remove all duplicates.

Both forms collapse *consecutive* equal elements by removing the following
duplicates.

2. sorted input is not assumed

*/

namespace algo_unique
{
  // when see two consequtive equal items, return a iterator to the first.
  // 1. reutrn last when empty or when there is one item.
  // 1. change its name. otherwise ambiguous with one from <algorithm>
  template <typename _Iterator>
  _Iterator my_adjacent_find(_Iterator first, _Iterator last)
  {
    // when empty
    if (first == last)
      return last;

    _Iterator next = first;

    while (++next != last)
    {
      if (*first == *next)
        return first;
      first = next;
    }

    return last;
  }

  // /usr/include/c++/4.9/bits/stl_algo.h
  template <typename _Iterator>
  _Iterator my_unique_1(_Iterator first, _Iterator last)
  {
    // to find the first of a pair which has the same value. so can skip
    // elements which are already unique. if not use this like "_2" then do
    // assign for unique elements.
    first = my_adjacent_find(first, last);
    if (first == last)
      return last;

    _Iterator dest = first;

    // since already checked "*first" from my_adjacent_find() so increase it.
    ++first;

    while (++first != last)
    {
      // not equal and assign(overwrite). so if equals, keep increase first.
      if (*dest != *first)
        *++dest = *first;
    }

    // one after from the last unique
    return ++dest;
  }

  template <typename _Iterator>
  _Iterator my_unique_2(_Iterator begin, _Iterator end)
  {
    // when size is 1
    if (std::next(begin) == end)
      return end;

    _Iterator unique_end_ = begin;

    while (++begin != end)
    {
      // not equal and assign(overwrite). so if equals, keep increase first.
      if (*unique_end_ != *begin)
        *++unique_end_ = *begin;
    }

    return ++unique_end_;
  }

  // 2020.07
  // these iterator artimetic only works for random access coll so error for
  // std::list.
  //
  // _Iterator run_{begin+1};
  // return unique_end_+1;

  template <typename _Iterator>
  _Iterator my_unique_x(_Iterator begin, _Iterator end)
  {
    // when size is 1
    if (std::next(begin) == end)
      return end;

    _Iterator unique_end_{begin};
    _Iterator run_{std::next(begin)};

    for (; run_ != end; ++run_)
    {
      if (*unique_end_ != *run_)
        *(++unique_end_) = *run_;
    }

    return std::next(unique_end_);
  }
} // namespace algo_unique

TEST(AlgoUnique, check_unique)
{
  using namespace algo_unique;

  //
  {
    std::vector<int> coll{1, 4, 4, 6};

    auto pos = my_unique_x(coll.begin(), coll.end());

    EXPECT_THAT(std::distance(pos, coll.end()), 1);

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 6}));
  }

  {
    std::list<int>
      coll{1, 2, 3, 1, 2, 3, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 4, 4};

    auto pos = std::unique(coll.begin(), coll.end());
    EXPECT_THAT(coll,
                ElementsAreArray(
                  {1, 2, 3, 1, 2, 3, 4, 6, 1, 2, 3, 1, 6, 4, 6, 6, 6, 4, 4}));

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll,
                ElementsAreArray({1, 2, 3, 1, 2, 3, 4, 6, 1, 2, 3, 1, 6, 4}));
  }

  {
    std::list<int>
      coll{1, 2, 3, 1, 2, 3, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 4, 4};

    auto pos = my_unique_x(coll.begin(), coll.end());
    EXPECT_THAT(coll,
                ElementsAreArray(
                  {1, 2, 3, 1, 2, 3, 4, 6, 1, 2, 3, 1, 6, 4, 6, 6, 6, 4, 4}));

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll,
                ElementsAreArray({1, 2, 3, 1, 2, 3, 4, 6, 1, 2, 3, 1, 6, 4}));
  }

  //
  {
    std::vector<int> coll{1, 4, 4, 6};

    auto pos = my_unique_1(coll.begin(), coll.end());

    EXPECT_THAT(std::distance(pos, coll.end()), 1);

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 6}));
  }

  {
    std::list<int>
      coll{1, 2, 3, 1, 2, 3, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 4, 4};

    auto pos = my_unique_1(coll.begin(), coll.end());
    EXPECT_THAT(coll,
                ElementsAreArray(
                  {1, 2, 3, 1, 2, 3, 4, 6, 1, 2, 3, 1, 6, 4, 6, 6, 6, 4, 4}));

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll,
                ElementsAreArray({1, 2, 3, 1, 2, 3, 4, 6, 1, 2, 3, 1, 6, 4}));
  }

  //
  {
    std::vector<int> coll{1, 4, 4, 6};

    auto pos = my_unique_2(coll.begin(), coll.end());

    EXPECT_THAT(std::distance(pos, coll.end()), 1);

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 6}));
  }

  {
    std::list<int>
      coll{1, 2, 3, 1, 2, 3, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 4, 4};

    auto pos = my_unique_2(coll.begin(), coll.end());
    EXPECT_THAT(coll,
                ElementsAreArray(
                  {1, 2, 3, 1, 2, 3, 4, 6, 1, 2, 3, 1, 6, 4, 6, 6, 6, 4, 4}));

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll,
                ElementsAreArray({1, 2, 3, 1, 2, 3, 4, 6, 1, 2, 3, 1, 6, 4}));
  }

  // o The second form removes all elements that follow an element e and for
  // which the binary predicate op(e,elem) yields true. In other words, the
  // predicate is not used to compare an element with its predecessor; the
  // element is compared with the previous element that was not removed (see the
  // following examples).

  // For example, the first 6 is greater than the following 1, 2, 2, 3, and 1,
  // so all these elements are removed. In other words, the predicate is not
  // used to compare an element with its predecessor; the element is compared
  // with the previous element that was not removed

  {
    std::list<int> coll{1, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 5, 7, 5, 4, 4};

    auto pos = std::unique(coll.begin(), coll.end(), std::greater<int>());
    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 4, 6, 6, 6, 6, 7}));
  }

  {
    std::string input{"1   2  3            4           "};
    EXPECT_THAT(input, "1   2  3            4           ");

    auto new_end =
      std::unique(input.begin(), input.end(), [](const char &x, const char &y) {
        return x == y and x == ' ';
      });

    input.erase(new_end, input.end());
    EXPECT_THAT(input, "1 2 3 4 ");
  }

  // o sorted input is not assumed. ???
  {
    vector<int> coll{1, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 5, 7, 5, 4, 4};

    auto pos = std::unique(coll.begin(), coll.end());
    EXPECT_THAT(
      coll,
      ElementsAreArray({1, 4, 6, 1, 2, 3, 1, 6, 5, 7, 5, 4, 5, 7, 5, 4, 4}));

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 6, 1, 2, 3, 1, 6, 5, 7, 5, 4}));
  }
}

/*
={=========================================================================
algo-partition algo-unique algo-leetcode-8 algo-remove-duplicates

26. Remove Duplicates from Sorted Array, Easy

Given a sorted array nums, remove the duplicates in-place such that each element
appear only once and return *the new length.*

Do not allocate extra space for another array, you must do this by modifying the
input array in-place with O(1) extra memory.

Example 1:

Given nums = [1,1,2],

Your function should return length = 2, with the first two elements of nums
being 1 and 2 respectively.

It doesn't matter what you leave beyond the returned length.

Example 2:

Given nums = [0,0,1,1,1,2,2,3,3,4],

Your function should return length = 5, with the first five elements of nums
being modified to 0, 1, 2, 3, and 4 respectively.

It doesn't matter what values are set beyond the returned length.

Internally you can think of this:

// nums is passed in by reference. (i.e., without making a copy)
int len = removeDuplicates(nums);

// any modification to nums in your function would be known by the caller.
// using the length returned by your function, it prints the first len elements.

for (int i = 0; i < len; i++) {
    print(nums[i]);
}

1. key point is "sorted input"
 
*/

namespace leetcode_easy_008
{
  // o end is index but shold return len so +1 -> revised. As algo-partition,
  // end represet start of not-interested, that is end of interested group + 1.
  // so no need to +1. have to think about swap on the same index
  //
  // | ... |end ...|
  //
  // Runtime: 24 ms, faster than 100.00% of C++ online submissions for Remove
  // Duplicates from Sorted Array.
  //
  // Memory Usage: 11 MB, less than 24.81% of C++ online submissions for Remove
  // Duplicates from Sorted Array.
  //
  // 1. since the input is sorted, where see bigger element is when see
  // different element.
  // 1. use swap instead assign.

  int unique_1(vector<int> &nums)
  {
    if (nums.empty())
      return 0;

    int current_max = nums[0];
    size_t end{1};

    for (size_t i = 1; i < nums.size(); ++i)
    {
      if (nums[i] > current_max)
      {
        current_max = nums[i];

        // to avoid swap on the same index
        if (end != i)
          swap(nums[end], nums[i]);

        ++end;
      }
    }

    return end;
  }

  // same as my_unique
  int unique_2(std::vector<int> &coll)
  {
    // when size 1
    if (coll.size() == 1)
      return 1;

    auto unique_end = coll.begin();
    auto run        = coll.begin();
    auto last       = coll.end();

    while (++run != last)
    {
      if (*unique_end != *run)
        *++unique_end = *run;
    }

    return std::distance(coll.begin(), ++unique_end);
  }
} // namespace leetcode_easy_008

TEST(AlgoUnique, check_unique_leetcode_008)
{
  using namespace leetcode_easy_008;

  auto imps = {unique_1, unique_2};

  for (auto f : imps)
  {
    {
      std::vector<int> coll{1, 4, 4, 6};
      auto ret = f(coll);
      EXPECT_THAT(ret, 3);
    }

    {
      std::vector<int> coll{1, 4, 4, 4, 6};
      auto ret = f(coll);
      EXPECT_THAT(ret, 3);
    }

    {
      std::vector<int> coll{0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
      auto ret = f(coll);
      EXPECT_THAT(ret, 5);
    }

    {
      std::vector<int> coll{1, 1, 1, 2, 2, 3, 4, 4, 4, 4, 5, 5, 6, 6, 6, 6, 7};
      auto ret = f(coll);
      EXPECT_THAT(ret, 7);
    }
  }
}

/*
={=========================================================================
algo-partition algo-unique algo-leetcode-9

27. Remove Element, Easy

Given an array nums and a value val, remove all instances of that value in-place
and return the new length.

Do not allocate extra space for another array, you must do this by modifying the
input array in-place with O(1) extra memory.

The order of elements can be changed. It doesn't matter what you leave beyond
the new length.

Example 1:

Given nums = [3,2,2,3], val = 3,

Your function should return length = 2, with the first two elements of nums
being 2.

It doesn't matter what you leave beyond the returned length.

Example 2:

Given nums = [0,1,2,2,3,0,4,2], val = 2,

Your function should return length = 5, with the first five elements of nums
containing 0, 1, 3, 0, and 4.

Note that the order of those five elements can be arbitrary.

It doesn't matter what values are set beyond the returned length.

*/

namespace algo_remove_leetcode_easy_009
{
  // old
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Remove
  // Element.
  //
  // Memory Usage: 9.3 MB, less than 56.68% of C++ online submissions for Remove
  // Element.

  int my_remove_1(std::vector<int> &nums, int val)
  {
    if (nums.empty())
      return 0;

    size_t end{};

    for (size_t i = 0; i < nums.size(); ++i)
    {
      if (nums[i] != val)
      {
        // to avoid doing on the same.
        if (end != i)
          swap(nums[end], nums[i]);

        ++end;
      }
    }

    return end;
  }

  // use "assign"
  int my_remove_2(std::vector<int> &nums, int val)
  {
    size_t start{};
    size_t end{nums.size()};

    // find pos where the matched is found and which is pos to start "remove"
    while (nums[start] != val)
      ++start;

    for (size_t scan = start + 1; scan < end; ++scan)
    {
      // found the unmatched
      if (nums[scan] != val)
      {
        nums[start] = nums[scan];
        ++start;
      }
    }

    return start;
  }

  int my_remove_3(std::vector<int> &nums, int val)
  {
    size_t start{};
    size_t end{nums.size()};

    // find pos where the matched is found and which is pos to start "remove"
    // note: if all elements of nums are "unmatched", there should way to stop
    // loop as my_remove_1 of iterator version from algo-partition algo-remove
    while (nums[start] != val)
      if (++start == end)
        return start;

    for (size_t scan = start + 1; scan < end; ++scan)
    {
      // found the unmatched
      if (nums[scan] != val)
      {
        nums[start] = nums[scan];
        ++start;
      }
    }

    return start;
  }
} // namespace algo_remove_leetcode_easy_009

TEST(AlgoRemove, check_remove_leetcode_009)
{
  using namespace algo_remove_leetcode_easy_009;

  {
    auto imps = {my_remove_1, my_remove_2, my_remove_3};

    for (auto f : imps)
    {
      std::vector<int> coll{0, 1, 2, 2, 3, 0, 4, 2};

      auto len = f(coll, 2);

      EXPECT_THAT(len, 5);

      std::vector<int> result{};

      for (int i = 0; i < len; ++i)
        result.push_back(coll[i]);

      EXPECT_THAT(result, ElementsAre(0, 1, 3, 0, 4));
    }
  }

  {
    auto imps = {my_remove_1, my_remove_2, my_remove_3};

    for (const auto &f : imps)
    {
      std::vector<int> coll{1, 2, 3, 4, 5, 6, 2, 7, 2, 8, 2, 9};

      auto size = f(coll, 2);

      EXPECT_THAT(size, 8);
      // EXPECT_THAT(coll, ElementsAre(1, 3, 4, 5, 6, 7, 8, 9));
    }
  }
}

/*
={=========================================================================
algo-rotate, algo-slide, algo-reverse

1. do not use additional space. HOW?

note:
Hint is std::swap()

/usr/include/c++/4.9.2/bits/stl_algo.h

/// This is a helper function for the rotate algorithm.
template<typename _ForwardIterator>
  _ForwardIterator
  __rotate(_ForwardIterator __first,
     _ForwardIterator __middle,
     _ForwardIterator __last,
     forward_iterator_tag)
{}

        ne           e
1  2  3 [4  5  6  7]     #1 move
      4  3
         5  3
            6  3
      ne       7  3
   ne[4  5  6  7] 3      #2 move
  [4  5  6  7] 2  3      #3 move
4  5  6  7] 1  2  3


use reverse:

1  2  3 [4  5  6  7]
3  2  1 [7  6  5  4]
4  5  6  7  1  2  3

*/

TEST(AlgoRotate, check_rotate)
{
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};

    // rotate one to the left
    // before *cxx-11* void rotate() so comment out if that's the case

    auto pos = std::rotate(coll.begin(),     // begin
                           coll.begin() + 1, // new begin
                           coll.end()        // end
    );

    EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6, 7, 8, 1));

    // return the new position of the (pervious) first element.
    EXPECT_THAT(*pos, 1);
  }

  {
    std::vector<int> coll{2, 3, 4, 5, 6, 7, 8, 1};

    auto pos = std::rotate(coll.begin(), coll.end() - 2, coll.end());

    EXPECT_THAT(coll, ElementsAre(8, 1, 2, 3, 4, 5, 6, 7));

    EXPECT_THAT(*pos, 2);
  }

  {
    std::vector<int> coll{8, 1, 2, 3, 4, 5, 6, 7};

    // rotate so that 4 is the beginning
    // pos = rotate(

    auto pos = std::rotate(coll.begin(),
                           std::find(coll.begin(), coll.end(), 4),
                           coll.end());
    EXPECT_THAT(coll, ElementsAre(4, 5, 6, 7, 8, 1, 2, 3));
    EXPECT_THAT(*pos, 8);
  }
}

namespace algo_rotate
{
  template <typename _Iterator>
  void my_rotate_1(_Iterator __begin, _Iterator __new_begin, _Iterator __end)
  {
    if ((__begin == __new_begin) || (__end == __new_begin))
      return;

    auto num_swap = std::distance(__new_begin, __end);

    for (; __new_begin != __begin; --__new_begin)
    {
      _Iterator start = __new_begin;

      for (int i = 0; i < num_swap; ++i)
      {
        swap(*start, *(start - 1));
        ++start;
      }
    }
  }

  template <typename _Iterator>
  _Iterator my_rotate_2(_Iterator begin, _Iterator new_begin, _Iterator end)
  {
    // size of the block to move which will be used as loop count
    auto size = std::distance(new_begin, end);

    while (begin != new_begin)
    {
      for (auto run = new_begin, i = 0; i < size; ++i, run = std::next(run))
      {
        std::iter_swap(std::prev(run), run);
      }

      new_begin = std::prev(new_begin);
    }

    return std::next(begin, size);
  }

  template <typename _Iterator>
  void my_rotate_3(_Iterator begin, _Iterator new_begin, _Iterator end)
  {
    std::reverse(begin, new_begin);
    std::reverse(new_begin, end);
    std::reverse(begin, end);
  }
} // namespace algo_rotate

TEST(AlgoRotate, check_my_rotate)
{
  using namespace algo_rotate;

  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};

    // cannot use this since it's template
    // auto func = rotate_1;

    // rotate one to the left
    my_rotate_1(coll.begin(),     // begin
                coll.begin() + 1, // new begin
                coll.end()        // end
    );

    EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6, 7, 8, 1));

    my_rotate_1(coll.begin(), coll.end() - 2, coll.end());
    EXPECT_THAT(coll, ElementsAre(8, 1, 2, 3, 4, 5, 6, 7));

    my_rotate_1(coll.begin(), find(coll.begin(), coll.end(), 4), coll.end());
    EXPECT_THAT(coll, ElementsAre(4, 5, 6, 7, 8, 1, 2, 3));
  }

  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};

    my_rotate_2(coll.begin(),     // begin
                coll.begin() + 1, // new begin
                coll.end()        // end
    );

    EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6, 7, 8, 1));

    my_rotate_2(coll.begin(), coll.end() - 2, coll.end());
    EXPECT_THAT(coll, ElementsAre(8, 1, 2, 3, 4, 5, 6, 7));

    my_rotate_2(coll.begin(), find(coll.begin(), coll.end(), 4), coll.end());
    EXPECT_THAT(coll, ElementsAre(4, 5, 6, 7, 8, 1, 2, 3));
  }

  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};

    my_rotate_3(coll.begin(),     // begin
                coll.begin() + 1, // new begin
                coll.end()        // end
    );

    EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6, 7, 8, 1));

    my_rotate_3(coll.begin(), coll.end() - 2, coll.end());
    EXPECT_THAT(coll, ElementsAre(8, 1, 2, 3, 4, 5, 6, 7));

    my_rotate_3(coll.begin(), find(coll.begin(), coll.end(), 4), coll.end());
    EXPECT_THAT(coll, ElementsAre(4, 5, 6, 7, 8, 1, 2, 3));
  }
}

/* 
algo-leetcode-189, Rotate Array, Easy

Given an array, rotate the array to the right by k steps, where k is
non-negative.

Example 1:

Input: [1,2,3,4,5,6,7] and k = 3
Output: [5,6,7,1,2,3,4]

Explanation:
rotate 1 steps to the right: [7,1,2,3,4,5,6]
rotate 2 steps to the right: [6,7,1,2,3,4,5]
rotate 3 steps to the right: [5,6,7,1,2,3,4]

Example 2:

Input: [-1,-100,3,99] and k = 2
Output: [3,99,-1,-100]

Explanation: 
rotate 1 steps to the right: [99,-1,-100,3]
rotate 2 steps to the right: [3,99,-1,-100]

Note:
Try to come up as many solutions as you can, there are at least 3 different ways
to solve this problem.  Could you do it in-place with O(1) extra space?

*/

namespace leetcode_easy_189
{
  // k poins to new end which is size()-k in pos

  void my_rotate_1(vector<int> &nums, int k)
  {
    int new_begin = nums.size() - k;

    // note that must be > 0 but not >= 0
    for (; new_begin > 0; --new_begin)
    {
      for (int index = new_begin, i = 0; i < k; ++i, ++index)
        swap(nums[index - 1], nums[index]);
    }
  }

  // index :  0   1   2   3   4   5   6
  // arr   :  1   2   3   4   5   6   7
  // k     :  7   6   5   4   3   2   1
  //          14  13  12  11  10  9   8
  //          21  20  19  18  17  16  15
  //
  // so except cases that falls to index 0 (% makes 0), others works with "%
  // size"
  //
  // However, it's too slow

  void my_rotate_2(vector<int> &nums, int k)
  {
    // note
    // as with condition check in algo-rotate, no need to when it rotates to
    // itself since it's same as before.

    int count = k % nums.size();

    if (count == 0)
      return;

    int new_begin = nums.size() - count;

    // note that must be > 0 but not >= 0
    for (; new_begin > 0; --new_begin)
    {
      for (int index = new_begin, i = 0; i < count; ++i, ++index)
        swap(nums[index - 1], nums[index]);
    }
  }

  // why faster? less swap() operations.
  //
  // 1  2  3 [4  5  6  7]
  //
  // 12 swaps for moving. 6 swaps for reversing.
  //
  // Runtime: 16 ms, faster than 100.00% of C++ online submissions for Rotate
  // Array.
  //
  // Memory Usage: 9.6 MB, less than 35.88% of C++ online submissions for Rotate
  // Array.

  void my_rotate_3(vector<int> &nums, int k)
  {
    // as with condition check in algo-rotate, no need to when it rotates to
    // itself.

    int count = k % nums.size();

    if (count == 0)
      return;

    int new_begin = nums.size() - count;

    std::reverse(nums.begin(), nums.begin() + new_begin);
    std::reverse(nums.begin() + new_begin, nums.end());
    std::reverse(nums.begin(), nums.end());
  }

  // algo-code algo-reverse
  // stl_algo.h
  //
  // /**
  //  *  This is an uglified reverse(_BidirectionalIterator,
  //  *                              _BidirectionalIterator)
  //  *  overloaded for bidirectional iterators.
  // */
  // template <typename _BidirectionalIterator>
  // void __reverse(_BidirectionalIterator __first,
  //                _BidirectionalIterator __last,
  //                bidirectional_iterator_tag)
  // {
  //   while (true)
  //     if (__first == __last || __first == --__last)
  //       return;
  //     else
  //     {
  //       std::iter_swap(__first, __last);
  //       ++__first;
  //     }
  // }

  template <typename _Iterator>
  void my_reverse_1(_Iterator begin, _Iterator end)
  {
    while (true)
    {
      // since assumes "BidirectionalIterator". decrease end.
      if (begin == end || begin == --end)
        return;
      else
      {
        std::iter_swap(begin, end);
        ++begin; // increase begin
      }
    }
  }
} // namespace leetcode_easy_189

TEST(AlgoRotate, check_rotate_leetcode_189)
{
  using namespace leetcode_easy_189;

  auto imps = {my_rotate_1, my_rotate_2, my_rotate_3};

  for (auto f : imps)
  {
    {
      std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8};

      f(coll, 7);
      EXPECT_THAT(coll, ElementsAre(2, 3, 4, 5, 6, 7, 8, 1));

      f(coll, 2);
      EXPECT_THAT(coll, ElementsAre(8, 1, 2, 3, 4, 5, 6, 7));

      f(coll, 4);
      EXPECT_THAT(coll, ElementsAre(4, 5, 6, 7, 8, 1, 2, 3));
    }

    {
      vector<int> coll{1, 2, 3, 4, 5, 6, 7};
      f(coll, 3);
      EXPECT_THAT(coll, ElementsAre(5, 6, 7, 1, 2, 3, 4));
    }

    {
      vector<int> coll{-1, -100, 3, 99};
      f(coll, 2);
      EXPECT_THAT(coll, ElementsAre(3, 99, -1, -100));
    }
  }

  {
    // no
    {
      std::vector<int> coll{1, 2};
      my_rotate_1(coll, 3);
      EXPECT_THAT(coll, Not(ElementsAre(2, 1)));
    }

    // ok
    {
      std::vector<int> coll{1, 2};
      my_rotate_2(coll, 3);
      EXPECT_THAT(coll, ElementsAre(2, 1));
    }

    // ok
    {
      std::vector<int> coll{1, 2};
      my_rotate_3(coll, 3);
      EXPECT_THAT(coll, ElementsAre(2, 1));
    }
  }
}

// ={=========================================================================
// algo-sort-quick

namespace algo_sort_quick
{

  // #define SORT_QUICK_DEBUG

  int build_partition(vector<int> &coll, int first, int last)
  {
    int pivot_pos   = (first + last) / 2;
    int pivot_value = coll[pivot_pos];

#ifdef SORT_QUICK_DEBUG
    cout << "pivot_pos(" << pivot_pos << ") build before(" << first << ", "
         << last << "): ";

    for (int i = first; i <= last; ++i)
      cout << coll[i] << ", ";

    cout << endl;
#endif // SORT_QUICK_DEBUG

    // move the pivot to the first pos
    swap(coll[first], coll[pivot_pos]);

    int last_small = first;

    for (int inspect = first + 1; inspect <= last; ++inspect)
    {
      // *cxx-less*
      if (coll[inspect] < pivot_value)
      {
        ++last_small;

        // last_small == inspect case does happens and it is enhancement from the
        // previous code

        if (last_small != inspect)
        {
          cout << "swap(coll[" << coll[last_small] << "], coll["
               << coll[inspect] << "]);" << endl;
          swap(coll[last_small], coll[inspect]);
        }
      }
    }

    // move the pivot back
    swap(coll[first], coll[last_small]);

#ifdef SORT_QUICK_DEBUG
    cout << "pivot_pos(" << pivot_pos << ") build after (" << first << ", "
         << last << "): ";

    for (int i = first; i <= last; ++i)
      cout << coll[i] << ", ";

    cout << endl;
#endif // SORT_QUICK_DEBUG

    return last_small;
  }

  int build_partition_descending(vector<int> &coll, int first, int last)
  {
    int pivot_pos   = (first + last) / 2;
    int pivot_value = coll[pivot_pos];

    // move the pivot to the first pos
    swap(coll[first], coll[pivot_pos]);

    int last_small = first;

    for (int inspect = first + 1; inspect <= last; ++inspect)
    {
      // *cxx-greater*
      if (coll[inspect] > pivot_value)
      {
        ++last_small;

        // last_small == inspect case does happens and it is enhancement from the
        // previous code

        if (last_small != inspect)
        {
          swap(coll[last_small], coll[inspect]);
        }
      }
    }

    // move the pivot back
    swap(coll[first], coll[last_small]);

    return last_small;
  }

  void sort_quick_01(vector<int> &coll, int first, int last)
  {
    int last_small{};

    if (first < last)
    {
      last_small = build_partition(coll, first, last);
      sort_quick_01(coll, first, last_small - 1);
      sort_quick_01(coll, last_small + 1, last);
    }
  }

  void sort_quick_01_descending(vector<int> &coll, int first, int last)
  {
    int last_small{};

    if (first < last)
    {
      last_small = build_partition_descending(coll, first, last);
      sort_quick_01_descending(coll, first, last_small - 1);
      sort_quick_01_descending(coll, last_small + 1, last);
    }
  }

  // from ansic, p87. exactly same way.
  void sort_quick_02(vector<int> &v, int left, int right)
  {
    int i, last;

    // do nothing if array contains fewer than two elements
    if (left >= right)
      return;

    // move partition elem
    // swap( v, left, (left+right)/2 );
    swap(v[left], v[(left + right) / 2]);

    last = left; // to v[0]

    // partition
    for (i = left + 1; i <= right; i++)
      if (v[i] < v[left])
      {
        // swap( v, ++last, i );   // shall ++last
        swap(v[++last], v[i]); // shall ++last
      }

    // restore partition elem
    // swap(v, left, last);
    swap(v[left], v[last]);

    sort_quick_02(v, left, last - 1);
    sort_quick_02(v, last + 1, right);
  }

} // namespace algo_sort_quick

TEST(AlgoSort, Quick)
{
  using namespace algo_sort_quick;

  // pivot_pos(3) build before(0, 6): 29, 33, 35, |26|, 19, 12, 22,
  // pivot_pos(3) build after (0, 6): 22, 19, 12, 26, 33, 35, 29,
  // pivot_pos(1) build before(0, 2): 22, |19|, 12,
  // pivot_pos(1) build after (0, 2): 12, 19, 22,
  // pivot_pos(5) build before(4, 6):                 33, |35|, 29,
  // pivot_pos(5) build after (4, 6):                 29, 33, 35,
  // pivot_pos(4) build before(4, 5):                 |29|, 33,
  // pivot_pos(4) build after (4, 5):                 29, 33,
  //                                  12  19  22  26  29  33  35

  {
    vector<int> coll{29, 33, 35, 26, 19, 12, 22};

    sort_quick_01(coll, 0, coll.size() - 1);

    EXPECT_THAT(coll, ElementsAre(12, 19, 22, 26, 29, 33, 35));
  }

  {
    vector<int> coll{29, 33, 35, 26, 19, 12, 22};

    sort_quick_01_descending(coll, 0, coll.size() - 1);

    EXPECT_THAT(coll, ElementsAre(35, 33, 29, 26, 22, 19, 12));
  }

  // build i(0, 12): 30, 2, 31, 5, 33, 6, 12, 10, 13, 15, 17, 29, 6,
  // build o(0, 12): 6, 2, 5, 6, 10, 12, 30, 33, 13, 15, 17, 29, 31,
  // build i(0, 4): 6, 2, 5, 6, 10,
  // build o(0, 4): 2, 5, 6, 6, 10,
  // build i(2, 4): 6, 6, 10,
  // build o(2, 4): 6, 6, 10,
  // build i(3, 4): 6, 10,
  // build o(3, 4): 6, 10,
  // build i(6, 12): 30, 33, 13, 15, 17, 29, 31,
  // build o(6, 12): 13, 15, 33, 30, 17, 29, 31,
  // build i(8, 12): 33, 30, 17, 29, 31,
  // build o(8, 12): 17, 30, 33, 29, 31,
  // build i(9, 12): 30, 33, 29, 31,
  // build o(9, 12): 31, 30, 29, 33,
  // build i(9, 11): 31, 30, 29,
  // build o(9, 11): 29, 30, 31,

  {
    vector<int> coll{30, 2, 31, 5, 33, 6, 12, 10, 13, 15, 17, 29, 6};

    sort_quick_01(coll, 0, coll.size() - 1);

    EXPECT_THAT(
      coll,
      ElementsAreArray({2, 5, 6, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }

  // when input is already sorted
  //
  // build i(0, 12): 2, 5, 6, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33,
  // build o(0, 12): 2, 5, 6, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33,
  // build i(0, 5): 2, 5, 6, 6, 10, 12,
  // build o(0, 5): 2, 5, 6, 6, 10, 12,
  // build i(0, 1): 2, 5,
  // build o(0, 1): 2, 5,
  // build i(3, 5): 6, 10, 12,
  // build o(3, 5): 6, 10, 12,
  // build i(7, 12): 15, 17, 29, 30, 31, 33,
  // build o(7, 12): 15, 17, 29, 30, 31, 33,
  // build i(7, 8): 15, 17,
  // build o(7, 8): 15, 17,
  // build i(10, 12): 30, 31, 33,
  // build o(10, 12): 30, 31, 33,

  {
    vector<int> coll{2, 5, 6, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};

    sort_quick_01(coll, 0, coll.size() - 1);

    EXPECT_THAT(
      coll,
      ElementsAreArray({2, 5, 6, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }
}

// ={=========================================================================
// algo-sort-heap

namespace algo_sort_heap
{
  // From 'Programming Pearls' by Jon Bentley, 14 Heap
  //
  // pre  : heap(1, n-1)
  // post : heap(1, n)
  //
  // void siftup(n)
  // {
  //   int i = n;
  //
  // loop:
  //   // invariant: heap(1, n) except between i and its parent
  //
  //   // terminates when reaches the root
  //   if (i == 1)
  //     break;
  //
  //   p = i / 2;
  //
  //   // terminates when it meets heap property
  //   if (x[i] >= x[p])
  //     break;
  //
  //   swap(i, p);
  //
  //   i = p;
  // }

  // while version
  void siftup_01(vector<int> &coll, int n)
  {
    int i = n;
    int p = i / 2;

    for (; (i != 1) && (coll[i] <= coll[p]);)
    {
      swap(coll[i], coll[p]);
      i = p;
      p = i / 2;
    }
  }

  void siftup_02(vector<int> &coll, int n)
  {
    int i = n;
    int p = i / 2;

    while (i != 1 && coll[i] <= coll[p])
    {
      swap(coll[i], coll[p]);
      i = p;
      p = i / 2;
    }
  }

  // // for version, (cxx-error)
  // void siftup_01(vector<int> &coll, int n)
  // {
  //   int i = n;
  //   int p = i / 2;
  //
  //   for (; (i != 1) && (coll[i] <= coll[p]); i = p)
  //   {
  //     swap(coll[i], coll[p]);
  //     p = i / 2;
  //   }
  //
  //   // effectively this and should update p after i so this is an error
  //   //
  //   // for (; (i != 1) && (coll[i] <= coll[p]);)
  //   // {
  //   //   swap(coll[i], coll[p]);
  //   //   p = i / 2;
  //   //   i = p;
  //   // }
  // }

  // // for version, (cxx-error)
  // void siftup_01_error(vector<int> &coll, int n)
  // {
  //   int i = n;
  //   int p = i / 2;
  //
  //   // for (; (i != 1) && (coll[i] <= coll[p]); i = p)
  //   // {
  //   //   p = i / 2;
  //   //   swap(coll[i], coll[p]);
  //   // }
  //
  //   // effectively this and cause error since has different pair of i and p in
  //   // check of the lopp.
  //   for (; (i != 1) && (coll[i] <= coll[p]);)
  //   {
  //     p = i / 2;
  //     swap(coll[i], coll[p]);
  //     i = p;
  //   }
  // }

  // for version from text
  void siftup_min_text(vector<int> &coll, int n)
  {
    int i, p;
    for (i = n; i > 1 && coll[p = i / 2] > coll[i]; i = p)
      swap(coll[i], coll[p]);
  }

  // pre  : heap(2, n)
  // post : heap(1, n)
  //
  // void siftdown(n)
  // {
  //   int i = 1;
  //
  // loop:
  //   // invariant: heap(1, n) except between i and its children(0, 1, or 2)
  //
  //   // c is index of the left child
  //   c = i * 2;
  //
  //   // terminates when no children
  //   if (c > n)
  //     break;
  //
  //   // when the right child is lesser
  //   if (c+1 <= n)
  //    if (x[c] > x[c+1])
  //      c++;
  //
  //   // terminates when it meets heap property
  //   if (x[i] <= x[c])
  //     break;
  //
  //   swap(i, c);
  //
  //   i = c;
  // }

  void siftdown_01(vector<int> &coll, int n)
  {
    int i = 1;
    int c = i * 2;

    if (c + 1 <= n)
      if (coll[c] > coll[c + 1])
        c++;

    while ((c < n) && (coll[i] > coll[c]))
    {
      swap(coll[i], coll[c]);

      i = c;

      c = i * 2;

      if (c + 1 <= n)
        if (coll[c] > coll[c + 1])
          c++;
    }
  }

  // void siftdown_01(vector<int> &coll, int n)
  // {
  //   int i = 1;
  //   int c = i * 2;
  //
  //   if (c+1 <= n)
  //     if (coll[c] > coll[c+1])
  //       c++;
  //
  //   for (; (c < n) && (coll[i] > coll[c]); i = c)
  //   {
  //     swap(coll[i], coll[c]);
  //
  //     if (c+1 <= n)
  //       if (coll[c] > coll[c+1])
  //         c++;
  //
  //     c = i * 2;
  //   }
  // }

  void siftdown_min_text(vector<int> &coll, int n)
  {
    int c;

    for (int i = 1; (c = i * 2) <= n; i = c)
    {
      if (c + 1 <= n && coll[c] > coll[c + 1])
        c++;

      if (coll[i] <= coll[c])
        break;

      swap(coll[i], coll[c]);
    }
  }
} // namespace algo_sort_heap

TEST(AlgoSort, HeapSiftUp)
{
  using namespace algo_sort_heap;

  {
    // not use coll[0], 13 elements
    vector<int> coll{0, 12, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    // +1, 14 elements
    coll.push_back(13);

    // range is [1, 13]
    siftup_01(coll, 13);

    //                   {0, 12,/20, 13,/29, 23, 15, 22,/35, 40, 26, 51, 19, 17}
    EXPECT_THAT(coll,
                ElementsAreArray(
                  {0, 12, 20, 13, 29, 23, 15, 22, 35, 40, 26, 51, 19, 17}));
  }

  {
    // not use coll[0], 13 elements
    vector<int> coll{0, 12, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    // +1, 14 elements
    coll.push_back(13);

    siftup_min_text(coll, 13);

    //                   {0, 12,/20, 13,/29, 23, 15, 22,/35, 40, 26, 51, 19, 17}
    EXPECT_THAT(coll,
                ElementsAreArray(
                  {0, 12, 20, 13, 29, 23, 15, 22, 35, 40, 26, 51, 19, 17}));
  }
}

TEST(AlgoSort, HeapSiftDown)
{
  using namespace algo_sort_heap;

  {
    // when did x[1] = 18
    vector<int> coll{0, 18, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    siftdown_01(coll, coll.size() - 1);
    EXPECT_THAT(
      coll,
      ElementsAreArray({0, 15, 20, 17, 29, 23, 18, 22, 35, 40, 26, 51, 19}));
  }
  {
    // when did x[1] = 18
    vector<int> coll{0, 18, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    siftdown_min_text(coll, coll.size() - 1);
    EXPECT_THAT(
      coll,
      ElementsAreArray({0, 15, 20, 17, 29, 23, 18, 22, 35, 40, 26, 51, 19}));
  }
}

namespace algo_sort_heap
{
  // template <typename T>
  // class PriQueue {
  //   public:
  //     PriQueue(int m)
  //     {
  //       maxsize_ = m;
  //       x_ = new T[maxsize_ +1];
  //       n_ = 0;
  //     }
  //
  //     void insert(T t)
  //     {
  //       if (n_ >= maxsize_)
  //         error;
  //
  //       x_[++n] = t;
  //
  //       // pre : heap(1, n-1)
  //       siftup();
  //       // post: heap(1, n)
  //     }
  //
  //     int extractmin()
  //     {
  //       T t;
  //
  //       if (n_ < 1)
  //         error;
  //
  //       t = x_[1];
  //
  //       x_[1] = x[n--];
  //
  //       // pre : heap(2, n)
  //       siftdown();
  //       // post: heap(1, n)
  //
  //       return t;
  //     }
  //
  //   private:
  //     int n_, maxsize_;
  //     T *x_;
  // };

  template <typename T>
  class PriQueue
  {
  public:
    PriQueue(int size)
    {
      maxsize_ = size;
      coll_    = new T[maxsize_ + 1];
      n_       = 0;
    }
    ~PriQueue()
    {
      if (coll_)
        delete coll_;
      n_ = 0;
    }

    void insert(T value)
    {
      int parent{};

      // error
      if (n_ >= maxsize_)
        return;

      coll_[++n_] = value;

      // heap(1, n-1)
      // siftup()
      // heap(1, n)
      // siftup(n)
      for (int i = n_; i > 1 && coll_[i] < coll_[parent = i / 2]; i = parent)
        swap(coll_[i], coll_[parent]);
    }

    int extractmin()
    {
      T min_value{};
      int child{};

      // error
      if (n_ < 1)
        return -1;

      min_value = coll_[1];

      coll_[1] = coll_[n_--];

      // heap(2, n)
      // siftdown(n)
      // heap(1,n)

      for (int i = 1; (child = i * 2) <= n_; i = child)
      {
        if (child + 1 <= n_ && coll_[child] > coll_[child + 1])
          child++;

        if (coll_[i] <= coll_[child])
          break;

        swap(coll_[i], coll_[child]);
      }

      return min_value;
    }

  private:
    int n_, maxsize_;
    T *coll_;
  };

  // simply use `opposite` comparisons
  template <typename T>
  class PriQueueMax
  {
  public:
    PriQueueMax(int size)
    {
      maxsize_ = size;
      coll_    = new T[maxsize_ + 1];
      n_       = 0;
    }
    ~PriQueueMax()
    {
      if (coll_)
        delete coll_;
      n_ = 0;
    }

    void insert(T value)
    {
      int parent{};

      // error
      if (n_ >= maxsize_)
        return;

      coll_[++n_] = value;

      // heap(1, n-1)
      // siftup()
      // heap(1, n)
      // siftup(n)
      for (int i = n_; i > 1 && coll_[i] > coll_[parent = i / 2]; i = parent)
        swap(coll_[i], coll_[parent]);
    }

    int extractmin()
    {
      T min_value{};
      int child{};

      // error
      if (n_ < 1)
        return -1;

      min_value = coll_[1];

      coll_[1] = coll_[n_--];

      // heap(2, n)
      // siftdown(n)
      // heap(1,n)

      for (int i = 1; (child = i * 2) <= n_; i = child)
      {
        if (child + 1 <= n_ && coll_[child] < coll_[child + 1])
          child++;

        if (coll_[i] >= coll_[child])
          break;

        swap(coll_[i], coll_[child]);
      }

      return min_value;
    }

  private:
    int n_, maxsize_;
    T *coll_;
  };

  template <typename T, typename F = std::less<T>>
  class PriQueueTemplate
  {
  public:
    PriQueueTemplate(int size)
    {
      maxsize_ = size;
      coll_    = new T[maxsize_ + 1];
      n_       = 0;
      f_       = F();
    }
    ~PriQueueTemplate()
    {
      if (coll_)
        delete coll_;
      n_ = 0;
    }

    void insert(T value)
    {
      int parent{};

      // error
      if (n_ >= maxsize_)
        return;

      coll_[++n_] = value;

      // heap(1, n-1)
      // siftup()
      // heap(1, n)
      // siftup(n)
      for (int i = n_; i > 1 && f_(coll_[i], coll_[parent = i / 2]); i = parent)
        swap(coll_[i], coll_[parent]);
    }

    int extractmin()
    {
      T min_value{};
      int child{};

      // error
      if (n_ < 1)
        return -1;

      min_value = coll_[1];

      coll_[1] = coll_[n_--];

      // heap(2, n)
      // siftdown(n)
      // heap(1,n)

      for (int i = 1; (child = i * 2) <= n_; i = child)
      {
        if (child + 1 <= n_ && !f_(coll_[child], coll_[child + 1]))
          child++;

        if (f_(coll_[i], coll_[child]) || coll_[i] == coll_[child])
          break;

        swap(coll_[i], coll_[child]);
      }

      return min_value;
    }

  private:
    int n_, maxsize_;
    T *coll_;
    F f_;
  };

} // namespace algo_sort_heap

TEST(AlgoSort, HeapPriQueue)
{
  using namespace algo_sort_heap;

  // min
  {
    vector<int> coll;

    int n = 10;
    PriQueue<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(i);

    for (int i = 0; i < n; ++i)
      coll.push_back(pq.extractmin());

    EXPECT_THAT(coll, ElementsAreArray({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
  }
  {
    vector<int> coll{4, 5, 6, 7, 8, 9, 1, 2, 3, 0};
    vector<int> collo;

    int n = 10;
    PriQueue<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
  }

  // max
  {
    vector<int> coll;

    int n = 10;
    PriQueueMax<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(i);

    for (int i = 0; i < n; ++i)
      coll.push_back(pq.extractmin());

    EXPECT_THAT(coll, ElementsAreArray({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
  }
  {
    vector<int> coll{4, 5, 6, 7, 8, 9, 1, 2, 3, 0};
    vector<int> collo;

    int n = 10;
    PriQueueMax<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
  }

  // min, template
  {
    vector<int> coll;

    int n = 10;
    PriQueueTemplate<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(i);

    for (int i = 0; i < n; ++i)
      coll.push_back(pq.extractmin());

    EXPECT_THAT(coll, ElementsAreArray({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
  }
  {
    vector<int> coll{4, 5, 6, 7, 8, 9, 1, 2, 3, 0};
    vector<int> collo;

    int n = 10;
    PriQueueTemplate<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
  }

  // max, template
  {
    vector<int> coll;

    int n = 10;
    PriQueueTemplate<int, std::greater<int>> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(i);

    for (int i = 0; i < n; ++i)
      coll.push_back(pq.extractmin());

    EXPECT_THAT(coll, ElementsAreArray({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
  }
  {
    vector<int> coll{4, 5, 6, 7, 8, 9, 1, 2, 3, 0};
    vector<int> collo;

    int n = 10;
    PriQueueTemplate<int, std::greater<int>> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
  }
}

/*

heap is one-based array; the easiest approach is to declare x[n+1] and waste
x[0]. Can use [0]  so that heapsort can be used to sort any input without
consdiering [0]? NO since "root == 1" affects how child and parent index is
calculated.

The simple one uses two arrays, one for the priority queue and one for the
element to be sorted.

Heapsort saves space by using just one. Two stage process:

1. build input into a heap
establish heap(1, n), by sifting elements to move up front in the array, the one
single array.

  | heap -> | ?  |
  1         i    n

[0] is not used and [1] so put from [2] into heap. 

2. the next n step extract the elements in `decreasing` order and build the
sorted

  | heap | <- sorted  |
  1      i            n

*/

namespace algo_sort_heap
{
  void sort_heap_min(vector<int> &coll)
  {
    int n = coll.size() - 1;

    // convert input to heap
    for (int i = 2; i <= n; ++i)
      siftup_min_text(coll, i);

    // // convert heap to sorted
    // for (int i = n; i > 1; --i)
    // {
    //   swap(coll[1], coll[i]);
    //   siftdown_min_text(coll, i-1);
    // }

    // convert heap to sorted
    for (int i = n; i > 1;)
    {
      swap(coll[1], coll[i]);
      siftdown_min_text(coll, --i);
    }
  }
} // namespace algo_sort_heap

TEST(AlgoSort, HeapSort)
{
  using namespace algo_sort_heap;

  {
    // add the dummpy at 0 to make heap
    vector<int> coll{0, 4, 5, 6, 7, 8, 9, 1, 2, 3, 0};

    sort_heap_min(coll);

    // descending order
    EXPECT_THAT(coll, ElementsAreArray({0, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
  }

  // max verison will make ascending order
  //
  // {
  //   // add the dummpy at 0 to make heap
  //   vector<int> coll{0,4,5,6,7,8,9,1,2,3,0};
  //
  //   sort_heap_02(coll);
  //
  //   EXPECT_THAT(coll, ElementsAreArray({0,0,1,2,3,4,5,6,7,8,9}));
  // }
  // {
  //   vector<int> coll{ 0, 33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3 };
  //   sort_heap_02(coll);
  //   EXPECT_THAT(coll,
  //       ElementsAreArray({0, 2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33 }));
  // }
}

// ={=========================================================================
// algo-sort-merge

// merge(combine) sorted input list

TEST(AlgoSort, MergeSortedList)
{
  std::list<int> coll1{26, 33, 35, 29};
  std::list<int> coll2{9, 12, 22};
  std::list<int> result{};

  auto first  = coll1.begin();
  auto second = coll2.begin();

  for (; first != coll1.end() && (second != coll2.end());)
  {
    // ascending order
    if (*first <= *second)
    {
      result.push_back(*first);
      ++first;
    }
    else
    {
      result.push_back(*second);
      ++second;
    }
  }

  // second has some left
  if (first == coll1.end() && second != coll2.end())
    result.splice(result.end(), coll2, second, coll2.end());
  // first has some left
  else if (first != coll1.end() && second == coll2.end())
    result.splice(result.end(), coll1, first, coll1.end());
  else
  {
    // no left from the both
  }

  // combined
  EXPECT_THAT(result, ElementsAre(9, 12, 22, 26, 33, 35, 29));
}

/*
={=========================================================================
algo-search-binary-search
*/

namespace algo_binary_search
{
  // To sum:
  // 1. GT version end loop when "first == end"
  // 2. GT and iterator version work for both returning bool and returing index
  // to insert.
  // 3. GT and index version works differently for both returning bool and
  // returning index.
  // 4. GT version check if it's found when ends the loop
  //
  // Q:should have empty input check? YES, if not, do access to first like
  // "*first"

  // GT(Greater) version, use iterator, return bool.
  //
  // NOTE: access to end, "*end" is okay?
  //
  // NOTE: since distance(first, last) exclude last, "last = mid" is missing out
  // last elements? `first` gets updated and will be the right pos.
  //
  // if (value <= *mid)
  // {
  //   last = mid;
  // }

  template <typename _Iterator, typename _T>
  bool my_binary_search_1_1(_Iterator first, _Iterator last, const _T value)
  {
    while (first < last)
    {
      _Iterator mid = first + std::distance(first, last) / 2;

      if (value <= *mid)
      {
        last = mid;
      }
      else if (value > *mid)
      {
        first = std::next(mid);
      }
    }

    return value == *first ? true : false;
  }

  // NOTE: has "equal check" in the loop.
  template <typename _Iterator, typename _T>
  bool my_binary_search_1_1_1(_Iterator first, _Iterator last, const _T value)
  {
    while (first < last)
    {
      _Iterator mid = first + std::distance(first, last) / 2;

      if (value == *mid)
      {
        return true;
      }
      else if (value <= *mid)
      {
        last = mid;
      }
      else if (value > *mid)
      {
        first = std::next(mid);
      }
    }

    return value == *first ? true : false;
  }

  // GT(Greater) version, use iterator, return index.
  // return index which it is found or is to insert if not found.
  template <typename _Iterator, typename _T>
  size_t my_binary_search_1_2(_Iterator first, _Iterator last, const _T value)
  {
    _Iterator start{first};

    while (first < last)
    {
      _Iterator mid = first + std::distance(first, last) / 2;

      // std::cout << "middle : " << std::distance(first, last) / 2 << std::endl;

      if (value <= *mid)
      {
        last = mid;
      }
      else if (value > *mid)
      {
        first = std::next(mid);
      }
    }

    return std::distance(start, first);
  }

  // GT(Greater) version, use index, return index.
  // return index which it is found or is to insert if not found.
  template <typename _T>
  size_t my_binary_search_1_3(vector<_T> &coll, const _T value)
  {
    size_t first = 0;
    size_t last  = coll.size() - 1;

    while (first < last)
    {
      size_t mid = (first + last) / 2;

      // std::cout << "middle : " << mid << std::endl;

      if (value <= coll[mid])
      {
        last = mid;
      }
      else if (value > coll[mid])
      {
        first = ++mid;
      }
    }

    return first;
  }

  // GT(Greater) version, use index, return *bool*.
  template <typename _T>
  bool my_binary_search_1_4(vector<_T> &coll, const _T value)
  {
    size_t first = 0;
    size_t last  = coll.size() - 1;

    while (first < last)
    {
      size_t mid = (first + last) / 2;

      // std::cout << "middle : " << mid << std::endl;

      if (value <= coll[mid])
      {
        last = mid;
      }
      else if (value > coll[mid])
      {
        first = ++mid;
      }
    }

    return value == coll[first] ? true : false;
  }

  // To sum:
  // 1. EQ ends loop when first > last.
  // 2. EQ check if it's found while in the loop
  //
  // ansic, p58
  // cracking the coding interview, p120
  // Programming Pearl, p46
  //
  // note:
  //
  // *cxx-undefined* since can be negative
  // size_t low{}; size_t high{}; size_t mid{};
  //
  // it can cause `overflow` when mid gets minus value
  // mid = (high-low)/2 + low;
  //
  // it has the same as distance() in iterator version or can use
  // length approach as stl version.

  // EQ(Equality) version, use index, return index
  int my_binary_search_2_1(vector<int> &coll, int key)
  {
    int low{};
    int high{};
    int mid{};

    low  = 0;
    high = coll.size() - 1;

    while (low <= high)
    {
      mid = (low + high) / 2;

      if (key == coll[mid])
        return mid;
      else if (key < coll[mid])
        high = mid - 1;
      else
        low = mid + 1;
    }

    // to return index
    return low;
  }

  int my_binary_search_2_2(vector<int> &coll, int key)
  {
    int low{};
    int high{};
    int mid{};

    low  = 0;
    high = coll.size() - 1;

    while (low <= high)
    {
      mid = (low + high) / 2;

      if (key == coll[mid])
        return true;
      else if (key < coll[mid])
        high = mid - 1;
      else
        low = mid + 1;
    }

    return false;
  }

  // EQ(Equality) version, use iterator, return bool
  template <typename _Iterator, typename _T>
  bool my_binary_search_2_3(_Iterator begin, _Iterator end, _T const key)
  {
    while (begin <= end)
    {
      auto middle = std::distance(begin, end) / 2;

      // cout << "while: middle : " << middle << endl;

      if (key == *(begin + middle))
        return true;
      else if (key > *(begin + middle))
        begin = begin + middle + 1;
      else
        end = begin + middle - 1;
    }

    // when not found only
    return false;
  }

  // EQ(Equality) version, use iterator, return index
  //
  // NOTE:
  // 1. access end() value.
  //
  // it sometimes fail or sometimes succeed because "*input_end" can be either
  // bigger or smaller then key value. if that's bigger then key, "begin" stays
  // the same which is "input_end" so pass. if not, begin advances and fails.
  //
  // EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 7), 4);
  //
  // 2. "<=" on iterator which assumes random access so limited.
  // quote: In particular, keep in mind that you can use operator < as an end
  // criterion in loops for random-access iterators only.

  template <typename _Iterator, typename _T>
  _T my_binary_search_2_4(_Iterator begin, _Iterator end, _T const key)
  {
    _Iterator saved_begin = begin;

    while (begin <= end)
    {
      auto middle = std::distance(begin, end) / 2;

      // cout << "while: middle : " << middle << endl;

      if (key == *(begin + middle))
        return std::distance(saved_begin, begin + middle);
      else if (key > *(begin + middle))
        begin = begin + middle + 1;
      else
        end = begin + middle - 1;
    }

    // when not found only
    return std::distance(saved_begin, begin);
  }

  // NOTE:
  // To sum, use EQ and index version.

} // namespace algo_binary_search

TEST(AlgoSearchBinarySearch, check_binary_search)
{
  using namespace algo_binary_search;

  // cxx-binary-search, stl version
  //
  // 11.10 Sorted-Range Algorithms
  //
  // The following algorithms search certain values in sorted ranges. Checking
  // Whether One Element Is Present
  //
  // bool binary_search (ForwardIterator beg, ForwardIterator end, const T&
  // value)
  //
  // bool binary_search (ForwardIterator beg, ForwardIterator end, const T&
  // value, BinaryPredicate op)
  {
    //                     0  1  2  3   4   5   6   7   8   9  10  11  12
    std::vector<int> coll1{2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};
    EXPECT_THAT(std::binary_search(coll1.cbegin(), coll1.cend(), 15), true);
    EXPECT_THAT(std::binary_search(coll1.cbegin(), coll1.cend(), 32), false);

    //                     0  1  2  3
    std::vector<int> coll2{1, 3, 5, 6};
    EXPECT_THAT(std::binary_search(coll2.cbegin(), coll2.cend(), 5), true);
    EXPECT_THAT(std::binary_search(coll2.cbegin(), coll2.cend(), 2), false);
  }

  //                    0  1  2  3
  std::vector<int> coll{1, 3, 5, 6};

  // GT(Greater) version, use iterator, return bool.
  {
    EXPECT_THAT(my_binary_search_1_1(coll.cbegin(), coll.cend(), 5), true);
    EXPECT_THAT(my_binary_search_1_1(coll.cbegin(), coll.cend(), 2), false);
    EXPECT_THAT(my_binary_search_1_1(coll.cbegin(), coll.cend(), 7), false);
    EXPECT_THAT(my_binary_search_1_1(coll.cbegin(), coll.cend(), 0), false);
  }

  {
    EXPECT_THAT(my_binary_search_1_1_1(coll.cbegin(), coll.cend(), 5), true);
    EXPECT_THAT(my_binary_search_1_1_1(coll.cbegin(), coll.cend(), 2), false);
    EXPECT_THAT(my_binary_search_1_1_1(coll.cbegin(), coll.cend(), 7), false);
    EXPECT_THAT(my_binary_search_1_1_1(coll.cbegin(), coll.cend(), 0), false);
  }

  // GT(Greater) version, use iterator, return index.
  {
    EXPECT_THAT(my_binary_search_1_2(coll.cbegin(), coll.cend(), 5), 2);
    EXPECT_THAT(my_binary_search_1_2(coll.cbegin(), coll.cend(), 2), 1);
    EXPECT_THAT(my_binary_search_1_2(coll.cbegin(), coll.cend(), 7), 4);
    EXPECT_THAT(my_binary_search_1_2(coll.cbegin(), coll.cend(), 0), 0);
  }

  // GT(Greater) version, use index, return index.
  {
    EXPECT_THAT(my_binary_search_1_3(coll, 5), 2);
    EXPECT_THAT(my_binary_search_1_3(coll, 2), 1);

    // fails
    // EXPECT_THAT(my_binary_search_1_3(coll, 7), 4);
    // wrong value
    EXPECT_THAT(my_binary_search_1_3(coll, 7), 3);

    EXPECT_THAT(my_binary_search_1_3(coll, 0), 0);
  }

  // GT(Greater) version, use index, return *bool*.
  {
    EXPECT_THAT(my_binary_search_1_4(coll, 5), true);
    EXPECT_THAT(my_binary_search_1_4(coll, 2), false);
    EXPECT_THAT(my_binary_search_1_4(coll, 7), false);
    EXPECT_THAT(my_binary_search_1_4(coll, 0), false);
  }

  // EQ(Equality) version, use index, return index
  {
    EXPECT_THAT(my_binary_search_2_1(coll, 5), 2);
    EXPECT_THAT(my_binary_search_2_1(coll, 2), 1);
    EXPECT_THAT(my_binary_search_2_1(coll, 7), 4);
    EXPECT_THAT(my_binary_search_2_1(coll, 0), 0);
  }

  // EQ(Equality) version, use index, return bool
  {
    EXPECT_THAT(my_binary_search_2_2(coll, 5), true);
    EXPECT_THAT(my_binary_search_2_2(coll, 2), false);
    EXPECT_THAT(my_binary_search_2_2(coll, 7), false);
    EXPECT_THAT(my_binary_search_2_2(coll, 0), false);
  }

  // EQ(Equality) version, use iterator, return bool
  {
    EXPECT_THAT(my_binary_search_2_3(coll.cbegin(), coll.cend(), 5), true);
    EXPECT_THAT(my_binary_search_2_3(coll.cbegin(), coll.cend(), 2), false);
    EXPECT_THAT(my_binary_search_2_3(coll.cbegin(), coll.cend(), 7), false);
    EXPECT_THAT(my_binary_search_2_3(coll.cbegin(), coll.cend(), 0), false);
  }

  // EQ(Equality) version, use iterator, return index
  {
    EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 5), 2);
    EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 2), 1);

    // *cxx-undefined*
    // EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 7), 4);

    EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 0), 0);
  }
}

namespace algo_binary_search
{
  // /usr/include/c++/4.9/bits/stl_algobase.h
  // /**
  //  *  @brief Finds the first position in which @p __val could be inserted
  //  *         without changing the ordering.
  //  *  @ingroup binary_search_algorithms
  //  *  @param  __first   An iterator.
  //  *  @param  __last    Another iterator.
  //  *  @param  __val     The search term.
  //  *  @param  __comp    A functor to use for comparisons.
  //  *  @return An iterator pointing to the first element <em>not less
  //  *           than</em> @p __val, or end() if every element is less
  //  *           than @p __val.
  //  *  @ingroup binary_search_algorithms
  //  *
  //  *  The comparison function should have the same effects on ordering as
  //  *  the function used for the initial sort.
  // */
  //
  // template<typename _ForwardIterator, typename _Tp>
  //   inline _ForwardIterator
  //   lower_bound(
  //      _ForwardIterator __first,
  //      _ForwardIterator __last,
  //      const _Tp& __val)
  //   {
  //     return std::__lower_bound(__first, __last, __val,
  //         __gnu_cxx::__ops::__iter_less_val());
  //   }

  // exact copy from stl but removes _Compare
  // 1. use "len" in the loop
  // 2. move "first"

  template <typename _ForwardIterator, typename _Tp>
  _ForwardIterator __my_lower_bound(_ForwardIterator __first,
                                    _ForwardIterator __last,
                                    const _Tp &__val)
  {
    typedef
      typename iterator_traits<_ForwardIterator>::difference_type _DistanceType;

    _DistanceType __len = std::distance(__first, __last);

    while (__len > 0)
    {
      _DistanceType __half      = __len >> 1;
      _ForwardIterator __middle = __first;
      std::advance(__middle, __half);

      // middle < val
      // if (__comp(__middle, __val))
      if (*__middle < __val)
      {
        // advance first
        __first = __middle;

        // do not include `middle`
        ++__first;
        __len = __len - __half - 1;
      }
      else
        __len = __half;
    }

    return __first;
  }

  template <typename _ForwardIterator, typename _Tp>
  inline _ForwardIterator my_lower_bound(_ForwardIterator __first,
                                         _ForwardIterator __last,
                                         const _Tp &__val)
  {
    return __my_lower_bound(__first, __last, __val);
  }

  // if implements binary search using lower_bound() way
  template <typename _Iterator, typename _T>
  _T my_binary_search_3(_Iterator first, _Iterator last, _T const value)
  {
    auto _saved_first{first};
    auto _length = std::distance(first, last);

    // _length is "distance between two elements"; (b,e)
    while (_length > 0)
    {
      decltype(_length) _half = _length >> 1;
      _Iterator _middle       = first;
      std::advance(_middle, _half);

      if (*_middle < value)
      {
        first = _middle;
        ++first;
        _length = _length - _half - 1;
      }
      else
      {
        _length = _half;
      }
    }

    // return found index or index to insert
    return std::distance(_saved_first, first);
  }

  // template<typename _ForwardIterator, typename _Tp>
  //   bool
  //   binary_search(_ForwardIterator __first, _ForwardIterator __last,
  //       const _Tp& __val)
  //   {
  //     typedef typename iterator_traits<_ForwardIterator>::value_type
  //       _ValueType;
  //
  //     _ForwardIterator __i
  //       = std::__lower_bound(__first, __last, __val,
  //           __gnu_cxx::__ops::__iter_less_val());
  //     return __i != __last && !(__val < *__i);
  //   }

  // since __my_lower_bound() can return `end`. so if found, then it should be
  // in [begin, end). also `iterator for element >= value. so two checks.
  //
  // return __i != __last && !(__val < *__i);

  template <typename _ForwardIterator, typename _Tp>
  bool my_binary_search_4(_ForwardIterator __first,
                          _ForwardIterator __last,
                          const _Tp &__val)
  {
    _ForwardIterator __i = __my_lower_bound(__first, __last, __val);

    return __i != __last && !(__val < *__i);
  }
} // namespace algo_binary_search

TEST(AlgoSearch, check_binary_search_stl_version)
{
  //  0  1  2  3  4  5  6  7  8  9  0  1  2
  // {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9};
  //                          ^^^^^^^
  // lower_bound() returns the position of the first element that has a value
  // equal to or greater than `value`. This is the first position where an
  // element with value could get inserted without breaking the sorting of the
  // range [beg,end).

  // since lower_bound() uses cbegin().
  //
  // : error: no matching function for call to
  // ‘distance(std::vector<int>::iterator, __gnu_cxx::__normal_iterator<const int*, std::vector<int> >&)’
  //
  // EXPECT_THAT(distance(coll.begin(), first), 8);

  // this is not member function as:
  // std::map<Key,T,Compare,Allocator>::lower_bound
  // std::set<Key,Compare,Allocator>::lower_bound

  {
    std::vector<int> coll{1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9};

    auto pos = std::lower_bound(coll.cbegin(), coll.cend(), 5);
    EXPECT_THAT(*pos, 5);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 8);
  }

  {
    std::vector<int> coll{2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};

    auto pos = std::lower_bound(coll.cbegin(), coll.cend(), 1);
    EXPECT_THAT(*pos, 2);
  }

  // // EQ(Equality) version, use iterator, return index
  // {
  //   EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 5), 2);
  //   EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 2), 1);
  //   // *cxx-undefined*
  //   // EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 7), 4);
  //   EXPECT_THAT(my_binary_search_2_4(coll.cbegin(), coll.cend(), 0), 0);
  // }

  // can do this with std::lower_bound()? YES
  {
    std::vector<int> coll{1, 3, 5, 6};

    auto pos = std::lower_bound(coll.cbegin(), coll.cend(), 5);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 2);

    pos = std::lower_bound(coll.cbegin(), coll.cend(), 2);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 1);

    pos = std::lower_bound(coll.cbegin(), coll.cend(), 7);
    // equals to end
    EXPECT_THAT(coll.cend(), pos);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 4);

    pos = std::lower_bound(coll.cbegin(), coll.cend(), 0);
    // equals to begin
    EXPECT_THAT(coll.cbegin(), pos);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 0);
  }

  {
    using namespace algo_binary_search;

    std::vector<int> coll{1, 3, 5, 6};

    auto pos = my_lower_bound(coll.cbegin(), coll.cend(), 5);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 2);

    pos = my_lower_bound(coll.cbegin(), coll.cend(), 2);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 1);

    pos = my_lower_bound(coll.cbegin(), coll.cend(), 7);
    // equals to end
    EXPECT_THAT(coll.cend(), pos);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 4);

    pos = my_lower_bound(coll.cbegin(), coll.cend(), 0);
    // equals to begin
    EXPECT_THAT(coll.cbegin(), pos);
    EXPECT_THAT(std::distance(coll.cbegin(), pos), 0);
  }

  // return index
  {
    using namespace algo_binary_search;

    std::vector<int> coll{1, 3, 5, 6};

    EXPECT_THAT(my_binary_search_3(coll.cbegin(), coll.cend(), 5), 2);
    EXPECT_THAT(my_binary_search_3(coll.cbegin(), coll.cend(), 2), 1);
    EXPECT_THAT(my_binary_search_3(coll.cbegin(), coll.cend(), 7), 4);
    EXPECT_THAT(my_binary_search_3(coll.cbegin(), coll.cend(), 0), 0);
  }

  {
    using namespace algo_binary_search;

    std::vector<int> coll2{1, 3, 5, 6};

    EXPECT_THAT(my_binary_search_4(coll2.cbegin(), coll2.cend(), 5), true);
    EXPECT_THAT(my_binary_search_4(coll2.cbegin(), coll2.cend(), 2), false);
  }

  // NOTE: stl version looks better.
}

/*
={=========================================================================
algo-search-binary-search algo-leetcode-17

69. Sqrt(x), Easy

Implement int sqrt(int x).

Compute and return the square root of x, where x is guaranteed to be a
non-negative integer.

Since the return type is an integer, the decimal digits are truncated and only
the integer part of the result is returned.

Example 1:
Input: 4
Output: 2

Example 2:
Input: 8
Output: 2
Explanation: The square root of 8 is 2.82842..., and since 
             the decimal part is truncated, 2 is returned.

NAME
       sqrt, sqrtf, sqrtl - square root function

SYNOPSIS
       #include <math.h>

       double sqrt(double x);
       float sqrtf(float x);
       long double sqrtl(long double x);


>>> import math
>>> print(math.sqrt(4))
2.0

>>> print(math.sqrt(8))
2.82842712475

>>> print(math.sqrt(9))
3.0

>>> print(math.sqrt(15))
3.87298334621

>>> print(math.sqrt(16))
4.0

*/

namespace leetcode_easy_017
{
  // use binary search to get "floor" square root
  //
  // https://www.geeksforgeeks.org/square-root-of-an-integer/
  //
  // O(logn)
  //
  //  0   1       mid   sqrt(x)                 x = sqrt(x)*sqrt(x)
  //  |---|--------|-------|--------------------|

  int get_sqrt_1(int x)
  {
    // base cases
    if (x == 0 || x == 1)
      return x;

    // starts from 1 since is covered in base cases and exclude x.
    int first{1};
    int last{x - 1};
    int ans{};

    while (first <= last)
    {
      int mid = (first + last) / 2;

      // equal and it's perfect square root
      if (mid * mid == x)
        return mid;
      else if (mid * mid < x)
      {
        // discard [1, mid]
        first = mid + 1;

        // to return "floor" value
        ans = mid;
      }
      else
      {
        // discard [mid, last]
        last = mid - 1;
      }
    }

    // when the loop ends, return "floor" value than "not found"
    return ans;
  }

  // Note: The Binary Search can be further optimized to start with ‘start’ = 0
  // and ‘end’ = x/2.
  // Floor of square root of x cannot be more than x/2 when x > 1.
  //
  // cxx-error-overflow Line 18: Char 15: runtime error: signed integer
  // overflow: 536848899 * 536848899 cannot be represented in type 'int'
  // (solution.cpp)
  //
  // where does overflow happends? since "mid*mid" used in comparisons. NO,
  // there is "int mid"
  //
  // >>> import math
  // >>> print(math.sqrt(536848899))
  // 23169.9999784
  //
  // _1 and _2 fail
  //
  // test_pad.cpp:159: Failure
  // Value of: f(536848899)
  // Expected: is equal to 4
  //   Actual: 266320211 (of type int)
  //
  // test_pad.cpp:159: Failure
  // Value of: f(536848899)
  // Expected: is equal to 4
  //   Actual: 268402686 (of type int)

  int get_sqrt_2(int x)
  {
    int first{0};
    int last{x / 2};
    int ans{};

    while (first <= last)
    {
      int mid = (first + last) / 2;

      // equal and it's perfect square root
      if (mid * mid == x)
        return mid;
      else if (mid * mid < x)
      {
        // discard [1, mid]
        first = mid + 1;

        // to return "floor" value
        ans = mid;
      }
      else
      {
        // discard [mid, last]
        last = mid - 1;
      }
    }

    // when the loop ends, return "floor" value than "not found"
    return ans;
  }

  // Runtime: 24 ms, faster than 21.88% of C++ online submissions for Sqrt(x).
  //
  // Memory Usage: 13.9 MB, less than 49.57% of C++ online submissions for
  // Sqrt(x).

  int get_sqrt_3(int x)
  {
    int first{0};
    int last{x / 2};
    int ans{};

    // base cases. NOTE: must have
    if (x == 0 || x == 1)
      return x;

    while (first <= last)
    {
      long long mid    = (first + last) / 2;
      long long square = mid * mid;

      // equal and it's perfect square root
      if (square == x)
        return mid;
      else if (square < x)
      {
        // discard [1, mid]
        first = mid + 1;

        // to return "floor" value
        ans = mid;
      }
      else
      {
        // discard [mid, last]
        last = mid - 1;
      }
    }

    // when the loop ends, return "floor" value than "not found"
    return ans;
  }

  // NOTE: having square variable for mid * mid causes performance penalty?
  //
  // Runtime: 12 ms, faster than 99.18% of C++ online submissions for Sqrt(x).
  //
  // Memory Usage: 13.8 MB, less than 84.46% of C++ online submissions for
  // Sqrt(x).

  int get_sqrt_4(int x)
  {
    int first{0};
    int last{x / 2};
    int ans{};

    // base cases
    if (x == 0 || x == 1)
      return x;

    while (first <= last)
    {
      long long mid = (first + last) / 2;

      // equal and it's perfect square root
      if (mid * mid == x)
        return mid;
      else if (mid * mid < x)
      {
        // discard [1, mid]
        first = mid + 1;

        // to return "floor" value
        ans = mid;
      }
      else
      {
        // discard [mid, last]
        last = mid - 1;
      }
    }

    // when the loop ends, return "floor" value than "not found"
    return ans;
  }
} // namespace leetcode_easy_017

TEST(AlgoSearchBinarySearch, check_sqrt)
{
  using namespace leetcode_easy_017;

  // when use sqrt() function
  {
    // 2
    // 2.82843
    // 3.16228
    // 4

    EXPECT_DOUBLE_EQ(sqrt(4), 2);
    EXPECT_NEAR(sqrt(8), 2.82843, 0.00001);

    // Expected equality of these values:
    //   sqrt(10)
    //     Which is: 3.1622776601683795
    //   3.16228
    // EXPECT_DOUBLE_EQ(sqrt(10), 3.16228);

    EXPECT_NEAR(sqrt(10), 3.16228, 0.00001);
    EXPECT_DOUBLE_EQ(sqrt(16), 4);
  }

  {
    // since _1 and _2 fails on the last case
    // auto imps = {get_sqrt_1, get_sqrt_2};

    auto imps = {get_sqrt_3, get_sqrt_4};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(4), 2);
      EXPECT_THAT(f(8), 2);
      EXPECT_THAT(f(10), 3);
      EXPECT_THAT(f(16), 4);
      EXPECT_THAT(f(536848899), 23169);

      // >>> 46339*46339, floor
      // 2,147,302,921
      //
      // 2,147,395,599
      //
      // >>> 46340*46340, ceiling
      // 2,147,395,600
      EXPECT_THAT(f(2147395599), 46339);
      EXPECT_NEAR(sqrt(2147395599), 46340, 0.1);
    }
  }
}

// really see performance difference between them? not really from simple run
// below. maybe we need to have random to simulate

TEST(AlgoSearchBinarySearch, check_sqrt_performance_1)
{
  using namespace leetcode_easy_017;

  auto f = get_sqrt_3;

  for (int i = 0; i < 1000000; ++i)
  {
    EXPECT_THAT(f(1), 1);
    EXPECT_THAT(f(2147395599), 46339);
  }
}

TEST(AlgoSearchBinarySearch, check_sqrt_performance_2)
{
  using namespace leetcode_easy_017;

  auto f = get_sqrt_4;

  for (int i = 0; i < 1000000; ++i)
  {
    EXPECT_THAT(f(1), 1);
    EXPECT_THAT(f(2147395599), 46339);
  }
}

/*
={=========================================================================

TODO: skip for now for full study

algo-search-binary-tree

10.1.1 Definitions

The linked list have great advantages of flexibility over contiguous
implementation but have one weak feature: *list-contiguous-and-linked* they are
sequential list; that is, have to access through them only one position at a
time, not random access.

Tree overcome this and is valuable for problems of information retrieval.


*adt-binary-tree*

A binary tree is either empty, or consist of a node called the "root" together
with two binary tree called the 'left-subtree' and the 'right-subtree' of the
root.

The left and right are important for binary tree and has recursive nature that
allows "empty binary tree" and the empty tree is base case for recursive and
determine when to stop.

note: 2-tree is different from binary tree since 2-tree has always 0 or 2
children but never 1.

Excercise: get all fourteen binary trees with four nodes.


10.1.2 Traversal of Binary Trees

There are many different traversal orders for trees and reduced to 'three' by
permitting only the ways in which the left is traversed before the right.

The (V)isiting a node, traversing the left subtree L, and the right subtree R.
<note> that visit, traverse and subtree in wording since traverse is not visit.

VLR(preorder) LVR(inorder) LRV(postorder)
^              ^             ^

Do this order for every node so has recursive nature. For example, traverse the
following tree

  ()1
    ()2
  ()3
()4  ()5

pre 12345, in 14352 and post 45321


={=========================================================================
algo-leetcode-19

100. Same Tree, Easy

Given two binary trees, write a function to check if they are the same or not.

Two binary trees are considered the same if they are structurally identical and
the nodes have the same value.

Example 1:

Input:     1         1
          / \       / \
         2   3     2   3

        [1,2,3],   [1,2,3]

Output: true

Example 2:

Input:     1         1
          /           \
         2             2

        [1,2],     [1,null,2]

Output: false

Example 3:

Input:     1         1
          / \       / \
         2   1     1   2

        [1,2,1],   [1,1,2]

Output: false


Definition for a binary tree node.

struct TreeNode {
   int val;
   TreeNode *left;
   TreeNode *right;
   TreeNode() : val(0), left(nullptr), right(nullptr) {}
   TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
   TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
public:
    bool isSameTree(TreeNode* p, TreeNode* q);
};
 
*/

namespace algo_binary_tree
{
  enum class ErrorCode
  {
    success,
    overflow,
    underflow,
    duplicate,
    not_present
  };

  // 10.1.3 Linked Implementation of Binary Trees

  // if want to express "Entry"
  // template <typename Entry>

  template <typename T>
  struct BinaryNode
  {
    BinaryNode<T> *_left;
    BinaryNode<T> *_right;
    T _data;

    BinaryNode()
        : _left{nullptr}
        , _right{nullptr}
    {}

    BinaryNode(const T &data)
        : _left{nullptr}
        , _right{nullptr}
        , _data(data)
    {}
  };

  // NOTE: do not implement full interface shown in the text since the text do
  // not have full code and left them as exercise.

  template <typename T>
  class BinaryTree
  {
  protected:
    BinaryNode<T> *_root;

  public:
    // ctor to make empty binary tree
    BinaryTree()
        : _root{nullptr}
    {}

    // true if tree is empty
    bool empty() const { return nullptr == _root; }

    // Recursion will make it especially easy for us to traverse the subtrees.
    // function * visit that does the desired task for each node.
    void inorder(void (*visit)(T &)) { recursive_inorder(_root, visit); }

  private:
    void recursive_inorder(BinaryNode<T> *root, void (*visit)(T &))
    {
      if (nullptr != root)
      {
        recursive_inorder(root->_left, visit);
        visit(root->_data);
        recursive_inorder(root->_right, visit);
      }
    }

    // void recursive_preorder(BinaryNode<T> *root, void(*visit)(T&))
    // void recursive_postorder(BinaryNode<T> *root, void(*visit)(T&))
  };

  // NOTE that the entries in a binary search tree must have keys that can be
  // compared
  //
  // 10.2.2 Tree Search
  //
  // NOTE BST can have different forms.
  //
  // Suppose, as an example, that we apply binary search to the list of seven
  // letters a , b , c , d , e , f , and g . The resulting tree is shown in part
  // (a) of Figure 10.8. If tree_search is applied to this tree, it will do the
  // same number of comparisons as binary search.
  //
  // It is quite possible, however, that the same letters may be built into a
  // binary search tree of a quite different shape, such as any of those shown
  // in the remaining parts of Figure 10.8.
  //
  // The tree shown as part (a) of Figure 10.8 is the best possible for
  // searching. It is as “bushy” as possible: It has the smallest possible
  // height for a given number of vertices. The number of vertices between the
  // root and the target, inclusive, is the number of comparisons that must be
  // done to find the target. The bushier the tree, therefore, the smaller the
  // number of comparisons that will usually need to be done.

  template <typename T>
  class BinarySearchTree : public BinaryTree<T>
  {
  public:
    ErrorCode insert(const T &data)
    {
      return search_and_insert(this->_root, data);
    }

    ErrorCode remove(const T &data)
    {
      return search_and_destroy(this->_root, data);
    }

    // if there is an entry in the tree whose key matches that in target, the
    // target is replaced by the corresponding record from the tree and a
    // success is returned.
    //
    // note: no need to save(output) target since that's what is requested to
    // search.
    //
    // note: Although the returned pointer can be used to gain access to the
    // data stored in a tree object, the only functions that can call the
    // auxiliary search must be tree methods, since only the methods are able to
    // pass the root as a parameter. Thus, returning a node pointer from the
    // auxiliary function will not compromise tree encapsulation.
    // valid point?

    ErrorCode search_1(T &target) const
    {
      ErrorCode ret{ErrorCode::success};

      BinaryNode<T> *found = search_for_node_1(this->_root, target);

      if (nullptr == found)
        return ErrorCode::not_present;
      else
        target = found->_data;

      return ret;
    }

    ErrorCode search_2(T &target) const
    {
      ErrorCode ret{ErrorCode::success};

      BinaryNode<T> *found = search_for_node_2(this->_root, target);

      if (nullptr == found)
        return ErrorCode::not_present;
      else
        target = found->_data;

      return ret;
    }

  private:
    // recursion version
    //
    // What event will be the termination condition for the recursive search?
    // Clearly, if we find the target, the function finishes successfully. If
    // not, then we continue searching until we hit an empty subtree, in which
    // case the search fails.
    //
    // returns:
    // 1. If the key of target is not in the subtree, returns NULL.
    // 2. a pointer to the ndoe if found.

    BinaryNode<T> *search_for_node_1(BinaryNode<T> *root, const T &target) const
    {
      if ((nullptr == root) || (root->_data == target))
        return root;
      else if (root->_data < target)
        return search_for_node_1(root->_right, target);
      else
        return search_for_node_1(root->_left, target);
    }

    // non-recursion, iterative version
    // Recursion occurs in this function only as tail recursion , that is, as
    // the last statement executed in the function.

    BinaryNode<T> *search_for_node_2(BinaryNode<T> *root, const T &target) const
    {
      while (root && (root->_data != target))
      {
        if (root->_data < target)
          root = root->_right;
        else
          root = root->_left;
      }

      return root;
    }

    // 10.2.3 Insertion into a Binary Search Tree
    //
    // If the keys to be inserted into an empty binary search tree are in their
    // natural order, then the method insert will produce a tree that degenerates
    // into an inefficient chain. The method insert should never be used with keys
    // that are already sorted into order.

    // as with search_for_node_1(), recursion version. add a node when not found
    // and not allow duplicated.

    // NOTE: reference
    // ErrorCode search_and_insert(BinaryNode<T> *root, const T &target)
    ErrorCode search_and_insert(BinaryNode<T> *&root, const T &target)
    {
      // when not found
      if (nullptr == root)
      {
        // use copy ctor
        root = new BinaryNode<T>(target);
        return ErrorCode::success;
      }
      else if (root->_data < target)
        return search_and_insert(root->_right, target);
      else if (root->_data > target)
        return search_and_insert(root->_left, target);
      else
        return ErrorCode::duplicate;
    }

    // 10.2.4 Treesort
    //
    // Hence inorder traversal always gives the sorted order for the keys.

    // 10.2.5 Removal from a Binary Search Tree

    // NOTE: reference
    // ErrorCode search_and_destroy(BinaryNode<T> *root, const T &target)
    ErrorCode search_and_destroy(BinaryNode<T> *&root, const T &target)
    {
      // when found or not-found. remove_root() handles a case when root is null.
      if ((nullptr == root) || (root->_data == target))
        return remove_root(root);
      else if (root->_data < target)
        return search_and_destroy(root->_right, target);
      else
        return search_and_destroy(root->_left, target);
    }

    // remove arg "root" from the tree. can remvoe any node in the tree and the
    // problem is that we still need to maintain "binary search tree" property.
    //
    // that is there are couple of cases to think when "both subtree are not
    // empty" when removing a node.
    //
    // 1. case when remove 4.
    //    move 3's data to 4 and delete 3 node. this still works when node 3 has
    //    left subtree but no right one.
    //
    // 2. case when remove 2.
    //
    //  tree of {0, ..., 13}
    //
    //              6
    //        2         ...
    //    0     4
    //  x   1 3   5
    //
    //    when follows the code
    //              6
    //        1
    //    0     4
    //  x   x 3   5
    //
    //    when do manually
    //              6
    //        3
    //    0     4
    //  x   1 x   5
    //
    // note that there is no need to handle from parant of arg node to delete.
    //
    // the both still BST. which one is right?
    //
    // from the text:
    //
    // First, we find the immediate predecessor of the node under inorder
    // traversal by moving to its left child and then as far right as possible.
    // (The immediate successor would work just as well.)
    //
    // NOTE: reference?
    //
    // Moreover, this parameter is passed by reference so that any changes to it
    // are reflected in the calling environment. Since the purpose is to update a
    // binary search tree, we must assume that in the calling program, the actual
    // parameter is one of the links of the tree, and not just a copy, or else the
    // tree structure itself will not be changed as it should.

    ErrorCode remove_root(BinaryNode<T> *&root)
    {
      if (nullptr == root)
        return ErrorCode::not_present;

      BinaryNode<T> *node_to_delete = root;

      if (nullptr == root->_right)
      {
        root = root->_left;
      }
      else if (nullptr == root->_left)
      {
        root = root->_right;
      }
      // both subtree are not empty
      else
      {
        node_to_delete = root->_left;

        BinaryNode<T> *parent = root;

        while (nullptr != node_to_delete->_right)
        {
          parent         = node_to_delete;
          node_to_delete = node_to_delete->_right;
        }

        root->_data = node_to_delete->_data;

        // when do not run while loop; that is, right subtree is null
        if (parent == root)
        {
          root->_left = node_to_delete->_left;
        }
        // when run while loop;
        else
        {
          parent->_right = node_to_delete->_left;
        }
      }

      // remove a node
      delete node_to_delete;

      return ErrorCode::success;
    }
  };

  void PrintTreeNode(int &entry) { std::cout << " " << entry << ","; }
} // namespace algo_binary_tree

// do not use dtor to clean up and use "remove" insted.
//
// [ RUN      ] AlgoSearchBinaryTree.check_binary_search_tree
// { 2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33,}
// {}
// [       OK ] AlgoSearchBinaryTree.check_binary_search_tree (0 ms)

TEST(AlgoSearchBinarySearch, check_binary_search_tree)
{
  using namespace algo_binary_tree;

  int arr[] = {33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};

  int size = (sizeof(arr) / sizeof(arr[0]));

  BinarySearchTree<int> bst;
  ErrorCode result;

  // insert elements to a tree
  for (int idx = 0; idx < size; idx++)
  {
    result = bst.insert(arr[idx]);

    if (result != ErrorCode::success)
    {
      std::cout << "insert failed: " << arr[idx] << " into a tree" << std::endl;
    }
  }

  // treesort and expects the ordered:
  // int arr[] = { 2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33 };
  std::cout << "{";
  bst.inorder(PrintTreeNode);
  std::cout << "}" << std::endl;

  // remove
  for (int idx = 0; idx < size; idx++)
  {
    result = bst.remove(arr[idx]);

    if (result != ErrorCode::success)
    {
      std::cout << "remove failed: " << arr[idx] << ", error: " << (int)result
                << std::endl;
    }
  }

  // print
  std::cout << "{";
  bst.inorder(PrintTreeNode);
  std::cout << "}" << std::endl;
}

// ={=========================================================================
// algo-queue

namespace queue_circular_vacant
{
  class CircularQueue
  {
  public:
    // *cxx-vector-ctor*
    // CircularQueue() : coll_(MAX_SIZE, 0) {}

    CircularQueue() {}

    bool empty() { return head_ == tail_; }

    bool full() { return (head_ + 1) % MAX_SIZE == tail_; }

    size_t size() { return (head_ - tail_ + MAX_SIZE) % MAX_SIZE; }

    void push(int value)
    {
      if (full())
        throw std::runtime_error("queue is full");

      // to see where exception happens since gmock do not show where it
      // throws
      //
      // if (full())
      // {
      //   cout << "queue is full, value = " << value << endl;
      //   return;
      // }

      head_        = (head_ + 1) % MAX_SIZE;
      coll_[head_] = value;
    }

    int pop()
    {
      if (empty())
        throw std::runtime_error("queue is empty");

      tail_ = (tail_ + 1) % MAX_SIZE;
      return coll_[tail_];
    }

    std::vector<int> snap()
    {
      std::vector<int> result{};

      // do not work like this
      //
      // for (auto run = tail_ + 1; run <= head_; ++run)
      //   result.push_back(coll_[run]);
      //
      // care about start value and <= condition. However, cannot use comparison
      // on head and tail since it warps around after all.
      //
      // for (int i = tail_ + 1; i <= head_; i = (i + 1) % MAX_SIZE)
      //     coll.push_back(coll_[i]);

      auto run = (tail_ + 1) % MAX_SIZE;
      for (size_t i = 0; i < size(); ++i)
      {
        result.push_back(coll_[run]);
        run = (run + 1) % MAX_SIZE;
      }

      return result;
    }

  private:
    static size_t const MAX_SIZE{10};

    // if MAX_SIZE is not static
    // std::vector<int> coll_;
    // and use ctor

    // if MAX_SIZE is not static
    // std::array<int, 10> coll_;

    // if MAX_SIZE is static
    std::array<int, MAX_SIZE> coll_;

    // if MAX_SIZE is static but still error
    // std::vector<int> coll_(MAX_SIZE, 0);

    // they are indexes
    size_t head_{};
    size_t tail_{};
  };

} // namespace queue_circular_vacant

TEST(AlgoQueue, CircularVacant)
{
  using namespace queue_circular_vacant;

  CircularQueue cq;

  cq.push(10);
  cq.push(11);
  cq.push(12);

  EXPECT_THAT(cq.size(), 3);
  EXPECT_THAT(cq.snap(), ElementsAre(10, 11, 12));

  cq.push(13);
  cq.push(14);
  cq.push(15);

  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(10, 11, 12, 13, 14, 15));

  EXPECT_THAT(cq.pop(), 10);
  EXPECT_THAT(cq.pop(), 11);
  EXPECT_THAT(cq.pop(), 12);

  EXPECT_THAT(cq.size(), 3);

  cq.push(100);
  cq.push(101);
  cq.push(102);

  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(13, 14, 15, 100, 101, 102));

  EXPECT_THAT(cq.pop(), 13);
  EXPECT_THAT(cq.pop(), 14);

  cq.push(300);
  cq.push(301);
  cq.push(302);

  EXPECT_THAT(cq.size(), 7);
  EXPECT_THAT(cq.snap(), ElementsAre(15, 100, 101, 102, 300, 301, 302));

  cq.push(400);
  cq.push(401);
  EXPECT_THROW(cq.push(402), std::runtime_error); // full, exception
  EXPECT_THROW(cq.push(403), std::runtime_error); // full, exception
  EXPECT_THROW(cq.push(404), std::runtime_error); // full, exception

  // since it is vacant version
  EXPECT_THAT(cq.size(), 9);
  EXPECT_THAT(cq.full(), true);

  EXPECT_THAT(cq.pop(), 15);
  EXPECT_THAT(cq.pop(), 100);
  EXPECT_THAT(cq.pop(), 101);

  // since it is vacant version
  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(102, 300, 301, 302, 400, 401));

  EXPECT_THAT(cq.full(), false);
}

namespace queue_circular_count
{

  class CircularQueue
  {
  public:
    // *cxx-vector-ctor*
    CircularQueue()
        : coll_(MAX_SIZE, 0)
    {}

    bool empty() const { return count_ == 0; }

    bool full() const { return count_ >= MAX_SIZE; }

    int size() const { return count_; }

    void push(int value)
    {
      if (full())
        return;

      head_          = head_ % MAX_SIZE;
      coll_[head_++] = value;
      count_++;
    }

    int pop()
    {
      int value{};

      if (empty())
        return -1;

      tail_ = tail_ % MAX_SIZE;
      value = coll_[tail_++];
      count_--;

      return value;
    }

    std::vector<int> snap()
    {
      std::vector<int> coll;

      int start = tail_;

      for (int i = 0; i < size(); ++i)
      {
        coll.push_back(coll_[start]);
        start = (start + 1) % MAX_SIZE;
      }

      return coll;
    }

  private:
    const int MAX_SIZE{10};
    int head_{};
    int tail_{};
    int count_{};
    std::vector<int> coll_; // {MAX_SIZE, 0};
  };

} // namespace queue_circular_count

TEST(AlgoQueue, CircularCount)
{
  using namespace queue_circular_count;

  CircularQueue cq;

  cq.push(10);
  cq.push(11);
  cq.push(12);

  EXPECT_THAT(cq.size(), 3);
  EXPECT_THAT(cq.snap(), ElementsAre(10, 11, 12));

  cq.push(13);
  cq.push(14);
  cq.push(15);

  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(10, 11, 12, 13, 14, 15));

  EXPECT_THAT(cq.pop(), 10);
  EXPECT_THAT(cq.pop(), 11);
  EXPECT_THAT(cq.pop(), 12);

  EXPECT_THAT(cq.size(), 3);

  cq.push(100);
  cq.push(101);
  cq.push(102);

  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(13, 14, 15, 100, 101, 102));

  EXPECT_THAT(cq.pop(), 13);
  EXPECT_THAT(cq.pop(), 14);

  cq.push(300);
  cq.push(301);
  cq.push(302);

  EXPECT_THAT(cq.size(), 7);
  EXPECT_THAT(cq.snap(), ElementsAre(15, 100, 101, 102, 300, 301, 302));

  cq.push(400);
  cq.push(401);
  cq.push(402);
  cq.push(403);
  cq.push(404);

  EXPECT_THAT(cq.size(), 10);
  EXPECT_THAT(cq.full(), true);

  EXPECT_THAT(cq.pop(), 15);
  EXPECT_THAT(cq.pop(), 100);
  EXPECT_THAT(cq.pop(), 101);

  EXPECT_THAT(cq.size(), 7);
  EXPECT_THAT(cq.snap(), ElementsAre(102, 300, 301, 302, 400, 401, 402));

  EXPECT_THAT(cq.full(), false);
}

// from Problem 46, circular buffer, the modern c++ challenge
// 1. use size(count) and head only
// 2. in push, no full check since it overwrites and in pop, it simply
// calculates first from head substracing size.
// 3. no iterator support is needed if not use begin()/end()

// no need to have `count` since std::vector() has size() to get current size
// and which is different from queue array implementation

namespace queue_circular_count_iterator
{
  template <typename T>
  class circular_buffer_iterator;

  template <typename T>
  class circular_buffer
  {
    typedef circular_buffer_iterator<T> const_iterator;
    friend class circular_buffer_iterator<T>;

  public:
    circular_buffer() = delete;
    explicit circular_buffer(size_t const size)
        : coll_(size)
    {}

    bool empty() const noexcept { return size_ == 0; }

    bool full() const noexcept { return size_ >= coll_.size(); }

    size_t capacity() const noexcept { return size_; }

    void clear() noexcept { head_ = -1, size_ = 0; }

    T pop()
    {
      if (empty())
        throw std::runtime_error("buffer is empty");

      auto pos = first_pos();

#ifdef QUEUE_CIRCULAR_DEBUG
      cout << "pop: pos: " << pos << ", coll_[]: " << coll_[pos] << endl;
#endif // QUEUE_CIRCULAR_DEBUG

      size_--;
      return coll_[pos];
    }

    void push(T const item)
    {
      // this is how the text is implemented and this allows overwrites
      // this make head and tail changed
      //
      // if (full())
      //   throw std::runtime_error("buffer is full");

      head_        = next_pos();
      coll_[head_] = item;

#ifdef QUEUE_CIRCULAR_DEBUG
      cout << "push: head_: " << head_ << ", coll_[]: " << coll_[head_] << endl;
#endif // QUEUE_CIRCULAR_DEBUG

      // due to overwrite feature
      if (size_ < coll_.size())
        size_++;
    }

    // iterators
    const_iterator begin() const
    {
      return const_iterator(*this, first_pos(), empty());
    }

    const_iterator end() const
    {
      return const_iterator(*this, next_pos(), true);
    }

  private:
    // same as `count`
    size_t size_{};

    // ?, set max value
    // size_t head_{-1};

    // to aviod narrowing warning
    int head_{-1};

    std::vector<T> coll_;

    // return `head` pos to push and the reason of having size_t == 0 is
    // that `head` starts from -1.

    size_t next_pos() const noexcept
    {
      return size_ == 0 ? 0 : ((head_ + 1) % coll_.size());
    }

    // return `tail` pos to pop and get tail from head and size
    // as with vacant case, normalise and +1 since no vacant item.

    size_t first_pos() const noexcept
    {
      return size_ == 0 ? 0 : (head_ - size_ + 1 + coll_.size()) % coll_.size();
    }
  };

  template <typename T>
  class circular_buffer_iterator
  {
    typedef circular_buffer_iterator self_type;
    typedef T const &const_reference;

  public:
    explicit circular_buffer_iterator(circular_buffer<T> const &buffer,
                                      size_t position,
                                      bool is_last)
        : buffer_(buffer)
        , position_(position)
        , is_last_(is_last)
    {}

    // cxx-operator-prefix
    self_type &operator++()
    {
      if (is_last_)
        throw std::out_of_range("past the end");

      position_ = (position_ + 1) % buffer_.coll_.size();

      // although it's circular queue which wraps around, iterator moves
      // around [tail, head] range. If increased pos is the same as head
      // then it reaches to the end.
      //
      // is_last_ get set either from ctor or ++()

      is_last_ = (position_ == buffer_.next_pos());

      return *this;
    }

    // cxx-operator-postfix which use prefix version
    self_type &operator++(int)
    {
      auto temp = *this;
      ++*this();
      return temp;
    }

    bool operator==(self_type const &other) const
    {
      // & address? since buffer do not support operator==()
      return &buffer_ == &other.buffer_ && position_ == other.position_ &&
             is_last_ == other.is_last_;
    }

    bool operator!=(self_type const &other) const { return !(*this == other); }

    const_reference operator*() const { return buffer_.coll_[position_]; }

  private:
    circular_buffer<T> const &buffer_;
    size_t position_;
    bool is_last_;
  };

  template <typename T>
  std::vector<T> print(circular_buffer<T> &buf)
  {
    std::vector<T> coll{};

    for (auto &e : buf)
      coll.push_back(e);

    return coll;
  }

} // namespace queue_circular_count_iterator

TEST(AlgoQueue, CircularCountIterator)
{
  using namespace queue_circular_count_iterator;

  {
    circular_buffer<int> cbuf(5); // {0, 0, 0, 0, 0} -> {}

    cbuf.push(1); // {1, 0, 0, 0, 0} -> {1}
    cbuf.push(2); // {1, 2, 0, 0, 0} -> {1, 2}
    cbuf.push(3); // {1, 2, 3, 0, 0} -> {1, 2, 3}

    auto item = cbuf.pop(); // {1, 2, 3, 0, 0} -> {X, 2, 3}
    EXPECT_THAT(item, 1);

    cbuf.push(4); // {1, 2, 3, 4, 0} -> {X, 2, 3, 4}
    cbuf.push(5); // {1, 2, 3, 4, (5)} -> {X, 2, 3, 4, 5}

    // see that it overwrites
    cbuf.push(6); // {(6), 2, 3, 4, 5} -> {2, 3, 4, 5, 6}
    cbuf.push(7); // {6, (7), 3, 4, 5} -> {3, 4, 5, 6, 7}
    cbuf.push(8); // {6, 7, (8), 4, 5} -> {4, 5, 6, 7, 8}

    item = cbuf.pop(); // {6, 7, 8, 4, 5} -> {5, 6, 7, 8}
    EXPECT_THAT(item, 4);
    item = cbuf.pop(); // {6, 7, 8, 4, 5} -> {6, 7, 8}
    EXPECT_THAT(item, 5);
    item = cbuf.pop(); // {6, 7, 8, 4, 5} -> {7, 8}
    EXPECT_THAT(item, 6);

    cbuf.pop();   // {6, 7, 8, 4, 5} -> {8}
    cbuf.pop();   // {6, 7, 8, 4, 5} -> {}
    cbuf.push(9); // {6, 7, 8, 9, 5} -> {9}
  }

  // to exercise iterator feature
  {
    circular_buffer<int> cbuf(5);

    cbuf.push(1);
    cbuf.push(2);
    cbuf.push(3);
    cbuf.push(4);
    cbuf.push(5);
    cbuf.push(6);
    cbuf.push(7);
    EXPECT_THAT(print(cbuf), ElementsAre(3, 4, 5, 6, 7));
  }
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

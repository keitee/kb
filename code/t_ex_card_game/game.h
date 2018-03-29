//
// Convert Arabic number to Roman number
//

// 1. How can support multiple players? Firstly, support multiple player which
// are allocated statically.
//

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

        // assume it's sorted. TODO: needs sorted container?
        void populateCards(int seed)
        {
            // if( seed == 1 )
            //     cards = std::vector<Card>{2, 3, 5, 6, 7, 9, 10, 11, 12};
            // else if( seed == 0 )
            //     cards = std::vector<Card>{1, 4, 8, 13, 14, 15, 16, 17, 18};
            
            // generate_n( back_inserter(cards, NUMBER_OF_CARDS_PER_PLAYER, CardSequence()));
            
            std::vector<int> ivec;
            std::generate_n( std::back_inserter(ivec, 12, std::rand));

            std::cout << name_ << " has " << cards.size() << std::endl;
        }

        bool hasCards() const
        {
            return !cards.empty();
        }

        Card getNextCard()
        {
            // card{0} means a invalid card
            Card card{};

            if(!cards.empty())
            {
                card = cards.back();
                cards.pop_back();
            }

            return card;
        }
        
        void scoreCard(const Card card)
        {
            if(card)
                scores.push_back(card);
        }

        Score getScore() const
        {
            return scores.size();
        }

        std::string getName() const
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
        std::vector<Card> cards{};
        std::vector<Card> scores{};
};

class CardGame
{
    public:
        CardGame() 
        {
            lsp.populateCards(0);
            rsp.populateCards(1);
        }

        void play()
        {
            Card rspCard{}, lspCard{};

            // assume rsp and lsp has the same num of cards
            while( rsp.hasCards() )
            {
                rspCard = rsp.getNextCard();
                lspCard = lsp.getNextCard();

                if( rspCard > lspCard )
                {
                    // std::cout << "rsp win: " << rspCard << std::endl;
                    rsp.scoreCard(rspCard);
                }
                else if ( rspCard < lspCard )
                {
                    // std::cout << "lsp win: " << lspCard << std::endl;
                    lsp.scoreCard(lspCard);
                }
                // TODO: what if both are the same?
                // else
            }

            Score rspScore = rsp.getScore();
            Score lspScore = lsp.getScore();

            rspScore > lspScore ?  printWinner(rsp) : printWinner(lsp);
        }

    private:
        // left and right side player
        CardPlayer lsp{"X"}, rsp{"Y"};

        void printWinner(const CardPlayer &player) const
        {
            std::cout << "winner " << player.getName() << " got " 
                << player.getScore() << " scores" << std::endl;
        }
};


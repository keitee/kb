#include "gmock/gmock.h"
#include "game.h"

using namespace testing;

class CardGameTest: public Test
{
public:
    CardGame game;
};

TEST_F(CardGameTest, PlayOneGame) {
    game.play();
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

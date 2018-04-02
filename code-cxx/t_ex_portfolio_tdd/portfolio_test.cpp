#include "gmock/gmock.h"
#include "portfolio.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"

using namespace testing;
using namespace std;

// fixture
class APortfolio : public Test 
{
    public:
        static const string IBM;
        static const string SAMSUNG;
        Portfolio portfolio_;
};

const string APortfolio::IBM{"IBM"};
const string APortfolio::SAMSUNG{"SS"};

TEST_F(APortfolio, IsEmptyWhenCreated)
{
    ASSERT_TRUE(portfolio_.IsEmpty());
}

TEST_F(APortfolio, IsNotEmptyAfterPurchase)
{
    portfolio_.Purchase(IBM, 1);
    ASSERT_FALSE(portfolio_.IsEmpty());
}

TEST_F(APortfolio, AnswerZeroForShareCountOfUnpurchasedSymbol)
{
    ASSERT_THAT(portfolio_.ShareCount("AAPL"), Eq(0u));
}

TEST_F(APortfolio, AnswersShareCountFotPurchasedSymbol)
{
    portfolio_.Purchase(IBM, 2);
    ASSERT_THAT(portfolio_.ShareCount(IBM), Eq(2u));
}

TEST_F(APortfolio, ThrowsOnPurchaseOfZeroShares)
{
    ASSERT_THROW(portfolio_.Purchase(IBM, 0), InvalidPurchaseException);
}

TEST_F(APortfolio, AnswersShareCountForAppropriateSymbol)
{
   portfolio_.Purchase(IBM, 5);
   portfolio_.Purchase(SAMSUNG, 10);

   ASSERT_THAT(portfolio_.ShareCount(IBM), Eq(5u));
}

TEST_F(APortfolio, ShareCountReflectsAccumulatePurchasesOfSameSymbol)
{
   portfolio_.Purchase(IBM, 5);
   portfolio_.Purchase(IBM, 15);

   ASSERT_THAT(portfolio_.ShareCount(IBM), Eq(5u + 15));
}

TEST_F(APortfolio, ReducesShareCountOfSymbolOnSell)
{
   portfolio_.Purchase(SAMSUNG, 30);
   portfolio_.Sell(SAMSUNG, 13);

   ASSERT_THAT(portfolio_.ShareCount(SAMSUNG), Eq(30u - 13));
}

TEST_F(APortfolio, ThrowsWhenSellingMoreSharesThanPurchased_LessCoverage)
{
   ASSERT_THROW(portfolio_.Sell(SAMSUNG, 1), InvalidSellException);
}

TEST_F(APortfolio, ThrowsWhenSellingMoreSharesThanPurchased_FullCoverage)
{
   portfolio_.Purchase(SAMSUNG, 10);
   ASSERT_THROW(portfolio_.Sell(SAMSUNG, 12), InvalidSellException);
}

// to support purchase history

TEST_F(APortfolio, AnswersThePurchaseRecordForASinglePurchase)
{
    date purchaseDate(2014, Jan, 1);
    portfolio_.Purchase(SAMSUNG, 5);

    // purchase history
    auto purchases = portfolio_.Purchases(SAMSUNG);

    auto purchase = purchases[0];

    ASSERT_THAT(purchase.count_, Eq(5u));
    ASSERT_THAT(purchase.date_, Eq(purchaseDate));
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

// For brevity, uses a single file and class declaration which has
// implementations.

// output
//
// [==========] Running 9 tests from 1 test case.
// [----------] Global test environment set-up.
// [----------] 9 tests from RewardServiceTest
// [ RUN      ] RewardServiceTest.checkRewards_EligibleCustomerVariation1_ReturnRewards
// [       OK ] RewardServiceTest.checkRewards_EligibleCustomerVariation1_ReturnRewards (0 ms)
// [ RUN      ] RewardServiceTest.checkRewards_EligibleCustomerVariation2_ReturnRewards
// [       OK ] RewardServiceTest.checkRewards_EligibleCustomerVariation2_ReturnRewards (0 ms)
// [ RUN      ] RewardServiceTest.checkRewards_EligibleCustomerVariation3_ReturnRewards
// [       OK ] RewardServiceTest.checkRewards_EligibleCustomerVariation3_ReturnRewards (0 ms)
// [ RUN      ] RewardServiceTest.checkRewards_EligibleCustomerVariation4_ReturnRewards
// [       OK ] RewardServiceTest.checkRewards_EligibleCustomerVariation4_ReturnRewards (1 ms)
// [ RUN      ] RewardServiceTest.checkRewards_EligibleCustomer_ReturnNoRewards
// [       OK ] RewardServiceTest.checkRewards_EligibleCustomer_ReturnNoRewards (0 ms)
// [ RUN      ] RewardServiceTest.checkRewards_InEligibleCustomer_ReturnNoRewards
// [       OK ] RewardServiceTest.checkRewards_InEligibleCustomer_ReturnNoRewards (0 ms)
// [ RUN      ] RewardServiceTest.checkRewards_TechnicalFailure_ReturnNoRewards
// [       OK ] RewardServiceTest.checkRewards_TechnicalFailure_ReturnNoRewards (0 ms)
// [ RUN      ] RewardServiceTest.checkRewards_TechnicalFailureFromOthers_ReturnNoRewards
// [       OK ] RewardServiceTest.checkRewards_TechnicalFailureFromOthers_ReturnNoRewards (0 ms)
// [ RUN      ] RewardServiceTest.checkRewards_InvalidAccountFailure_ReturnNoRewards
// [       OK ] RewardServiceTest.checkRewards_InvalidAccountFailure_ReturnNoRewards (0 ms)
// [----------] 9 tests from RewardServiceTest (2 ms total)
// 
// [----------] Global test environment tear-down
// [==========] 9 tests from 1 test case ran. (3 ms total)
// [  PASSED  ] 9 tests.


#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"
#include "gmock/gmock-generated-matchers.h"

using testing::_;
using testing::Return;
using testing::AtLeast;
using testing::ElementsAre;
using testing::IsEmpty;
using testing::Throw;


// Subscription and Portfolio
enum class Subscription {SPORTS, KIDS, MUSIC, NEWS, MOVIES};

typedef std::set<Subscription> Portfolio;

// Exceptions
class TechnicalFailure : public std::exception
{
    public:
        const char *what() const noexcept
        {
            return "Technical Failure";
        }
};

class InvalidAccount : public std::exception
{
    public:
        const char *what() const noexcept
        {
            return "Invalid Account Number";
        }
};

// Eligibility
enum class Eligibility {CUSTOMER_ELIGIBLE, CUSTOMER_INELIGIBLE};

// EligibilityService
class EligibilityService
{
    public:
        explicit EligibilityService() {}
        virtual ~EligibilityService() {}

        virtual Eligibility checkEligible(const std::string &accountNo) const = 0;
};


using RewardReturn = std::pair<bool, std::vector<std::string>>;

// RewardService
class RewardService {
    public:
        RewardService(const std::shared_ptr<EligibilityService> &eilgibility_service)
            : eilgibility_service_(eilgibility_service) {}

        virtual ~RewardService() {}

        // NB. To support differnt reward scheme, can override or can cosider
        // injecting a reward table from outside.

        // virtual std::vector<std::string> 
        virtual RewardReturn 
            checkRewards(const std::string &account_number, bool &account_valid,
                    const Portfolio &subscriptions)
        {
            const std::map<Subscription, std::string> rewardsTable{
                {Subscription::SPORTS, "CHAMPIONS_LEAGUE_FINAL_TICKET"},
                {Subscription::MUSIC, "KARAOKE_PRO_MICROPHONE"},
                {Subscription::MOVIES, "PIRATES_OF_THE_CARIBBEAN_COLLECTION"}
            };

            Eligibility found_eligibility{Eligibility::CUSTOMER_INELIGIBLE};
            std::vector<std::string> found_rewards{};

            // Handle exceptions
            try {
                found_eligibility = eilgibility_service_->checkEligible(account_number);
            } catch (InvalidAccount &except)
            {
                account_valid = false;
                // return found_rewards;
                return RewardReturn(account_valid, found_rewards);
            } catch (std::exception &except)
            {
                account_valid = true;
                // return found_rewards;
                return RewardReturn(account_valid, found_rewards);
            }
            
            if (found_eligibility == Eligibility::CUSTOMER_ELIGIBLE)
            {
                for (auto e : subscriptions)
                {
                    const auto &pos = rewardsTable.find(e);
                    const auto &end = rewardsTable.end();

                    if (pos != end)
                        found_rewards.push_back(pos->second);
                }
            }

            account_valid = true;
            // return found_rewards;
            return RewardReturn(account_valid, found_rewards);
        }

    private:
        std::shared_ptr<EligibilityService> eilgibility_service_;
};


// Mock
class MockEligibilityService : public EligibilityService
{
    public:
        MOCK_CONST_METHOD1(checkEligible, Eligibility(const std::string &accountNo));
};


// TESTS

// Eligible and rewards
// NB. If there are more subscriptions to support, might consider usning
// value-parameterized tests for more combinations.


MATCHER_P(EqReturnPair, expected, "")
{
    return arg.first == expected.first && arg.second == expected.second;
}

TEST(RewardServiceTest, checkRewards_EligibleCustomerVariation1_ReturnRewards)
{
    auto eilgibility_service = std::make_shared<MockEligibilityService>();
    bool account_valid{false};
 
    EXPECT_CALL(*eilgibility_service, checkEligible(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Eligibility::CUSTOMER_ELIGIBLE));
 
    RewardService reward_service(eilgibility_service);
 
    const auto result = reward_service.checkRewards("111", account_valid,
            Portfolio({Subscription::SPORTS, Subscription::MUSIC}));
 
    // ASSERT_THAT(result, ElementsAre("CHAMPIONS_LEAGUE_FINAL_TICKET", "KARAOKE_PRO_MICROPHONE"));
    // ASSERT_EQ(account_valid, true);
    
    ASSERT_THAT(result, 
            EqReturnPair(RewardReturn(true, std::vector<std::string>{"CHAMPIONS_LEAGUE_FINAL_TICKET", "KARAOKE_PRO_MICROPHONE"})));
}

TEST(RewardServiceTest, checkRewards_EligibleCustomerVariation2_ReturnRewards)
{
    auto eilgibility_service = std::make_shared<MockEligibilityService>();
    bool account_valid{false};
 
    EXPECT_CALL(*eilgibility_service, checkEligible(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Eligibility::CUSTOMER_ELIGIBLE));
 
    RewardService reward_service(eilgibility_service);
 
    const auto result = reward_service.checkRewards("111", account_valid, 
            Portfolio({Subscription::MUSIC, Subscription::SPORTS}));
 
    // ASSERT_THAT(result, ElementsAre("CHAMPIONS_LEAGUE_FINAL_TICKET", "KARAOKE_PRO_MICROPHONE"));
    // ASSERT_EQ(account_valid, true);

    ASSERT_THAT(result, 
            EqReturnPair(RewardReturn(true, std::vector<std::string>{"CHAMPIONS_LEAGUE_FINAL_TICKET", "KARAOKE_PRO_MICROPHONE"})));
}


TEST(RewardServiceTest, checkRewards_EligibleCustomerVariation3_ReturnRewards)
{
    auto eilgibility_service = std::make_shared<MockEligibilityService>();
    bool account_valid{false};
 
    EXPECT_CALL(*eilgibility_service, checkEligible(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Eligibility::CUSTOMER_ELIGIBLE));
 
    RewardService reward_service(eilgibility_service);
 
    const auto result = reward_service.checkRewards("111", account_valid,
            Portfolio({Subscription::SPORTS, Subscription::MUSIC, Subscription::KIDS}));
 
    // ASSERT_THAT(result, ElementsAre("CHAMPIONS_LEAGUE_FINAL_TICKET", "KARAOKE_PRO_MICROPHONE"));
    // ASSERT_EQ(account_valid, true);

    ASSERT_THAT(result, 
            EqReturnPair(RewardReturn(true, std::vector<std::string>{"CHAMPIONS_LEAGUE_FINAL_TICKET", "KARAOKE_PRO_MICROPHONE"})));
}

TEST(RewardServiceTest, checkRewards_EligibleCustomerVariation4_ReturnRewards)
{
    auto eilgibility_service = std::make_shared<MockEligibilityService>();
    bool account_valid{false};
 
    EXPECT_CALL(*eilgibility_service, checkEligible(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Eligibility::CUSTOMER_ELIGIBLE));
 
    RewardService reward_service(eilgibility_service);
 
    const auto result = reward_service.checkRewards("111", account_valid,
            Portfolio({Subscription::SPORTS, Subscription::KIDS, Subscription::MUSIC}));
 
    // ASSERT_THAT(result, ElementsAre("CHAMPIONS_LEAGUE_FINAL_TICKET", "KARAOKE_PRO_MICROPHONE"));
    // ASSERT_EQ(account_valid, true);
    
    ASSERT_THAT(result, 
            EqReturnPair(RewardReturn(true, std::vector<std::string>{"CHAMPIONS_LEAGUE_FINAL_TICKET", "KARAOKE_PRO_MICROPHONE"})));
}

// Eligible and no rewards
TEST(RewardServiceTest, checkRewards_EligibleCustomer_ReturnNoRewards)
{
    auto eilgibility_service = std::make_shared<MockEligibilityService>();
    bool account_valid{false};
 
    EXPECT_CALL(*eilgibility_service, checkEligible(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Eligibility::CUSTOMER_ELIGIBLE));
 
    RewardService reward_service(eilgibility_service);
 
    const auto result = reward_service.checkRewards("111", account_valid,
            Portfolio({Subscription::KIDS, Subscription::NEWS}));
 
    // ASSERT_THAT(result, IsEmpty());
    // ASSERT_EQ(account_valid, true);

    ASSERT_THAT(result, 
            EqReturnPair(RewardReturn(true, std::vector<std::string>())));
}

// Ineligible and no rewards
TEST(RewardServiceTest, checkRewards_InEligibleCustomer_ReturnNoRewards)
{
    auto eilgibility_service = std::make_shared<MockEligibilityService>();
    bool account_valid{false};
 
    EXPECT_CALL(*eilgibility_service, checkEligible(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Eligibility::CUSTOMER_INELIGIBLE));
 
    RewardService reward_service(eilgibility_service);
 
    const auto result = reward_service.checkRewards("111", account_valid,
            Portfolio({Subscription::SPORTS, Subscription::MUSIC}));
 
    // ASSERT_THAT(result, IsEmpty());
    // ASSERT_EQ(account_valid, true);
    
    ASSERT_THAT(result, 
            EqReturnPair(RewardReturn(true, std::vector<std::string>())));
}

// technical failure and no rewards
TEST(RewardServiceTest, checkRewards_TechnicalFailure_ReturnNoRewards)
{
    auto eilgibility_service = std::make_shared<MockEligibilityService>();
    bool account_valid{false};
 
    EXPECT_CALL(*eilgibility_service, checkEligible(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Throw(TechnicalFailure()));
 
    RewardService reward_service(eilgibility_service);
 
    const auto result = reward_service.checkRewards("111", account_valid,
            Portfolio({Subscription::SPORTS, Subscription::MUSIC}));
 
    // ASSERT_THAT(result, IsEmpty());
    // ASSERT_EQ(account_valid, true);
    
    ASSERT_THAT(result, 
            EqReturnPair(RewardReturn(true, std::vector<std::string>())));
}

// technical failure from other exception and no rewards
TEST(RewardServiceTest, checkRewards_TechnicalFailureFromOthers_ReturnNoRewards)
{
    auto eilgibility_service = std::make_shared<MockEligibilityService>();
    bool account_valid{false};
 
    EXPECT_CALL(*eilgibility_service, checkEligible(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Throw(std::runtime_error("HALTED")));
 
    RewardService reward_service(eilgibility_service);
 
    const auto result = reward_service.checkRewards("111", account_valid,
            Portfolio({Subscription::SPORTS, Subscription::MUSIC}));
 
    // ASSERT_THAT(result, IsEmpty());
    // ASSERT_EQ(account_valid, true);

    ASSERT_THAT(result, 
            EqReturnPair(RewardReturn(true, std::vector<std::string>())));
}

// invalid account failure and no rewards
TEST(RewardServiceTest, checkRewards_InvalidAccountFailure_ReturnNoRewards)
{
    auto eilgibility_service = std::make_shared<MockEligibilityService>();
    bool account_valid{false};

    EXPECT_CALL(*eilgibility_service, checkEligible(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Throw(InvalidAccount()));

    RewardService reward_service(eilgibility_service);

    const auto result = reward_service.checkRewards("111", account_valid,
            Portfolio({Subscription::SPORTS, Subscription::MUSIC}));

    // ASSERT_THAT(result, IsEmpty());
    // ASSERT_EQ(account_valid, false);
    
    ASSERT_THAT(result, 
        EqReturnPair(RewardReturn(false, std::vector<std::string>())));
}


int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

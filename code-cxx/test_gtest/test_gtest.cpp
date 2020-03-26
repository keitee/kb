#include <exception>
#include <iostream>
#include <memory>
#include <set>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std;
using namespace testing;

// ={=========================================================================
// gtest-basic

/*
https://github.com/google/googletest/blob/master/googletest/docs/advanced.md

FAIL();
ADD_FAILURE();
ADD_FAILURE_AT("file_path", line_number);

FAIL() generates a fatal failure, while ADD_FAILURE() and ADD_FAILURE_AT()
generate a nonfatal failure. These are useful when control flow, rather than a
Boolean expression, determines the test's success or failure. For example, you
might want to write something like:

switch(expression) {
  case 1:
     ... some checks ...
  case 2:
     ... some other checks ...
  default:
     FAIL() << "We shouldn't get here.";
}

NOTE: you can only use FAIL() in functions that return void. See the Assertion
Placement section for more information.

[ RUN      ] Gtest.FailAssertion
/home/keitee/git/kb/code-cxx/gtest/test_gtest.cpp:293: Failure
Failed
should generate a fatal failure
[  FAILED  ] Gtest.FailAssertion (0 ms)
[ RUN      ] Gtest.Message
/home/keitee/git/kb/code-cxx/gtest/test_gtest.cpp:299: Failure
Value of: false
  Actual: false
Expected: true
expected true but false
[  FAILED  ] Gtest.Message (0 ms)

*/

// "DISABLED" works for both case
TEST(DISABLED_GtestAssert, fail1)
{
  FAIL() << "should generate a fatal failure";
}

TEST(GtestAssert, DISABLED_fail2)
{
  FAIL() << "should generate a fatal failure";
}

TEST(GtestAssert, DISABLED_add_message)
{
  // do print message but make a test failed
  EXPECT_TRUE(false) << "expected true but false";

  // do not print message
  EXPECT_TRUE(true) << "expected true but true";
}

// OUT(object under test)
//
// Which way of displays is preferable??
//
// [ RUN      ] GtestAssert.shows_argument_order1
//
// /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:93: Failure
// Value of: OUT
// Expected: is equal to 100
//   Actual: 101 (of type int)
//
// NOTE: GMOCK use the second below
//
// /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:94: Failure
// Value of: 100
// Expected: is equal to 101
//   Actual: 100 (of type int)
// [  FAILED  ] GtestAssert.shows_argument_order1 (0 ms)
//
// However, see below case:
//
// [ RUN      ] OsGlibc.check_memcmp
//  /home/keitee/git/kb/code-cxx/os/test_os.cpp:224: Failure
//  Value of: 0
//  Expected: is equal to -1
//    Actual: 0 (of type int)
// [  FAILED  ] OsGlibc.check_memcmp (0 ms)
//
// EXPECT_THAT(0, memcmp(coll1, coll2, 6));
//
// [ RUN      ] OsGlibc.check_memcmp
// /home/keitee/git/kb/code-cxx/os/test_os.cpp:232: Failure
// Value of: memcmp(coll1, coll2, 6)
// Expected: is equal to 0
//   Actual: -1 (of type int)
// [  FAILED  ] OsGlibc.check_memcmp (0 ms)
//
// EXPECT_THAT(memcmp(coll1, coll2, 6), 0);

TEST(GtestAssert, DISABLED_shows_argument_order1)
{
  int OUT{101};

  EXPECT_THAT(OUT, 100);
  EXPECT_THAT(100, OUT);
}

// the order of arg can cause compile error
TEST(GtestAssert, shows_argument_order2)
{
  std::string result{"result"};

  EXPECT_THAT(result, "result");

  // googlemock/include/gmock/gmock-matchers.h:541:39: error: no matching
  // function for call to ‘ImplicitCast_(const
  // std::__cxx11::basic_string<char>&)’
  //
  // EXPECT_THAT("result", result);
}

// [ RUN      ] GtestAssert.shows_argument_order2
// /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:103: Failure
// Expected equality of these values:
//   value
//     Which is: 101
//   100
// /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:104: Failure
// Expected equality of these values:
//   100
//   value
//     Which is: 101
// [  FAILED  ] GtestAssert.shows_argument_order2 (0 ms)

TEST(GtestAssert, DISABLED_shows_argument_order2)
{
  int value{101};

  EXPECT_EQ(value, 100);
  EXPECT_EQ(100, value);
}

// to see which makes better messages
//
// [ RUN      ] Gtest.ExpectOrEq
// gtest.cpp:283: Failure
// Value of: 101
// Expected: is equal to 100
//   Actual: 101 (of type int)
//
// gtest.cpp:284: Failure
// Expected equality of these values:
//   101
//   value
//     Which is: 100
// [  FAILED  ] Gtest.ExpectOrEq (0 ms)
// [----------] 2 tests from Gtest (0 ms total)

// for EXPECT_THROW()
TEST(GtestAssert, shows_check_exception)
{
  std::vector<int> coll{1, 2, 3};

  EXPECT_THAT(coll.empty(), false);

  EXPECT_THAT(coll.at(0), 1);
  EXPECT_THAT(coll.at(2), 3);

  // Expected: coll.at(10) throws an exception of type std::range_error.
  //   Actual: it throws a different type.
  // EXPECT_THROW(coll.at(10), std::range_error);

  // vector::_M_range_check: __n (which is 10) >= this->size() (which is 3)
  try
  {
    coll.at(10);
  } catch (exception &e)
  {
    std::cout << e.what() << std::endl;
  }
}

// ={=========================================================================
// gtest-case1

namespace
{
  class WeatherException : public std::exception
  {
  public:
    const char *what() const noexcept { return "weather exception"; }
  };

  class WeatherStation
  {
  public:
    enum class Direction
    {
      North,
      South,
      East,
      West
    };

    enum class Outlook
    {
      Optimistic,
      Pessimistic
    };

    virtual void wind(Direction *direction, double *strength) const = 0;
    virtual double rainfall() const                                 = 0;
    virtual std::string prediction(Outlook) const                   = 0;
    virtual std::string snow(Outlook) const                         = 0;
    virtual std::string sample(std::string) const                   = 0;

    // to test gmock features so not in WeatherStation context
    virtual void complexargs(const std::vector<int> &coll1,
                             const std::vector<int> &coll2) const = 0;
  };

  class UserInterface
  {
  private:
    std::shared_ptr<WeatherStation> station_;

  public:
    explicit UserInterface(const std::shared_ptr<WeatherStation> &station)
        : station_(station)
    {}

    enum class Range
    {
      Heavy,
      Medium,
      Light
    };

    Range rain()
    {
      auto rainfall = station_->rainfall();

      if ((0.0 <= rainfall) && (rainfall < 2.0))
        return Range::Light;
      else if ((2.0 <= rainfall) && (rainfall < 4.0))
        return Range::Medium;
      else
        return Range::Heavy;
    }

    Range wind()
    {
      WeatherStation::Direction direction;
      double strength;

      station_->wind(&direction, &strength);

      if ((0.0 <= strength) && (strength < 5.0))
        return Range::Light;
      else if ((5.0 <= strength) && (strength < 10.0))
        return Range::Medium;
      else
        return Range::Heavy;
    }

    std::pair<std::string, std::string> prediction()
    {
      return std::make_pair(
        station_->prediction(WeatherStation::Outlook::Optimistic),
        station_->prediction(WeatherStation::Outlook::Pessimistic));
    }

    std::string snow()
    {
      try
      {
        return station_->snow(WeatherStation::Outlook::Pessimistic);
      } catch (WeatherException &e)
      {
        return "exception in snow";
      }
    }

    std::string sample() { return station_->sample("this is sample call"); }

    void complexargs()
    {
      station_->complexargs(std::vector<int>({1, 2, 3, 4}),
                            std::vector<int>({5, 6, 7, 8}));
    }
  };

  // define mock

  class MockWeatherStation : public WeatherStation
  {
  public:
    MOCK_CONST_METHOD2(wind, void(Direction *direction, double *strength));
    MOCK_CONST_METHOD0(rainfall, double());
    MOCK_CONST_METHOD1(prediction, std::string(Outlook));
    MOCK_CONST_METHOD1(snow, std::string(Outlook));
    MOCK_CONST_METHOD1(sample, std::string(std::string));
    MOCK_CONST_METHOD2(complexargs,
                       void(const std::vector<int> &coll1,
                            const std::vector<int> &coll2));
  };
} // namespace

// check rain()
TEST(WeatherStationUserInterface, check_rain_when_rainfall_is_havey1)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall()).WillOnce(Return(5.0));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

TEST(WeatherStationUserInterface, check_rain_when_rainfall_is_havey2)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall()).WillRepeatedly(Return(5.0));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

// when use ON_CALL
//
// [ RUN      ] WeatherStationUserInterface.get_heavy_when_rainfall_is_havey2
//
// GMOCK WARNING:
// Uninteresting mock function call - taking default action specified at:
// /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:259:
//     Function call: rainfall()
//           Returns: 5
// NOTE: You can safely ignore the above warning unless this call should not happen.  Do not suppress it by blindly adding an EXPECT_CALL() if you don't mean to enforce the call.  See https://github.com/google/googletest/blob/master/googlemock/docs/CookBook.md#knowing-when-to-expect for details.
// [       OK ] WeatherStationUserInterface.get_heavy_when_rainfall_is_havey2 (0 ms)

TEST(WeatherStationUserInterface, check_rain_when_rainfall_is_havey3)
{
  auto station = std::make_shared<MockWeatherStation>();

  ON_CALL(*station, rainfall()).WillByDefault(Return(5.0));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

// suppress warning.
TEST(WeatherStationUserInterface, check_rain_when_rainfall_is_havey4)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall()).Times(AnyNumber());

  UserInterface ui(station);

  // since mock do not return values.
  // EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

// use nicemock
TEST(WeatherStationUserInterface, check_rain_when_rainfall_is_havey5)
{
  auto station = std::make_shared<NiceMock<MockWeatherStation>>();

  UserInterface ui(station);
}

// wind()
TEST(WeatherStationUserInterface, check_wind_when_rainfall_is_light)
{
  auto station = std::make_shared<MockWeatherStation>();

  // since
  // void wind(Direction *direction, double *strength) const = 0;

  EXPECT_CALL(*station, wind(_, _))
    .WillOnce(DoAll(SetArgPointee<0>(WeatherStation::Direction::North),
                    SetArgPointee<1>(0.5)));

  UserInterface ui(station);

  EXPECT_THAT(ui.wind(), UserInterface::Range::Light);
}

// prediction
TEST(WeatherStationUserInterface, check_prediction1)
{
  auto station = std::make_shared<MockWeatherStation>();

  // expect a call with *exact* arg

  EXPECT_CALL(*station, prediction(WeatherStation::Outlook::Optimistic))
    .WillOnce(Return("Sunny"));

  EXPECT_CALL(*station, prediction(WeatherStation::Outlook::Pessimistic))
    .WillOnce(Return("Overcast"));

  UserInterface ui(station);

  EXPECT_THAT(ui.prediction(), std::make_pair("Sunny", "Overcast"));
}

// if use gmock-invoke
// so invoke is for something complex to do.
TEST(WeatherStationUserInterface, check_prediction2)
{
  auto station = std::make_shared<MockWeatherStation>();

  // expect a call with *exact* arg

  EXPECT_CALL(*station, prediction(WeatherStation::Outlook::Optimistic))
    .WillOnce(
      Invoke([](WeatherStation::Outlook x) -> std::string { return "Sunny"; }));

  EXPECT_CALL(*station, prediction(WeatherStation::Outlook::Pessimistic))
    .WillOnce(Invoke(
      [](WeatherStation::Outlook x) -> std::string { return "Overcast"; }));

  UserInterface ui(station);

  EXPECT_THAT(ui.prediction(), std::make_pair("Sunny", "Overcast"));
}

// snow()
TEST(WeatherStationUserInterface, check_snow)
{
  auto station = std::make_shared<MockWeatherStation>();

  // don't care arg and throw exception.
  EXPECT_CALL(*station, snow(_)).WillOnce(Throw(WeatherException()));

  UserInterface ui(station);

  EXPECT_THAT(ui.snow(), "exception in snow");
}

// *gmock-default-action*
// when not specify action, return default constructed of T and this is
// std::string in this case.

TEST(WeatherStationUserInterface, check_default_action)
{
  auto station = std::make_shared<MockWeatherStation>();

  // don't care arg
  EXPECT_CALL(*station, sample(_));

  UserInterface ui(station);

  EXPECT_THAT(ui.sample(), "");
}

// see if std::vector arg works
TEST(WeatherStationGmock, verify_complex_arguments1)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station,
              complexargs(ElementsAre(1, 2, 3, 4), ElementsAre(5, 6, 7, 8)));

  UserInterface ui(station);

  ui.complexargs();
}

// cause mismatch
//[ RUN      ] WeatherStationGmock.verify_complex_arguments2
// unknown file: Failure
// 
// Unexpected mock function call - returning directly.
//     Function call: complexargs(@0x7ffd1c8e2870 { 1, 2, 3, 4 }, @0x7ffd1c8e2850 { 5, 6, 7, 8 })
// Google Mock tried the following 1 expectation, but it didn't match:
// 
// /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:436: EXPECT_CALL(*station, complexargs(ElementsAre(1, 2, 3, 4), ElementsAre(5, 6, 7, 9)))...
//   Expected arg #1: has 4 elements where
// element #0 is equal to 5,
// element #1 is equal to 6,
// element #2 is equal to 7,
// element #3 is equal to 9
//            Actual: { 5, 6, 7, 8 }, whose element #3 doesn't match
//          Expected: to be called once
//            Actual: never called - unsatisfied and active
// /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:436: Failure
// Actual function call count doesn't match EXPECT_CALL(*station, complexargs(ElementsAre(1, 2, 3, 4), ElementsAre(5, 6, 7, 9)))...
//          Expected: to be called once
//            Actual: never called - unsatisfied and active
// [  FAILED  ] WeatherStationGmock.verify_complex_arguments2 (0 ms)

TEST(WeatherStationGmock, verify_complex_arguments2)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station,
              complexargs(ElementsAre(1, 2, 3, 4), ElementsAre(5, 6, 7, 9)));

  UserInterface ui(station);

  ui.complexargs();
}

// when use savearg
TEST(WeatherStationGmock, verify_complex_arguments3)
{
  auto station = std::make_shared<MockWeatherStation>();

  std::vector<int> coll1;
  std::vector<int> coll2;

  EXPECT_CALL(*station, complexargs(_, _))
    .WillOnce(DoAll(SaveArg<0>(&coll1), SaveArg<1>(&coll2)));

  UserInterface ui(station);

  ui.complexargs();

  EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4));
  EXPECT_THAT(coll2, ElementsAre(5, 6, 7, 8));
}

// cause mismatch
// [ RUN      ] WeatherStationGmock.verify_complex_arguments4
// /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:478: Failure
// Value of: coll2
// Expected: has 4 elements where
// element #0 is equal to 5,
// element #1 is equal to 6,
// element #2 is equal to 7,
// element #3 is equal to 9
//   Actual: { 5, 6, 7, 8 }, whose element #3 doesn't match
// [  FAILED  ] WeatherStationGmock.verify_complex_arguments4 (0 ms)

TEST(WeatherStationGmock, verify_complex_arguments4)
{
  auto station = std::make_shared<MockWeatherStation>();

  std::vector<int> coll1;
  std::vector<int> coll2;

  EXPECT_CALL(*station, complexargs(_, _))
    .WillOnce(DoAll(SaveArg<0>(&coll1), SaveArg<1>(&coll2)));

  UserInterface ui(station);

  ui.complexargs();

  EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4));
  EXPECT_THAT(coll2, ElementsAre(5, 6, 7, 9));
}

#if 0
TODO: link error
// ={=========================================================================
// test MATCHER_P

class RetweetCollection
{
    public:
        RetweetCollection() {}

        bool isEmpty() const
        {
            return 0 == size();
        }

        unsigned int size() const 
        {
            return 0;
        }
};

class ARetweetCollection : public Test 
{
    public:
        RetweetCollection collection;
};

MATCHER_P(HasSize, expected, "")
{
    return arg.size() == expected 
        && arg.isEmpty() == (0 == expected); 
}

TEST_F(ARetweetCollection, MatcherWithSingleArgument)
{
    EXPECT_THAT(collection, HasSize(0u));
}


// ={=========================================================================
// gtest-matcher
// 
// user matcher for std::pair

MATCHER_P(EqPair, expected, "")
{
    return arg.first == expected.first && arg.second.empty() == expected.second.empty();
}

TEST(Gtest, MatcherForPair)
{
    auto p1 = make_pair(1, std::vector<std::string>());
    auto p2 = make_pair(1, std::vector<std::string>());
    auto p3 = make_pair(2, std::vector<std::string>());

    EXPECT_THAT(p1, EqPair(p2));
    // EXPECT_THAT(p1, EqPair(make_pair(1,std::vector<std::string>{"PAIR"})));
    // EXPECT_THAT(p1, EqPair(p3));
}
#endif

int main(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test.  Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

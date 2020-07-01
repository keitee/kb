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
    virtual std::string sample_1(std::string) const                 = 0;
    virtual void sample_2(const std::string &) const                = 0;

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

    std::string sample_1() { return station_->sample_1("this is sample call"); }

    std::string sample_2() { station_->sample_2("this is sample_2 call"); }

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
    MOCK_CONST_METHOD1(sample_1, std::string(std::string));
    MOCK_CONST_METHOD1(sample_2, void(const std::string &));
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

  // NOTE: pass `mock` to user, ui, as arg and which enables `mocking`
  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

// use "WillRepeatdly*
TEST(WeatherStationUserInterface, check_rain_when_rainfall_is_havey2)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall()).WillRepeatedly(Return(5.0));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

// if comment out:
// EXPECT_CALL(*station, rainfall()).WillRepeatedly(Return(5.0));
//
// [ RUN      ] WeatherStationUserInterface.check_rain_when_rainfall_is_havey2_1
//
// GMOCK WARNING:
// Uninteresting mock function call - returning default value.
//     Function call: rainfall()
//           Returns: 0
// NOTE: You can safely ignore the above warning unless this call should not happen.  Do not suppress it by blindly adding an EXPECT_CALL() if you don't mean to enforce the call.  See https://github.com/google/googletest/blob/master/googlemock/docs/cook_book.md#knowing-when-to-expect for details.
// /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:356: Failure
// Value of: ui.rain()
// Expected: is equal to 4-byte object <00-00 00-00>
//   Actual: 4-byte object <02-00 00-00>
// [  FAILED  ] WeatherStationUserInterface.check_rain_when_rainfall_is_havey2_1 (0 ms)

TEST(WeatherStationUserInterface, DISABLED_check_rain_when_rainfall_is_havey2_1)
{
  auto station = std::make_shared<MockWeatherStation>();

  // EXPECT_CALL(*station, rainfall()).WillRepeatedly(Return(5.0));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

// can use nicemock instead to supress warning but has to comment out ui.rain()
// call as well since no return action from mock from expectation.
TEST(WeatherStationUserInterface, check_rain_when_rainfall_is_havey2_2)
{
  auto station = std::make_shared<NiceMock<MockWeatherStation>>();
  // auto station = std::shared_ptr<NiceMock<MockWeatherStation>>(
  //   new NiceMock<MockWeatherStation>);

  // EXPECT_CALL(*station, rainfall()).WillRepeatedly(Return(5.0));

  UserInterface ui(station);

  // EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
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

  // since "direction" is output arg and ui call wind() get get direction and
  // strength:
  //
  // void WeatherStation::wind(Direction *direction, double *strength) const = 0;
  //
  // *gmock-action-side-effects*
  //
  // SetArgPointee<N>(value)
  // Assign value to the variable pointed by the N-th (0-based) argument.

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
  EXPECT_CALL(*station, sample_1(_));

  UserInterface ui(station);

  EXPECT_THAT(ui.sample_1(), "");
}

TEST(WeatherStationUserInterface, check_reference_output_arg)
{
  auto station = std::make_shared<MockWeatherStation>();

  // don't care arg
  EXPECT_CALL(*station, sample_2("this is sample_2 call"));

  UserInterface ui(station);

  ui.sample_2();
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
// https://github.com/google/googletest/blob/master/googlemock/docs/cook_book.md
//
// Verifying Complex Arguments {#SaveArgVerify}
//
// If you want to verify that a method is called with a particular argument but
// the match criteria is complex, it can be difficult to distinguish between
// cardinality failures (calling the method the wrong number of times) and
// argument match failures. Similarly, if you are matching multiple parameters,
// it may not be easy to distinguishing which argument failed to match. For
// example:
//
//   // Not ideal: this could fail because of a problem with arg1 or arg2, or maybe
//   // just the method wasn't called.
//
//   EXPECT_CALL(foo, SendValues(_, ElementsAre(1, 4, 4, 7), EqualsProto( ... )));
//
// You can instead save the arguments and test them individually:
//
//   EXPECT_CALL(foo, SendValues)
//       .WillOnce(DoAll(SaveArg<1>(&actual_array), SaveArg<2>(&actual_proto)));
//
//   ... run the test
//
//   EXPECT_THAT(actual_array, ElementsAre(1, 4, 4, 7));
//   EXPECT_THAT(actual_proto, EqualsProto( ... ));

TEST(WeatherStationGmock, verify_complex_arguments3)
{
  auto station = std::make_shared<MockWeatherStation>();

  std::vector<int> coll1;
  std::vector<int> coll2;

  // save args into coll1, and coll2
  EXPECT_CALL(*station, complexargs(_, _))
    .WillOnce(DoAll(SaveArg<0>(&coll1), SaveArg<1>(&coll2)));

  UserInterface ui(station);

  ui.complexargs();

  // check on the saved args
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
={=========================================================================
gtest-action

Using Functions/Methods/Functors/Lambdas as Actions {#FunctionsAsActions}

If the built-in actions do not suit you, you can easily use an existing
function, method, or functor as an action:

using ::testing::_;
using ::testing::Invoke;

Invoke(f)
Invoke f `with the arguments passed to the mock function`, where f can be a
global/static function or a functor.

Invoke(object_pointer, &class::method)
Invoke the method on the object with the arguments passed to the mock function.

The return value of the invoked function is used as the return value of the
action.


Writing New Actions Quickly {#QuickNewActions}

If the built-in actions do not work for you, you can easily define your own one.

Just define a functor class with a (possibly templated) call operator, matching
the signature of your action.

struct Increment {
  template <typename T>
  T operator()(T* arg) {
    return ++(*arg);
  }
}

The same approach works with stateful functors (or any callable, really):

struct MultiplyBy {
  template <typename T>
  T operator()(T arg) { return arg * multiplier; }

  int multiplier;
}

Then use:
EXPECT_CALL(...).WillOnce(MultiplyBy{7});


Legacy macro-based Actions

Before C++11, the functor-based actions were not supported; the old way of
writing actions was through a set of ACTION* macros. 

We suggest to avoid them in new code; they hide a lot of logic behind the macro,
potentially leading to harder-to-understand compiler errors. Nevertheless, we
cover them here for completeness.

(skipped)

#endif

namespace gtestaction
{
  class Foo
  {
    virtual int Sum(int, int)    = 0;
    virtual bool ComplexJob(int) = 0;
  };

  class MockFoo : public Foo
  {
  public:
    MOCK_METHOD2(Sum, int(int x, int y));
    MOCK_METHOD1(ComplexJob, bool(int x));
  };

  int CalculateSum(int x, int y) { return x + y; }

  class Helper
  {
  public:
    // return true when x is even.
    bool ComplexJob(int x) { return x % 2 ? false : true; }
  };

  struct CalSum
  {
    template <typename T>
      T operator()(T x, T y) { return x + y; }
  };
} // namespace gtestaction

TEST(GMock, check_action)
{
  using namespace gtestaction;

  // use invoke
  {
    MockFoo foo;
    Helper helper;

    EXPECT_CALL(foo, Sum(_, _)).WillOnce(Invoke(CalculateSum));

    EXPECT_CALL(foo, ComplexJob(_))
      .WillOnce(Invoke(&helper, &Helper::ComplexJob));

    // Invokes CalculateSum(5, 6).
    EXPECT_THAT(foo.Sum(5, 6), 11);

    // Invokes helper.ComplexJob(10);
    EXPECT_THAT(foo.ComplexJob(10), true);
  }

  // without using matcher
  {
    MockFoo foo;
    Helper helper;

    EXPECT_CALL(foo, Sum).WillOnce(Invoke(CalculateSum));

    EXPECT_CALL(foo, ComplexJob).WillOnce(Invoke(&helper, &Helper::ComplexJob));

    // Invokes CalculateSum(5, 6).
    EXPECT_THAT(foo.Sum(5, 6), 11);

    // Invokes helper.ComplexJob(10);
    EXPECT_THAT(foo.ComplexJob(10), true);
  }

  // use function address
  {
    MockFoo foo;
    Helper helper;

    EXPECT_CALL(foo, Sum).WillOnce(&CalculateSum);

    EXPECT_CALL(foo, ComplexJob).WillOnce(Invoke(&helper, &Helper::ComplexJob));

    // Invokes CalculateSum(5, 6).
    EXPECT_THAT(foo.Sum(5, 6), 11);

    // Invokes helper.ComplexJob(10);
    EXPECT_THAT(foo.ComplexJob(10), true);
  }

  // use f, lambda
  {
    MockFoo foo;
    Helper helper;

    EXPECT_CALL(foo, Sum).WillOnce([](int x, int y) { return x + y; });

    EXPECT_CALL(foo, ComplexJob).WillOnce([](int x) {
      return x % 2 ? false : true;
    });

    // Invokes CalculateSum(5, 6).
    EXPECT_THAT(foo.Sum(5, 6), 11);

    // Invokes helper.ComplexJob(10);
    EXPECT_THAT(foo.ComplexJob(10), true);
  }

  // use custom function
  {
    MockFoo foo;
    Helper helper;

    EXPECT_CALL(foo, Sum).WillOnce(CalSum{});

    // Invokes CalculateSum(5, 6).
    EXPECT_THAT(foo.Sum(5, 6), 11);
  }
}

#if 0
={=========================================================================
gtest-async

Have tried to mock async behavior of 3rd party package and not found nice way to
do yet.

Testing Asynchronous Behavior

Have tried this but found that gmock do not support
WaitForNotificationWithTimeout(ms) which is mentioned in the above setion.

The difficulty is to mock the same function call with different return or args
depending on state.

This is what tried:

use single mock and use real service from 3rd party.

TEST(MRPlayerTest, mock_asinterface)
{
  // FutarqueOptArguments opt(argc, argv);
  FutarqueOptArguments opt(0, nullptr);
  BasicInit(&opt);

  FutarqueString url("http://192.168.1.102:40660?p=json");

  LOG_MSG("Connecting to %s", url.c_str());

  // real
  auto connection = std::make_shared<MediaRiteApiConnectionHandler>(nullptr);

  // mock
  MockASPlayerInterface *mock_asinterface = new MockASPlayerInterface();

  // expects
  // idle(1), pending(x), playing(x), pending(x), playing(x)

  InSequence dummy;

  EXPECT_CALL(*mock_asinterface, updateStatus(IDLE_STATE)).Times(1);

  EXPECT_CALL(*mock_asinterface, updateStatus(PEND_STATE))
    .Times(AtLeast(2))
    .RetiresOnSaturation();
  EXPECT_CALL(*mock_asinterface, updateStatus(PLAY_STATE))
    .Times(AtLeast(10))
    .RetiresOnSaturation();

  EXPECT_CALL(*mock_asinterface, updateStatus(PEND_STATE))
    .Times(AtLeast(2))
    .RetiresOnSaturation();
  EXPECT_CALL(*mock_asinterface, updateStatus(PLAY_STATE))
    .Times(AtLeast(10))
    .RetiresOnSaturation();

  std::shared_ptr<Player> m_mrp =
    std::make_shared<MRPlayer>(mock_asinterface, connection);

  // first play
  LOG_MSG("Requesting to 141");
  m_mrp->play(std::string("141"), 0.0, 0, false);

  LOG_MSG("Waiting for some time");
  std::this_thread::sleep_for(std::chrono::seconds(20));

  // second play but without stop
  LOG_MSG("Requesting to 28");
  m_mrp->play(std::string("28"), 0.0, 0, false);

  LOG_MSG("Waiting for some time");
  std::this_thread::sleep_for(std::chrono::seconds(20));

  m_mrp.reset();

  delete mock_asinterface;
}

// ={=========================================================================
// mock asinterface and MR as well.

CoreStatus getCoreStatusForCase1()
{
  std::vector<CoreStatus> coll{eCsDecoderSinkAwaitingFirstFrames,
                               eCsDecoderSinkAwaitingFirstFrames,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell, // 10
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell, // 18
                               eCsDecoderSinkAwaitingFirstFrames,
                               eCsDecoderSinkAwaitingFirstFrames,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell,
                               eCsAllIsWell};

  static size_t index{};

  if (index < coll.size())
  {
    std::cout << "getCoreStatusForCase1: index: " << index << std::endl;
    return coll[index++];
  }

  std::cout << "getCoreStatusForCase1: index: " << index << std::endl;
  return eCsAllIsWell;
}

TEST(MRPlayerTest, mock_all)
{
  // mock
  auto mock_connection  = std::make_shared<MockMediaRiteApiConnectionHandler>();
  auto mock_asinterface = new MockASPlayerInterface();
  auto mock_channellist = new MockChannelListRpcHelper;
  auto mock_player      = new MockPlayerRpcHelper;

  // set expects
  EXPECT_CALL(*mock_connection, connect).WillOnce(Return());
  EXPECT_CALL(*mock_connection, getChannelList)
    .WillRepeatedly(Return(mock_channellist));
  EXPECT_CALL(*mock_connection, getPlayer).WillRepeatedly(Return(mock_player));

  // expects
  // idle(1), pending(x), playing(x), pending(x), playing(x)

  EXPECT_CALL(*mock_player, getCoreStatus).WillRepeatedly([](const char *name) {
    return getCoreStatusForCase1();
  });

  InSequence dummy;

  // status
  EXPECT_CALL(*mock_asinterface, updateStatus(IDLE_STATE))
    .Times(1)
    .RetiresOnSaturation();

  EXPECT_CALL(*mock_asinterface, updateStatus(PEND_STATE))
    .Times(AtLeast(1))
    .RetiresOnSaturation();

  EXPECT_CALL(*mock_asinterface, updateStatus(PLAY_STATE))
    .Times(AtLeast(10))
    .RetiresOnSaturation();

  EXPECT_CALL(*mock_asinterface, updateStatus(PEND_STATE))
    .Times(AtLeast(2))
    .RetiresOnSaturation();

  EXPECT_CALL(*mock_asinterface, updateStatus(PLAY_STATE))
    .Times(AtLeast(10))
    .RetiresOnSaturation();

  std::shared_ptr<Player> m_mrp =
    std::make_shared<MRPlayer>(mock_asinterface, mock_connection);

  // first play
  LOG_MSG("Requesting to 141");
  m_mrp->play(std::string("141"), 0.0, 0, false);

  LOG_MSG("============== Waiting for some time");
  std::this_thread::sleep_for(std::chrono::seconds(20));

  // second play but without stop
  LOG_MSG("Requesting to 28");
  m_mrp->play(std::string("28"), 0.0, 0, false);

  LOG_MSG("Waiting for some time");
  std::this_thread::sleep_for(std::chrono::seconds(20));

  m_mrp.reset();

  delete mock_channellist;
  delete mock_player;
  delete mock_asinterface;
  mock_connection.reset();
}
#endif

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

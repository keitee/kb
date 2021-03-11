#include <exception>
#include <future>
#include <iostream>
#include <memory>
#include <set>
#include <thread>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std;
using namespace testing;

// to use Notification
using namespace testing::internal;

// ={=========================================================================
// gtest-basic

/*
https://github.com/google/googletest
https://github.com/google/googletest/blob/master/docs/primer.md

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

// ={=========================================================================
// "DISABLED" works for both case
TEST(DISABLED_Gtest, fail1)
{
  FAIL() << "should generate a fatal failure";
}

// ={=========================================================================
TEST(Gtest, DISABLED_fail2)
{
  FAIL() << "should generate a fatal failure";
}

// ={=========================================================================
TEST(Gtest, DISABLED_add_message)
{
  // do print message but make a test failed
  EXPECT_TRUE(false) << "expected true but false";

  // do not print message
  EXPECT_TRUE(true) << "expected true but true";
}

/*
Which way of displays is preferable?

1.

OUT(object under test)

  EXPECT_THAT(OUT, 100);

.cpp:124: Failure
Value of: OUT
Expected: is equal to 100
  Actual: 101 (of type int)


  EXPECT_THAT(100, OUT);

.cpp:125: Failure
Value of: 100
Expected: is equal to 101
  Actual: 100 (of type int)

2.

  EXPECT_THAT(0, memcmp(coll1, coll2, 6));

.cpp:224: Failure
Value of: 0
Expected: is equal to -1
  Actual: 0 (of type int)

  EXPECT_THAT(memcmp(coll1, coll2, 6), 0);

.cpp:232: Failure
Value of: memcmp(coll1, coll2, 6)
Expected: is equal to 0
  Actual: -1 (of type int)

NOTE So prefer this form over _EQ/_TRUE as shown below tests

  EXPECT_THAT(out, value);


https://github.com/google/googletest/blob/master/docs/advanced.md#asserting-using-gmock-matchers

Asserting Using gMock Matchers

gMock comes with a library of matchers for validating arguments passed to mock
objects. A gMock matcher is basically a predicate that knows how to describe
itself. It can be used in these assertion macros:

Fatal assertion	              Nonfatal assertion	          Verifies
ASSERT_THAT(value, matcher);	EXPECT_THAT(value, matcher);	value matches matcher

For example, StartsWith(prefix) is a matcher that matches a string starting
with prefix, and you can write:

using ::testing::StartsWith;
...
    // Verifies that Foo() returns a string starting with "Hello".
    EXPECT_THAT(Foo(), StartsWith("Hello"));

*/

// ={=========================================================================
// TEST(Gtest, argument_order_form_1)
TEST(Gtest, DISABLED_argument_order_form_1)
{
  int OUT{101};

  EXPECT_THAT(OUT, 100);
  EXPECT_THAT(100, OUT);
}

/* _EQ or _THAT?

  EXPECT_EQ(value, 100);

.cpp:103: Failure
Expected equality of these values:
  value
    Which is: 101
  100

  EXPECT_EQ(100, value);

.cpp:103: Failure
Expected equality of these values:
  100
  value
    Which is: 101

NOTE so pefer _THAT

*/

// ={=========================================================================
// TEST(Gtest, argument_order_form_2)
TEST(Gtest, DISABLED_argument_order_form_2)
{
  int value{101};

  EXPECT_EQ(value, 100);
  EXPECT_EQ(100, value);
}

/* _EQ or _TRUE?

From Binary Comparison
However, when possible, ASSERT_EQ(actual, expected) is preferred to
ASSERT_TRUE(actual == expected), since it tells you actual and expected's
values on failure.

  EXPECT_TRUE(value == 100);

.cpp:192: Failure
Value of: value == 100
  Actual: false
Expected: true

  EXPECT_EQ(value, 100);

.cpp:192: Failure
Expected equality of these values:
  value
    Which is: 101
  100

*/

// TEST(Gtest, argument_order_form_3)
TEST(Gtest, DISABLED_argument_order_form_3)
{
  int value{101};

  EXPECT_TRUE(value == 100);
  EXPECT_EQ(value, 100);
}

// ={=========================================================================
// the order of arg can cause compile error
TEST(Gtest, argument_order_can_cause_error)
{
  std::string result{"result"};

  EXPECT_THAT(result, "result");

  // googlemock/include/gmock/gmock-matchers.h:541:39: error: no matching
  // function for call to ‘ImplicitCast_(const
  // std::__cxx11::basic_string<char>&)’
  //
  // EXPECT_THAT("result", result);
}

/*
={=========================================================================

1.

[ RUN      ] GtestAssert.shows_check_exception
unknown file: Failure
C++ exception with description "vector::_M_range_check: __n (which is 10) >= this->size() (which is 3)" thrown in the test body.
[  FAILED  ] GtestAssert.shows_check_exception (0 ms)

2. when use

    EXPECT_THROW(coll.at(10), std::range_error);

[ RUN      ] GtestAssert.shows_check_exception
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:210: Failure
Expected: coll.at(10) throws an exception of type std::range_error.
  Actual: it throws a different type.
vector::_M_range_check: __n (which is 10) >= this->size() (which is 3)
[  FAILED  ] GtestAssert.shows_check_exception (0 ms)

*/

// ={=========================================================================
TEST(Gtest, handle_exception)
{
  // see exception raised
  {
    std::vector<int> coll{1, 2, 3};

    EXPECT_THAT(coll.empty(), false);

    EXPECT_THAT(coll.at(0), 1);
    EXPECT_THAT(coll.at(2), 3);

    // coll.at(10);
  }

  // handle it
  {
    std::vector<int> coll{1, 2, 3};

    EXPECT_THAT(coll.empty(), false);

    EXPECT_THAT(coll.at(0), 1);
    EXPECT_THAT(coll.at(2), 3);

    EXPECT_THROW(coll.at(10), std::out_of_range);
  }
}

/*
// ={=========================================================================
Binary Comparison
This section describes assertions that compare two values.

ASSERT_LE(val1, val2);	EXPECT_LE(val1, val2);	val1 <= val2

However, when possible, ASSERT_EQ(actual, expected) is preferred to
ASSERT_TRUE(actual == expected), since it tells you actual and expected's
values on failure.

*/

TEST(Gtest, binary_comparison)
{
  {
    int value{5};
    EXPECT_LE(value, 5);
  }

  {
    int value{4};
    EXPECT_LE(value, 5);
  }

  // expect to fail
  {
    int value{7};
    EXPECT_LE(value, 5);
  }
}

/*
// ={=========================================================================
GMock
https://github.com/google/googletest/blob/master/docs/gmock_for_dummies.md

*/
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
    // to have int arg
    virtual double rainfall_1(int) const             = 0;
    virtual std::string prediction(Outlook) const    = 0;
    virtual std::string snow(Outlook) const          = 0;
    virtual std::string sample_1(std::string) const  = 0;
    virtual void sample_2(const std::string &) const = 0;

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

    double rain_more()
    {
      auto rainfall = station_->rainfall();
      rainfall      = station_->rainfall();
      rainfall      = station_->rainfall();
      rainfall      = station_->rainfall();

      return rainfall;
    }

    double rain_call_3_times_1()
    {
      auto rainfall = station_->rainfall_1(10);
      rainfall      = station_->rainfall_1(10);
      rainfall      = station_->rainfall_1(10);

      return rainfall;
    }

    double rain_call_3_times_2()
    {
      auto rainfall = station_->rainfall_1(10);
      rainfall      = station_->rainfall_1(10);
      rainfall      = station_->rainfall_1(20);

      return rainfall;
    }

    double rain_call_order()
    {
      auto rainfall = station_->rainfall_1(10);
      rainfall      = station_->rainfall_1(20);
      rainfall      = station_->rainfall_1(30);

      return rainfall;
    }

    Range rain_do_not_call_station()
    {
      auto rainfall = 5.0;

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

    // std::string sample_2() { station_->sample_2("this is sample_2 call"); }
    std::string sample_2()
    {
      station_->sample_2("this is sample_2 call");
      return "";
    }

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
    MOCK_CONST_METHOD1(rainfall_1, double(int));
    MOCK_CONST_METHOD1(prediction, std::string(Outlook));
    MOCK_CONST_METHOD1(snow, std::string(Outlook));
    MOCK_CONST_METHOD1(sample_1, std::string(std::string));
    MOCK_CONST_METHOD1(sample_2, void(const std::string &));
    MOCK_CONST_METHOD2(complexargs,
                       void(const std::vector<int> &coll1,
                            const std::vector<int> &coll2));
  };
} // namespace

/*
// ={=========================================================================
https://github.com/google/googletest/blob/master/docs/gmock_for_dummies.md#setting-expectations

Setting Expectations

googlemock/include/gmock/gmock-spec-builders.h
{
// Google Mock - a framework for writing C++ mock classes.
//
// This file implements the ON_CALL() and EXPECT_CALL() macros.
//
// A user can use the ON_CALL() macro to specify the default action of
// a mock method.  The syntax is:
//
//   ON_CALL(mock_object, Method(argument-matchers))
//       .With(multi-argument-matcher)
//       .WillByDefault(action);
//
//  where the .With() clause is optional.
//
// A user can use the EXPECT_CALL() macro to specify an expectation on
// a mock method.  The syntax is:
//
//   EXPECT_CALL(mock_object, Method(argument-matchers))
//       .With(multi-argument-matchers)
//       .Times(cardinality)
//       .InSequence(sequences)
//       .After(expectations)
//       .WillOnce(action)
//       .WillRepeatedly(action)
//       .RetiresOnSaturation();
//
// where all clauses are optional, and .InSequence()/.After()/
// .WillOnce() can appear any number of times.
}

*/

// ={=========================================================================
TEST(WeatherStationUserInterface, when_expectation_is_met)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall()).WillOnce(Return(5.0));

  // NOTE: dependancy injection. inject `mock` to user, ui, as arg and which
  // enables `mocking`

  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

// ={=========================================================================
// use "WillRepeatdly* see gmock-action
TEST(WeatherStationUserInterface,
     when_expectation_is_met_with_different_cardinality)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall()).WillRepeatedly(Return(5.0));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

/*
// ={=========================================================================
https://github.com/google/googletest/blob/master/docs/gmock_for_dummies.md#cardinalities-how-many-times-will-it-be-called

Cardinalities: How Many Times Will It Be Called?

The Times() clause can be omitted. If you omit Times(), gMock will infer
the cardinality for you. The rules are easy to remember:

If neither WillOnce() nor WillRepeatedly() is in the EXPECT_CALL(), the
inferred cardinality is Times(1).

If there are n WillOnce()'s but no WillRepeatedly(), where n >= 1, the
cardinality is Times(n).

If there are n WillOnce()'s and one WillRepeatedly(), where n >= 0, the
cardinality is Times(AtLeast(n)).

Quick quiz: what do you think will happen if a function is expected to be
called twice but actually called four times?

[ RUN      ] WeatherStationUserInterface.when_expectation_is_met_but_called_more
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:543: Failure
Mock function called more times than expected - returning default value.
    Function call: rainfall()
          Returns: 0
         Expected: to be called twice
           Actual: called 3 times - over-saturated and active
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:543: Failure
Mock function called more times than expected - returning default value.
    Function call: rainfall()
          Returns: 0
         Expected: to be called twice
           Actual: called 4 times - over-saturated and active
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:586: Failure
Value of: ui.rain_more()
Expected: is equal to 5
  Actual: 0 (of type double)
[  FAILED  ] WeatherStationUserInterface.when_expectation_is_met_but_called_more (0 ms)
 
*/

TEST(WeatherStationUserInterface, when_expectation_is_met_but_called_more)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall()).WillOnce(Return(5.0)).WillOnce(Return(5.0));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_more(), 5.0);
}

/*
// ={=========================================================================
https://github.com/google/googletest/blob/master/docs/gmock_for_dummies.md#using-multiple-expectations-multiexpectations

Using Multiple Expectations {#MultiExpectations}

By default, when a mock method is invoked, gMock will search the
"expectations in the reverse order they are defined", and stop when an active
expectation that matches the arguments is found (you can think of it as
"newer rules override older ones."). If the matching expectation cannot
take any more calls, you will get an upper-bound-violated failure. Here's
an example:

EXPECT_CALL(turtle, Forward(_));  // #1
EXPECT_CALL(turtle, Forward(10))  // #2
    .Times(2);

If Forward(10) is called three times in a row, the third time it will be an
error, as the last matching expectation (#2) has been "saturated." If,
however, the third Forward(10) call is replaced by Forward(20), then it
would be OK, as now #1 will be the matching expectation.

[ RUN      ] WeatherStationUserInterface.when_expectation_is_met_but_called_more_2
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:635: Failure
Mock function called more times than expected - returning default value.
    Function call: rainfall_1(10)
          Returns: 0
         Expected: to be called twice
           Actual: called 3 times - over-saturated and active
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:634: Failure
Actual function call count doesn't match EXPECT_CALL(*station, rainfall_1(_))...
         Expected: to be called once
           Actual: never called - unsatisfied and active
[  FAILED  ] WeatherStationUserInterface.when_expectation_is_met_but_called_more_2 (1 ms)


[ RUN      ] WeatherStationUserInterface.when_expectation_is_met_but_called_more_3
[       OK ] WeatherStationUserInterface.when_expectation_is_met_but_called_more_3 (0 ms)


So each EXPECT_CALL makes "expectation":

EXPECT_CALL(turtle, Forward(10))  // #2
    .Times(2);

and three calls to mock function:

      auto rainfall = station_->rainfall_1(10);
      rainfall      = station_->rainfall_1(10);
      rainfall      = station_->rainfall_1(10);

the last call matches with arg and do not move to the #1 expectation.
Rather it raise an error that says expectation #2 is "saturated" and #1
expectaion is not met.

However, when calls:

      auto rainfall = station_->rainfall_1(10);
      rainfall      = station_->rainfall_1(10);
      rainfall      = station_->rainfall_1(20);

all expectations are met.


NOTE: gmock-best-practice
Why does gMock search for a match in the reverse order of the expectations?
The reason is that this allows a user to set up the default expectations in
a mock object's constructor or the test fixture's set-up phase and then
customize the mock by writing more specific expectations in the test body.
So, if you have two expectations on the same method, you want to put the
one with more specific matchers after the other, or the more specific rule
would be shadowed by the more general one that comes after it.

*/

TEST(WeatherStationUserInterface, when_expectation_is_met_but_called_more_2)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall_1(_));
  EXPECT_CALL(*station, rainfall_1(10)).Times(2);

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_call_3_times_1(), 0.0);
}

TEST(WeatherStationUserInterface, when_expectation_is_met_but_called_more_3)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall_1(_));
  EXPECT_CALL(*station, rainfall_1(10)).Times(2);

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_call_3_times_2(), 0.0);
}

/*
// ={=========================================================================
https://github.com/google/googletest/blob/master/docs/gmock_for_dummies.md#all-expectations-are-sticky-unless-said-otherwise-stickyexpectations

All Expectations Are Sticky (Unless Said Otherwise) {#StickyExpectations}

Suppose turtle.GoTo(0, 0) is called three times. In the third time, gMock
will see that the arguments match expectation #2 (remember that we always
pick the last matching expectation). Now, since we said that there should
be only two such calls, gMock will report an error immediately. This is
basically what we've told you in the Using Multiple Expectations section
above.

This example shows that expectations in gMock are "sticky" by default, in
the sense that they remain active even after we have reached their
invocation upper bounds. This is an important rule to remember, as it
affects the meaning of the spec, and is different to how it's done in many
other mocking frameworks (Why'd we do that? Because we think our rule makes
the common cases easier to express and understand.).

And, there's a better way to do it: in this case, we expect the calls to
occur in a specific order, and we line up the actions to match the order.
Since the order is important here, we should make it explicit using a
sequence:

{
  InSequence s;

  for (int i = 1; i <= n; i++) {
    EXPECT_CALL(turtle, GetX())
        .WillOnce(Return(10*i))
        .RetiresOnSaturation();
  }
}

By the way, the other situation where an expectation may not be sticky is
when it's in a sequence - as soon as another expectation that comes after
it in the sequence has been used, it automatically retires (and will never
be used to match any call).

[ RUN      ] WeatherStationUserInterface.when_expectation_is_met_but_called_more_4
[       OK ] WeatherStationUserInterface.when_expectation_is_met_but_called_more_4 (0 ms)
*/

TEST(WeatherStationUserInterface, when_expectation_is_met_but_called_more_4)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall_1(_));
  EXPECT_CALL(*station, rainfall_1(10)).Times(2).RetiresOnSaturation();

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_call_3_times_1(), 0.0);
}

/*
// ={=========================================================================
RUN      ] WeatherStationUserInterface.when_expectation_is_not_met
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:481: Failure
Actual function call count doesn't match EXPECT_CALL(*station, rainfall())...
         Expected: to be called once
           Actual: never called - unsatisfied and active
[  FAILED  ] WeatherStationUserInterface.when_expectation_is_not_met (0 ms)
*/

TEST(WeatherStationUserInterface, when_expectation_is_not_met)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall()).WillOnce(Return(5.0));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_do_not_call_station(), UserInterface::Range::Heavy);
}

/*
// ={=========================================================================
negative expectation but called

RUN      ] WeatherStationUserInterface.when_expects_not_called
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:503: Failure
Mock function called more times than expected - returning default value.
    Function call: rainfall()
          Returns: 0
         Expected: to be never called
           Actual: called once - over-saturated and active
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:510: Failure
Value of: ui.rain()
Expected: is equal to 4-byte object <00-00 00-00>
  Actual: 4-byte object <02-00 00-00>
[  FAILED  ] WeatherStationUserInterface.when_expects_not_called (0 ms)


*gmock-default-action*

https://github.com/google/googletest/blob/master/docs/gmock_for_dummies.md#actions-what-should-it-do

First, if the return type of a mock function is a built-in type or a
pointer, the function has a default action (a void function will just
return, a bool function will return false, and other functions will return
0). In addition, in C++ 11 and above, a mock function whose return type is
default-constructible (i.e. has a default constructor) has a default action
of returning a default-constructed value. If you don't say anything, this
behavior will be used.

So mock object returns 0, rainfall(), which is default created and in
rain(), it returns Range::Light which is 2 but expect Range::Heavy(0) in
the code.

*/

TEST(WeatherStationUserInterface, when_expects_but_not_called)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall()).Times(0);

  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

/*
// ={=========================================================================
https://github.com/google/googletest/blob/master/docs/gmock_for_dummies.md#actions-what-should-it-do

Important note: The EXPECT_CALL() statement evaluates the action clause
only once, even though the action may be performed many times. Therefore
you must be careful about side effects. The following may not do what you
want:

Instead of returning 100, 101, 102, ..., consecutively, this mock function
will always return 100 as n++ is only evaluated once. Similarly, Return(new
Foo) will create a new Foo object when the EXPECT_CALL() is executed, and
will return the same pointer every time. If you want the side effect to
happen every time, you need to define a custom action, which we'll teach in
the cook book.

[ RUN      ] WeatherStationUserInterface.when_action_has_side_effects
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:669: Failure
Value of: ui.rain_more()
Expected: is equal to 103
  Actual: 100 (of type double)
[  FAILED  ] WeatherStationUserInterface.when_action_has_side_effects (0 ms)
 

Obviously turtle.GetY() is expected to be called four times. But if you
think it will return 100 every time, think twice! Remember that one
WillOnce() clause will be consumed each time the function is invoked and
the default action will be taken afterwards. So the right answer is that
turtle.GetY() will return 100 the first time, but return 0 from the second
time on, as returning 0 is the default action for int functions.

[ RUN      ] WeatherStationUserInterface.when_action_has_side_effects_2

GMOCK WARNING:
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:702: Too few actions specified in EXPECT_CALL(*station, rainfall())...
Expected to be called 4 times, but has only 1 WillOnce().
GMOCK WARNING:
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:702: Actions ran out in EXPECT_CALL(*station, rainfall())...
Called 2 times, but only 1 WillOnce() is specified - returning default value.
Stack trace:

GMOCK WARNING:
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:702: Actions ran out in EXPECT_CALL(*station, rainfall())...
Called 3 times, but only 1 WillOnce() is specified - returning default value.
Stack trace:

GMOCK WARNING:
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:702: Actions ran out in EXPECT_CALL(*station, rainfall())...
Called 4 times, but only 1 WillOnce() is specified - returning default value.
Stack trace:
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:706: Failure
Value of: ui.rain_more()
Expected: is equal to 103
  Actual: 0 (of type double)
[  FAILED  ] WeatherStationUserInterface.when_action_has_side_effects_2 (1 ms)

*/

TEST(WeatherStationUserInterface, when_action_has_side_effects_1)
{
  auto station = std::make_shared<MockWeatherStation>();

  double value{100.0};

  EXPECT_CALL(*station, rainfall()).Times(4).WillRepeatedly(Return(value++));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_more(), 103.0);
}

TEST(WeatherStationUserInterface, when_action_has_side_effects_2)
{
  auto station = std::make_shared<MockWeatherStation>();

  double value{100.0};

  EXPECT_CALL(*station, rainfall()).Times(4).WillOnce(Return(value++));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_more(), 103.0);
}

/*
// ={=========================================================================
when comment out:

EXPECT_CALL(*station, rainfall()).WillRepeatedly(Return(5.0));

[ RUN      ] WeatherStationUserInterface.check_rain_when_rainfall_is_havey2_1

GMOCK WARNING:
Uninteresting mock function call - returning default value.
    Function call: rainfall()
          Returns: 0

NOTE: You can safely ignore the above warning unless this call should
not happen. Do not suppress it by blindly adding an EXPECT_CALL() if you
don't mean to enforce the call. 

See
https://github.com/google/googletest/blob/master/googlemock/docs/cook_book.md#knowing-when-to-expect
for details.

/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:356: Failure
Value of: ui.rain()
Expected: is equal to 4-byte object <00-00 00-00>
  Actual: 4-byte object <02-00 00-00>
[  FAILED  ] WeatherStationUserInterface.check_rain_when_rainfall_is_havey2_1 (0 ms)

and fails on:

EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);

because do not run "Return(5.0)" so gets different return from rain() call.


https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#understanding-uninteresting-vs-unexpected-calls-uninteresting-vs-unexpected

Understanding Uninteresting vs Unexpected Calls
{#uninteresting-vs-unexpected}

Uninteresting calls and unexpected calls are different concepts in gMock.
Very different.

A call x.Y(...) is uninteresting if there's not even a single
EXPECT_CALL(x, Y(...)) set. In other words, the test isn't interested in
the x.Y() method at all, as evident in that the test doesn't care to say
anything about it.

A call x.Y(...) is unexpected if there are some EXPECT_CALL(x, Y(...))s
set, but none of them matches the call. Put another way, the test is
interested in the x.Y() method (therefore it explicitly sets some
EXPECT_CALL to verify how it's called); however, the verification fails as
the test doesn't expect this particular call to happen.

An unexpected call is always an error, as the code under test doesn't
behave the way the test expects it to behave.

By default, an uninteresting call is not an error, as it violates no
constraint specified by the test. (gMock's philosophy is that saying
nothing means there is no constraint.) However, it leads to a warning, as
it might indicate a problem (e.g. the test author might have forgotten to
specify a constraint).

In gMock, NiceMock and StrictMock can be used to make a mock class "nice"
or "strict". How does this affect uninteresting calls and unexpected calls?

A nice mock suppresses uninteresting call warnings. It is less chatty than
the default mock, but otherwise is the same. If a test fails with a default
mock, it will also fail using a nice mock instead. And vice versa. Don't
expect making a mock nice to change the test's result.

A strict mock turns uninteresting call warnings into errors. So making a
mock strict may change the test's result.

Let's look at an example:

TEST(...) {
  NiceMock<MockDomainRegistry> mock_registry;
  EXPECT_CALL(mock_registry, GetDomainOwner("google.com"))
          .WillRepeatedly(Return("Larry Page"));

  // Use mock_registry in code under test.
  ... &mock_registry ...
}

The sole EXPECT_CALL here says that all calls to GetDomainOwner() must have
"google.com" as the argument. If GetDomainOwner("yahoo.com") is called, it
will be an unexpected call, and thus an error. Having a nice mock doesn't
change the severity of an unexpected call.


So how do we tell gMock that GetDomainOwner() can be called with some other
arguments as well? The standard technique is to add a "catch all"
EXPECT_CALL:

  EXPECT_CALL(mock_registry, GetDomainOwner(_))
        .Times(AnyNumber());  // catches all other calls to this method.
  EXPECT_CALL(mock_registry, GetDomainOwner("google.com"))
        .WillRepeatedly(Return("Larry Page"));

Remember that _ is the wildcard matcher that matches anything. 

With this, if GetDomainOwner("google.com") is called, it will do what the
second EXPECT_CALL says; if it is called with a different argument, it will
do what the first EXPECT_CALL says.

Note that the order of the two EXPECT_CALLs is important, as a "newer"
EXPECT_CALL takes precedence over an older one.

In gMock, if you are not interested in a method, just don't say anything
about it. If a call to this method occurs, you'll see a warning in the test
output, but it won't be a failure. This is called "naggy" behavior; to
change, see The Nice, the Strict, and the Naggy.


https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#the-nice-the-strict-and-the-naggy-nicestrictnaggy

The Nice, the Strict, and the Naggy {#NiceStrictNaggy}

However, sometimes you may want to ignore these uninteresting calls, and
sometimes you may want to treat them as errors. gMock lets you make the
decision on a per-mock-object basis.

NOTE: Our general recommendation is to use nice mocks (not yet the default)
most of the time, use naggy mocks (the current default) when developing or
debugging tests, and use strict mocks only as the last resort.

*/

// ={=========================================================================
// no expectation so "uninteresting call"
TEST(WeatherStationUserInterface, when_has_no_expect)
{
  auto station = std::make_shared<MockWeatherStation>();

  // EXPECT_CALL(*station, rainfall()).WillRepeatedly(Return(5.0));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

// ={=========================================================================
// can use "nicemock" instead to suppress warning but has to comment out
// ui.rain() call as well since no return action from mock

TEST(WeatherStationUserInterface, when_has_no_expect_but_suppress_it_1)
{
  auto station = std::make_shared<NiceMock<MockWeatherStation>>();

  // EXPECT_CALL(*station, rainfall()).WillRepeatedly(Return(5.0));

  UserInterface ui(station);

  // EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

/*
// ={=========================================================================
If you are bothered by the "Uninteresting mock function call" message
printed when a mock method without an EXPECT_CALL is called, you may use a
NiceMock instead to suppress all such messages for the mock object, or
suppress the message for specific methods by adding
EXPECT_CALL(...).Times(AnyNumber()). 

*/

// use ON_CALL so same as "no expectation" and warning. use "nicemock" to
// suppress it.
TEST(WeatherStationUserInterface, when_has_no_expect_but_suppress_it_2)
{
  auto station = std::make_shared<NiceMock<MockWeatherStation>>();

  ON_CALL(*station, rainfall()).WillByDefault(Return(5.0));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

// use times() but do not return. so have to suppress warning.
TEST(WeatherStationUserInterface, when_has_no_expect_but_suppress_it_3)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall()).Times(AnyNumber());

  UserInterface ui(station);

  // since mock do not return values.
  // EXPECT_THAT(ui.rain(), UserInterface::Range::Heavy);
}

/*
// ={=========================================================================
When use ON_CALL

GMOCK WARNING:
Uninteresting mock function call - taking default action specified at:
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:259:
    Function call: rainfall()
          Returns: 5
NOTE: You can safely ignore the above warning unless this call should not happen.  Do not suppress it by blindly adding an EXPECT_CALL() if you don't mean to enforce the call.  See https://github.com/google/googletest/blob/master/googlemock/docs/CookBook.md#knowing-when-to-expect for details.

since using ON_CALL do not place expectation on CUT(code under test) and
has the same effect without using EXPECT_CALL as above. Hence warning. Can
supress this with nice mock.


https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#knowing-when-to-expect-useoncall

Knowing When to Expect {#UseOnCall}

ON_CALL is likely the single most under-utilized construct in gMock.

There are basically two constructs for defining the behavior of a mock
object: ON_CALL and EXPECT_CALL. The difference? 

"ON_CALL defines what happens when a mock method is called", but doesn't
imply any expectation on the method being called. EXPECT_CALL not only
defines the behavior, but "also sets an expectation" that the method will
be called with the given arguments, for the given number of times (and in
the given order when you specify the order too).

Since EXPECT_CALL does more, isn't it better than ON_CALL? Not really.

Every EXPECT_CALL adds a constraint on the behavior of the code under test.
Having more constraints than necessary is baaad - even worse than not
having enough constraints.

This may be counter-intuitive. How could tests that verify more be worse
than tests that verify less? Isn't verification the whole point of tests?

The answer lies in what a test should verify. A good test verifies the
contract of the code. If a test over-specifies, it doesn't leave enough
freedom to the implementation. As a result, changing the implementation
without breaking the contract (e.g. refactoring and optimization), which
should be perfectly fine to do, can break such tests. Then you have to
spend time fixing them, only to see them broken again the next time the
implementation is changed.

Keep in mind that one doesn't have to verify more than one property in one
test. In fact, it's a good style to verify only one thing in one test. If
you do that, a bug will likely break only one or two tests instead of
dozens (which case would you rather debug?). If you are also in the habit
of giving tests descriptive names that tell what they verify, you can often
easily guess what's wrong just from the test log itself.

So use ON_CALL by default, and only use EXPECT_CALL when you actually
intend to verify that the call is made. 


NOTE: gmock-best-practice
For example, you may have a bunch of ON_CALLs in your test fixture to set
"the common mock behavior shared by all tests in the same group", and write
(scarcely) different EXPECT_CALLs in different TEST_Fs to verify different
aspects of the code's behavior. Compared with the style where each TEST
has many EXPECT_CALLs, this leads to tests that are more resilient to
implementational changes (and thus less likely to require maintenance) and
makes the intent of the tests more obvious (so they are easier to maintain
when you do need to maintain them).

If you are bothered by the "Uninteresting mock function call" message
printed when a mock method without an EXPECT_CALL is called, you may use a
NiceMock instead to suppress all such messages for the mock object, or
suppress the message for specific methods by adding
EXPECT_CALL(...).Times(AnyNumber()). 

DO NOT suppress it by blindly adding an EXPECT_CALL(...), or you'll have a
test that's a pain to maintain.

*/

/*
// ={=========================================================================
https://github.com/google/googletest/blob/master/docs/gmock_for_dummies.md#ordered-vs-unordered-calls-orderedcalls

Ordered vs Unordered Calls {#OrderedCalls}

By default, an expectation can match a call even though an earlier
expectation hasn't been satisfied. In other words, the calls don't have to
occur in the order the expectations are specified.

By creating an object of type InSequence, all expectations in its scope are
put into a sequence and have to occur sequentially. Since we are just
relying on the constructor and destructor of this object to do the actual
work, its name is really irrelevant.


[ RUN      ] WeatherStationUserInterface.when_do_not_care_expectation_order_1
[       OK ] WeatherStationUserInterface.when_do_not_care_expectation_order_1 (0 ms)
[ RUN      ] WeatherStationUserInterface.when_do_not_care_expectation_order_2
[       OK ] WeatherStationUserInterface.when_do_not_care_expectation_order_2 (1 ms)

[ RUN      ] WeatherStationUserInterface.when_do_care_expectation_order_but_failed
unknown file: Failure

Unexpected mock function call - returning default value.
    Function call: rainfall_1(10)
          Returns: 0
Google Mock tried the following 3 expectations, but none matched:

/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:1159: tried expectation #0: EXPECT_CALL(*station, rainfall_1(20))...
  Expected arg #0: is equal to 20
           Actual: 10
         Expected: to be called once
           Actual: never called - unsatisfied and active
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:1160: tried expectation #1: EXPECT_CALL(*station, rainfall_1(30))...
  Expected arg #0: is equal to 30
           Actual: 10
         Expected: to be called once
           Actual: never called - unsatisfied and active
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:1161: tried expectation #2: EXPECT_CALL(*station, rainfall_1(10))...
         Expected: all pre-requisites are satisfied
           Actual: the following immediate pre-requisites are not satisfied:
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:1160: pre-requisite #0
                   (end of pre-requisites)
         Expected: to be called once
           Actual: never called - unsatisfied and active
/home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:1161: Failure
Actual function call count doesn't match EXPECT_CALL(*station, rainfall_1(10))...
         Expected: to be called once
           Actual: never called - unsatisfied and active
[  FAILED  ] WeatherStationUserInterface.when_do_care_expectation_order_but_failed (0 ms)

*/
TEST(WeatherStationUserInterface, when_do_not_care_expectation_order_1)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall_1(30));
  EXPECT_CALL(*station, rainfall_1(20));
  EXPECT_CALL(*station, rainfall_1(10));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_call_order(), 0.0);
}

TEST(WeatherStationUserInterface, when_do_not_care_expectation_order_2)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, rainfall_1(20));
  EXPECT_CALL(*station, rainfall_1(30));
  EXPECT_CALL(*station, rainfall_1(10));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_call_order(), 0.0);
}

TEST(WeatherStationUserInterface, when_do_care_expectation_order_but_failed)
{
  auto station = std::make_shared<MockWeatherStation>();

  InSequence seq;

  EXPECT_CALL(*station, rainfall_1(20));
  EXPECT_CALL(*station, rainfall_1(30));
  EXPECT_CALL(*station, rainfall_1(10));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_call_order(), 0.0);
}

TEST(WeatherStationUserInterface, when_do_care_expectation_order_and_matched)
{
  auto station = std::make_shared<MockWeatherStation>();

  InSequence seq;

  EXPECT_CALL(*station, rainfall_1(10));
  EXPECT_CALL(*station, rainfall_1(20));
  EXPECT_CALL(*station, rainfall_1(30));

  UserInterface ui(station);

  EXPECT_THAT(ui.rain_call_order(), 0.0);
}

// ={=========================================================================
TEST(WeatherStationUserInterface, use_invoke_1)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, prediction(WeatherStation::Outlook::Optimistic))
    .WillOnce(Return("Sunny"));

  EXPECT_CALL(*station, prediction(WeatherStation::Outlook::Pessimistic))
    .WillOnce(Return("Overcast"));

  UserInterface ui(station);

  EXPECT_THAT(ui.prediction(), std::make_pair("Sunny", "Overcast"));
}

/*
https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#using-functionsmethodsfunctorslambdas-as-actions-functionsasactions

Using Functions/Methods/Functors/Lambdas as Actions {#FunctionsAsActions}

If the built-in actions don't suit you, you can use an existing callable
(function, std::function, method, functor, lambda) as an action.

*/

// ={=========================================================================
TEST(WeatherStationUserInterface, use_invoke_2)
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

// ={=========================================================================
TEST(WeatherStationUserInterface, throw_exception)
{
  auto station = std::make_shared<MockWeatherStation>();

  // Ok, don't care arg and throw exception.
  // EXPECT_CALL(*station, snow(_)).WillOnce(Throw(WeatherException()));

  // Also, ok.
  EXPECT_CALL(*station, snow).WillOnce(Throw(WeatherException()));

  UserInterface ui(station);

  EXPECT_THAT(ui.snow(), "exception in snow");
}

/*
// ={=========================================================================
Again, default action. When not specify action, return default constructed
of T and this is std::string in this case.
*/

TEST(WeatherStationUserInterface, see_default_action)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, sample_1(_));

  UserInterface ui(station);

  EXPECT_THAT(ui.sample_1(), "");
}

/*
// ={=========================================================================
Set output arguments

since "direction" is output arg and ui call wind() get get direction and
strength and wind() has output args:

  void WeatherStation::wind(Direction *direction, double *strength) const = 0;

*gmock-action-side-effects*

SetArgPointee<N>(value)
Assign value to the variable pointed by the N-th (0-based) argument.

*/

// ={=========================================================================
TEST(WeatherStationUserInterface, use_output_arguments)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station, wind(_, _))
    .WillOnce(DoAll(SetArgPointee<0>(WeatherStation::Direction::North),
                    SetArgPointee<1>(0.5)));

  UserInterface ui(station);

  EXPECT_THAT(ui.wind(), UserInterface::Range::Light);
}

/*

when use:

  class UserInterface
  {
    std::string sample_2() { station_->sample_2("this is sample_2 call"); }
  }

[ RUN      ] WeatherStationUserInterface.check_reference_output_arg
free(): double free detected in tcache 2
Aborted (core dumped)

is fixed when use:
    std::string sample_2() { station_->sample_2("this is sample_2 call"); return ""; }

TODO: revise test. want to test "reference output arguments"?

// ={=========================================================================
TEST(WeatherStationUserInterface, use_reference_output_arguments)
{
  auto station = std::make_shared<MockWeatherStation>();

  // don't care arg
  EXPECT_CALL(*station, sample_2("this is sample_2 call"));

  UserInterface ui(station);

  ui.sample_2();
}
*/

// ={=========================================================================
// see if std::vector arg works
TEST(WeatherStationGmock, complex_arguments_1)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station,
              complexargs(ElementsAre(1, 2, 3, 4), ElementsAre(5, 6, 7, 8)));

  UserInterface ui(station);

  ui.complexargs();
}

/*
cause mismatch

From:
              complexargs(ElementsAre(1, 2, 3, 4), ElementsAre(5, 6, 7, 8)));
To:
              complexargs(ElementsAre(1, 2, 3, 4), ElementsAre(5, 6, 7, 9)));

[ RUN      ] WeatherStationGmock.verify_complex_arguments2
 unknown file: Failure

 Unexpected mock function call - returning directly.
     Function call: complexargs(@0x7ffd1c8e2870 { 1, 2, 3, 4 }, @0x7ffd1c8e2850 { 5, 6, 7, 8 })
 Google Mock tried the following 1 expectation, but it didn't match:

 /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:436: EXPECT_CALL(*station, complexargs(ElementsAre(1, 2, 3, 4), ElementsAre(5, 6, 7, 9)))...
   Expected arg #1: has 4 elements where
 element #0 is equal to 5,
 element #1 is equal to 6,
 element #2 is equal to 7,
 element #3 is equal to 9
            Actual: { 5, 6, 7, 8 }, whose element #3 doesn't match
          Expected: to be called once
            Actual: never called - unsatisfied and active
 /home/keitee/git/kb/code-cxx/test_gtest/test_gtest.cpp:436: Failure
 Actual function call count doesn't match EXPECT_CALL(*station, complexargs(ElementsAre(1, 2, 3, 4), ElementsAre(5, 6, 7, 9)))...
          Expected: to be called once
            Actual: never called - unsatisfied and active
 [  FAILED  ] WeatherStationGmock.verify_complex_arguments2 (0 ms)
*/

TEST(WeatherStationGmock, complex_arguments_2)
{
  auto station = std::make_shared<MockWeatherStation>();

  EXPECT_CALL(*station,
              complexargs(ElementsAre(1, 2, 3, 4), ElementsAre(5, 6, 7, 9)));

  UserInterface ui(station);

  ui.complexargs();
}

/*
https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#verifying-complex-arguments-saveargverify

Verifying Complex Arguments {#SaveArgVerify}

If you want to verify that a method is called with a particular argument
but the match criteria is complex, it can be difficult to distinguish
between cardinality failures (calling the method the wrong number of times)
and argument match failures. Similarly, if you are matching multiple
parameters, it may not be easy to distinguishing which argument failed to
match. For example:

  // Not ideal: this could fail because of a problem with arg1 or arg2, or
  // maybe just the method wasn't called.

  EXPECT_CALL(foo, SendValues(_, ElementsAre(1, 4, 4, 7), EqualsProto( ... )));

You can instead "save the arguments and test them individually":

  EXPECT_CALL(foo, SendValues)
      .WillOnce(DoAll(SaveArg<1>(&actual_array), SaveArg<2>(&actual_proto)));

  ... run the test

  EXPECT_THAT(actual_array, ElementsAre(1, 4, 4, 7));
  EXPECT_THAT(actual_proto, EqualsProto( ... ));

*/

TEST(WeatherStationGmock, complex_arguments_3)
{
  auto station = std::make_shared<MockWeatherStation>();

  std::vector<int> coll1{};
  std::vector<int> coll2{};

  // save args into coll1, and coll2
  EXPECT_CALL(*station, complexargs(_, _))
    .WillOnce(DoAll(SaveArg<0>(&coll1), SaveArg<1>(&coll2)));

  UserInterface ui(station);

  ui.complexargs();

  // check on the saved args
  EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4));
  EXPECT_THAT(coll2, ElementsAre(5, 6, 7, 8));
}

/*
={=========================================================================
NOTE: use invoke() after all.

https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#writing-new-actions-quickly-quicknewactions

Writing New Actions Quickly {#QuickNewActions}

If the built-in actions do not work for you, you can easily define your own
one.

Just define a functor class with a (possibly templated) call operator,
matching the signature of your action.

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

"We suggest to avoid them in new code"; they hide a lot of logic behind the
macro, potentially leading to harder-to-understand compiler errors.
Nevertheless, we cover them here for completeness.

(skipped)

Legacy macro-based parameterized Actions

Sometimes you'll want to parameterize an action you define. For that we
have another macro

ACTION_P(name, param) { statements; }

(skipped)

*/

namespace gmock_action
{
  class Foo
  {
  private:
    virtual int Sum(int, int)    = 0;
    virtual bool ComplexJob(int) = 0;

  public:
    // Overloaded on the types and/or numbers of arguments.
    virtual int Add()          = 0;
    virtual int Add(int times) = 0;

    // Overloaded on the const-ness of this object.
    // virtual Bar& GetBar() = 0;
    // virtual const Bar& GetBar() const = 0;
    virtual int GetBar()       = 0;
    virtual int GetBar() const = 0;
  };

  class MockFoo : public Foo
  {
  public:
    MOCK_METHOD2(Sum, int(int x, int y));
    MOCK_METHOD1(ComplexJob, bool(int x));

    MOCK_METHOD(int, Add, (), (override));
    MOCK_METHOD(int, Add, (int times), (override));

    MOCK_METHOD(int, GetBar, (), (override));
    MOCK_METHOD(int, GetBar, (), (const, override));
  };

  int CalculateSum(int x, int y) { return x + y; }

  class Helper
  {
    bool m_even{false};

  public:
    // return true when x is even.
    bool ComplexJobRelay(int x)
    {
      m_even = x % 2 ? false : true;
      return m_even;
    }

    bool getValue() { return m_even; }
  };

  struct CalSum
  {
    template <typename T>
    T operator()(T x, T y)
    {
      return x + y;
    }
  };
} // namespace gmock_action

/*
// ={=========================================================================
o Can calls mock object directly without client/user that use a mock

o With invoke(), can relay mock calls to other object, or free function.
  Should have the same signature.
*/

TEST(GMock, actions_and_invoke)
{
  using namespace gmock_action;

  // use invoke
  {
    MockFoo foo;
    Helper helper;

    EXPECT_CALL(foo, Sum(_, _)).WillOnce(Invoke(CalculateSum));

    EXPECT_CALL(foo, ComplexJob(_))
      .WillRepeatedly(Invoke(&helper, &Helper::ComplexJobRelay));

    // relays to CalculateSum(5, 6).
    EXPECT_THAT(foo.Sum(5, 6), 11);

    // foo.complexJob() gets called and it relays to
    // helper.ComplexJobRelay(10) and get a return from it.

    // helper gets changed as well? yes
    EXPECT_THAT(helper.getValue(), false);
    EXPECT_THAT(foo.ComplexJob(10), true);
    EXPECT_THAT(helper.getValue(), true);

    EXPECT_THAT(foo.ComplexJob(5), false);
    EXPECT_THAT(helper.getValue(), false);
  }

  // without using matcher
  {
    MockFoo foo;
    Helper helper;

    EXPECT_CALL(foo, Sum).WillOnce(Invoke(CalculateSum));

    EXPECT_CALL(foo, ComplexJob)
      .WillOnce(Invoke(&helper, &Helper::ComplexJobRelay));

    // Invokes CalculateSum(5, 6).
    EXPECT_THAT(foo.Sum(5, 6), 11);

    // Invokes helper.ComplexJob(10);
    EXPECT_THAT(foo.ComplexJob(10), true);
  }

  {
    MockFoo foo;
    Helper helper;

    // use function address
    EXPECT_CALL(foo, Sum).WillOnce(&CalculateSum);

    EXPECT_CALL(foo, ComplexJob)
      .WillOnce(Invoke(&helper, &Helper::ComplexJobRelay));

    // Invokes CalculateSum(5, 6).
    EXPECT_THAT(foo.Sum(5, 6), 11);

    // Invokes helper.ComplexJob(10);
    EXPECT_THAT(foo.ComplexJob(10), true);
  }

  // use f, lambda. have to match up with a signature of function to mock.
  {
    MockFoo foo;

    EXPECT_CALL(foo, Sum).WillOnce([](int x, int y) { return x + y; });

    EXPECT_CALL(foo, ComplexJob).WillOnce([](int x) {
      return x % 2 ? false : true;
    });

    // Invokes CalculateSum(5, 6).
    EXPECT_THAT(foo.Sum(5, 6), 11);

    // Invokes helper.ComplexJob(10);
    EXPECT_THAT(foo.ComplexJob(10), true);
  }

  // use functor
  {
    MockFoo foo;

    EXPECT_CALL(foo, Sum).WillOnce(CalSum{});

    // Invokes CalculateSum(5, 6).
    EXPECT_THAT(foo.Sum(5, 6), 11);
  }
}

/*
={=========================================================================
https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#mocking-private-or-protected-methods

Mocking Private or Protected Methods

You must always put a mock method definition (MOCK_METHOD) in a public:
section of the mock class, regardless of the method being mocked being
public, protected, or private in the base class. This allows ON_CALL and
EXPECT_CALL to reference the mock function from outside of the mock class.
(Yes, C++ allows a subclass to change the access level of a virtual
function in the base class.) 

(see Foo above which mocks private methods)

Mocking Overloaded Methods

You can mock overloaded functions as usual. No special attention is
required:

*/

TEST(GMock, mock_methods)
{
  using namespace gmock_action;

  {
    MockFoo foo;

    // note that return type should be matched as well. otherwise compile
    // error and has "return 0" in it.
    EXPECT_CALL(foo, Add()).WillOnce([]() {
        std::cout << "foo.Add() called\n";
        return 0;
        });

    EXPECT_CALL(foo, Add(_)).WillOnce([](int) {
        std::cout << "foo.Add(int) called\n";
        return 0;
        });

    foo.Add();
    foo.Add(10);
  } 

  {
    MockFoo foo;
    const MockFoo cfoo;

    // note that return type should be matched as well. otherwise compile
    // error and has "return 0" in it.
    EXPECT_CALL(foo, GetBar()).WillOnce([]() {
        std::cout << "foo.GetBar() called\n";
        return 0;
        });

    EXPECT_CALL(cfoo, GetBar()).WillOnce([]() {
        std::cout << "cfoo.GetBar() called\n";
        return 0;
        });

    foo.GetBar();
    cfoo.GetBar();
  } 
}

/*
={=========================================================================
https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#testing-asynchronous-behavior

Testing Asynchronous Behavior

One oft-encountered problem with gMock is that it can be hard to test
asynchronous behavior. Suppose you had a EventQueue class that you wanted
to test, and you created a separate EventDispatcher interface so that you
could easily mock it out. However, the implementation of the class fired
all the events on a background thread, which made test timings difficult.
You could just insert sleep() statements and hope for the best, but that
makes your test behavior nondeterministic. A better way is to use gMock
actions and Notification objects to force your asynchronous test to behave
synchronously.

class MockEventDispatcher : public EventDispatcher {
  MOCK_METHOD(bool, DispatchEvent, (int32), (override));
};

// define action "Notiry(notification)"

ACTION_P(Notify, notification) {
  notification->Notify();
}

TEST(EventQueueTest, EnqueueEventTest) 
{
  MockEventDispatcher mock_event_dispatcher;

  EventQueue event_queue(&mock_event_dispatcher);

  const int32 kEventId = 321;
  absl::Notification done;
  EXPECT_CALL(mock_event_dispatcher, DispatchEvent(kEventId))
      .WillOnce(Notify(&done));

  event_queue.EnqueueEvent(kEventId);
  done.WaitForNotification();
}

In the example above, we set our normal gMock expectations, but then add an
additional action to notify the Notification object. Now we can just call
Notification::WaitForNotification() in the main thread to wait for the
asynchronous call to finish. After that, our test suite is complete and we
can safely exit.

{: .callout .note} Note: this example has a downside: namely, if the
expectation is not satisfied, our test will run forever. It will eventually
time-out and fail, but it will take longer and be slightly harder to debug.
To alleviate this problem, you can use WaitForNotificationWithTimeout(ms)
instead of WaitForNotification().

*/

/*
// ={=========================================================================
As can see from:

googletest/include/gtest/internal/gtest-port.h

# elif GTEST_HAS_PTHREAD
// Allows a controller thread to pause execution of newly created
// threads until notified.  Instances of this class must be created
// and destroyed in the controller thread.
//
// This class is only for testing Google Test's own constructs. Do not
// use it in user tests, either directly or indirectly.
class Notification {
 public:
  Notification() : notified_(false) {
    GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_init(&mutex_, nullptr));
  }
  ~Notification() {
    pthread_mutex_destroy(&mutex_);
  }

  // Notifies all threads created with this notification to start. Must
  // be called from the controller thread.
  void Notify() {
    pthread_mutex_lock(&mutex_);
    notified_ = true;
    pthread_mutex_unlock(&mutex_);
  }

  // Blocks until the controller thread notifies. Must be called from a test
  // thread.
  void WaitForNotification() {
    for (;;) {
      pthread_mutex_lock(&mutex_);
      const bool notified = notified_;
      pthread_mutex_unlock(&mutex_);
      if (notified)
        break;
      SleepMilliseconds(10);
    }
  }

 private:
  pthread_mutex_t mutex_;
  bool notified_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(Notification);
};

simply waits for "notified" in a loop.

*/

TEST(GMock, mock_asnchronous_behaviours_1)
{
  using namespace gmock_action;

  MockFoo foo;

  Notification done;

  // "Notify" action is not available so use invoke instead
  // EXPECT_CALL(foo, Sum).WillOnce(Notify(&done));

  EXPECT_CALL(foo, Sum).WillOnce([&](int, int) {
    done.Notify();
    return 0;
  });

  std::async(std::launch::async, [&] {
    for (int i = 0; i < 20; ++i)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::cout << "waits count is " << i << std::endl;
    }
    foo.Sum(10, 10);
  });

  // not available
  // done.WaitForNotificationWithTimeout(1000);

  done.WaitForNotification();
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

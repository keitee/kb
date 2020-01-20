// https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#top

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <QString>

unsigned int Factorial(unsigned int number)
{
  return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial]")
{
  // cause error
  // REQUIRE( Factorial(0) == 1 );
  REQUIRE(Factorial(1) == 1);
  REQUIRE(Factorial(2) == 2);
  REQUIRE(Factorial(3) == 6);
  REQUIRE(Factorial(10) == 3628800);
}

// Test cases and sections
//
// Most test frameworks have a class-based fixture mechanism. That is, test
// cases map to methods on a class and common setup and teardown can be
// performed in setup() and teardown() methods (or constructor/ destructor in
// languages, like C++, that support deterministic destruction).

// While Catch fully supports this way of working there are a few problems with
// the approach. In particular the way your code must be split up, and the blunt
// granularity of it, may cause problems. You can only have one setup/ teardown
// pair across a set of methods, but sometimes you want slightly different setup
// in each method, or you may even want several levels of setup (a concept which
// we will clarify later on in this tutorial). It was problems like these that
// led James Newkirk, who led the team that built NUnit, to start again from
// scratch and build xUnit).

// Catch takes a different approach (to both NUnit and xUnit) that is a more
// natural fit for C++ and the C family of languages.

// For each SECTION "the TEST_CASE is executed from the start" - so as we enter
// each section we know that size is 5 and capacity is at least 5. We enforced
// those requirements with the REQUIREs at the top level so we can be confident
// in them. This works because the SECTION macro contains an if statement that
// calls back into Catch to see if the section should be executed. One leaf
// section is executed on each run through a TEST_CASE. The other sections are
// skipped. Next time through the next section is executed, and so on until no
// new sections are encountered.

TEST_CASE("vectors can be sized and resized", "[vector]")
{
  std::vector<int> v(5);

  REQUIRE(v.size() == 5);
  REQUIRE(v.capacity() >= 5);

  SECTION("resizing bigger changes size and capacity")
  {
    v.resize(10);

    REQUIRE(v.size() == 10);
    REQUIRE(v.capacity() >= 10);
  }

  SECTION("resizing smaller changes size but not capacity")
  {
    v.resize(0);

    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() >= 5);
  }

  /*
  SECTION("reserving bigger changes capacity but not size")
  {
    v.reserve(10);

    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 10);
  }

  SECTION("reserving smaller does not change size or capacity")
  {
    v.reserve(0);

    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 5);
  }
  */

  // So far so good - this is already an improvement on the setup/teardown
  // approach because now we see our setup code inline and use the stack.
  //
  // The power of sections really shows, however, when we need to execute a
  // "sequence of checked operations." Continuing the vector example, we might
  // want to verify that attempting to reserve a capacity smaller than the
  // current capacity of the vector changes nothing. We can do that, naturally,
  // like so:
  //
  // Sections can be nested to an arbitrary depth (limited only by your stack
  // size). Each leaf section (i.e. a section that contains no nested sections)
  // will be executed exactly once, on a separate path of execution from any
  // other leaf section (so no leaf section can interfere with another). A
  // failure in a parent section will prevent nested sections from running - but
  // then that's the idea.

  SECTION("reserving bigger changes capacity but not size")
  {
    v.reserve(10);

    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 10);

    SECTION("reserving smaller again does not change capacity")
    {
      v.reserve(7);

      REQUIRE(v.capacity() >= 10);
    }
  }
}

SCENARIO("vectors can be sized and resized", "[vector]")
{
  GIVEN("A vector with some items")
  {
    std::vector<int> v(5);

    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 5);

    WHEN("the size is increased")
    {
      v.resize(10);

      THEN("the size and capacity change")
      {
        REQUIRE(v.size() == 10);
        REQUIRE(v.capacity() >= 10);
      }
    }
    WHEN("the size is reduced")
    {
      v.resize(0);

      THEN("the size changes but not capacity")
      {
        REQUIRE(v.size() == 0);
        REQUIRE(v.capacity() >= 5);
      }
    }
    WHEN("more capacity is reserved")
    {
      v.reserve(10);

      THEN("the capacity changes but not the size")
      {
        REQUIRE(v.size() == 5);
        REQUIRE(v.capacity() >= 10);
      }
    }
    WHEN("less capacity is reserved")
    {
      v.reserve(0);

      THEN("neither size nor capacity are changed")
      {
        REQUIRE(v.size() == 5);
        REQUIRE(v.capacity() >= 5);
      }
    }
  }
}

TEST_CASE("Some.Tests", "[xx]")
{
  SECTION("Something", "")
  {
    CHECK(QString() == QString());
    const QString someString(QLatin1String("foo"));
    CHECK(someString == QLatin1String("foo"));
  }
}

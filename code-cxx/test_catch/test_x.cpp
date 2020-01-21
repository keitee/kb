#include "catch.hpp"

TEST_CASE("Some.Tests", "[xx]")
{
  SECTION("Something", "")
  {
    std::string s1;
    CHECK(std::string() == s1);
  }
}

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <bitset>
// #include <boost/algorithm/string.hpp>
// #include <boost/regex.hpp>
// #include <boost/cast.hpp>
// #include <boost/lexical_cast.hpp>
#include <regex>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::StrEq;
using testing::FloatEq;


// ={=========================================================================
// string-pos

namespace U30_Text
{
  struct uri_parts
  {
    std::string protocol;
    std::string domain;
    int port;
    std::string path;
    std::string query;
    std::string fragment;
  };

  // C++17 features:
  //
  // struct uri_parts
  // {
  //   std::string                protocol;
  //   std::string                domain;
  //   std::optional<int>         port;
  //   std::optional<std::string> path;
  //   std::optional<std::string> query;
  //   std::optional<std::string> fragment;
  // };

  uri_parts parse_uri(std::string uri)
  {
    std::regex rx(R"(^(\w+):\/\/([\w+.-]+)(:(\d+))?([\w\/\.]+)?(\?([\w=&]*)(#?(\w+))?)?$)");

    auto matches = std::smatch();

    auto found = std::regex_match(uri, matches, rx);
    EXPECT_TRUE(found);

    // if (found)
    // {
    //   uri_parts parts;
    //   parts.protocol = matches[1].str();
    // }
  }

} // namespace

TEST(XX, XX)
{
  using namespace U30_Text;

  {
    auto p2 = parse_uri("https://bbc.com:80/en/index.html?lite=true#ui");
  }

}



// ={=========================================================================
int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

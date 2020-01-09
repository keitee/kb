#include "gmock/gmock.h"
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>

using namespace std;
using namespace std::placeholders;
using namespace testing;

/*
https://github.com/open-source-parsers/jsoncpp


Installation and running

With apt or apt-get

The easiest way to use it from Ubuntu or another flavor of Debian Linux, is to
install is as:

sudo apt-get install libjsoncpp-dev

One drawback is that it will not install the last version. For Ubuntu 18.04.1,
the version is 1.7.4, while the last version as per August 2018 is 1.8.4. The
problem was much worse for 14.04, which installed 0.6.0, while the last version
as per February 2016 was 1.7.4.

To use JsonCpp, include:

   #include <jsoncpp/json/json.h>

To compile a file, add flag

   -ljsoncpp

*/

/*
// ={=========================================================================

Create file alice.json with the following contents:

{
    "book":"Alice in Wonderland",
    "year":1865,
    "characters":
    [
        {"name":"Jabberwock", "chapter":1},
        {"name":"Cheshire Cat", "chapter":6},
        {"name":"Mad Hatter", "chapter":7}
    ]
}

*/

TEST(CxxJSON, ex1)
{
  std::ifstream ifs("alice.json");

  Json::Reader reader;
  Json::Value val;

  reader.parse(ifs, val);

  std::cout << "book: " << val["book"].asString() << std::endl;
  std::cout << "year: " << val["year"].asUInt() << std::endl;

  // array of characters
  const Json::Value &chars= val["characters"];
  
  for (int i = 0; i < chars.size(); i++)
  {
    std::cout << "characters:    name: " << chars[i]["name"].asString() << std::endl;
    std::cout << "characters: chapter: " << chars[i]["chapter"].asString() << std::endl;
  }
} // namespace use_sizeof


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include "gmock/gmock.h"
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>

using namespace std;
using namespace testing;


/*
// ={=========================================================================
https://en.wikibooks.org/wiki/JsonCpp

While strict JSON syntax does not allow any comments, and requires the root
value to be array or object, JsonCpp allows both C-style and C++-style comments,
and allows the root value to be of any type.

As for February 2016, there are totally hundreds of libraries for parsing and
generating JSON on 62 languages, including 22 different libraries for C++.[1]

JsonCpp is probably the most popular C++ library. Another popular library is
rapidjson, which is very fast.


An array. It is denoted by a comma-separated list of any values in square
brackets. Example:

   [1, 2, 3, "Hello world\n", true]

An object, also called associative array, keyed list etc. It is denoted by
comma-separated list of pairs in braces. A pair has a form

   key : value

where key is a string, value is any JSON value. Example:

   {"foo":1, "bar":2, "baz":3.14, "hello world":[1,2,3]}


// project repo
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


example:

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
  std::ifstream ifs("../alice.json");

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

// [ RUN      ] CxxJSON.ex2
// {
//         "book" : "Alice in Wonderland",
//         "characters" :
//         [
//                 {
//                         "chapter" : 1,
//                         "name" : "Jabberwock"
//                 },
//                 {
//                         "chapter" : 6,
//                         "name" : "Cheshire Cat"
//                 },
//                 {
//                         "chapter" : 7,
//                         "name" : "Mad Hatter"
//                 }
//         ],
//         "year" : 1865
// }
// [       OK ] CxxJSON.ex2 (0 ms)

TEST(CxxJSON, ex2)
{
  // create the characters array
  Json::Value ch;
  ch[0]["name"] = "Jabberwock";
  ch[0]["chapter"] = 1;
  ch[1]["name"] = "Cheshire Cat";
  ch[1]["chapter"] = 6;
  ch[2]["name"] = "Mad Hatter";
  ch[2]["chapter"] = 7;

  // create the main object
  Json::Value val;
  val["book"] = "Alice in Wonderland";
  val["year"] = 1865;
  val["characters"] = ch;

  std::cout << val << std::endl;
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

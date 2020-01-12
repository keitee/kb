#include "gmock/gmock.h"
#include <fstream>
#include <incbin.h>
#include <iostream>
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
  const Json::Value &chars = val["characters"];

  for (int i = 0; i < chars.size(); i++)
  {
    std::cout << "characters:    name: " << chars[i]["name"].asString()
              << std::endl;
    std::cout << "characters: chapter: " << chars[i]["chapter"].asString()
              << std::endl;
  }
}

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
  ch[0]["name"]    = "Jabberwock";
  ch[0]["chapter"] = 1;
  ch[1]["name"]    = "Cheshire Cat";
  ch[1]["chapter"] = 6;
  ch[2]["name"]    = "Mad Hatter";
  ch[2]["chapter"] = 7;

  // create the main object
  Json::Value val;
  val["book"]       = "Alice in Wonderland";
  val["year"]       = 1865;
  val["characters"] = ch;

  std::cout << val << std::endl;
}

namespace
{
  // https://github.com/graphitemaster/incbin
  // NOTE: "alice.json" should be in the same directory where compilation
  // happens.
  INCBIN(AConfig, "../alice.json");
} // namespace

// __asm__(".section " ".rodata" "\n" ".global " "g" "DrmConfigData" "\n" ".type
// " "g" "DrmConfigData" ", @object\n" ".balign " "16" "\n" "" "g" "DrmConfig"
// "Data" ":\n" ".incbin" " \"" "alice.json" "\"\n" ".global " "g"
// "DrmConfigEnd" "\n" ".type " "g" "DrmConfigEnd" ", @object\n" ".balign 1\n"
// "" "g" "DrmConfig" "End" ":\n" ".byte " "1\n" ".global " "g" "DrmConfigSize"
// "\n" ".type " "g" "DrmConfigSize" ", @object\n" ".balign " "16" "\n" "" "g"
// "DrmConfig" "Size" ":\n" ".int " "" "g" "DrmConfig" "End" " - " "" "g"
// "DrmConfig" "Data" "\n" ".balign " "16" "\n" ".text\n" );
// extern "C" const __attribute__((aligned(16))) unsigned char gDrmConfigData[];
// extern "C" const __attribute__((aligned(16))) unsigned char *const
// gDrmConfigEnd; extern "C" const unsigned int gDrmConfigSize;

/*
[ RUN      ] CxxJSON.ex3
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

//
{
        "book" : "Alice in Wonderland",
        "characters" :
        [
                {
                        "chapter" : 1,
                        "name" : "Jabberwock"
                },
                {
                        "chapter" : 6,
                        "name" : "Cheshire Cat"
                },
                {
                        "chapter" : 7,
                        "name" : "Mad Hatter"
                }
        ],
        "year" : 1865
}

// from styled write
{
   "book" : "Alice in Wonderland",
   "characters" : [
      {
         "chapter" : 1,
         "name" : "Jabberwock"
      },
      {
         "chapter" : 6,
         "name" : "Cheshire Cat"
      },
      {
         "chapter" : 7,
         "name" : "Mad Hatter"
      }
   ],
   "year" : 1865
}

[       OK ] CxxJSON.ex3 (0 ms)


http://open-source-parsers.github.io/jsoncpp-docs/doxygen/class_json_1_1_reader.html#a0b3c4e24c8393354bab57a6ba3ffc27f

parse() [2/3]

bool Json::Reader::parse
(
const char * 	beginDoc,
const char * 	endDoc,
Value & 	root,
bool 	collectComments = true
)

Read a Value from a JSON document.

Parameters
beginDoc	Pointer on the beginning of the UTF-8 encoded string of the
document to read.
endDoc	Pointer on the end of the UTF-8 encoded string of the document to read.
Must be >= beginDoc.
root	[out] Contains the root value of the document if it was successfully
parsed. collectComments	true to collect comment and allow writing them back
during serialization, false to discard comments. This parameter is ignored if
Features::allowComments_ is false.

Returns
true if the document was successfully parsed, false if an error occurred.

*/

TEST(CxxJSON, ex3)
{
  // manually print out
  {
    for (int i = 0; i < gAConfigSize; i++)
    {
      std::cout << gAConfigData[i];
    }
    std::cout << std::endl;
  }

  // use `reader`
  {
    Json::Reader reader;
    Json::Value root;
    std::string errors;

    // parse the config file
    const char *fstart = reinterpret_cast<const char *>(gAConfigData);
    const char *fend   = reinterpret_cast<const char *>(fstart + gAConfigSize);

    // if (!reader.parse(fstart, fend, root, &errors))
    if (!reader.parse(fstart, fend, root))
    {
      std::cout << "failed to parse builtin json: " << errors << std::endl;
    }

    std::cout << root << std::endl;

    Json::StyledWriter writer;
    // virtual std::string write(const Value &root); // write JSON object to a
    // string
    std::cout << writer.write(root) << std::endl;
  }
}

/*
more drm.json
{
  "uris": [
    {
      "path": "/as/drm/status",
      "method": "ws",
      "thread": "AS_WS_DRM",
      "content": {
        "status": "UNAVAILABLE"
      }
    }
  ]
}

more netflix.json
{
  "uris": [
    {
      "path": "/as/apps/netflix",
      "method": "get",
      "content": {
        "version": 1,
        "esn": "0",
        "swversion": "",
        "suspended": false
      }
    }
  ]
}
*/

namespace
{
  // https://github.com/graphitemaster/incbin
  INCBIN(DrmConfig, "../drm.json");
  INCBIN(NetflixConfig, "../netflix.json");
} // namespace

TEST(CxxJSON, ex4)
{
  // (to make life easier) we want to support json with single quotes
  // rather than mandated double quotes, so we use the non-standard
  // json reader object
  Json::CharReaderBuilder builder;
  builder["allowComments"]     = true;
  builder["allowSingleQuotes"] = true;

  auto reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());

  const std::map<std::string, std::pair<const char *, size_t>> config_files = {
    {"drm", {reinterpret_cast<const char *>(gDrmConfigData), gDrmConfigSize}},
    {"net",
     {reinterpret_cast<const char *>(gNetflixConfigData), gNetflixConfigSize}}};

  // pair{filename, root value}
  std::map<std::string, const Json::Value> parsed;

  // parse config files
  for (const auto &f : config_files)
  {
    const char *fstart      = f.second.first;
    const char *fend        = fstart + f.second.second;
    const std::string fname = f.first;

    Json::Value root;
    std::string error;

    // see Json::CharReaderBuilder
    if (!reader->parse(fstart, fend, &root, &error))
    {
      std::cout << "failed to parse " << fname << std::endl;
      continue;
    }

    parsed.emplace(std::move(fname), std::move(root));
  }

  // create `Value` with the given type
  //    enum ValueType
  //    {
  //       nullValue = 0, ///< 'null' value
  //       intValue,      ///< signed integer value
  //       uintValue,     ///< unsigned integer value
  //       realValue,     ///< double value
  //       stringValue,   ///< UTF-8 string value
  //       booleanValue,  ///< bool value
  //       arrayValue,    ///< array value (ordered list)
  //       objectValue    ///< object value (collection of name/value pairs).
  //    };

  const std::vector<std::string> allowed_fields{"path", "method", "thread"};

  Json::Value urls(Json::arrayValue);

  // urls
  //   [{
  //       "method" : "ws",
  //       "path" : "/as/drm/status",
  //       "thread" : "AS_WS_DRM"
  //    },
  //    {
  //       "method" : "get",
  //       "path" : "/as/apps/netflix"
  //    }
  //   ]

  for (const auto &f : parsed)
  {
    const auto &root = f.second;

    // get an value for key, "uris" and which is an array for this case
    auto config_urls = root["uris"];

    // std::cout << "config_urls: " << config_urls << std::endl;

    if (config_urls.isArray())
    {
      // each item in the array. there is single item in the array for this case
      // and which is a map.
      for (const auto &u : config_urls)
      {
        // std::cout << "u: " << u << std::endl;

        Json::Value item(Json::objectValue);

        // get fields to look for from a map
        for (const auto &field : allowed_fields)
        {
          // overwrite or append??
          if (u.isMember(field))
            item[field] = u[field];
        }

        urls.append(item);
      }
    }
    else
    {
      std::cout << "invalid uri field" << std::endl;
    }
  }

  // std::cout << "urls: " << urls << std::endl;
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

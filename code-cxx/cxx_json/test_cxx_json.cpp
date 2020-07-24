#include "gmock/gmock.h"
#include <fstream>
#include <incbin.h>
#include <iostream>
#include <jsoncpp/json/json.h>

using namespace std;
using namespace testing;

/*
// ={=========================================================================
https://www.json.org/json-en.html

Introducing JSON

NOTE: see pictures in the link which shows JOSN forms clearly.

JSON (JavaScript Object Notation) is a lightweight data-interchange format. It
is easy for humans to read and write. It is easy for machines to parse and
generate. It is based on a subset of the JavaScript Programming Language
Standard ECMA-262 3rd Edition - December 1999. JSON is a text format that is
completely language independent but uses conventions that are familiar to
programmers of the C-family of languages, including C, C++, C#, Java,
JavaScript, Perl, Python, and many others. These properties make JSON an ideal
data-interchange language.

JSON is built on two structures:

A collection of name/value pairs. In various languages, this is realized as an
object, record, struct, dictionary, hash table, keyed list, or associative
array.  An ordered list of values. In most languages, this is realized as an
array, vector, list, or sequence.  These are universal data structures.
Virtually all modern programming languages support them in one form or another.
It makes sense that a data format that is interchangeable with programming
languages also be based on these structures.

In JSON, they take on these forms:

An object is an unordered set of name/value pairs. An object begins with `{left
brace and ends with `}right brace`. Each name is followed by :colon and the
name/value pairs are separated by ,comma.

An array is an ordered collection of values. An array begins with [left bracket
and ends with ]right bracket. Values are separated by ,comma.

A value can be :
a "string" in double quotes, or 
a "number", or 
"true or false" or 
"null", or 
an "object" or 
an "array". 

These structures can be nested.

A string is a sequence of zero or more Unicode characters, wrapped in double
quotes, using backslash escapes. A character is represented as a single
character string. A string is very much like a C or Java string.

A number is very much like a C or Java number, except that the octal and
hexadecimal formats are not used.

Whitespace can be inserted between any pair of tokens. Excepting a few encoding
details, that completely describes the language.


// ={=========================================================================
https://en.wikibooks.org/wiki/JsonCpp
http://open-source-parsers.github.io/jsoncpp-docs/doxygen/index.html

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

/usr/include/jsoncpp/json/json.h
/usr/include/jsoncpp/json/value.h


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


Deprecated

http://open-source-parsers.github.io/jsoncpp-docs/doxygen/deprecated.html

when use writer:

// write() to std::string
// http://open-source-parsers.github.io/jsoncpp-docs/doxygen/class_json_1_1_styled_writer.html
// virtual std::string write(const Value &root); // write JSON object to a
// string
// std::cout << writer.write(servicelist) << std::endl;

use writer

Json::StyledWriter writer;
std::string response;
response.assign(writer.write(servicelist));

However, it's in the deprecated list:

Class Json::StyledWriter
Use StreamWriterBuilder.

Class Json::Reader
Use CharReader and CharReaderBuilder.


use stringstream for reading and writing.

std::ostringstream oss;
oss << servicelist;
std::string response;
response.assign(oss.str());


include/json/config.h
140:using OStream = std::ostream;
148:using JSONCPP_OSTREAM = Json::OStream;

//jsoncpp/src/lib_json/json_writer.cpp

OStream& operator<<(OStream& sout, Value const& root) {
  StreamWriterBuilder builder;
  StreamWriterPtr const writer(builder.newStreamWriter());
  writer->write(root, &sout);
  return sout;
}

//jsoncpp/src/lib_json/json_reader.cpp

IStream& operator>>(IStream& sin, Value& root) {
  CharReaderBuilder b;
  String errs;
  bool ok = parseFromStream(b, sin, &root, &errs);
  if (!ok) {
    throwRuntimeError(errs);
  }
  return sin;
}

*/

// [ RUN      ] CxxJSON.ex1
// book: Alice in Wonderland
// year: 1865
// characters:    name: Jabberwock
// characters: chapter: 1
// characters:    name: Cheshire Cat
// characters: chapter: 6
// characters:    name: Mad Hatter
// characters: chapter: 7
// book: Alice in Wonderland
// year: 1865
// characters:    name: Jabberwock
// characters: chapter: 1
// characters:    name: Cheshire Cat
// characters: chapter: 6
// characters:    name: Mad Hatter
// characters: chapter: 7
// [       OK ] CxxJSON.ex1 (1 ms)

// read json file
TEST(CxxJSON, jsoncpp_ex1)
{
  {
    std::ifstream ifs("../alice.json");

    Json::Reader reader;
    Json::Value root;

    // use reader and get `root`
    reader.parse(ifs, root);

    // check
    EXPECT_THAT(root.isMember("year"), true);
    EXPECT_THAT(root.isMember("book"), true);

    // check if it's string
    {
      const Json::Value &book = root["book"];
      EXPECT_THAT(book.isString(), true);
    }

    // access map
    // std::string asString() const
    std::cout << "year: " << root["year"].asUInt() << std::endl;
    std::cout << "book: " << root["book"].asString() << std::endl;
    std::cout << "year: " << root["year"].asUInt() << std::endl;

    // Json::Value is `variant`
    const Json::Value &book = root["book"];
    std::cout << "book: " << book.asString() << std::endl;

    const Json::Value &year = root["year"];
    std::cout << "year: " << year.asUInt() << std::endl;

    // `characters` maps to array
    const Json::Value &chars = root["characters"];

    // each array element is a map.
    for (int i = 0; i < chars.size(); i++)
    {
      const Json::Value &object = chars[i];

      // bool Json::Value::isObject	() const

      if (object.isObject() && !object.empty())
      {
        std::cout << "i : " << i << " is object and not empty" << std::endl;
      }

      std::cout << "characters:    name: " << chars[i]["name"].asString()
                << std::endl;
      std::cout << "characters: chapter: " << chars[i]["chapter"].asString()
                << std::endl;
    }
  }

  {
    // (to make life easier) we want to support json with single quotes
    // rather than mandated double quotes, so we use the non-standard
    // json reader object

    Json::CharReaderBuilder builder;
    builder["allowComments"]     = true;
    builder["allowSingleQuotes"] = true;

    std::ifstream ifs("../alice.json");
    Json::Value root;
    std::string errors;

    Json::parseFromStream(builder, ifs, &root, &errors);

    std::cout << "book: " << root["book"].asString() << std::endl;
    std::cout << "year: " << root["year"].asUInt() << std::endl;

    // NOTE: compiles but raise exception. what's the above comment?
    //
    // std::cout << "book: " << root['book'].asString() << std::endl;
    // std::cout << "year: " << root['year'].asUInt() << std::endl;

    // array of characters
    const Json::Value &chars = root["characters"];

    for (int i = 0; i < chars.size(); i++)
    {
      std::cout << "characters:    name: " << chars[i]["name"].asString()
                << std::endl;
      std::cout << "characters: chapter: " << chars[i]["chapter"].asString()
                << std::endl;
    }
  }
}

// read json file but use stringstream

TEST(CxxJSON, jsoncpp_ex1_1)
{
  {
    std::ifstream ifs("../alice.json");

    // Json::Reader reader;
    // Json::Value root;
    // reader.parse(ifs, root);

    Json::Value root;
    ifs >> root;

    // check
    EXPECT_THAT(root.isMember("year"), true);
    EXPECT_THAT(root.isMember("book"), true);

    // check if it's string
    {
      const Json::Value &book = root["book"];
      EXPECT_THAT(book.isString(), true);
    }

    // access map
    // std::string asString() const
    std::cout << "year: " << root["year"].asUInt() << std::endl;
    std::cout << "book: " << root["book"].asString() << std::endl;
    std::cout << "year: " << root["year"].asUInt() << std::endl;

    // Json::Value is `variant`
    const Json::Value &book = root["book"];
    std::cout << "book: " << book.asString() << std::endl;

    const Json::Value &year = root["year"];
    std::cout << "year: " << year.asUInt() << std::endl;

    // `characters` maps to array
    const Json::Value &chars = root["characters"];

    // each array element is a map.
    for (int i = 0; i < chars.size(); i++)
    {
      const Json::Value &object = chars[i];

      // bool Json::Value::isObject	() const

      if (object.isObject() && !object.empty())
      {
        std::cout << "i : " << i << " is object and not empty" << std::endl;
      }

      std::cout << "characters:    name: " << chars[i]["name"].asString()
                << std::endl;
      std::cout << "characters: chapter: " << chars[i]["chapter"].asString()
                << std::endl;
    }
  }
}

// what does `empty` mean?
//
// {
//     "book":"Alice in Wonderland",
//     "year":1865,
//     "characters":
//     [
//         {"name":"Jabberwock", "chapter":1},
//         {"name":"Cheshire Cat", "chapter":6},
//         {"name":"Mad Hatter", "chapter":7}
//         {"name":, "chapter":}                  // is it empty?
//     ],
//     "name":
// }
//
// [ RUN      ] CxxJSON.jsoncpp_ex1_1
// book: Alice in Wonderland
// year: 1865
// book: Alice in Wonderland
// year: 1865
// i : 0 is object and not empty
// characters:    name: Jabberwock
// characters: chapter: 1
// i : 1 is object and not empty
// characters:    name: Cheshire Cat
// characters: chapter: 6
// i : 2 is object and not empty
// characters:    name: Mad Hatter
// characters: chapter: 7
// [       OK ] CxxJSON.jsoncpp_ex1_1 (0 ms)

TEST(CxxJSON, jsoncpp_ex1_2)
{
  {
    std::ifstream ifs("../alice_empty.json");

    Json::Reader reader;
    Json::Value root;

    // use reader and get `root`
    reader.parse(ifs, root);

    // access map
    std::cout << "book: " << root["book"].asString() << std::endl;
    std::cout << "year: " << root["year"].asUInt() << std::endl;

    // Json::Value is `variant`
    const Json::Value &book = root["book"];
    std::cout << "book: " << book.asString() << std::endl;

    const Json::Value &year = root["year"];
    std::cout << "year: " << year.asUInt() << std::endl;

    // `characters` maps to array
    const Json::Value &chars = root["characters"];

    //         {"name":, "chapter":}                  // is it empty?
    // is not counted in size()
    for (int i = 0; i < chars.size(); i++)
    {
      const Json::Value &object = chars[i];

      // bool Json::Value::isObject	() const

      if (object.isObject() && !object.empty())
      {
        std::cout << "i : " << i << " is object and not empty" << std::endl;
      }
      else
      {
        std::cout << "i : " << i << " is object but empty" << std::endl;
      }

      std::cout << "characters:    name: " << chars[i]["name"].asString()
                << std::endl;
      std::cout << "characters: chapter: " << chars[i]["chapter"].asString()
                << std::endl;
    }

    // here it is "empty"
    const Json::Value name = root["name"];

    EXPECT_THAT(name.isNull(), true);
    EXPECT_THAT(name.empty(), true);
  }
}

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
//         "year1" : 1865,
//         "year2" : "1865"
// }

// construct json from code
TEST(CxxJSON, jsoncpp_ex2)
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
  val["year1"]       = 1865;
  val["year2"]       = "1865";
  val["characters"] = ch;

  std::cout << val << std::endl;
}

// "string" and number are different? how?
TEST(CxxJSON, jsoncpp_check_type)
{
  // create the main object
  Json::Value val;

  val["year1"]       = 1865;
  val["year2"]       = "1865";

  // "year1" is number
  EXPECT_THAT(val["year1"].isString(), false);
  EXPECT_THAT(val["year1"].isNumeric(), true);

  // get type
  // std::cout << val["year1"].type() << std::endl;
  EXPECT_THAT(val["year1"].type(), Json::intValue);     // 1

  // it can be converted to string as well
  std::cout << val["year1"].asString() << std::endl;
  std::cout << val["year1"].asInt() << std::endl;

  // "year2" is string
  EXPECT_THAT(val["year2"].isString(), true);
  EXPECT_THAT(val["year2"].isNumeric(), false);

  // get type
  // std::cout << val["year2"].type() << std::endl;
  EXPECT_THAT(val["year2"].type(), Json::stringValue);  // 4

  std::cout << val["year2"].asString() << std::endl;

  // NOTE: however, string not be converted to number
  try
  {
    std::cout << val["year2"].asInt() << std::endl;
  }
  catch(const exception &e)
  {
    EXPECT_THAT(std::string(e.what()), "Value is not convertible to Int.");
  }
}

namespace
{
  // https://github.com/graphitemaster/incbin
  // TODO: "alice.json" should be in the same directory where compilation
  // happens. HOW can specify a directory??

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
// extern "C" const __attribute__((aligned(16))) unsigned char *const gDrmConfigEnd;
// extern "C" const unsigned int gDrmConfigSize;

/*
[ RUN      ] CxxJSON.jsoncpp_ex3
======================
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

======================
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
======================
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

[       OK ] CxxJSON.jsoncpp_ex3 (0 ms)


http://open-source-parsers.github.io/jsoncpp-docs/doxygen/class_json_1_1_reader.html#a0b3c4e24c8393354bab57a6ba3ffc27f

bool Json::Reader::parse(
  const char * 	beginDoc,
  const char * 	endDoc,
  Value & 	root,
  bool 	collectComments = true
);

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

TEST(CxxJSON, jsoncpp_ex3)
{
  std::cout << "======================" << std::endl;
  // char array as reads
  {
    for (int i = 0; i < gAConfigSize; i++)
    {
      std::cout << gAConfigData[i];
    }
    std::cout << std::endl;
  }

  std::cout << "======================" << std::endl;
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

    // use writer
    std::cout << "======================" << std::endl;

    Json::StyledWriter writer;

    // write() to std::string
    // http://open-source-parsers.github.io/jsoncpp-docs/doxygen/class_json_1_1_styled_writer.html
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

TEST(CxxJSON, jsoncpp_ex4)
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

    // push `root` of the parsed
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

  // http://open-source-parsers.github.io/jsoncpp-docs/doxygen/class_json_1_1_value.html#ada6ba1369448fb0240bccc36efaa46f7
  // Create a default Value of the given type.

  Json::Value urls(Json::arrayValue);

  // filters out the parsed json and construct one:
  //
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

        // objectValue
        // object value (collection of name/value pairs).

        Json::Value item(Json::objectValue);

        // get fields to look for from a map
        // NOTE: filters out them based on `allowed_fields`

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

// read directly from raw std::string

namespace
{
  const std::string ConfigJson = R"JSON(
{
  "uris": [
    {
      "path": "/as/drm/status",
      "method": "ws",
      "thread": "AS_WS_PLAYER"
    },
    {
      "path": "/as/players/<vwid>/status",
      "method": "ws",
      "thread": "AS_WS_PLAYER"
    },
    {
      "path": "/as/players/<vwid>/action/watchpvr",
      "method": "post",
      "expectsBody": false
    },
    {
      "path": "/as/players/<vwid>/action/watchbdl",
      "method": "post",
      "expectsBody": false
    }
  ]
}
)JSON";

} // namespace

TEST(CxxJSON, jsoncpp_ex5) 
{
  Json::Reader reader;
  Json::Value root;

  // parse the json
  if (!reader.parse(ConfigJson.data(),
                    ConfigJson.data() + ConfigJson.length(),
                    root))
  {
    std::cout << "failed to parse config json" << std::endl;
    return;
  }

  if (!root.isObject())
  {
    std::cout << "invalid root config object" << std::endl;
    return;
  }

  static const Json::StaticString domain("domain");
  static const Json::StaticString urisName("uris");
  static const Json::StaticString sysInfoName("providesSystemInfo");
  static const Json::StaticString sysStatusName("providesSystemStatus");
  static const Json::StaticString testPrefsName("testPreferences");
  static const Json::StaticString sysSettingsName("systemSettings");

  {
    const Json::Value domainName = root[domain];

    if (domainName.isString())
    {
      std::cout << "domain : " << domainName.asString() << std::endl;
    }
    else if (!domainName.isNull())
    {
      // not string and not null. shall be string or null.
      std::cout << "invalid domain name format" << std::endl;
    }
  }

  {
    const Json::Value uris = root[urisName];

    if (uris.isArray())
    {
      for (const Json::Value &uri : uris)
      {
        const Json::Value path = uri["path"];
        const Json::Value method = uri["method"];

        if (path.isString())
        {
          std::cout << "path   : " << path.asString() << std::endl;
        }
        else if (!path.isNull())
        {
          std::cout << "invalid path format" << std::endl;
        }

        if (method.isString())
        {
          std::cout << "method : " << method.asString() << std::endl;
        }
        else if (!method.isNull())
        {
          std::cout << "invalid method format" << std::endl;
        }
      }
    }
    else if (!uris.isNull())
    {
      // not string and not null. shall be array or null.
      std::cout << "invalid uris format" << std::endl;
    }
  }
}

// do the same as above but use std::string directly
//
// http://open-source-parsers.github.io/jsoncpp-docs/doxygen/class_json_1_1_reader.html#af1da6c976ad1e96c742804c3853eef94
//
// bool Json::Reader::parse	(	const std::string & 	document,
// Value & 	root,
// bool 	collectComments = true 
// )	

TEST(CxxJSON, jsoncpp_ex5_1) 
{
  Json::Reader reader;
  Json::Value root;

  // parse the json
  if (!reader.parse(ConfigJson, root))
  {
    std::cout << "failed to parse config json" << std::endl;
    return;
  }

  if (!root.isObject())
  {
    std::cout << "invalid root config object" << std::endl;
    return;
  }

  static const Json::StaticString domain("domain");
  static const Json::StaticString urisName("uris");
  static const Json::StaticString sysInfoName("providesSystemInfo");
  static const Json::StaticString sysStatusName("providesSystemStatus");
  static const Json::StaticString testPrefsName("testPreferences");
  static const Json::StaticString sysSettingsName("systemSettings");

  {
    const Json::Value domainName = root[domain];

    if (domainName.isString())
    {
      std::cout << "domain : " << domainName.asString() << std::endl;
    }
    else if (!domainName.isNull())
    {
      // not string and not null. shall be string or null.
      std::cout << "invalid domain name format" << std::endl;
    }
  }

  {
    const Json::Value uris = root[urisName];

    if (uris.isArray())
    {
      for (const Json::Value &uri : uris)
      {
        const Json::Value path = uri["path"];
        const Json::Value method = uri["method"];

        if (path.isString())
        {
          std::cout << "path   : " << path.asString() << std::endl;
        }
        else if (!path.isNull())
        {
          std::cout << "invalid path format" << std::endl;
        }

        if (method.isString())
        {
          std::cout << "method : " << method.asString() << std::endl;
        }
        else if (!method.isNull())
        {
          std::cout << "invalid method format" << std::endl;
        }
      }
    }
    else if (!uris.isNull())
    {
      // not string and not null. shall be array or null.
      std::cout << "invalid uris format" << std::endl;
    }
  }
}

//  {
//    "dvbtriplet": "318.4.8583", 
//    "servicetypes": [
//      "DTT"
//    ], 
//    "c": "21", 
//    "t": "Rai 4", 
//    "sid": "M13e-4-2187", 
//    "sf": "sd", 
//    "xsg": 8
//  }, 
//  {
//    "dvbtriplet": "318.4.8585", 
//    "servicetypes": [
//      "DTT"
//    ], 
//    "c": "24", 
//    "t": "Rai Movie", 
//    "sid": "M13e-4-2189", 
//    "sf": "sd", 
//    "xsg": 8
//  },
//
// [ RUN      ] CxxJSON.jsoncpp_ex6
// {
//    "documentId" : "1563970127340",
//    "services" : [
//       {
//          "c" : "21",
//          "dvbtriplet" : "318.4.8583",
//          "servicetypes" : [ "DTT", "OTT" ],
//          "sf" : "sd",
//          "sid" : "M13e-4-2187",
//          "t" : "Rai 4",
//          "xsg" : 8
//       },
//       {
//          "c" : "24",
//          "dvbtriplet" : "318.4.8585",
//          "servicetypes" : [ "DTT" ],
//          "sf" : "sd",
//          "sid" : "M13e-4-2189",
//          "t" : "Rai Movie",
//          "xsg" : 8
//       }
//    ],
//    "version" : 1
// }
// 
// [       OK ] CxxJSON.jsoncpp_ex6 (0 ms)

// construct and write ex
TEST(CxxJSON, jsoncpp_ex6)
{
  Json::Value servicelist;

  servicelist["documentId"] = "1563970127340";
  servicelist["version"] = 1;

  Json::Value service1;
  service1["dvbtriplet"] = "318.4.8583"; 

  service1["servicetypes"] = Json::Value(Json::arrayValue);
  service1["servicetypes"].append("DTT");
  service1["servicetypes"].append("OTT");

  service1["c"] = "21";
  service1["t"] = "Rai 4";
  service1["sid"] = "M13e-4-2187";
  service1["sf"] = "sd";
  service1["xsg"] = 8;

  Json::Value service2;
  service2["dvbtriplet"] = "318.4.8585"; 
  service2["servicetypes"] = Json::Value(Json::arrayValue);
  service2["servicetypes"].append("DTT");
  service2["c"] = "24";
  service2["t"] = "Rai Movie";
  service2["sid"] = "M13e-4-2189";
  service2["sf"] = "sd";
  service2["xsg"] = 8;

  servicelist["services"] = Json::Value(Json::arrayValue);
  servicelist["services"].append(service1);
  servicelist["services"].append(service2);

  // use writer
  Json::StyledWriter writer;

  // write() to std::string
  // http://open-source-parsers.github.io/jsoncpp-docs/doxygen/class_json_1_1_styled_writer.html
  // virtual std::string write(const Value &root); // write JSON object to a
  // string
  // std::cout << writer.write(servicelist) << std::endl;

  std::string response;

  // no need to to this since write() returns std::string.
  // response.assign(writer.write(servicelist).c_str());

  response.assign(writer.write(servicelist));

  std::cout << response << std::endl;
}

// use stringstream instead and see commment about deprecated list above

TEST(CxxJSON, jsoncpp_ex6_1)
{
  Json::Value servicelist;

  servicelist["documentId"] = "1563970127340";
  servicelist["version"] = 1;

  Json::Value service1;
  service1["dvbtriplet"] = "318.4.8583"; 

  service1["servicetypes"] = Json::Value(Json::arrayValue);
  service1["servicetypes"].append("DTT");
  service1["servicetypes"].append("OTT");

  service1["c"] = "21";
  service1["t"] = "Rai 4";
  service1["sid"] = "M13e-4-2187";
  service1["sf"] = "sd";
  service1["xsg"] = 8;

  Json::Value service2;
  service2["dvbtriplet"] = "318.4.8585"; 
  service2["servicetypes"] = Json::Value(Json::arrayValue);
  service2["servicetypes"].append("DTT");
  service2["c"] = "24";
  service2["t"] = "Rai Movie";
  service2["sid"] = "M13e-4-2189";
  service2["sf"] = "sd";
  service2["xsg"] = 8;

  servicelist["services"] = Json::Value(Json::arrayValue);
  servicelist["services"].append(service1);
  servicelist["services"].append(service2);

  // use
  std::string response;
  std::ostringstream oss;
  oss << servicelist;
  response.assign(oss.str());

  std::cout << response << std::endl;
}

// std::string and std::string.c_str() differs? NO
//
// service1["dvbtriplet"] = "318.4.8583";
// service1["dvbtriplet"] = dvb;
// service1["dvbtriplet"] = dvb.c_str();

TEST(CxxJSON, jsoncpp_ex6_2)
{
  {
    Json::Value service1;
    service1["dvbtriplet"] = "318.4.8583"; 

    service1["servicetypes"] = Json::Value(Json::arrayValue);
    service1["servicetypes"].append("DTT");
    service1["servicetypes"].append("OTT");

    // use
    std::string response;
    std::ostringstream oss;
    oss << service1;
    response.assign(oss.str());

    std::cout << response << std::endl;
  }

  {
    std::string dvb{"318.4.8583"}; 

    Json::Value service1;
    service1["dvbtriplet"] = dvb;

    service1["servicetypes"] = Json::Value(Json::arrayValue);
    service1["servicetypes"].append("DTT");
    service1["servicetypes"].append("OTT");

    // use
    std::string response;
    std::ostringstream oss;
    oss << service1;
    response.assign(oss.str());

    std::cout << response << std::endl;
  }

  {
    std::string dvb{"318.4.8583"}; 

    Json::Value service1;
    service1["dvbtriplet"] = dvb.c_str();

    service1["servicetypes"] = Json::Value(Json::arrayValue);
    service1["servicetypes"].append("DTT");
    service1["servicetypes"].append("OTT");

    // use
    std::string response;
    std::ostringstream oss;
    oss << service1;
    response.assign(oss.str());

    std::cout << response << std::endl;
  }
}

// string vs number
//
// {
//         "dvbtriplet" : "318"
// }
// {
//         "dvbtriplet" : 318
// }

TEST(CxxJSON, jsoncpp_ex6_3)
{
  {
    Json::Value service1;
    service1["dvbtriplet"] = "318"; 

    // use
    std::string response;
    std::ostringstream oss;
    oss << service1;
    response.assign(oss.str());

    std::cout << response << std::endl;
  }

  {
    Json::Value service1;
    service1["dvbtriplet"] = 318;

    // use
    std::string response;
    std::ostringstream oss;
    oss << service1;
    response.assign(oss.str());

    std::cout << response << std::endl;
  }
}

// append() on array
//
// "/sysroots/cortexa15t2hf-neon-rdk-linux-gnueabi/usr/include/json/value.h" 3 4
//
// Value& append(const Value& value);
// Value& append(Value&& value);
//
// servicelist1: {
//         "uris" :
//         [
//                 "DTT",
//                 "OTT"
//         ]
// }
//
// "DTT" is overwritten
//
// servicelist2: {
//        "uris" : "OTT"
// }

TEST(CxxJSON, jsoncpp_ex7)
{
  {
    Json::Value servicelist;

    // set a array
    servicelist["uris"] = Json::Value(Json::arrayValue);
    servicelist["uris"].append("DTT");
    servicelist["uris"].append("OTT");

    std::cout << "servicelist1: " << servicelist << std::endl;
  }

  // same result as above
  {
    Json::Value servicelist;

    // set a array
    servicelist["uris"] = Json::Value(Json::arrayValue);
    servicelist["uris"].append(Json::Value("DTT"));
    servicelist["uris"].append(Json::Value("OTT"));

    std::cout << "servicelist2: " << servicelist << std::endl;
  }

  {
    Json::Value servicelist;

    // set a array
    servicelist["uris"] = Json::Value(Json::arrayValue);
    servicelist["uris"] = Json::Value("DTT");
    servicelist["uris"] = Json::Value("OTT");

    std::cout << "servicelist3: " << servicelist << std::endl;
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

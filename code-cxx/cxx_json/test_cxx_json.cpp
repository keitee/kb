#include "gmock/gmock.h"
#include <chrono>
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
Standard ECMA-262 3rd Edition - December 1999. 

JSON is a text format that is completely language independent but uses
conventions that are familiar to programmers of the C-family of languages,
including C, C++, C#, Java, JavaScript, Perl, Python, and many others. These
properties make JSON an ideal data-interchange language.

JSON is built on two structures:

o A collection of name/value pairs. In various languages, this is realized as an
  object, record, struct, dictionary, hash table, keyed list, or associative
  array.  

o An ordered list of values. In most languages, this is realized as an array,
  vector, list, or sequence.

These are universal data structures.  Virtually all modern programming languages
support them in one form or another.  It makes sense that a data format that is
interchangeable with programming languages also be based on these structures.


In JSON, they take on these forms:

o An "object" is an unordered set of name/value pairs.

An object begins with "{left brace" and ends with "}right brace". Each name is
followed by :colon and the name/value pairs are separated by ,comma.

o An "array" is an ordered collection of values. 

An array begins with "[left bracket" and ends with "]right bracket". Values are
separated by ,comma.

A "value" can be a "string" in double quotes, or a "number", or "true or false" or
"null", or an "object" or an "array". These structures can be nested.


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

http://open-source-parsers.github.io/jsoncpp-docs/doxygen/json__reader_8cpp.html
jsoncpp/src/lib_json/json_reader.cpp

IStream& operator>>(IStream& sin, Value& root) {
  CharReaderBuilder b;
  String errs;
  bool ok = parseFromStream(b, sin, &root, &errs);
  if (!ok) {
    throwRuntimeError(errs);
  }
  return sin;
}



jq tool
https://stedolan.github.io/jq/tutorial/


you can install jq which is faster (written in C) from your package manager
(e.g. sudo apt install jq in Ubuntu/Debian, sudo dnf install jq on
 Fedora/RHEL/CentOS) or from source and then in vim, type:

:%!jq .


{
	"documentId" : "1607619413",
	"services" : 
	[
		{
			"c" : "5003",
			"dvbtriplet" : "9018.8197.8261",
			"servicetypes" : 
			[
				"DTT"
			],
			"sf" : "sd",
			"sid" : "M233a-2005-2045-686",
			"t" : "ITV",
			"xsg" : 8
		},
		{
			"adult" : false,
			"dvbtriplet" : "",
			"local" : false,
			"schedule" : false,
			"servicetype" : "OTT",
			"servicetypes" : 
			[
				"DSAT"
			],
			"sf" : "sd",
			"sg" : 0,
			"sid" : "1151",
			"sk" : 1151,
			"startover" : null,
			"startoverents" : false,
			"t" : "IEPG data 1",
			"timeshifted" : false,
			"xsg" : 0
		}
  ]
}

$ cat dtt_list_llama_after.json | jq '.["services"][] | {channel: .c, title: .t, type: .servicetype, servicetypes: .servicetypes}'

$ cat dtt_list_llama_after.json | jq '.["services"][] | select(.servicetypes == ["DTT"])'
$ cat dtt_list_llama_after.json | jq '[.["services"][] | select(.servicetypes == ["DTT"])] | length'

get total services
$ cat uk_after.json | jq '[.["services"][]] | length'
609

get total services which has OTT
$ cat uk_after.json | jq '[.["services"][] | select(.servicetypes | contains(["OTT"]))] | length'
169

get total services which only has DSAT
$ cat uk_after.json | jq '[.["services"][] | select(.servicetypes == ["DSAT"])]'
$ cat uk_after.json | jq '[.["services"][] | select(.servicetypes == ["DSAT"])] | length'
440

>>> 440+169
609


$ cat dtt_list_llama_after.json | jq '.["services"][] | select(.servicetypes | contains(["OTT"])) | {channel: .c, title: .t, type: .servicetype, servicetypes: .servicetypes}'
$ cat dtt_list_llama_after.json | jq '.["services"][] | select(.servicetypes | contains(["OTT"])) | {channel: .c, title: .t, type: .servicetype, servicetypes: .servicetypes}' > out.json

{
  "channel": "103",
  "title": "ITV",
  "type": "OTT",
  "servicetypes": [
    "DSAT",
    "OTT"
  ]
}
{
  "channel": "104",
  "title": "Channel 4",
  "type": "OTT",
  "servicetypes": [
    "DSAT",
    "OTT"
  ]
}
...

If you want to get the output as a single array, you can tell jq to "collect" all of the answers by wrapping the filter in square brackets:

$ cat dtt_list_llama_after.json | jq '[.["services"][] | select(.servicetypes | contains(["OTT"])) | {channel: .c, title: .t, type: .servicetype, servicetypes: .servicetypes}]' > out.json

[
  {
    "channel": "103",
    "title": "ITV",
    "type": "OTT",
    "servicetypes": [
      "DSAT",
      "OTT"
    ]
  },
  {
    "channel": "104",
    "title": "Channel 4",
    "type": "OTT",
    "servicetypes": [
      "DSAT",
      "OTT"
    ]
  },
...
]

$ cat dtt_list_llama_after.json | jq '[.["services"][] | select(.servicetypes | contains(["OTT"])) | {channel: .c, title: .t, type: .servicetype, servicetypes: .servicetypes}] | length'
169

*/

// ={=========================================================================
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
TEST(CxxJSON, read_from_file)
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

// ={=========================================================================
// read json file but use stringstream so no use of parse()

TEST(CxxJSON, read_from_file_and_use_stringstream)
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

// ={=========================================================================
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

// ={=========================================================================
TEST(CxxJSON, check_empty_meaning)
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

// ={=========================================================================
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

namespace cxx_json
{
  const std::string build_json_expected = R"({
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
   "year1" : 1865,
   "year2" : "1865"
}
)";

  const std::string build_json_expected_2 = R"({
   "book" : "Alice in Wonderland",
   "characters" : [
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
      ]
   ],
   "year1" : 1865,
   "year2" : "1865"
}
)";
} // namespace cxx_json

// ={=========================================================================
// construct json from code
//
// http://open-source-parsers.github.io/jsoncpp-docs/doxygen/class_json_1_1_value.html#a7e49ac977e4bcf59745a09d426669f75
//
// Value & Json::Value::append (const Value &value)
// Append value to array at the end.
// Equivalent to jsonvalue[jsonvalue.size()] = value;
//
// NOTE: So where does this difference come?
//
//    // create the characters array
//    Json::Value ch;
//
//    ch[0]["name"]    = "Jabberwock";
//    ch[0]["chapter"] = 1;
//    ch[1]["name"]    = "Cheshire Cat";
//    ch[1]["chapter"] = 6;
//    ch[2]["name"]    = "Mad Hatter";
//    ch[2]["chapter"] = 7;
//
// makes array *implicitly*
//
// Value & Json::Value::operator[] (int index)
//
// Access an array element (zero based index ).
//
// If the array contains less than index element, then null value are inserted
// in the array so that its size is index+1. (You may need to say 'value[0u]' to
// get your compiler to distinguish this from the operator[] which takes a
// string.)

TEST(CxxJSON, assign_and_append)
{
  {
    using namespace cxx_json;

    // create the characters array
    Json::Value ch;

    ch[0]["name"]    = "Jabberwock";
    ch[0]["chapter"] = 1;
    ch[1]["name"]    = "Cheshire Cat";
    ch[1]["chapter"] = 6;
    ch[2]["name"]    = "Mad Hatter";
    ch[2]["chapter"] = 7;

    EXPECT_THAT(ch.isArray(), true);

    // create the main object
    Json::Value val;

    val["book"]       = "Alice in Wonderland";
    val["year1"]      = 1865;
    val["year2"]      = "1865";
    val["characters"] = ch;

    // std::cout << val << std::endl;

    // use writer
    Json::StyledWriter writer;
    std::string response;

    response.assign(writer.write(val));

    EXPECT_THAT(response, build_json_expected);
  }

  // append() works on array implicitly
  {
    using namespace cxx_json;

    // create the characters array
    Json::Value ch;

    ch[0]["name"]    = "Jabberwock";
    ch[0]["chapter"] = 1;
    ch[1]["name"]    = "Cheshire Cat";
    ch[1]["chapter"] = 6;
    ch[2]["name"]    = "Mad Hatter";
    ch[2]["chapter"] = 7;

    EXPECT_THAT(ch.isArray(), true);

    // create the main object
    Json::Value val;

    val["book"]  = "Alice in Wonderland";
    val["year1"] = 1865;
    val["year2"] = "1865";

    // val["characters"] = ch;

    val["characters"].append(ch);

    // std::cout << val << std::endl;

    // use writer
    Json::StyledWriter writer;
    std::string response;

    response.assign(writer.write(val));

    EXPECT_THAT(response, build_json_expected_2);
  }

  // explicit way
  {
    using namespace cxx_json;

    // create the characters array
    Json::Value ch;

    ch[0]["name"]    = "Jabberwock";
    ch[0]["chapter"] = 1;
    ch[1]["name"]    = "Cheshire Cat";
    ch[1]["chapter"] = 6;
    ch[2]["name"]    = "Mad Hatter";
    ch[2]["chapter"] = 7;

    EXPECT_THAT(ch.isArray(), true);

    // create the main object
    Json::Value val;

    val["book"]  = "Alice in Wonderland";
    val["year1"] = 1865;
    val["year2"] = "1865";

    val["characters"] = Json::Value(Json::arrayValue);
    val["characters"].append(ch);

    // std::cout << val << std::endl;

    // use writer
    Json::StyledWriter writer;
    std::string response;

    response.assign(writer.write(val));

    EXPECT_THAT(response, build_json_expected_2);
  }
}

// ={=========================================================================
// does support "operator==()"?
TEST(CxxJSON, check_operator_equal)
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
  Json::Value val1;

  val1["book"]       = "Alice in Wonderland";
  val1["year1"]      = 1865;
  val1["year2"]      = "1865";
  val1["characters"] = ch;

  // create the main object
  Json::Value val2;

  val2["book"]       = "Alice in Wonderland";
  val2["year1"]      = 1865;
  val2["year2"]      = "1865";
  val2["characters"] = ch;

  // std::cout << val1 << std::endl;
  // std::cout << val2 << std::endl;

  EXPECT_THAT(val1 == val2, true);
}

// ={=========================================================================
// "string" and number are different? how?
TEST(CxxJSON, string_and_number_1)
{
  // create the main object
  Json::Value val;

  val["year1"] = 1865;
  val["year2"] = "1865";

  // "year1" is number
  EXPECT_THAT(val["year1"].isString(), false);
  EXPECT_THAT(val["year1"].isNumeric(), true);

  // get type
  EXPECT_THAT(val["year1"].type(), Json::intValue); // 1

  // it can be converted to string as well
  EXPECT_THAT(val["year1"].asString(), "1865");
  EXPECT_THAT(val["year1"].asInt(), 1865);

  // "year2" is string
  EXPECT_THAT(val["year2"].isString(), true);
  EXPECT_THAT(val["year2"].isNumeric(), false);

  // get type
  EXPECT_THAT(val["year2"].type(), Json::stringValue); // 4

  EXPECT_THAT(val["year2"].asString(), "1865");

  // NOTE: however, string not be converted to number
  try
  {
    std::cout << val["year2"].asInt() << std::endl;
  } catch (const exception &e)
  {
    EXPECT_THAT(std::string(e.what()), "Value is not convertible to Int.");
  }
}

// ={=========================================================================
// can use without using "asXXX()"? No, compile error
TEST(CxxJSON, string_and_number_2)
{
  std::string asString{};
  int asInt{};

  // create the main object
  Json::Value val;

  val["year1"] = 1865;
  val["year2"] = "1865";

  // "year1" is number
  EXPECT_THAT(val["year1"].isString(), false);
  EXPECT_THAT(val["year1"].isNumeric(), true);

  // get type
  EXPECT_THAT(val["year1"].type(), Json::intValue); // 1

  // compile error
  // asString = val["year1"]
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
// ={=========================================================================

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

// ={=========================================================================
TEST(CxxJSON, read_from_array_1)
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

// ={=========================================================================
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

TEST(CxxJSON, read_from_array_2)
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

  const std::string ConfigJson2 = R"({
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
   "year1" : 1865,
   "year2" : "1865"
}
)";

} // namespace

/*
// ={=========================================================================
// read directly from raw std::string

[ RUN      ] CxxJSON.read_from_string_1
path   : /as/drm/status
method : ws
path   : /as/players/<vwid>/status
method : ws
path   : /as/players/<vwid>/action/watchpvr
method : post
path   : /as/players/<vwid>/action/watchbdl
method : post
[       OK ] CxxJSON.read_from_string_1 (0 ms)

[ RUN      ] CxxJSON.read_from_string_2
path   : /as/drm/status
method : ws
path   : /as/players/<vwid>/status
method : ws
path   : /as/players/<vwid>/action/watchpvr
method : post
path   : /as/players/<vwid>/action/watchbdl
method : post
[       OK ] CxxJSON.read_from_string_2 (0 ms)

*/

TEST(CxxJSON, read_from_string_1)
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
        const Json::Value path   = uri["path"];
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

// ={=========================================================================
// do the same as above but use std::string directly
//
// http://open-source-parsers.github.io/jsoncpp-docs/doxygen/class_json_1_1_reader.html#af1da6c976ad1e96c742804c3853eef94
//
// bool Json::Reader::parse	(	const std::string & 	document,
// Value & 	root,
// bool 	collectComments = true
// )

TEST(CxxJSON, read_from_string_2)
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
        const Json::Value path   = uri["path"];
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

// ={=========================================================================
// how to get number of itmes in a array? can loop over it but any function from
// cppjson?

TEST(CxxJSON, read_from_string_3)
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

  const Json::Value uris = root["uris"];

  EXPECT_THAT(uris.isArray(), true);
  EXPECT_THAT(uris.size(), 4);
}

/*
// ={=========================================================================
does parse() remove duplicates? Yes, no errors but

https://stackoverflow.com/questions/32480121/parsing-json-with-duplicate-keys-json-cpp

Like the JSON RFC sad the object names (keys) should be unique.

The names within an object SHOULD be unique.

Also the RFC defines if they are not, the behavior is unpredictable.

See this quote from the RFC:

An object whose names are all unique is interoperable in the sense that all
software implementations receiving that object will agree on the name-value
mappings. When the names within an object are not unique, the behavior of
software that receives such an object is unpredictable. Many implementations
report the last name/value pair only. Other implementations report an error or
fail to parse the object, and some implementations report all of the name/value
pairs, including duplicates.

*/

TEST(CxxJSON, read_from_string_4)
{
  std::string input{ConfigJson};

  EXPECT_THAT(input.size(), ConfigJson.size());
  EXPECT_THAT(input.size(), 459);

  input.append(ConfigJson);

  EXPECT_THAT(input.size(), 2 * ConfigJson.size());
  EXPECT_THAT(input.size(), 2 * 459);

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

  const Json::Value uris = root["uris"];

  EXPECT_THAT(uris.isArray(), true);
  EXPECT_THAT(uris.size(), 4);
}

TEST(CxxJSON, read_from_string_5)
{
  std::vector<std::string> expected{"book", "characters", "year1", "year2"};
  std::vector<std::string> coll{};

  std::string input{ConfigJson};

  Json::Reader reader{};
  Json::Value root{};

  // parse the json
  if (!reader.parse(ConfigJson2, root))
  {
    std::cout << "failed to parse config json" << std::endl;
    return;
  }

  // Return a list of the member names.
  // getMemberNames()
  // Value::Members Json::Value::getMemberNames() const
  //
  // typedef std::vector< std::string > Json::Value::Members
  for (const auto &e : root.getMemberNames())
  {
      // std::cout << e << std::endl;
      coll.push_back(e);
  }

  EXPECT_THAT(coll, expected);
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

namespace json_write
{
  const std::string expected = R"({
   "documentId" : "1563970127340",
   "services" : [
      {
         "c" : "21",
         "dvbtriplet" : "318.4.8583",
         "servicetypes" : [ "DTT", "OTT" ],
         "sf" : "sd",
         "sid" : "M13e-4-2187",
         "t" : "Rai 4",
         "xsg" : 8
      },
      {
         "c" : "24",
         "dvbtriplet" : "318.4.8585",
         "servicetypes" : [ "DTT" ],
         "sf" : "sd",
         "sid" : "M13e-4-2189",
         "t" : "Rai Movie",
         "xsg" : 8
      }
   ],
   "version" : 1
}
)";

  const std::string expected_2 = R"({
	"documentId" : "1563970127340",
	"services" : 
	[
		{
			"c" : "21",
			"dvbtriplet" : "318.4.8583",
			"servicetypes" : 
			[
				"DTT",
				"OTT"
			],
			"sf" : "sd",
			"sid" : "M13e-4-2187",
			"t" : "Rai 4",
			"xsg" : 8
		},
		{
			"c" : "24",
			"dvbtriplet" : "318.4.8585",
			"servicetypes" : 
			[
				"DTT"
			],
			"sf" : "sd",
			"sid" : "M13e-4-2189",
			"t" : "Rai Movie",
			"xsg" : 8
		}
	],
	"version" : 1
})";
} // namespace json_write

// ={=========================================================================
TEST(CxxJSON, write_to_json_using_writer)
{
  using namespace json_write;

  Json::Value servicelist;

  servicelist["documentId"] = "1563970127340";
  servicelist["version"]    = 1;

  Json::Value service1;
  service1["dvbtriplet"] = "318.4.8583";

  service1["servicetypes"] = Json::Value(Json::arrayValue);
  service1["servicetypes"].append("DTT");
  service1["servicetypes"].append("OTT");

  service1["c"]   = "21";
  service1["t"]   = "Rai 4";
  service1["sid"] = "M13e-4-2187";
  service1["sf"]  = "sd";
  service1["xsg"] = 8;

  Json::Value service2;
  service2["dvbtriplet"]   = "318.4.8585";
  service2["servicetypes"] = Json::Value(Json::arrayValue);
  service2["servicetypes"].append("DTT");
  service2["c"]   = "24";
  service2["t"]   = "Rai Movie";
  service2["sid"] = "M13e-4-2189";
  service2["sf"]  = "sd";
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

  // std::cout << response << std::endl;
  // std::cout << expected << std::endl;

  EXPECT_THAT(response, expected);
}

// ={=========================================================================
// use stringstream instead and see commment about deprecated list above
TEST(CxxJSON, write_to_json_using_stringstream)
{
  // when use stringstream, it has "\t" instead of spaces
  // const std::string expected = R"({\n\t\"documentId\" : \"1563970127340\",\n\t\"services\" : \n\t[\n\t\t{\n\t\t\t\"c\" : \"21\",\n\t\t\t\"dvbtriplet\" : \"318.4.8583\",\n\t\t\t\"servicetypes\" : \n\t\t\t[\n\t\t\t\t\"DTT\",\n\t\t\t\t\"OTT\"\n\t\t\t],\n\t\t\t\"sf\" : \"sd\",\n\t\t\t\"sid\" : \"M13e-4-2187\",\n\t\t\t\"t\" : \"Rai 4\",\n\t\t\t\"xsg\" : 8\n\t\t},\n\t\t{\n\t\t\t\"c\" : \"24\",\n\t\t\t\"dvbtriplet\" : \"318.4.8585\",\n\t\t\t\"servicetypes\" : \n\t\t\t[\n\t\t\t\t\"DTT\"\n\t\t\t],\n\t\t\t\"sf\" : \"sd\",\n\t\t\t\"sid\" : \"M13e-4-2189\",\n\t\t\t\"t\" : \"Rai Movie\",\n\t\t\t\"xsg\" : 8\n\t\t}\n\t],\n\t\"version\" : 1\n})";

  Json::Value servicelist;

  servicelist["documentId"] = "1563970127340";
  servicelist["version"]    = 1;

  Json::Value service1;
  service1["dvbtriplet"] = "318.4.8583";

  service1["servicetypes"] = Json::Value(Json::arrayValue);
  service1["servicetypes"].append("DTT");
  service1["servicetypes"].append("OTT");

  service1["c"]   = "21";
  service1["t"]   = "Rai 4";
  service1["sid"] = "M13e-4-2187";
  service1["sf"]  = "sd";
  service1["xsg"] = 8;

  Json::Value service2;
  service2["dvbtriplet"]   = "318.4.8585";
  service2["servicetypes"] = Json::Value(Json::arrayValue);
  service2["servicetypes"].append("DTT");
  service2["c"]   = "24";
  service2["t"]   = "Rai Movie";
  service2["sid"] = "M13e-4-2189";
  service2["sf"]  = "sd";
  service2["xsg"] = 8;

  servicelist["services"] = Json::Value(Json::arrayValue);
  servicelist["services"].append(service1);
  servicelist["services"].append(service2);

  // use
  std::string response;
  std::ostringstream oss;
  oss << servicelist;
  response.assign(oss.str());

  // std::cout << response << std::endl;
}

// ={=========================================================================
// use stringstream instead and see commment about deprecated list above
TEST(CxxJSON, DISABLED_write_to_json_using_asstring)
{
  Json::Value servicelist;

  servicelist["documentId"] = "1563970127340";
  servicelist["version"]    = 1;

  Json::Value service1;
  service1["dvbtriplet"] = "318.4.8583";

  service1["servicetypes"] = Json::Value(Json::arrayValue);
  service1["servicetypes"].append("DTT");
  service1["servicetypes"].append("OTT");

  service1["c"]   = "21";
  service1["t"]   = "Rai 4";
  service1["sid"] = "M13e-4-2187";
  service1["sf"]  = "sd";
  service1["xsg"] = 8;

  Json::Value service2;
  service2["dvbtriplet"]   = "318.4.8585";
  service2["servicetypes"] = Json::Value(Json::arrayValue);
  service2["servicetypes"].append("DTT");
  service2["c"]   = "24";
  service2["t"]   = "Rai Movie";
  service2["sid"] = "M13e-4-2189";
  service2["sf"]  = "sd";
  service2["xsg"] = 8;

  servicelist["services"] = Json::Value(Json::arrayValue);
  servicelist["services"].append(service1);
  servicelist["services"].append(service2);

  // // use
  // std::string response;
  // std::ostringstream oss;
  // oss << servicelist;
  // response.assign(oss.str());

  // C++ exception with description "Type is not convertible to string" thrown in the test body.
  std::cout << servicelist.asString() << std::endl;
}

// ={=========================================================================
TEST(CxxJSON, write_json_to_file_and_read_back_1)
{
  Json::Value servicelist;

  servicelist["documentId"] = "1563970127340";
  servicelist["version"]    = 1;

  Json::Value service1;
  service1["dvbtriplet"] = "318.4.8583";

  service1["servicetypes"] = Json::Value(Json::arrayValue);
  service1["servicetypes"].append("DTT");
  service1["servicetypes"].append("OTT");

  service1["c"]   = "21";
  service1["t"]   = "Rai 4";
  service1["sid"] = "M13e-4-2187";
  service1["sf"]  = "sd";
  service1["xsg"] = 8;

  Json::Value service2;
  service2["dvbtriplet"]   = "318.4.8585";
  service2["servicetypes"] = Json::Value(Json::arrayValue);
  service2["servicetypes"].append("DTT");
  service2["c"]   = "24";
  service2["t"]   = "Rai Movie";
  service2["sid"] = "M13e-4-2189";
  service2["sf"]  = "sd";
  service2["xsg"] = 8;

  servicelist["services"] = Json::Value(Json::arrayValue);
  servicelist["services"].append(service1);
  servicelist["services"].append(service2);

  {
    // write to a file
    std::ostringstream oss;
    oss << servicelist;

    std::ofstream ofs{"fsout.json"};

    ofs << oss.str();
  }

  {
    using namespace json_write;

    // read from a file
    std::ifstream ifs{"fsout.json"};

    Json::Reader reader;
    Json::Value root;

    // use reader and get `root`
    reader.parse(ifs, root);

    // can print directly
    // std::cout << root << std::endl;

    std::string response;
    std::ostringstream oss;
    oss << root;
    response.assign(oss.str());

    EXPECT_THAT(response, expected_2);
  }
}

// ={=========================================================================
TEST(CxxJSON, write_json_to_file_and_read_back_2)
{
  Json::Value servicelist;

  servicelist["documentId"] = "1563970127340";
  servicelist["version"]    = 1;

  Json::Value service1;
  service1["dvbtriplet"] = "318.4.8583";

  service1["servicetypes"] = Json::Value(Json::arrayValue);
  service1["servicetypes"].append("DTT");
  service1["servicetypes"].append("OTT");

  service1["c"]   = "21";
  service1["t"]   = "Rai 4";
  service1["sid"] = "M13e-4-2187";
  service1["sf"]  = "sd";
  service1["xsg"] = 8;

  Json::Value service2;
  service2["dvbtriplet"]   = "318.4.8585";
  service2["servicetypes"] = Json::Value(Json::arrayValue);
  service2["servicetypes"].append("DTT");
  service2["c"]   = "24";
  service2["t"]   = "Rai Movie";
  service2["sid"] = "M13e-4-2189";
  service2["sf"]  = "sd";
  service2["xsg"] = 8;

  servicelist["services"] = Json::Value(Json::arrayValue);
  servicelist["services"].append(service1);
  servicelist["services"].append(service2);

  // note that can write Json::Value on stream directly
  {
    std::ofstream ofs{"fsout.json"};
    ofs << servicelist;
  }

  {
    using namespace json_write;

    // read from a file
    std::ifstream ifs{"fsout.json"};

    Json::Reader reader;
    Json::Value root;

    // use reader and get `root`
    reader.parse(ifs, root);

    // can print directly
    // std::cout << root << std::endl;

    std::string response;
    std::ostringstream oss;
    oss << root;
    response.assign(oss.str());

    EXPECT_THAT(response, expected_2);
  }
}

// ={=========================================================================
// std::string and std::string.c_str() differs? NO
//
// service1["dvbtriplet"] = "318.4.8583";
// service1["dvbtriplet"] = dvb;
// service1["dvbtriplet"] = dvb.c_str();

TEST(CxxJSON, assign_value)
{
  {
    Json::Value service1;

    // use literal
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

    // use string
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

    // use cstring
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

// ={=========================================================================
// string vs number
//
// {
//         "dvbtriplet" : "318"
// }
// {
//         "dvbtriplet" : 318
// }

TEST(CxxJSON, assign_string_and_value)
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

// ={=========================================================================
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

TEST(CxxJSON, append_on_array)
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
// [ RUN      ] CxxJSON.time_output
// current utc      : 1601563661
// current utc in ms: 1601563661000
// current utc in ms: 1601563661000
// current utc in ms: 1601563661000
//
// 1. servicelist: {
//         "time1" : 1601563661,
//         "time2" : -459140408,
//         "time3" : -459140408,
//         "time4" : -459140408
// }
//
// 2. servicelist: {
//         "time1" : 1601563661,
//         "time2" : 1601563661000,
//         "time3" : 1601563661000,
//         "time4" : 1601563661000
// }
//
// 3. servicelist: {
//         "time3" : 1601562347722
// }
//
// casted to "unsigned"
//
// 4. servicelist: {
//         "time1" : 1601563661,
//         "time2" : 3835826888,
//         "time3" : 3835826888,
//         "time4" : 3835826888
// }
//
// [       OK ] CxxJSON.time_output (0 ms)

TEST(CxxJSON, time_output)
{
  auto now = std::time(0);
  std::cout << "current utc      : " << now << std::endl;
  std::cout << "sizeof  utc      : " << sizeof now << std::endl;

  auto now_to_ms_1 = now * 1000;
  std::cout << "current utc in ms: " << now_to_ms_1 << std::endl;
  std::cout << "sizeof  utc in ms: " << sizeof now_to_ms_1 << std::endl;

  uint64_t now_to_ms_2 = static_cast<uint64_t>(now * 1000);
  std::cout << "current utc in ms: " << now_to_ms_2 << std::endl;
  std::cout << "sizeof  utc in ms: " << sizeof now_to_ms_2 << std::endl;

  uint64_t now_to_ms_3 = now * 1000;
  std::cout << "current utc in ms: " << now_to_ms_3 << std::endl;

  // error: conversion from ‘long int’ to ‘Json::Value’ is ambiguous
  //      servicelist["time1"] = now;
  //                             ^~~
  // error: conversion from ‘uint64_t {aka long unsigned int}’ to ‘Json::Value’ is ambiguous
  //      servicelist["time4"] = now_to_ms_3;
  //                             ^~~~~~~~~~~
  // same for the rest

  // {
  //   Json::Value servicelist;
  //
  //   // set a array
  //   servicelist["time1"] = now;
  //   servicelist["time2"] = now_to_ms_1;
  //   servicelist["time3"] = now_to_ms_2;
  //   servicelist["time4"] = now_to_ms_3;
  //
  //   std::cout << "servicelist1: " << servicelist << std::endl;
  // }

  // avoids errors via static_cast but json output is wrong

  {
    Json::Value servicelist;

    // set a array
    servicelist["time1"] = static_cast<int>(now);
    servicelist["time2"] = static_cast<int>(now_to_ms_1);
    servicelist["time3"] = static_cast<int>(now_to_ms_2);
    servicelist["time4"] = static_cast<int>(now_to_ms_3);

    std::cout << "1. servicelist: " << servicelist << std::endl;
  }

  // ok
  {
    Json::Value servicelist;

    // set a array
    servicelist["time1"] = static_cast<unsigned int>(now);
    servicelist["time2"] = static_cast<Json::UInt64>(now_to_ms_1);
    servicelist["time3"] = static_cast<Json::UInt64>(now_to_ms_2);
    servicelist["time4"] = static_cast<Json::UInt64>(now_to_ms_3);

    std::cout << "2. servicelist: " << servicelist << std::endl;
  }

  // error on define JSONCPP_VERSION_STRING "1.7.7"
  // but not error on  1.7.4-3 on PC
  //
  // error: conversion from ‘uint64_t {aka long unsigned int}’ to ‘Json::Value’ is ambiguous
  //     servicelist["time4"] = static_cast<uint64_t>(now_to_ms_3);
  //
  // /usr/include/jsoncpp/json/value.h:312:3: note: candidate: Json::Value::Value(bool)
  //    Value(bool value);
  //    ^~~~~
  // /usr/include/jsoncpp/json/value.h:289:3: note: candidate: Json::Value::Value(double)
  //    Value(double value);
  //    ^~~~~
  // /usr/include/jsoncpp/json/value.h:287:3: note: candidate: Json::Value::Value(Json::Value::UInt64)
  //    Value(UInt64 value);
  //    ^~~~~
  // /usr/include/jsoncpp/json/value.h:286:3: note: candidate: Json::Value::Value(Json::Value::Int64)
  //    Value(Int64 value);
  //    ^~~~~
  // /usr/include/jsoncpp/json/value.h:284:3: note: candidate: Json::Value::Value(Json::Value::UInt)
  //    Value(UInt value);
  //    ^~~~~
  // /usr/include/jsoncpp/json/value.h:283:3: note: candidate: Json::Value::Value(Json::Value::Int)
  //    Value(Int value);
  //    ^~~~~
  // /usr/include/jsoncpp/json/value.h:323:10: note:   initializing argument 1 of ‘Json::Value& Json::Value::operator=(Json::Value)’
  //    Value& operator=(Value other);
  //           ^~~~~~~~

  // {
  //   Json::Value servicelist;
  //
  //   servicelist["time3"] = static_cast<uint64_t>(now_to_ms_2);
  //   servicelist["time4"] = static_cast<uint64_t>(now_to_ms_3);
  //
  //   std::cout << "3. servicelist: " << servicelist << std::endl;
  // }
  //
  // value is bigger than int::max so use uint64_t but still see the same error
  //
  // {
  //   Json::Value servicelist;
  //
  //   uint64_t now{1601562347722};
  //
  //   // set a array
  //   servicelist["time3"] = static_cast<uint64_t>(now);
  //
  //   std::cout << "3. servicelist: " << servicelist << std::endl;
  // }

  // no error and works
  {
    Json::Value servicelist;

    double now{1601562347722};

    // set a array
    servicelist["time3"] = now;

    std::cout << "3. servicelist: " << servicelist << std::endl;
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

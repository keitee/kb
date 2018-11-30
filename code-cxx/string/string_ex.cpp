#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <bitset>
// #include <boost/algorithm/string.hpp>
// #include <boost/regex.hpp>
// #include <boost/cast.hpp>
// #include <boost/lexical_cast.hpp>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::StrEq;
using testing::FloatEq;


// ={=========================================================================
// string-pos

namespace string_ex 
{
  template<typename Coll>
    class is_any_of
    {
      public:
        explicit is_any_of(const Coll &range) : range_(range) {}
        bool operator ()(const typename Coll::value_type value)
        {
          for (auto const e : range_)
            if (e == value)
              return true;
          return false;
        }

      private:
          Coll range_;
    };

  template<typename Coll, typename Predicate>
    void split(Coll &coll, const char *text, Predicate pred) 
    {
      typename Coll::value_type token{};

      while (*text)
      {
        if (pred(*text))
        {
          coll.push_back(token);
          token.clear();
        } 
        else
          token.append(1, *text);

        ++text;
      }

      coll.push_back(token);
    }

} // namespace


TEST(XX, XX)
{
  using namespace string_ex;

  {
    string coll{"|,."};

    auto f = is_any_of<string>(coll);
    EXPECT_TRUE(f('|'));
    EXPECT_TRUE(f(','));
    EXPECT_TRUE(f('.'));
  }

  {
    string input{"Name|Address|Phone"};
    string delim{"|,."};
    vector<string> result;

    auto f = is_any_of<string>(delim);

    string token{};

    for (const auto ch : input)
    {
      if(f(ch))
      {
        result.push_back(token);
        token.clear();
      }
      else
        token += ch;
    }

    // push what's left.
    result.push_back(token);

    EXPECT_THAT(result, ElementsAre("Name", "Address", "Phone"));
  }

}

namespace U29_2018_11_29
{
  // so it's pattern matching but no idea.

} // namespace

namespace U29_Text
{
  // The simplest way to solve this problem is by using regular expressions. The
  // regular expression that meets the described format is 
  //
  // "[A-Z]{3}-[A-Z]{2} \d{3,4}".

  // The first function only has to validate that an input string contains only
  // text that matches this regular expression. For that, we can use
  // std::regex_match(), as follows:

  bool validate_license_plate_format(std::string str)
  {
    std::regex rx(R"([A-Z]{3}-[A-Z]{2} \d{3,4})");
    return std::regex_match(str.data(), rx);
  }

} // namespace

TEST(U29, Text)
{
  using namespace U29_Text;

  EXPECT_TRUE(validate_license_plate_format("ABC-DE 123"));
  EXPECT_TRUE(validate_license_plate_format("ABC-DE 1234"));
  EXPECT_TRUE(!validate_license_plate_format("ABC-DE 12345"));
  EXPECT_TRUE(!validate_license_plate_format("abc-de 1234"));
}

TEST(XX,XXZ)
{
  // find XML/HTML-tagged value (using default syntax):
  regex reg1("<.*>.*</.*>");
  bool found = regex_match ("<tag>value</tag>", // data
      reg1); // regular expression
  out(found);
  // find XML/HTML-tagged value (tags before and after the value must match):
  regex reg2("<(.*)>.*</\\1>");
  found = regex_match ("<tag>value</tag>", // data
      reg2); // regular expression
  out(found);
  // find XML/HTML-tagged value (using grep syntax):
  regex reg3("<\\(.*\\)>.*</\\1>",regex_constants::grep);
  found = regex_match ("<tag>value</tag>", // data
      reg3); // regular expression
  out(found);
  // use C-string as regular expression (needs explicit cast to regex):
  found = regex_match ("<tag>value</tag>", // data
      regex("<(.*)>.*</\\1>")); // regular expression
  out(found);
  cout << endl;
  // regex_match() versus regex_search():
  found = regex_match ("XML tag: <tag>value</tag>",
      regex("<(.*)>.*</\\1>")); // fails to match
  out(found);
  found = regex_match ("XML tag: <tag>value</tag>",
      regex(".*<(.*)>.*</\\1>.*")); // matches
  out(found);
  found = regex_search ("XML tag: <tag>value</tag>",
      regex("<(.*)>.*</\\1>")); // matches
  out(found);
  found = regex_search ("XML tag: <tag>value</tag>",
      regex(".*<(.*)>.*</\\1>.*")); // matches
  out(found);
}


// ={=========================================================================
int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

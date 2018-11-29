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


namespace U27_Text 
{
  // namespace std {
  //   template <typename charT,
  //            typename traits = char_traits<charT>,
  //            typename Allocator = allocator<charT> >
  //              class basic_string;
  //  
  //   typedef basic_string<char> string;
  // }
  // 
  // Q: WHY define tstring when it looks the same as std::string??
  //
  // template <typename T>
  //   using tstring = std::basic_string<T, std::char_traits<T>,
  //         std::allocator<T>>;
  //
  // template <typename T>
  //   using tstringstream = std::basic_stringstream<T, std::char_traits<T>,
  //         std::allocator<T>>;

  // template <typename T>
  //   inline std::vector<tstring<T>> split(tstring<T> text, const T delim)
  //   {
  //     auto ss = tstringstream<T>{text};
  //     auto tokens = std::vector<tstring<T>>{};
  //     auto token = tstring<T>{};

  //     while (std::getline(ss, token, delim))
  //     {
  //       if (!token.empty()) tokens.push_back(token);
  //     }

  //     return tokens;
  //   }

  // As boost version, no return and reference arg

  inline void split(std::vector<string> &coll, const std::string &text, const char delim)
  {
    std::stringstream ss{text};
    std::string token{};

    while (std::getline(ss, token, delim))
    {
      if (!token.empty()) coll.push_back(token);
    }
  }

  inline void split(std::vector<string> &coll, const std::string &text, const string delims)
  {
    size_t pos{}, prev_pos{};

    while ((pos = text.find_first_of(delims, prev_pos)) != std::string::npos)
    {
      coll.push_back(text.substr(prev_pos, pos - prev_pos));
      prev_pos = ++pos;
    }

    coll.push_back(text.substr(prev_pos, pos));
  }

  // text version
  //
  // inline void split(std::vector<string> &coll, const std::string &text, const string delims)
  // {
  //   size_t pos{}, prev_pos{};
  //
  //   while ((pos = text.find_first_of(delims, prev_pos)) != std::string::npos)
  //   {
  //     if (pos > prev_pos)
  //       coll.push_back(text.substr(prev_pos, pos - prev_pos));
  //
  //     prev_pos = ++pos;
  //   }
  //
  //   if (prev_pos < text.length())
  //     coll.push_back(text.substr(prev_pos, pos));
  // }
}


TEST(U27, Text)
{
  using namespace U27_Text;

  std::vector<std::string> coll1{}, coll2{};

  split(coll1, "this is a sample", ' ');
  EXPECT_THAT(coll1, ElementsAre("this", "is", "a", "sample"));

  split(coll2, "this is a sample", " |,.");
  EXPECT_THAT(coll2, ElementsAre("this", "is", "a", "sample"));
}


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

  {
    vector<string> coll;
    string delim{"|,."};
    // auto f = is_any_of<string>(delim);
    split(coll, "Name|Address|Phone", is_any_of<string>(delim));

    EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
  }
}


// ={=========================================================================
int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

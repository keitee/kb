#include "gmock/gmock.h"

#include <iostream>
#include <list>
#include <deque>
#include <regex>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

// From The Modern C++ Challenge Chapter 03 since it is about string as well.

// ={=========================================================================

/*

23. Binary to string conversion

Write a function that, given a range of 8-bit integers (such as an array or
vector), returns a string that contains a hexadecimal representation of the
input data. The function should be able to produce both uppercase and lowercase
content. Here are some input and output examples:

Input: { 0xBA, 0xAD, 0xF0, 0x0D }, output: "BAADF00D" or "baadf00d"
Input: { 1,2,3,4,5,6 }, output: "010203040506"

{
   std::vector<unsigned char> v{ 0xBA, 0xAD, 0xF0, 0x0D };
   std::array<unsigned char, 6> a{ {1,2,3,4,5,6} };
   unsigned char buf[5] = {0x11, 0x22, 0x33, 0x44, 0x55};

   assert(bytes_to_hexstr(v, true) == "BAADF00D");
   assert(bytes_to_hexstr(a, true) == "010203040506");
   assert(bytes_to_hexstr(buf, true) == "1122334455");

   assert(bytes_to_hexstr(v) == "baadf00d");
   assert(bytes_to_hexstr(a) == "010203040506");
   assert(bytes_to_hexstr(buf) == "1122334455");
}

*/

namespace U23_2018_11_27 {

  // as itoa, can use modulus and reverse the result. How does the text? 
  // use *cxx-stringstream*

  template<typename T>
    string bytes_to_hexstr(T coll)
    {
      ostringstream os;

      for(const auto e : coll)
        os << hex << (int)e;

      return os.str();
    }

} // namespace


TEST(U23, 20181127)
{
  using namespace U23_2018_11_27;

  {
    std::vector<unsigned char> v{ 0xBA, 0xAD, 0xF0, 0x0D };

    // EXPECT_THAT(bytes_to_hexstr(v), "BAADF00D");
    EXPECT_THAT(bytes_to_hexstr(v), "baadf0d");
  }
}

/*

In order to write a general-purpose function that can handle various sorts of
ranges, such as an std::array, std::vector, a C-like array, or others, we should
write a function template. 

In the following, there are two overloads; one that takes a container as an
argument and a flag indicating the casing style, and one that takes a pair of
iterators (to mark the first and then one past the end element of the range) and
the flag to indicate casing. The content of the range is written to an
std::ostringstream object, with the appropriate I/O manipulators, such as width,
filling character, or case flag:

*/

namespace U23_Text {

  template<typename Iter>
    string bytes_to_hexstr(Iter begin, Iter end, const bool uppercase = false)
    {
      ostringstream os;

      if (uppercase)
        os.setf(std::ios_base::uppercase);

      // *cxx-error*  *cxx-stringstream*
      // if there is no cast to int to stringstream then output is wrong. why?

      for(; begin != end; ++begin)
        os << hex << setw(2) << setfill('0')
          << static_cast<int>(*begin);

      return os.str();
    }

  template<typename Coll>
    string bytes_to_hexstr(const Coll &coll, const bool uppercase = false)
    {
      // *cxx-begin* to support stl coll and others
      return bytes_to_hexstr(begin(coll), end(coll), uppercase);
    }

} // namespace

TEST(U23, Text)
{
  using namespace U23_Text;

  std::vector<unsigned char> v{ 0xBA, 0xAD, 0xF0, 0x0D };
  std::array<unsigned char, 6> a{ {1,2,3,4,5,6} };
  unsigned char buf[5] = {0x11, 0x22, 0x33, 0x44, 0x55};

  EXPECT_TRUE(bytes_to_hexstr(v, true) == "BAADF00D");
  EXPECT_TRUE(bytes_to_hexstr(a, true) == "010203040506");
  EXPECT_TRUE(bytes_to_hexstr(buf, true) == "1122334455");

  EXPECT_TRUE(bytes_to_hexstr(v) == "baadf00d");
  EXPECT_TRUE(bytes_to_hexstr(a) == "010203040506");
  EXPECT_TRUE(bytes_to_hexstr(buf) == "1122334455");
}


// ={=========================================================================

/*

24. String to binary conversion

Write a function that, given a string containing hexadecimal digits as the input
argument, returns a vector of 8-bit integers that represent the numerical
deserialization of the string content. The following are examples:

Input: "BAADF00D" or "baadF00D", output: {0xBA, 0xAD, 0xF0, 0x0D}
Input "010203040506", output: {1, 2, 3, 4, 5, 6}

{
  std::vector<unsigned char> expected{ 0xBA, 0xAD, 0xF0, 0x0D, 0x42 };

  assert(hexstr_to_bytes("BAADF00D42") == expected);
  assert(hexstr_to_bytes("BaaDf00d42") == expected);
}

*/

namespace U24_2018_11_28 {

  // as itoa(), if use stringstream again, can speficy to read 2 chars such as
  // setw(2) as done for output? No. How does text? 

} // namespace

namespace U24_Text {

  unsigned char char_to_int(const char ch)
  {
    if ((ch >= '0') && (ch <= '9')) return ch - '0';
    if ((ch >= 'a') && (ch <= 'f')) return ch - 'a' + 10;
    if ((ch >= 'A') && (ch <= 'F')) return ch - 'A' + 10;

    // if reaches here
    throw std::invalid_argument("invalid hex char input");
  }

  vector<unsigned char> str_to_bytes(string str)
  {
    vector<unsigned char> coll;

    // +=2 is a trick

    for(size_t i = 0; i < str.size(); i += 2)
    {
      // "<< 4" since it's hex and it's 2^4(16).

      auto result = char_to_int(str[i]) << 4 | char_to_int(str[i+1]);
      coll.push_back(result);
    }

    return coll;
  }

} // namespace

TEST(U24, Text)
{
  using namespace U24_Text;

  std::vector<unsigned char> expected{ 0xBA, 0xAD, 0xF0, 0x0D, 0x42 };

  EXPECT_THAT(str_to_bytes("BAADF00D42"), ElementsAre(0xBA, 0xAD, 0xF0, 0x0D, 0x42));
  EXPECT_THAT(str_to_bytes("BaaDf00d42"), ElementsAre(0xBA, 0xAD, 0xF0, 0x0D, 0x42));
}


// ={=========================================================================

/*

25. Capitalizing an article title

Write a function that transforms an input text into a capitalized version, where
every word starts with an uppercase letter and has all the other letters in
lowercase. For instance, the text "the c++ challenger" should be transformed to
"The C++ Challenger".

{
   using namespace std::string_literals;

   assert("The C++ Challenger"s ==
          capitalize("the c++ challenger"s));
   assert("This Is An Example, Should Work!"s == 
          capitalize("THIS IS an ExamplE, should wORk!"s));
}

*/

namespace U25_2018_11_28 {

  string capitalize(const string &input)
  {
    stringstream ss{input};
    string token{}, result{};

    while (ss >> token)
    {
      token[0] = toupper(token[0]);
      result += token + " ";
    }

    // remove the last " "
    return result.erase(result.size()-1);
  }
} // namespace

TEST(U25, 2018_11_28)
{
  using namespace U25_2018_11_28;

  EXPECT_THAT(capitalize("the c++ challenger"), "The C++ Challenger");
}

namespace U25_Text {

  string capitalize(const string &input)
  {
    stringstream result;
    bool new_word{true};

    for (const auto ch : input)
    {
      // to find when start a new word and always one from the start

      new_word = new_word || ispunct(ch) || isspace(ch);

      if (std::isalpha(ch))
      {
        if (new_word)
        {
          // *cxx-error*  *cxx-stringstream*
          // if there is no cast to int to stringstream then output is wrong. why?
          result << static_cast<char>(std::toupper(ch));
          new_word = false;
        }
        else
          result << ch;
      }
      else
        result << ch;
    }

    return result.str();
  }

} // namespace


TEST(U25, Text)
{
  using namespace U25_Text;

  EXPECT_THAT(capitalize("the c++ challenger"), "The C++ Challenger");
}


// ={=========================================================================

/*

26. Joining strings together separated by a delimiter

Write a function that, given a list of strings and a delimiter, creates a new
string by concatenating all the input strings separated with the specified
delimiter. The delimiter must not appear after the last string, and when no
input string is provided, the function must return an empty string.

Example: 
input { "this","is","an","example" } and delimiter ' ' (space), 
output: "this is an example".

{
   using namespace std::string_literals;

   std::vector<std::string> v1{ "this","is","an","example" };
   std::vector<std::string> v2{ "example" };
   std::vector<std::string> v3{ };
 
   assert(join_strings(v1, " ") == "this is an example"s);
   assert(join_strings(v2, " ") == "example"s);
   assert(join_strings(v3, " ") == ""s);
}
 
*/

// to see why the text uses "-1"

TEST(U26, 2018_11_28)
{
  ostringstream os;
  std::vector<std::string> coll{ "this","is","an","example" };

  copy(coll.begin(), coll.end(), ostream_iterator<string>(os, ","));
  EXPECT_THAT(os.str(), "this,is,an,example,");
}


namespace U26_Text
{
  template<typename Iter>
    string join_string(Iter begin, Iter end, const char *delimeter)
    {
      ostringstream os;

      // copy(begin, end-1, ostream_iterator<string>(os, delimeter));
      // os << *(end-1);

      // *cxx-prev*
      copy(begin, prev(end), ostream_iterator<string>(os, delimeter));
      os << *prev(end);

      return os.str();
    }

  template<typename Coll>
    string join_string(Coll coll, const char *delimeter)
    {
      if (coll.size() == 0)
        return string();

      return join_string(coll.begin(), coll.end(), delimeter);
    }
}

TEST(U26, Text)
{
  using namespace U26_Text;

  std::vector<std::string> coll{ "this","is","an","example" };

  EXPECT_THAT(join_string(coll, " "), "this is an example");
}


// ={=========================================================================

/*

27. Splitting a string into tokens with a list of possible delimiters

Write a function that, given a string and a list of possible delimiter
characters, splits the string into tokens separated by any of the delimiters and
returns them in an std::vector.

Example: 
input: "this,is.a sample!!" with delimiters ",.! ", 
output: {"this", "is", "a", "sample"}.

*/

namespace U27_2018_11_29
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

TEST(U27, 2018_11_29)
{
  using namespace U27_2018_11_29;

  // attempt to write boost::is_any_of(). As boost, can make it function?

  {
    string coll{"|,."};

    auto f = is_any_of<string>(coll);
    EXPECT_TRUE(f('|'));
    EXPECT_TRUE(f(','));
    EXPECT_TRUE(f('.'));
  }

  // find and substr approach uses idx and length so to support multiple
  // delimeters, have to scan. so this approach but later, it turns out that
  // find_first_of() supports multiple delimeters.
  
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


// ={=========================================================================

/*

28. Longest palindromic substring

Write a function that, given an input string, locates and returns the longest
sequence in the string that is a palindrome. If multiple palindromes of the same
length exist, the first one should be returned.

// from wikipedia

In computer science, the longest palindromic substring or longest *symmetric*
factor problem is the problem of finding a maximum-length contiguous substring
of a given string that is also a palindrome. 

For example, the longest palindromic substring of "bananas" is "anana". The
longest palindromic substring is not guaranteed to be unique; for example, in
the string "abracadabra", there is no palindromic substring with length greater
than three, but there are two palindromic substrings with length three, namely,
"aca" and "ada". In some applications it may be necessary to return all maximal
palindromic substrings (that is, all substrings that are themselves palindromes
and cannot be extended to larger palindromic substrings) rather than returning
only one substring or returning the maximum length of a palindromic substring.


The simplest solution to this problem is to try a brute-force approach, checking
if each substring is a palindrome. However, this means we need to check C(N, 2)
substrings (where N is the number of characters in the string), and the time
complexity would be O(N^3). The complexity could be reduced to O(N^2) by storing
results of sub problems. To do so we need a table of Boolean values, of size,
where the element at [i, j] indicates whether the substring from position i to j
is a palindrome. 

We start by initializing all elements [i,i] with true (one-character
palindromes) and all the elements [i,i+i] with true for all consecutive two
identical characters (for two-character palindromes). We then go on to inspect
substrings greater than two characters, setting the element at [i,j] to true if
the element at [i+i,j-1] is true and the characters on the positions i and j in
the string are also equal. Along the way, we retain the start position and
length of the longest palindromic substring in order to extract it after
finishing computing the table.

{
   using namespace std::string_literals;
   assert(longest_palindrome("sahararahnide") == "hararah");
   assert(longest_palindrome("level") == "level");
   assert(longest_palindrome("s") == "s");
}

*/


// ={=========================================================================

/*

29. License plate validation

Considering license plates with the format LLL-LL DDD or LLL-LL DDDD (where L is
an uppercase letter from A to Z and D is a digit), write:

    One function that validates that a license plate number is of the correct
    format

    One function that, given an input text, extracts and returns all the license
    plate numbers found in the text

{
   std::vector<std::string> expected {
      "AAA-AA 123", "ABC-DE 1234", "XYZ-WW 0001"};

   std::string text("AAA-AA 123qwe-ty 1234 ABC-DE 123456..XYZ-WW 0001");
   assert(expected == extract_license_plate_numbers(text));
}

requires *cxx-regex*

*/

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

// ={=========================================================================

/*

30. Extracting URL parts

Write a function that, given a string that represents a URL, parses and extracts
the parts of the URL (protocol, domain, port, path, query, and fragment).

{
   auto p1 = parse_uri("https://packt.com");
   assert(p1.has_value());
   assert(p1->protocol == "https");
   assert(p1->domain == "packt.com");
   assert(!p1->port.has_value());
   assert(!p1->path.has_value());
   assert(!p1->query.has_value());
   assert(!p1->fragment.has_value());

   auto p2 = parse_uri("https://bbc.com:80/en/index.html?lite=true#ui");
   assert(p2.has_value());
   assert(p2->protocol == "https");
   assert(p2->domain == "bbc.com");
   assert(p2->port == 80);
   assert(p2->path.value() == "/en/index.html");
   assert(p2->query.value() == "lite=true");
   assert(p2->fragment.value() == "ui");
}

requires *cxx-regex*

For this task we will consider that a URL has the following parts: protocol and
domain are mandatory, and port, path, query, and fragment are all optional. The
following structure is used to return results from parsing an URL
(alternatively, you could return a tuple and use structured binding to bind
variables to the various sub parts of the tuple):

std::regex rx(R"(^(\w+):\/\/([\w.-]+)(:(\d+))?([\w\/\.]+)?(\?([\w=&]*)(#?(\w+))?)?$)");

*/


// ={=========================================================================

/*

31. Transforming dates in strings

Write a function that, given a text containing dates in the format dd.mm.yyyy or
dd-mm-yyyy, transforms the text so that it contains dates in the format
yyyy-mm-dd.

{
   using namespace std::string_literals;

   assert(transform_date("today is 01.12.2017!"s) == 
          "today is 2017-12-01!"s);
}

requires *cxx-regex*

*/


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

#include "gmock/gmock.h"

#include <iostream>
#include <list>
#include <deque>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


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

// ={=========================================================================

/*

28. Longest palindromic substring

Write a function that, given an input string, locates and returns the longest
sequence in the string that is a palindrome. If multiple palindromes of the same
length exist, the first one should be returned.

*/

// ={=========================================================================

/*

29. License plate validation

Considering license plates with the format LLL-LL DDD or LLL-LL DDDD (where L is
an uppercase letter from A to Z and D is a digit), write:

    One function that validates that a license plate number is of the correct format

    One function that, given an input text, extracts and returns all the license
    plate numbers found in the text
 
*/

// ={=========================================================================

/*

30. Extracting URL parts

Write a function that, given a string that represents a URL, parses and extracts
the parts of the URL (protocol, domain, port, path, query, and fragment).

*/


// ={=========================================================================

/*

31. Transforming dates in strings

Write a function that, given a text containing dates in the format dd.mm.yyyy or
dd-mm-yyyy, transforms the text so that it contains dates in the format
yyyy-mm-dd.

*/


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

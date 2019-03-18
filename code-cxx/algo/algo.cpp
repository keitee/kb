#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <bitset>
#include <list>
#include <deque>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp
// (gdb) b AlgoList_Divide_Test::TestBody()

using namespace std;
using namespace testing;


// ={=========================================================================
template <typename T>
void PRINT_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << elem << " ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}

template <typename T>
void PRINT_M_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << "(" << elem.first << ", " << elem.second << ") ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}


// ={=========================================================================
// algo-swap: swap without a temporary

void t_algo_swap_01(int &a, int &b)
{
  a = a + b;
  b = a - b;      // b = a
  a = a - b;      // a = b
}

TEST(AlgoSwap, SwapUseArithmetic)
{
  int a = 9, b = 4;

  t_algo_swap_01(a, b);

  EXPECT_THAT(a, Eq(4));
  EXPECT_THAT(b, Eq(9));
}


// X XOR  X  = 0
// X XOR  0  = X
// X XOR  1  = ~X    // X XOR (~0) = ~X
// X XOR ~X  = 1     
// 
// 00000000000000000000111111000000 // x
// 00000000000000000000111111000000 // x^0
// 11111111111111111111000000111111 // x^(~0) but not x^1
// 
// x =  1010; y = 0011;          // before
// x =  1001 =  1010  ^ 0011     // x = x^y
// y =  1010 = [1001] ^ 0011     // y = x^y, y = (x^y)^y = (x^0) = x
// x = [0011] =  1001 ^ [1010]   // x = x^y, x = (x^y)^x = (y^0) = y
// x = 0011; y = 1010;           // after

void t_algo_swap_02(int &a, int &b)
{
  a = a ^ b;
  b = a ^ b;      // b = a^b = (a^b)^b = a^0 = a
  a = a ^ b;      // a = a^b = (a^b)^a = b^0 = b
}

TEST(AlgoSwap, SwapUseXOR)
{
  int a = 9, b = 4;

  t_algo_swap_02(a, b);

  EXPECT_THAT(a, Eq(4));
  EXPECT_THAT(b, Eq(9));
}


// ={=========================================================================
// algo-occurance find a number seen odd times

namespace algo_occurance
{
  unsigned int find_number_odd_times_1(const vector<unsigned int> &input)
  {
    unsigned int result{};

    for (const auto e : input)
      result ^= e;

    return result;
  }

  unsigned int find_number_odd_times_2(const vector<unsigned int> &input)
  {
    size_t result{};
    map<size_t, size_t> imap{};

    // put them into a map<key, count>
    for (const auto e : input)
      ++imap[e];

    auto pos_return = find_if( imap.cbegin(), imap.cend(),
        [] (const pair<size_t,size_t> &e)
        { 
        if (e.second % 2) 
        return true; 

        return false;
        }
        );

    result = (pos_return != imap.cend()) ? pos_return->first : 0;
    return result;
  }

  unsigned int find_number_odd_times_3(const vector<unsigned int> &input)
  {
    size_t result{};
    multiset<size_t> imset{input.cbegin(), input.cend()};

    // put them into a map<key, count>
    for (const auto e : imset)
    {
      if ( imset.count(e) % 2)
      {
        result = e;
        break;
      }
    }

    return result;
  }
} // namespace

TEST(AlgoOccurance, FindNumberSeenOddTimes)
{
  using namespace algo_occurance;

  {
    // 2 2 4 4 4 4 6 6 8 8 10 10 12 12 12 (15)
    const vector<unsigned int> input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};
    EXPECT_THAT(find_number_odd_times_1(input), 12);
  }

  // find_if() returns the first match. so remove '4' to make it the first odd
  // num of sequence:
  // 2 2 4 4 4 6 6 8 8 10 10 12 12 12 (15)
  {
    const vector<unsigned int> input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2};
    EXPECT_THAT(find_number_odd_times_2(input), 4);
  }

  {
    const vector<unsigned int> input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};
    EXPECT_THAT(find_number_odd_times_2(input), 12);
  }

  {
    const vector<unsigned int> input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};
    EXPECT_THAT(find_number_odd_times_3(input), 12);
  }
}


// ={=========================================================================
// algo-occurance

// When input a, and b are 0 <= a <= b <= 100,000,000), write a problem to find
// out how many k integer appears.
// 
// e.g.,
// input: 11, 12, 13, 14, 15, k = 1
// 
// k appears 6 times.

int count_occurance_from_sequence(const vector<int> &input, int key) 
{
  map<char,int> count_map;

  for (auto value : input)
  {
    string str = to_string(value);
    for (auto e : str)
    {
      ++count_map[e];
    }
  }

  // string stringkey = to_string(key);
  // auto ret = count_map.find(stringkey[0]);
  
  // if values are [0,9] and are ASCII then, can use:
  auto ret = count_map.find(key+'0');

  return ret->second;
}

int count_occurance_from_sequence_0719(const vector<int> &input, int key) 
{
  int digit{}, count{};

  for (auto value : input)
  {
    // as algo-itoa
    for(; value;)
    {
      digit = value % 10;

      if (digit == key)
        ++count;

      value /= 10;
    }
  }

  return count;
}

TEST(AlgoOccurance, CountKey) 
{
  vector<int> input_value{11,12,13,14,15};

  EXPECT_THAT(count_occurance_from_sequence(input_value, 1), 6);
  EXPECT_THAT(count_occurance_from_sequence_0719(input_value, 1), 6);
}


// ={=========================================================================
// algo-find if a string has all unique chars
// 
// o. Space? assume ASCII 256 chars
//    ask for clarity. Since it's only for alphabet uppercase then use 32
//    bset and reduce space requirement.
//
// o. One simple optimization. return false if the length of input string is greater
//    than the number of uniques chars in the set; e.g., ASCII, 256
// 
//    if( sizeString > 256 ) return false;
//    
// o. cstring or std::string?
//
// o. time O(n) and space O(1)

// use lookup table
bool if_unique_01(const char *str)
{
    std::bitset<256> bset{};

    for (; *str; ++str)
    {
        if (bset[*str])
        {
            return false;
        }
        else
            bset[*str] = 1;
    }

    return true;
}


// use cxx-string-find()
bool if_unique_02(const char *str)
{
    std::string unique_set{};

    for (; *str; ++str)
    {
        if (unique_set.find(*str) != string::npos)
        {
            return false;
        }
        else
            unique_set += *str;
    }

    return true;
}

TEST(AlgoUnique, FindIUniqueCharsUseLookup)
{
    EXPECT_THAT(if_unique_01("abcdefghijklmnopqa"), false);
    EXPECT_THAT(if_unique_01("abcdefghijklmnopqr"), true);
}

TEST(AlgoUnique, FindIUniqueCharsUseStringFind)
{
    EXPECT_THAT(if_unique_02("abcdefghijklmnopqa"), false);
    EXPECT_THAT(if_unique_02("abcdefghijklmnopqr"), true);
}


// ={=========================================================================
// algo-reserve
//
// * use in/out parameter
// * cstring but efficient
// * strlen()-1 since array indexing is always [0, size-1], or [0,size) and in
//   this code, [begin, end] but not [begin, end)

namespace algo_reverse
{
  void reverse_string_1(char *input)
  {
    if (!input)
      return;

    char *begin = input;
    char *end = input + strlen(input)-1;
    char temp{};

    for (; begin < end; ++begin, --end)
    {
      // swap(begin, end);
      temp = *begin;
      *begin = *end;
      *end = temp;
    }
  }

  // ansic, p62

  void reverse_string_2(char *input)
  {
    int str{}, end{};
    char temp{}; 

    for (end = strlen(input)-1; str < end; ++str, --end)
    {
      // swap(str, end)
      temp = input[str], input[str] = input[end], input[end] = temp;
    }
  }

  // * if can use std::string and can return although c version is better.
  // * use *cxx-reverse-iterator*

  std::string reverse_string_3(const std::string &input)
  {
    return std::string(input.crbegin(), input.crend());
  }
  // when want to modify input itself

  void reverse_string_4(std::string &input)
  {
    std::reverse(input.begin(), input.end());
  }

  std::string reverse_string_5(const std::string &input)
  {
    std::string reversed{};

    for (auto len = input.size(); len > 0; --len)
      reversed.push_back(input[len-1]);

    return reversed;
  }

} // namespace

TEST(AlgoReverse, ReverseCstring)
{
  using namespace algo_reverse;

  {
    auto func = reverse_string_1;
    char input[] = "REVERSE IT";
    func(input);
    EXPECT_THAT(input, StrEq("TI ESREVER"));
  }

  {
    auto func = reverse_string_2;
    char input[] = "REVERSE IT";
    func(input);
    EXPECT_THAT(input, StrEq("TI ESREVER"));
  }
  {
    auto func = reverse_string_3;
    std::string input{"REVERSE IT"};
    EXPECT_THAT(func(input), Eq("TI ESREVER"));
  }

  {
    auto func = reverse_string_4;
    std::string input{"REVERSE IT"};
    func(input);
    EXPECT_THAT(input, Eq("TI ESREVER"));
  }

  {
    auto func = reverse_string_5;
    std::string input{"REVERSE IT"};
    EXPECT_THAT(func(input), Eq("TI ESREVER"));
  }
}

TEST(AlgoReverse, ReverseCstringTestCases)
{
  using namespace algo_reverse;

  auto func = reverse_string_1;

  {
    char input[] = "REVERSE IT";
    func(input);
    EXPECT_THAT(input, StrEq("TI ESREVER"));
  }

  {
    char *input = nullptr;
    func(input);
    EXPECT_THAT(input, nullptr);
  }

  {
    char input[] = "";
    func(input);
    EXPECT_THAT(input, StrEq(""));
  }

  {
    char input[] = "         HAY";
    func(input);
    EXPECT_THAT(input, StrEq("YAH         "));
  }

  {
    char input[] = "HAY         ";
    func(input);
    EXPECT_THAT(input, StrEq("         YAH"));
  }

  {
    char input[] = "\n\r\a\bHAY\n\r\a\b";
    func(input);
    EXPECT_THAT(input, StrEq("\b\a\r\nYAH\b\a\r\n"));
  }
}


namespace algo_reverse
{
  // Write a program to reverse a string with all its duplicates removed. Only the
  // last instance of a character in the reverse string has to appear. Also, the
  // following conditions are to be satisfied: Assume only Capital Letters.
  //
  // o. assume that input is ASCII and is all upper case chars. so can use
  // unsigned int to check if it's a duplicate or not. if needs more range to
  // cover then need to use something else.
  //
  // o. from the net

  std::string reverse_string_6(const std::string &input)
  {
    std::string sin{input};
    std::string sout{};
    unsigned int bappeared{};

    // remove duplicates from input
    for (size_t i = 0, size = sin.size(); i < size; ++i)
    {
      // only if not appeared before. use bitwise
      if (!(bappeared & (1 << (sin[i] - 'A'))))
      {
        sout += sin[i];
        bappeared |= (1 << (sin[i]-'A'));
      }
    }

    // sout: JTVAKISHL

    // return reverse;
    return std::string{sout.crbegin(), sout.crend()};
  }
} // namespace

TEST(AlgoReverse, ReverseCstringRemoveDuplicates)
{
  using namespace algo_reverse;

  auto func = reverse_string_6;

  std::string input{"JTVAKAVISHAAAL"};
  EXPECT_THAT(func(input), Eq("LHSIKAVTJ"));
}


// ={=========================================================================
// algo-anagram
//
// o. Space? assume ASCII 256 chars
//    ask for clarity. Since it's only for alphabet uppercase then use 32
//    bset and reduce space requirement.
//
// o. One simple optimization. return false if the length of input strings
//    are different
// 
// o. cstring or std::string?
//
// o. time O(n) and space O(1)

bool find_anagram_01(string one, string two)
{ 
    if (one.size() != two.size())
        return false;

    sort(one.begin(), one.end());
    sort(two.begin(), two.end());

    return (one == two) ? true : false;
}

TEST(AlgoAnagram, FindAnagramUseSort)
{
    EXPECT_TRUE(find_anagram_01("PARK", "APRK"));
    EXPECT_FALSE(find_anagram_01("PARK", "APRKPARK"));
    EXPECT_FALSE(find_anagram_01("PARK", "CARK"));
    EXPECT_FALSE(find_anagram_01("PARK", "PAAA"));
}


bool find_anagram_02(const string one, const string two)
{ 
    if (one.size() != two.size())
        return false;

    bitset<256> bset{};

    for (const auto &e : one)
        bset[e] = 1;

    for (const auto &e : two)
    {
        if (!bset[e])
        {
            return false; 
        }
    }

    return true;
}

TEST(AlgoAnagram, FindAnagramUseLookupNoDuplicate)
{
    EXPECT_TRUE(find_anagram_02("PARK", "APRK"));
    EXPECT_FALSE(find_anagram_02("PARK", "APRKPARK"));
    EXPECT_FALSE(find_anagram_02("PARK", "CARK"));

    // find_anagram_02() fails on:
    // EXPECT_FALSE(find_anagram_02("PARK", "PAAA"));
}


// To pass when there are duplicates in the input:
//  1. remove duplicates
//  2. move size check after removing duplicates.

bool find_anagram_03(string one, string two)
{ 
    bitset<256> bset{};

    auto one_end_unique = unique( one.begin(), one.end() );
    one.erase( one_end_unique, one.end() );

    auto two_end_unique = unique( two.begin(), two.end() );
    two.erase( two_end_unique, two.end() );

    if (one.size() != two.size())
        return false;

    for (const auto &e : one)
        bset[e] = 1;

    for (const auto &e : two)
    {
        if (!bset[e])
        {
            return false; 
        }
    }

    return true;
}

TEST(AlgoAnagram, FindAnagramUseLookupDuplicate)
{
    // find_anagram_02() fails on:
    EXPECT_FALSE(find_anagram_03("PARK", "PAAA"));
}


// ={=========================================================================
// algo-find first unique byte
// * time O(n) space O(1)
// * Do not check input.
// * Each input value are less than 256 adn the # of input are less than unit
//   max.
// * What does "first" mean? first unique byte seen or first unique byte in
//   input order?
//
//   Not byte seen first since every byte is seen first at first time. The
//   problem is that it can be shown in later of a stream.

unsigned char algo_find_first_unique(const vector<unsigned char> &input)
{
  // note: 
  // input order depends on the input size. Here, the order is one for that
  // byte. NO.
  size_t occurance[256]={}, order[256]={};

  // build occurance and order
  size_t input_order{};

  for (const auto e: input)
  {
    ++occurance[e];
    // here order starts from 1
    order[e] = ++input_order;
  }

  // find the first byte *cxx-limits*
  unsigned char saved_input{};

  // const unsigned int UINT_MAX_ORDER = ~((unsigned int)0);
  // unsigned int saved_order = UINT_MAX_ORDER;
  size_t saved_order{numeric_limits<size_t>::max()};

  for (auto i = 0; i < 256; ++i)
  {
    if ((occurance[i] == 1) && (order[i] < saved_order))
    {
      // *TN* i refers to input value
      saved_input = i;
      saved_order = order[i];
    }
  }

  // o. to print a char to int, have to define a var as int or to defind a
  // char and to use +saved_input trick.
  // http://www.cs.technion.ac.il/users/yechiel/c++-faq/print-char-or-ptr-as-number.html
  cout << "saved input : " << +saved_input << endl;
  cout << "saved order : " << saved_order << endl;

  return saved_input;
}

TEST(AlgoFindUniqueByte, FindFirstUniqueByteFromStream)
{
    const vector<unsigned char> input_stream{20, 11, 23, 33, 34, 54, 44, 38, 215, 126, 101, 20, 11, 20, 54, 54, 44, 38, 38, 215, 126, 215, 23};
    EXPECT_THAT(algo_find_first_unique(input_stream),Eq(33));
}

unsigned char algo_find_first_unique_0618(const vector<unsigned char> &input)
{
  // pair<occurance, index> and 255
  vector<pair<size_t, size_t>> lookup_table(255);

  // first pass, build table with occurance and input index
  size_t input_index{};

  for (const auto e : input)
  {
    auto &entry = lookup_table[e];
    entry.first += 1;
    entry.second = input_index;

    ++input_index;
  }

  // second pass, find first and unique input byte
  unsigned char current_byte{};
  size_t current_order = numeric_limits<size_t>::max();

  unsigned char current_index{};

  for (const auto e: lookup_table)
  {
    if (e.first == 1 && e.second < current_order)
    {
      current_byte = current_index;
      current_order = e.second;
    }

    ++current_index;
  }

  return current_byte;
}

TEST(AlgoFindUniqueByte, FindFirstUniqueByteFromStream_0618)
{
    const vector<unsigned char> input_stream{20, 11, 23, 33, 34, 54, 44, 38, 215, 126, 101, 20, 11, 20, 54, 54, 44, 38, 38, 215, 126, 215, 23};
    EXPECT_THAT(algo_find_first_unique_0618(input_stream),Eq(33));
}


// ={=========================================================================
// algo-occurance find the longest sequence of input char array

namespace algo_occurance
{
  MATCHER_P(EqPair, expected, "")
  {
    return arg.first == expected.first &&
      arg.second == expected.second;
  }

  MATCHER_P(NotEqPair, expected, "")
  {
    return arg.first != expected.first ||
      arg.second != expected.second;
  }

  // this is the second try and works fine.
  // O(n)

  pair<char, size_t> find_longest_1(const string &input)
  {
    size_t current_occurance{0}, longest_occurance{0};
    char current_char{0}, longest_char{0};

    for( auto letter : input )
    {
      if( letter != current_char )
      {
        current_char = letter;
        current_occurance = 1;
      }
      // if( letter == current_char )
      else
        ++current_occurance;

      if( current_occurance > longest_occurance )
      {
        longest_char = current_char;
        longest_occurance = current_occurance;
      }
    }

    return pair<char, size_t>(longest_char, longest_occurance);
  }
} // namspace

TEST(AlgoOccurance, FindLongest_1)
{
  using namespace algo_occurance;

  auto func = find_longest_1;

  const string input1{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"};
  EXPECT_THAT(func(input1), 
      EqPair(pair<char, size_t>('F', 18)));

  const string input2{"AAABBCCCCDDD"};
  EXPECT_THAT(func(input2), 
      EqPair(pair<char, size_t>('C', 4)));

  const string input3{"AAAAAAAAAAAA"};
  EXPECT_THAT(func(input3), 
      EqPair(pair<char, size_t>('A', 12)));
}


namespace algo_occurance
{
  pair<char, size_t> find_longest_2(const string &input)
  {
    char current_char{}, longest_char{};
    size_t current_occurance{}, longest_occurance{};

    for (auto letter : input)
    {
      // cout << "diff: curr" << current_char << ", letter: " << letter << endl;

      // if see the different char. use XOR and looks fancy?
      if (current_char^letter)
      {
        // cout << "diff: letter" << letter << endl;

        // save it if it's the longest so far
        if(current_occurance > longest_occurance)
        {
          longest_occurance = current_occurance;
          longest_char = current_char;
        }

        // reset and str a search again
        current_char = letter;
        current_occurance = 1;
      }
      // if see the same before
      else
        ++current_occurance;
    }

    return pair<char, size_t>(longest_char, longest_occurance);
  }

  // 0618
  pair<unsigned char, size_t> find_longest_3(const string &input)
  {
    unsigned char current_char{}, saved_char{};
    size_t        current_occurance{};
    size_t        saved_occurance = numeric_limits<size_t>::min();

    for(const auto e : input)
    {
      if (e != current_char)
      {
        // if see a longer sequence
        if (saved_occurance < current_occurance)
        {
          saved_char = current_char;
          saved_occurance = current_occurance;
        }

        current_char = e;
        current_occurance = 1;
      }
      else
        ++current_occurance;
    }

    // if see a longer sequence when input ends or input has only one sequence
    if (saved_occurance < current_occurance)
    {
      saved_char = current_char;
      saved_occurance = current_occurance;
    }

    return pair<unsigned char, size_t>(saved_char, saved_occurance);
  }

} // namespace

TEST(AlgoOccurance, FindLongest_2)
{
  using namespace algo_occurance;

  {
    auto func = find_longest_2;

    const string input1{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"};
    EXPECT_THAT(func(input1), 
        EqPair(pair<char, size_t>('F', 18)));

    const string input2{"AAABBCCCCDDD"};
    EXPECT_THAT(func(input2), 
        EqPair(pair<char, size_t>('C', 4)));

    // Firstly, looks better but it fails when the input has one long sequence.
    // returns {0, 0} because variable longest_xxx gets updated only in if case
    // but not else(when char is the same)

    const string input3{"AAAAAAAAAAAA"};
    EXPECT_THAT(func(input3), 
        NotEqPair(pair<char, size_t>('A', 12)));
  }
  {
    auto func = find_longest_3;

    const string input1{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"};
    EXPECT_THAT(func(input1), 
        EqPair(pair<char, size_t>('F', 18)));

    const string input2{"AAABBCCCCDDD"};
    EXPECT_THAT(func(input2), 
        EqPair(pair<char, size_t>('C', 4)));

    // now okay
    const string input3{"AAAAAAAAAAAA"};
    EXPECT_THAT(func(input3), 
        EqPair(pair<char, size_t>('A', 12)));
  }
}


// Can do better than O(n)? Can skip some chars in searching a sequence.

namespace algo_occurance
{
  pair<char, size_t> find_longest_4(const string &input)
  {
    char current_char{}, longest_char{};
    size_t current_occurance{}, longest_occurance{};

    size_t input_size = input.size();

    // take the first
    current_char = input[0];
    current_occurance = 1;

    // *TN* 
    // should kepp ++i here. Otherwise, would have no increase of i and
    // compare the same input in a loop.
    for (size_t i = 1; i < input_size; ++i)
    {
      // if see the different char. use XOR and looks fancy?
      if (current_char^input[i])
      {
        // save the previous sequence if it's the longest so far
        if (current_occurance > longest_occurance)
        {
          longest_occurance = current_occurance;
          longest_char = current_char;
        }

        // see i and i + (current longest sequence-1) and skip them in
        // between if they are different
        //
        // if they are the same, don't skip so don't change i
        size_t check_skip = i + (current_occurance-1);

        // *TN*
        // Have to have this check. Otherwise, access to out of range
        // of input.
        if (check_skip > input_size)
          break;

        // if they are different
        if (input[i]^input[check_skip])
        {
          // cout << "skipped : " << current_occurance-1 << endl;
          i += current_occurance-1;
        }

        // reset and str a search again
        current_char = input[i];
        current_occurance = 1;
      }
      // if see the same before
      else
        ++current_occurance;
    }

    // cout << "longest char      : " << longest_char << endl;
    // cout << "longest occurance : " << longest_occurance << endl;

    return pair<char, size_t>(longest_char, longest_occurance);
  }

} // namespace

TEST(AlgoOccurance, FindLongest_3)
{
  using namespace algo_occurance;

  auto func = find_longest_4;

  const string input1{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"};
  EXPECT_THAT(func(input1), 
      EqPair(pair<char, size_t>('F', 18)));

  const string input2{"AAABBCCCCDDD"};
  EXPECT_THAT(func(input2), 
      EqPair(pair<char, size_t>('C', 4)));

  // fails, return {0, 0}
  const string input3{"AAAAAAAAAAAA"};
  EXPECT_THAT(func(input3), 
      NotEqPair(pair<char, size_t>('A', 12)));

  const string input4{"AAAABCBBBBBCCCCDDD"};
  EXPECT_THAT(func(input4), 
      EqPair(pair<char, size_t>('B', 5)));

  // fails, return {A, 4}
  const string input5{"AAAABCCCCCCCC"};
  EXPECT_THAT(func(input5), 
      NotEqPair(pair<char, size_t>('C', 8)));
}


// ={=========================================================================
// algo-grade

// F: 60 > score
// D: 70 >
// C: 80 >
// B: 90 >
// A: 100 >
//
// plus '+' for 8, 9 and '-' for 0,1,2 for each grade

vector<string> grade_scores(const vector<int> scores)
{
  const vector<string> grade_table{"F", "D", "C", "B", "A"};
  vector<string> result;
  string grade{};

  for (auto e : scores)
  {
    if (e < 60)
      result.push_back(grade_table[0]);
    else
    {
      grade.clear();

      int band = (e - 50)/10;
      grade = grade_table[band];

      int plus_or_minus = (e - 50)%10;
      if (plus_or_minus <3)
        grade += "-";
      else if (plus_or_minus > 7)
        grade += "+";

      result.push_back(grade);
    }
  }

  return result;
}

vector<string> grade_scores_model(const vector<int> scores)
{
  const vector<string> grade_table{"F", "D", "C", "B", "A"};
  vector<string> result;
  string grade{};

  for (auto e : scores)
  {
    if (e < 60)
      result.push_back(grade_table[0]);
    else
    {
      grade.clear();

      grade = grade_table[(e - 50)/10];

      int plus_or_minus = e % 10;
      if (plus_or_minus <3)
        grade += "-";
      else if (plus_or_minus > 7)
        grade += "+";

      result.push_back(grade);
    }
  }

  return result;
}

TEST(AlgoGrade, UseTable)
{
    const vector<int> coll{54, 60, 62, 66, 68, 71, 73, 78, 89, 98};
    EXPECT_THAT(grade_scores(coll), 
            ElementsAre("F", "D-", "D-","D", "D+","C-", "C", "C+", "B+", "A+"));

    EXPECT_THAT(grade_scores_model(coll), 
            ElementsAre("F", "D-", "D-","D", "D+","C-", "C", "C+", "B+", "A+"));
}


// CodeComplete 18.4 Stair-Step Access Tables.
//
// A: 90.0% >=
// B: 90.0% <
// C: 75.0% <
// D: 65.0% <
// F: 50.0% <
//
// Suppose scores are floating numbers.

const string grade_stair_step(const double score)
{
  const vector<double> range_limit{50.0, 65.0, 75.0, 90.0, 100.0};
  const vector<string> grade{"F", "D", "C", "B", "A"};

  int max_grade_level = grade.size()-1;
  int grade_level{};
  string student_grade{"A"};

  while ((student_grade == "A") && (grade_level < max_grade_level))
  {
    if (score < range_limit[grade_level])
      student_grade = grade[grade_level];

    ++grade_level;
  }

  return student_grade;
}

TEST(AlgoGrade, UseStairStep)
{
    EXPECT_THAT(grade_stair_step(49.0), "F");
    EXPECT_THAT(grade_stair_step(60.0), "D");
    EXPECT_THAT(grade_stair_step(62.0), "D");
    EXPECT_THAT(grade_stair_step(66.0), "C");
    EXPECT_THAT(grade_stair_step(68.0), "C");
    EXPECT_THAT(grade_stair_step(89.0), "B");
    EXPECT_THAT(grade_stair_step(98.0), "A");
}


// ={=========================================================================
// algo-intersect find if rectangles intersect
//
// from ANSIC 130. Handles point rather than each value.
//
// assumption about bot/and.
// 
//     +--------+ top(x2, y2)      +--------+ bot(x2, y2)   
//     |        |                  |        |            
//     |        |                  |        |            
//     +--------+                  +--------+            
//  bot(x1,y1)                  top(x1,y1)               
//
//
// since depending on how Rect is defined, checking can be different in
// is_point_in_rect(). 
//
// this assumes bot < point < top and if use top/bot way, it fails
//
// bool is_point_in_rect(const Point &point, const Rect &rect)
// {
//     return ((rect.bot_.x_ <= point.x_) && (point.x_ <= rect.top_.x_)) &&
//         ((rect.bot_.y_ <= point.y_) && (point.y_ <= rect.top_.y_));
// }
//
// this affect also the order of arguments to Rect(). However, not much point
// of supporting no-assumption case. No, when tried it again, spend hours why
// the check code fails depending on how Rect() is defined.

struct Point
{
    // not a default ctor so have to define it
    Point(const int x, const int y) : x_(x), y_(y) {}
    int x_{};
    int y_{};
};

struct Rect
{
    // not a default ctor so have to define it
    // Rect(bot, top);
    Rect(const Point &a, const Point &b) : bot_(a), top_(b) {}
    Point bot_;
    Point top_;
};

// bot.x <= x <= top.x AND bot.y <= y <= top.y
// should be AND
//
// support both cases which Rect defines.

bool is_point_in_rect(const Point &point, const Rect &rect)
{
  return ((rect.top_.x_ >= point.x_ && rect.bot_.x_ <= point.x_) || (rect.bot_.x_ >= point.x_ && rect.top_.x_ <= point.y_)) &&
    ((rect.top_.y_ >= point.y_ && rect.bot_.y_ <= point.y_) || (rect.bot_.y_ >= point.y_ && rect.top_.y_ <= point.y_));
}

bool algo_is_intersected(const Rect &r1, const Rect &r2)
{
  return is_point_in_rect(r1.top_, r2) || is_point_in_rect(r1.bot_, r2);
}


TEST(AlgoIntersect, CheckIfRectIntersected)
{
  {
    // bot/top
    Rect a(Point(10,10), Point(20,20));
    Rect b(Point(15,15), Point(25,25));
    EXPECT_THAT(algo_is_intersected(a, b), Eq(true));
  }

  {
    // top/bot
    Rect a(Point(20, 20), Point(10,10));
    Rect b(Point(25, 25), Point(15,15));
    EXPECT_THAT(algo_is_intersected(a, b), Eq(true));
  }

  {
    // bot/top, inclues the same point
    Rect a(Point(10,10), Point(20,20));
    Rect b(Point(20,20), Point(25,25));
    EXPECT_THAT(algo_is_intersected(a, b), Eq(true));
  }

  {
    // bot/top, inclues the same point
    Rect a(Point(10,10), Point(20,20));
    Rect b(Point(25,25), Point(35,35));
    EXPECT_THAT(algo_is_intersected(a, b), Eq(false));
  }
}


// ={=========================================================================
// algo-roman-numeric

// Convert Arabic number to Roman number
//
// 1. the roman number has fixed mappings:
//
//  1       : I
//  5       : V
//  10      : X
//  50      : L
//  100     : C
//  500     : D
//  1000    : M
//
// 2. As itoa(), use loop, %, and / to get a digit to convert:
//  
//      tens    2   1   0
//              X   X   X
//      digit   D   D   D
//
// 3. Divide 0-9 into sub-groups to get:
//
//  * 0 < digit < 4:
//
//      tens = 0:
//          1       (1, 10**0)      I
//          2       (2, 10**0)      II
//          3       (3, 10**0)      III
//
//      tens = 1:
//          10      (1, 10**1)      X
//          20      (2, 10**1)      XX
//          30      (3, 10**1)      XXX
//
//      tens = 2:
//          100     (1, 10**2)      C
//          200     (2, 10**2)      CC
//          300     (3, 10**2)      CCC
//      ...
//
//      To use the same function as 5 < digit < 9 case:
//
//      tens = 0:
//                          (base, repeat, roman to repeat)
//          1, 0+1          (0*10**0, 1, 10**0)     I
//          2, 0+1          (0*10**0, 2, 10**0)     II
//          3, 0+1          (0*10**0, 3, 10**0)     III
//
//  * 5 < digit < 9:
//
//      tens = 0:
//                          (base, repeat, roman to repeat)
//          6, 5+1          (5*10**0, 1, 10**0)     VI
//          7, 5+2          (5*10**0, 2, 10**0)     VII
//          8, 5+3          (5*10**0, 3, 10**0)     VIII
//
//      tens = 1:
//          60, 50+10       (5*10**1, 1, 10**1)     LX
//          70, 50+20       (5*10**1, 2, 10**1)     LXX
//          89, 50+30       (5*10**1, 3, 10**1)     LXXX
//
//      tens = 2:
//          600, 500+100    (5*10**1, 1, 10**1)     DC
//          700, 500+200    (5*10**1, 2, 10**1)     DCC
//          890, 500+300    (5*10**1, 3, 10**1)     DCCC
//      ...
//
//  * 4 or 9
//
//      tens = 0:
//          4, 5-1          (10**0 + (4+1)*10**0)   IV
//          9, 10-1         (10**0 + (9+1)*10**0)   IX
//
//      tens = 1:
//          40, 50-10       (10**1 + (4+1)*10**1)   XL
//          90, 100-10      (10**1 + (9+1)*10**1)   XC
//      ...
//
//  * 5
//
//      tens = 0:
//          5,              (10**0*5)
//
//      tens = 1:
//          50              (10**1*5)
//
//      tens = 2:
//          500             (10**2*5)

class RomanConvert
{
  public:

    // 1. tried to use input/output string arg like 
    //
    //      void convert(uint32_t, std::string &)
    //
    // and use public string member in the fixture class.
    // However, since there are many asserts in a test, have to reset
    // this string member before using the next assert. dropped.
    //
    // 2. uses one single string and add at the front whenever converted a
    // letter. By this, no need to reverse the result as itoa() does but
    // there will be a downside; relocation and performance. Howerver, since
    // it is not a long string, do not think they matter here. 

    std::string convert(uint32_t number) const 
    {
      std::string converted;
      uint32_t tens{0}, digit{0};

      // for fixed roman letters
      // the code works without this to cover fixed romans since the loop
      // below will find it eventually. However, it uses a map already and
      // may save some time.

      auto it = roman_table.find(number);
      if (it != roman_table.cend())
        return it->second;

      for (; number;)
      {
        digit = number % 10;

        if (digit < 4)
        {
          padWithFoundRoman(digit, tens, 0, converted);
        }
        else if (digit > 5 && digit < 9)
        {
          padWithFoundRoman(digit, tens, 5, converted);
        }
        else if (!(digit % 5))
        {
          findFiveBaseRoman(digit, tens, converted);
        }
        else if (digit == 4 || digit == 9)
        {
          findSubstractRoman(digit, tens, converted);
        }

        ++tens;

        number /= 10;
      }

      return converted;
    }

  private:
    const std::map<uint32_t, std::string> roman_table{
      {0, ""},
        {1, "I"}, {5, "V"},
        {10, "X"}, {50, "L"},
        {100, "C"}, {500, "D"},
        {1000, "M"}
    };

    void padWithFoundRoman(uint32_t number, uint32_t tens, uint32_t fixed, std::string &padded) const
    {
      std::string result;

      // find the letter to pad
      auto it = roman_table.find( pow(10, tens) );
      if ( it == roman_table.cend() )
        std::cout << "roman_table(" << pow(10, tens) << ") not found" << std::endl;

      std::string letter = it->second;

      // find the base
      it = roman_table.find( fixed*pow(10, tens) );
      if ( it == roman_table.cend() )
        std::cout << "roman_table(" << fixed*pow( 10, tens ) << ") not found" << std::endl;

      std::string base = it->second;

      // add the base once
      result += base;

      // add the pad
      for (uint32_t count = number - fixed; count; --count)
        result += letter;

      padded.insert(0, result);
    }

    void findSubstractRoman(uint32_t number, uint32_t tens, std::string &converted) const
    {
      std::string padded;

      // find the letter in substract form
      auto it = roman_table.find( pow(10, tens) );
      if ( it == roman_table.cend() )
        std::cout << "roman_table(" << pow(10, tens) << ") not found" << std::endl;

      std::string letter = it->second;

      // find the base
      it = roman_table.find( (number+1)*pow(10, tens) );
      if ( it == roman_table.cend() )
        std::cout << "roman_table(" << (number+1)*pow( 10, tens ) << ") not found" << std::endl;

      std::string base = it->second;

      converted.insert( 0, letter + base );
    }

    void findFiveBaseRoman(uint32_t number, uint32_t tens, std::string &converted) const
    {
      // find the letter in substract form
      auto it = roman_table.find( number * pow(10, tens) );
      if ( it == roman_table.cend() )
        std::cout << "roman_table(" << pow(10, tens) << ") not found" << std::endl;

      converted.insert( 0, it->second );
    }
};


TEST(AlgoRomanConvert, ConvertToRomansFirstAttempt) {

  RomanConvert converter;

  EXPECT_THAT(converter.convert(1), Eq("I"));
  EXPECT_THAT(converter.convert(2), Eq("II"));
  EXPECT_THAT(converter.convert(3), Eq("III"));
  EXPECT_THAT(converter.convert(4), Eq("IV"));
  EXPECT_THAT(converter.convert(5), Eq("V"));
  EXPECT_THAT(converter.convert(6), Eq("VI"));
  EXPECT_THAT(converter.convert(7), Eq("VII"));
  EXPECT_THAT(converter.convert(8), Eq("VIII"));
  EXPECT_THAT(converter.convert(9), Eq("IX"));
  EXPECT_THAT(converter.convert(10), Eq("X"));
  EXPECT_THAT(converter.convert(11), Eq("XI"));
  EXPECT_THAT(converter.convert(12), Eq("XII"));
  EXPECT_THAT(converter.convert(13), Eq("XIII"));
  EXPECT_THAT(converter.convert(16), Eq("XVI"));
  EXPECT_THAT(converter.convert(17), Eq("XVII"));
  EXPECT_THAT(converter.convert(18), Eq("XVIII"));
  EXPECT_THAT(converter.convert(20), Eq("XX"));
  EXPECT_THAT(converter.convert(23), Eq("XXIII"));
  EXPECT_THAT(converter.convert(41), Eq("XLI"));
  EXPECT_THAT(converter.convert(45), Eq("XLV"));
  EXPECT_THAT(converter.convert(50), Eq("L"));
  EXPECT_THAT(converter.convert(80), Eq("LXXX"));
  EXPECT_THAT(converter.convert(91), Eq("XCI"));
  EXPECT_THAT(converter.convert(95), Eq("XCV"));
  EXPECT_THAT(converter.convert(100), Eq("C"));
  EXPECT_THAT(converter.convert(122), Eq("CXXII"));
  EXPECT_THAT(converter.convert(152), Eq("CLII"));
  EXPECT_THAT(converter.convert(196), Eq("CXCVI"));
  EXPECT_THAT(converter.convert(247), Eq("CCXLVII"));
  EXPECT_THAT(converter.convert(288), Eq("CCLXXXVIII"));
  EXPECT_THAT(converter.convert(298), Eq("CCXCVIII"));
  EXPECT_THAT(converter.convert(500), Eq("D"));
  EXPECT_THAT(converter.convert(1000), Eq("M"));
  EXPECT_THAT(converter.convert(1513), Eq("MDXIII"));
  EXPECT_THAT(converter.convert(2999), Eq("MMCMXCIX"));
  EXPECT_THAT(converter.convert(3447), Eq("MMMCDXLVII"));
}


string convert_to_roman(unsigned int arabic) 
{
  string convert{};

  // note:
  // 1. the order of element in the map matters
  // 2. do not need 6?? in the map since it follows the same addition rule
  // 3. to fix a warning on signed int and unsigned int comparion, use "u"
  // suffix.

  const auto lookup_table = {
    make_pair(1000u, "M"),
    make_pair(900u, "CM"),
    // make_pair(600, "DC"),
    make_pair(500u, "D"),
    make_pair(400u, "CD"),
    make_pair(100u, "C"),
    //
    make_pair(90u, "XC"),
    // make_pair(60, "LX"),
    make_pair(50u, "L"),
    make_pair(40u, "XL"),
    make_pair(10u, "X"),
    //
    make_pair(9u, "IX"),
    // make_pair(6, "VI"),
    make_pair(5u, "V"),
    make_pair(4u, "IV"),
    make_pair(1u, "I")
  };

  for (const auto e : lookup_table)
  {
    while (e.first <= arabic)
    {
      arabic -= e.first;
      convert += e.second;
    }
  }

  // cout << "converted: " << convert << endl;

  return convert;
}

TEST(AlgoRomanConvert, ConvertToRomansTDD) 
{
  EXPECT_THAT(convert_to_roman(1), Eq("I"));
  EXPECT_THAT(convert_to_roman(2), Eq("II"));
  EXPECT_THAT(convert_to_roman(3), Eq("III"));
  EXPECT_THAT(convert_to_roman(4), Eq("IV"));
  EXPECT_THAT(convert_to_roman(5), Eq("V"));
  EXPECT_THAT(convert_to_roman(6), Eq("VI"));
  EXPECT_THAT(convert_to_roman(7), Eq("VII"));
  EXPECT_THAT(convert_to_roman(8), Eq("VIII"));
  EXPECT_THAT(convert_to_roman(9), Eq("IX"));
  EXPECT_THAT(convert_to_roman(10), Eq("X"));
  EXPECT_THAT(convert_to_roman(11), Eq("XI"));
  EXPECT_THAT(convert_to_roman(12), Eq("XII"));
  EXPECT_THAT(convert_to_roman(13), Eq("XIII"));
  EXPECT_THAT(convert_to_roman(16), Eq("XVI"));
  EXPECT_THAT(convert_to_roman(17), Eq("XVII"));
  EXPECT_THAT(convert_to_roman(18), Eq("XVIII"));
  EXPECT_THAT(convert_to_roman(20), Eq("XX"));
  EXPECT_THAT(convert_to_roman(23), Eq("XXIII"));
  EXPECT_THAT(convert_to_roman(41), Eq("XLI"));
  EXPECT_THAT(convert_to_roman(45), Eq("XLV"));
  EXPECT_THAT(convert_to_roman(50), Eq("L"));
  EXPECT_THAT(convert_to_roman(80), Eq("LXXX"));
  EXPECT_THAT(convert_to_roman(91), Eq("XCI"));
  EXPECT_THAT(convert_to_roman(95), Eq("XCV"));
  EXPECT_THAT(convert_to_roman(100), Eq("C"));
  EXPECT_THAT(convert_to_roman(122), Eq("CXXII"));
  EXPECT_THAT(convert_to_roman(152), Eq("CLII"));
  EXPECT_THAT(convert_to_roman(196), Eq("CXCVI"));
  EXPECT_THAT(convert_to_roman(247), Eq("CCXLVII"));
  EXPECT_THAT(convert_to_roman(288), Eq("CCLXXXVIII"));
  EXPECT_THAT(convert_to_roman(298), Eq("CCXCVIII"));
  EXPECT_THAT(convert_to_roman(500), Eq("D"));
  EXPECT_THAT(convert_to_roman(1000), Eq("M"));
  EXPECT_THAT(convert_to_roman(1513), Eq("MDXIII"));
  EXPECT_THAT(convert_to_roman(2999), Eq("MMCMXCIX"));
  EXPECT_THAT(convert_to_roman(3447), Eq("MMMCDXLVII"));
}

const string roman_numeral_0711(unsigned int value)
{
  const auto roman_table{
    make_pair(1000u, "M"),
    make_pair(900u, "CM"),
    make_pair(500u, "D"),
    make_pair(400u, "CD"),
    make_pair(100u, "C"),
    make_pair(90u, "XC"),
    make_pair(50u, "L"),
    make_pair(40u, "XL"),
    make_pair(10u, "X"),
    make_pair(9u, "IX"),
    make_pair(5u, "V"),
    make_pair(4u, "IV"),
    make_pair(1u, "I")
  };

  string result{};

  for (const auto &e : roman_table)
  {
    // note: must be "<=" but not "<"
    
    while (e.first <= value)
    {
      value -= e.first;
      result += e.second;
    }
  }

  return result;
}

// 2018
const string roman_numeral_1011(unsigned int value)
{
  const initializer_list<pair<unsigned int, string>> roman_table{
    {1000u, "M"},
    {900u, "CM"},
    {500u, "D"},
    {400u, "CD"},
    {100u, "C"},
    {90u, "XC"},
    {50u, "L"},
    {40u, "XL"},
    {10u, "X"},
    {9u, "IX"},
    {5u, "V"},
    {4u, "IV"},
    {1u, "I"}
  };

  string result{};

  for (const auto &e : roman_table)
  {
    // note: must be "<=" but not "<"
    
    while (e.first <= value)
    {
      value -= e.first;
      result += e.second;
    }
  }

  return result;
}

TEST(AlgoRomanConvert, 0711) 
{
  EXPECT_THAT(roman_numeral_0711(1), Eq("I"));
  EXPECT_THAT(roman_numeral_0711(2), Eq("II"));
  EXPECT_THAT(roman_numeral_0711(3), Eq("III"));
  EXPECT_THAT(roman_numeral_0711(4), Eq("IV"));
  EXPECT_THAT(roman_numeral_0711(5), Eq("V"));
  EXPECT_THAT(roman_numeral_0711(6), Eq("VI"));
  EXPECT_THAT(roman_numeral_0711(7), Eq("VII"));
  EXPECT_THAT(roman_numeral_0711(8), Eq("VIII"));
  EXPECT_THAT(roman_numeral_0711(9), Eq("IX"));
  EXPECT_THAT(roman_numeral_0711(10), Eq("X"));
  EXPECT_THAT(roman_numeral_0711(11), Eq("XI"));
  EXPECT_THAT(roman_numeral_0711(12), Eq("XII"));
  EXPECT_THAT(roman_numeral_0711(13), Eq("XIII"));
  EXPECT_THAT(roman_numeral_0711(16), Eq("XVI"));
  EXPECT_THAT(roman_numeral_0711(17), Eq("XVII"));
  EXPECT_THAT(roman_numeral_0711(18), Eq("XVIII"));
  EXPECT_THAT(roman_numeral_0711(20), Eq("XX"));
  EXPECT_THAT(roman_numeral_0711(23), Eq("XXIII"));
  EXPECT_THAT(roman_numeral_0711(41), Eq("XLI"));
  EXPECT_THAT(roman_numeral_0711(45), Eq("XLV"));
  EXPECT_THAT(roman_numeral_0711(50), Eq("L"));
  EXPECT_THAT(roman_numeral_0711(80), Eq("LXXX"));
  EXPECT_THAT(roman_numeral_0711(91), Eq("XCI"));
  EXPECT_THAT(roman_numeral_0711(95), Eq("XCV"));
  EXPECT_THAT(roman_numeral_0711(100), Eq("C"));
  EXPECT_THAT(roman_numeral_0711(122), Eq("CXXII"));
  EXPECT_THAT(roman_numeral_0711(152), Eq("CLII"));
  EXPECT_THAT(roman_numeral_0711(196), Eq("CXCVI"));
  EXPECT_THAT(roman_numeral_0711(247), Eq("CCXLVII"));
  EXPECT_THAT(roman_numeral_0711(288), Eq("CCLXXXVIII"));
  EXPECT_THAT(roman_numeral_0711(298), Eq("CCXCVIII"));
  EXPECT_THAT(roman_numeral_0711(500), Eq("D"));
  EXPECT_THAT(roman_numeral_0711(1000), Eq("M"));
  EXPECT_THAT(roman_numeral_0711(1513), Eq("MDXIII"));
  EXPECT_THAT(roman_numeral_0711(2999), Eq("MMCMXCIX"));
  EXPECT_THAT(roman_numeral_0711(3447), Eq("MMMCDXLVII"));
}

TEST(AlgoRomanConvert, 1011) 
{
  EXPECT_THAT(roman_numeral_1011(1), Eq("I"));
  EXPECT_THAT(roman_numeral_1011(2), Eq("II"));
  EXPECT_THAT(roman_numeral_1011(3), Eq("III"));
  EXPECT_THAT(roman_numeral_1011(4), Eq("IV"));
  EXPECT_THAT(roman_numeral_1011(5), Eq("V"));
  EXPECT_THAT(roman_numeral_1011(6), Eq("VI"));
  EXPECT_THAT(roman_numeral_1011(7), Eq("VII"));
  EXPECT_THAT(roman_numeral_1011(8), Eq("VIII"));
  EXPECT_THAT(roman_numeral_1011(9), Eq("IX"));
  EXPECT_THAT(roman_numeral_1011(10), Eq("X"));
  EXPECT_THAT(roman_numeral_1011(11), Eq("XI"));
  EXPECT_THAT(roman_numeral_1011(12), Eq("XII"));
  EXPECT_THAT(roman_numeral_1011(13), Eq("XIII"));
  EXPECT_THAT(roman_numeral_1011(16), Eq("XVI"));
  EXPECT_THAT(roman_numeral_1011(17), Eq("XVII"));
  EXPECT_THAT(roman_numeral_1011(18), Eq("XVIII"));
  EXPECT_THAT(roman_numeral_1011(20), Eq("XX"));
  EXPECT_THAT(roman_numeral_1011(23), Eq("XXIII"));
  EXPECT_THAT(roman_numeral_1011(41), Eq("XLI"));
  EXPECT_THAT(roman_numeral_1011(45), Eq("XLV"));
  EXPECT_THAT(roman_numeral_1011(50), Eq("L"));
  EXPECT_THAT(roman_numeral_1011(80), Eq("LXXX"));
  EXPECT_THAT(roman_numeral_1011(91), Eq("XCI"));
  EXPECT_THAT(roman_numeral_1011(95), Eq("XCV"));
  EXPECT_THAT(roman_numeral_1011(100), Eq("C"));
  EXPECT_THAT(roman_numeral_1011(122), Eq("CXXII"));
  EXPECT_THAT(roman_numeral_1011(152), Eq("CLII"));
  EXPECT_THAT(roman_numeral_1011(196), Eq("CXCVI"));
  EXPECT_THAT(roman_numeral_1011(247), Eq("CCXLVII"));
  EXPECT_THAT(roman_numeral_1011(288), Eq("CCLXXXVIII"));
  EXPECT_THAT(roman_numeral_1011(298), Eq("CCXCVIII"));
  EXPECT_THAT(roman_numeral_1011(500), Eq("D"));
  EXPECT_THAT(roman_numeral_1011(1000), Eq("M"));
  EXPECT_THAT(roman_numeral_1011(1513), Eq("MDXIII"));
  EXPECT_THAT(roman_numeral_1011(2999), Eq("MMCMXCIX"));
  EXPECT_THAT(roman_numeral_1011(3447), Eq("MMMCDXLVII"));
}


// ={=========================================================================
// algo-recursion-hanoi

namespace algo_recursion_hanoi
{
  unsigned int recursion_depth;

  void print_depth(bool dash, unsigned int depth)
  {
    for( unsigned int i=0; i <= depth; ++i)
    {
      if(dash)
        cout << "--";
      else
        cout << "  ";
    }

    if(dash)
      cout << "(" << depth << ") ";
    else
      cout << "      ";
  }

  void move_disk(int count, int start, int finish, int temp)
  {
    // ++calls;
    ++recursion_depth;
    print_depth(true, recursion_depth);

    cout << "Move(" << count << "," << start << "," << finish << "," << temp << ")" << endl;

    if (count > 0)
    {
      move_disk(count-1, start, temp, finish);

      print_depth(false, recursion_depth);
      cout << "move " << count << " disk, " << start << " -> " << finish << endl;

      move_disk(count-1, temp, finish, start);
    }
    else
    {
      print_depth(false, recursion_depth);
      cout << "move " << count << " disk, " << start << " -> " << finish << endl;
    }

    --recursion_depth;
  }

} // namespace

// disks are 0, 1, 2, 3
//
// [ RUN      ] AlgoRecursion.Hanoi
// ----(1) Move(3,1,3,2)
// ------(2) Move(2,1,2,3)                {
// --------(3) Move(1,1,3,2)
// ----------(4) Move(0,1,2,3)
//                 move 0 disk, 1 -> 2
//               move 1 disk, 1 -> 3
// ----------(4) Move(0,2,3,1)
//                 move 0 disk, 2 -> 3
//
//             move 2 disk, 1 -> 2
//
// --------(3) Move(1,3,2,1)
// ----------(4) Move(0,3,1,2)
//                 move 0 disk, 3 -> 1
//               move 1 disk, 3 -> 2
// ----------(4) Move(0,1,2,3)
//                 move 0 disk, 1 -> 2    }
//
//           move 3 disk, 1 -> 3
//
// ------(2) Move(2,2,3,1)
// --------(3) Move(1,2,1,3)
// ----------(4) Move(0,2,3,1)
//                 move 0 disk, 2 -> 3
//               move 1 disk, 2 -> 1
// ----------(4) Move(0,3,1,2)
//                 move 0 disk, 3 -> 1
//             move 2 disk, 2 -> 3
// --------(3) Move(1,1,3,2)
// ----------(4) Move(0,1,2,3)
//                 move 0 disk, 1 -> 2
//               move 1 disk, 1 -> 3
// ----------(4) Move(0,2,3,1)
//                 move 0 disk, 2 -> 3
// [       OK ] AlgoRecursion.Hanoi (3 ms)

TEST(AlgoRecursion, Hanoi) 
{
  using namespace algo_recursion_hanoi;

  const int DISKS{3};

  move_disk(DISKS, 1, 3, 2);
}


// ={=========================================================================
// algo-recursion-fibonacci

namespace algo_recursion_fibonacci
{
  // n is nth fibonacci term

  int fibonacci_1(int n)
  {
    if (n <= 0)
      return 0;
    else if (n == 1)
      return 1;
    else
      return fibonacci_1(n-1) + fibonacci_1(n-2);
  }

  int fibonacci_2(int n)
  {
    int twoback{};  // f(n-2)
    int oneback{};  // f(n-1)
    int current{};

    if (n <= 0)
      return 0;
    else if (n == 1)
      return 1;
    else
    {
      // back from current
      twoback = 0;
      oneback = 1;

      for (int i = 2; i <= n; ++i)
      {
        current = twoback + oneback;

        // for next f
        twoback = oneback;
        oneback = current;
      }
    }

    return current;
  }

} // namespace

TEST(AlgoRecursion, Fibonacci) 
{
  using namespace algo_recursion_fibonacci;

  EXPECT_THAT(fibonacci_1(4), 3);
  EXPECT_THAT(fibonacci_1(5), 5);
  EXPECT_THAT(fibonacci_1(6), 8);
  EXPECT_THAT(fibonacci_1(7), 13);

  EXPECT_THAT(fibonacci_2(4), 3);
  EXPECT_THAT(fibonacci_2(5), 5);
  EXPECT_THAT(fibonacci_2(6), 8);
  EXPECT_THAT(fibonacci_2(7), 13);
}


// algo-leetcode-18
/*
70. Climbing Stairs, Easy

You are climbing a stair case. It takes n steps to reach to the top.

Each time you can either climb 1 or 2 steps. In how many distinct ways can you
climb to the top?

Note: Given n will be a positive integer.

Example 1:
Input: 2
Output: 2
Explanation: There are two ways to climb to the top.
1. 1 step + 1 step
2. 2 steps

Example 2:
Input: 3
Output: 3
Explanation: There are three ways to climb to the top.
1. 1 step + 1 step + 1 step
2. 1 step + 2 steps
3. 2 steps + 1 step
 
*/

/*

Approach 1: Brute Force

In this brute force approach we take all possible step combinations i.e. 1 and
2, at every step. At every step we are calling the function
climbStairsclimbStairs for step 1 and 2, and return the sum of returned values
of both functions.

climbStairs(i,n)=(i+1,n)+climbStairs(i+2,n)

where i defines the current step and n defines the destination step.

Time complexity : O(2^n)

since the size of recursion tree will be 2^n

N = 2
          (0,2)

    (1,2)       (2,2)
                ret 1
(2,2)   (3,2) 
ret 1   ret 0

return 1 means found and return 0 means not found.

Space complexity : O(n) The depth of the recursion tree can go upto n.

*/ 

namespace leetcode_easy_018
{
  int climb_stairs(int start, int end)
  {
    if (start == end)
      return 1;
    else if (start > end)
      return 0;

    return climb_stairs(start + 1, end) + climb_stairs(start + 2, end);
  }

  int climbStairs_1(int n) 
  {
    return climb_stairs(0, n);
  }
} // namespace

TEST(AlgoRecusrion, LeetCode_Easy_018_ClimbStairs_1)
{
  using namespace leetcode_easy_018;
  auto func = climbStairs_1;

  EXPECT_THAT(func(2), 2);
  EXPECT_THAT(func(3), 3);
  EXPECT_THAT(func(4), 5);
  EXPECT_THAT(func(30), 1346269);
}


/*
Approach 2: Recursion with memoization

In the previous approach we are redundantly calculating the result for every
step. Instead, we can store the result at each step in memomemo array and
directly returning the result from the memo array whenever that function is
called again.

In this way we are *pruning* recursion tree with the help of memo array and
reducing the size of recursion tree upto n.

(Like fibonacci problem, the right part of recursion tree uses the same
calculation which are calculated already but calculate them again since they are
lost. So can keep them and use it then better performance)

Time complexity : O(n). Size of recursion tree can go upto nn.
Space complexity : O(n). The depth of recursion tree can go upto nn. 

see time difference between recursion and iterative version

[ RUN      ] LeetCode.Easy_018_ClimbStairs_1
[       OK ] LeetCode.Easy_018_ClimbStairs_1 (52 ms)
[ RUN      ] LeetCode.Easy_018_ClimbStairs_2
[       OK ] LeetCode.Easy_018_ClimbStairs_2 (0 ms)
[ RUN      ] LeetCode.Easy_018_ClimbStairs_3
[       OK ] LeetCode.Easy_018_ClimbStairs_3 (0 ms)
 
*/

namespace leetcode_easy_018
{
  int climb_stairs(int start, int end, vector<int> &memo)
  {
    if (start == end)
      return 1;
    else if (start > end)
      return 0;
    else if(memo[start])
      return memo[start];

    memo[start] = climb_stairs(start + 1, end, memo) + climb_stairs(start + 2, end, memo);
    return memo[start];
  }

  int climbStairs_2(int n) 
  {
    vector<int> memo(n + 1, 0);
    return climb_stairs(0, n, memo);
  }
} // namespace

TEST(AlgoRecusrion, LeetCode_Easy_018_ClimbStairs_2)
{
  using namespace leetcode_easy_018;
  auto func = climbStairs_2;

  EXPECT_THAT(func(2), 2);
  EXPECT_THAT(func(3), 3);
  EXPECT_THAT(func(4), 5);
  EXPECT_THAT(func(30), 1346269);
}


/*
Approach 3: Dynamic Programming

As we can see this problem can be broken into subproblems, and it contains the
optimal substructure property i.e. its optimal solution can be constructed
efficiently from optimal solutions of its subproblems, we can use dynamic
programming to solve this problem.

One can reach ith step in one of the two ways:

Taking a single step from (i-1) th step.

Taking two step from (i−2) th step.

(since it is about way to reach to n but not number of steps)

So, the total number of ways to reach i th is equal to sum of ways of reaching
(i−1)th step and ways of reaching (i-2)th step.  

Let dp[i] denotes the number of ways to reach on i th step:

dp[i]=dp[i-1]+dp[i-2]


Approach 4: Fibonacci Number

In the above approach we have used dpdp array where dp[i]=dp[i-1]+dp[i-2]. It
can be easily analysed that dp[i] is nothing but ith fibonacci number.

means the dp value sequence. this is fibonacci sequence:

0, 1, 1, 2, 3, 5, 8, 13, 21, 34, ...

dp value sequence:

0, 1, 2, 3, 5, 8, 13, 21, 34, ...

Now we just have to find n th number of the fibonacci series having 1 and 2
their first and second term respectively, i.e. Fib(1)=1 and Fib(2)=2.


  int fibonacci_2(int n)
  {
    int twoback{};  // f(n-2)
    int oneback{};  // f(n-1)
    int current{};

    if (n <= 0)
      return 0;
    else if (n == 1)
      return 1;
    else
    {
      // back from current
      twoback = 0;
      oneback = 1;

      for (int i = 2; i <= n; ++i)
      {
        current = twoback + oneback;

        // for next f
        twoback = oneback;
        oneback = current;
      }
    }

    return current;
  }
*/

namespace leetcode_easy_018
{
  int climbStairs_3(int n) 
  {
    // base cases
    if(n <= 0) return 0;
    if(n == 1) return 1;
    if(n == 2) return 2;

    int one_step_before = 2;    // when n == 2
    int two_steps_before = 1;   // when n == 1
    int all_ways = 0;

    // starts from n == 3
    for(int i=3; i <= n; i++)
    {
      all_ways = one_step_before + two_steps_before;
      two_steps_before = one_step_before;
      one_step_before = all_ways;
    }

    return all_ways;
  };
} // namespace

TEST(AlgoRecusrion, LeetCode_Easy_018_ClimbStairs_3)
{
  using namespace leetcode_easy_018;
  auto func = climbStairs_3;

  EXPECT_THAT(func(2), 2);
  EXPECT_THAT(func(3), 3);
  EXPECT_THAT(func(4), 5);
  EXPECT_THAT(func(30), 1346269);
}


// ={=========================================================================
// algo-recursion-factorial

namespace algo_recursion_factorial
{
  // CPR 227
  int factorial_1(int value)
  {
    // CodeComplete 440, #19
    if (value > 1)
      return factorial_1(value-1)*value;

    return 1;
  }

  // CodeComplete 397, #17.2,
  // Don’t use recursion for factorials or Fibonacci numbers

  int factorial_2(int value)
  {
    int result{1};

    // for (int i = 1; i <= value; ++i)

    for (int i = 2; i <= value; ++i)
    {
      result *= i;
    }

    return result;
  }

  int factorial_3(int value)
  {
    int result{1};

    for (; 0 < value; --value)
    {
      result *= value;
    }

    return result;
  }
} // namespace


// not good idea to use factorial to see performance difference due to tail
// recursion since number gets bigger quickly

TEST(AlgoRecursion, Factorial) 
{
  using namespace algo_recursion_factorial;

  EXPECT_THAT(factorial_1(5), 120);
  EXPECT_THAT(factorial_1(10), 3628800);

  EXPECT_THAT(factorial_2(5), 120);
  EXPECT_THAT(factorial_2(10), 3628800);

  EXPECT_THAT(factorial_3(5), 120);
}


// ={=========================================================================
// algo-recursion-maze algo-maze

using MazePoint = std::pair<int, int>;

// 1. input and point(row, col) which is the same as point(y, x) but 
// not point(x, y). This has to do with array access.
//
// 2. If it's valid point which are not checked against to traveled points, call
// find_path for every possible move from a point and then this move is cheked
// against to the traveled points. So supporting diagonal move needs more
// recursive calls. Therefore, it use the traveled path to exit the call for
// points that it already traveled; to move forward, to move back when see the
// dead end, and naturally to prevent circular path.
//
// See again that calls find_path() for every possible move.
//
// 3. To support range from (1,1) to (N,N), used padding to the input data:
//
// {2,2,2,2,2,2},
// {2,0,0,0,0,0}, 
// {2,1,1,1,1,0},
// {2,0,0,0,0,0},
// {2,0,1,1,1,1},
// {2,2,0,0,0,0}
//
// the result of run:
//
// (1, 2) (1, 3) (1, 4) (1, 5) (2, 5) (3, 5) (3, 4) (3, 3) (3, 2) (3, 1) (4, 1) (5, 1) (5, 2) (5, 3) (5, 4) (5, 5) (16)
//
//             [ 10578] | find_path(1, 1) {
//             [ 10578] |   find_path(1, 2) {
//             [ 10578] |     find_path(1, 1) {
//   12.428 us [ 10578] |     } /* find_path */
//             [ 10578] |     find_path(1, 3) {
//             [ 10578] |       find_path(1, 2) {
//   10.787 us [ 10578] |       } /* find_path */
//             [ 10578] |       find_path(1, 4) {
//             [ 10578] |         find_path(1, 3) {
//   13.404 us [ 10578] |         } /* find_path */
//             [ 10578] |         find_path(1, 5) {
//             [ 10578] |           find_path(1, 4) {
//   12.767 us [ 10578] |           } /* find_path */
//             [ 10578] |           find_path(2, 5) {
//             [ 10578] |             find_path(1, 5) {
//   85.506 us [ 10578] |             } /* find_path */
//             [ 10578] |             find_path(3, 5) {
//             [ 10578] |               find_path(3, 4) {
//             [ 10578] |                 find_path(3, 3) {
//             [ 10578] |                   find_path(3, 2) {
//             [ 10578] |                     find_path(3, 1) {
//             [ 10578] |                       find_path(3, 2) {
//   28.249 us [ 10578] |                       } /* find_path */
//             [ 10578] |                       find_path(4, 1) {
//             [ 10578] |                         find_path(3, 1) {
//  182.678 us [ 10578] |                         } /* find_path */
//             [ 10578] |                         find_path(5, 1) {
//             [ 10578] |                           find_path(5, 2) {
//             [ 10578] |                             find_path(5, 1) {
//   82.954 us [ 10578] |                             } /* find_path */
//             [ 10578] |                             find_path(5, 3) {
//             [ 10578] |                               find_path(5, 2) {
//   17.163 us [ 10578] |                               } /* find_path */
//             [ 10578] |                               find_path(5, 4) {
//             [ 10578] |                                 find_path(5, 3) {
//   19.277 us [ 10578] |                                 } /* find_path */
//             [ 10578] |                                 find_path(5, 5) {
//   19.241 us [ 10578] |                                 } /* find_path */
//  116.223 us [ 10578] |                               } /* find_path */
//  213.627 us [ 10578] |                             } /* find_path */
//  391.920 us [ 10578] |                           } /* find_path */
//  967.572 us [ 10578] |                         } /* find_path */
//    2.857 ms [ 10578] |                       } /* find_path */
//    3.996 ms [ 10578] |                     } /* find_path */
//    4.060 ms [ 10578] |                   } /* find_path */
//    4.126 ms [ 10578] |                 } /* find_path */
//    4.220 ms [ 10578] |               } /* find_path */
//    4.287 ms [ 10578] |             } /* find_path */
//    5.416 ms [ 10578] |           } /* find_path */
//    6.411 ms [ 10578] |         } /* find_path */
//    6.485 ms [ 10578] |       } /* find_path */
//    6.559 ms [ 10578] |     } /* find_path */
//    6.708 ms [ 10578] |   } /* find_path */
//    7.233 ms [ 10578] | } /* find_path */


struct Maze
{
  std::vector<std::vector<int>> input;

  // std::array<std::array<int, 11>, 11> input;
  // std::array<std::array<int, 6>, 6> input;

  // set of visited points
  std::set<MazePoint> visited_points{};

  // array of path points
  std::vector<MazePoint> path_points{};

  Maze(int row, int col)
    : row_(row), col_(col) {}

  bool AlreadyTried(MazePoint position)
  {
    return visited_points.find(position) == visited_points.end() ? false : true;
  }

  bool FoundTheExit(MazePoint position)
  {
    return position == MazePoint(row_-1, col_-1) ? true : false;
  }

  void RememberPosition(MazePoint position)
  {
    auto result = visited_points.insert(position);
    if (!result.second)
    {
      cout << "RememberPosition: founds duplicates" << endl;
      cout << "RememberPosition: (" << position.first << ", " << position.second << ")" << endl;
      PRINT_M_ELEMENTS(visited_points);
    }
  }

  // if cannot move, return the input position
  MazePoint GetPositionToMoveLeft(MazePoint position)
  {
    MazePoint point_to_move{position.first, position.second-1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  MazePoint GetPositionToMoveRight(MazePoint position)
  {
    MazePoint point_to_move{position.first, position.second+1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  MazePoint GetPositionToMoveUp(MazePoint position)
  {
    MazePoint point_to_move{position.first-1, position.second};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  MazePoint GetPositionToMoveDown(MazePoint position)
  {
    MazePoint point_to_move{position.first+1, position.second};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  MazePoint GetPositionToDiagRightUp(MazePoint position)
  {
    MazePoint point_to_move{position.first-1, position.second+1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  MazePoint GetPositionToDiagRightDown(MazePoint position)
  {
    MazePoint point_to_move{position.first+1, position.second+1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  MazePoint GetPositionToDiagLeftUp(MazePoint position)
  {
    MazePoint point_to_move{position.first-1, position.second-1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  MazePoint GetPositionToDiagLeftDown(MazePoint position)
  {
    MazePoint point_to_move{position.first+1, position.second-1};

    if (ValidMazePoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  private:

  bool ValidMazePoint(MazePoint position)
  {
    if ((0 <= position.first && position.first < row_) &&
        (0 <= position.second && position.second < col_) &&
        input[position.first][position.second] != 1
       )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  int row_{};
  int col_{};
};


// To use uftrace log, changed it from 
// bool find_path(Maze &maze, MazePoint position)

bool find_path(Maze &maze, int row, int col)
{
  MazePoint position{row, col};

  // to prevent circular path and going backwards and this make it search
  // forward. 
  if (maze.AlreadyTried(position))
  {
    return false;
  }

  // found a path which is stop condition
  if (maze.FoundTheExit(position))
  {
    return true;
  }

  // remember a position tried
  maze.RememberPosition(position);

  MazePoint new_position{};

  // left
  if ((new_position = maze.GetPositionToMoveLeft(position)) != position)
  {
    // cout << "l:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "left:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // right
  if ((new_position = maze.GetPositionToMoveRight(position)) != position)
  {
    // cout << "r:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "right:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // up
  if ((new_position = maze.GetPositionToMoveUp(position)) != position)
  {
    // cout << "u:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "up:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // down
  if ((new_position = maze.GetPositionToMoveDown(position)) != position)
  {
    // cout << "d:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "down:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag right up 
  if ((new_position = maze.GetPositionToDiagRightUp(position)) != position)
  {
    // cout << "dru:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:rup:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag right down 
  if ((new_position = maze.GetPositionToDiagRightDown(position)) != position)
  {
    // cout << "drd:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:rdown:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag left up 
  if ((new_position = maze.GetPositionToDiagLeftUp(position)) != position)
  {
    // cout << "dlu:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:lup:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag left down 
  if ((new_position = maze.GetPositionToDiagLeftDown(position)) != position)
  {
    // cout << "dld:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:ldown:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  return false;
}

TEST(AlgoMaze, ExerciseInterfaces)
{
  Maze maze(5, 5);
  maze.input = {
    {0,0,0,0,0},
    {1,1,1,1,0},
    {0,0,0,0,0},
    {0,1,1,1,1},
    {2,0,0,0,0}
  };

  maze.RememberPosition(MazePoint(1, 1));
  maze.RememberPosition(MazePoint(1, 2));

  EXPECT_THAT(maze.AlreadyTried(MazePoint(1, 1)), true);
  EXPECT_THAT(maze.AlreadyTried(MazePoint(1, 3)), false);

  EXPECT_THAT(maze.FoundTheExit(MazePoint(4, 5)), false);
  EXPECT_THAT(maze.FoundTheExit(MazePoint(5, 5)), false);

  // not able to move
  EXPECT_THAT(maze.GetPositionToMoveLeft(MazePoint(0, 0)), Eq(MazePoint(0, 0)));

  // able to move
  EXPECT_THAT(maze.GetPositionToMoveLeft(MazePoint(0, 1)), Eq(MazePoint(0, 0)));
}

TEST(DISABLED_AlgoMaze, Array5x5)
{
  Maze maze(5, 5);
  maze.input = {
    {0,0,0,0,0},
    {1,1,1,1,0},
    {0,0,0,0,0},
    {0,1,1,1,1},
    {2,0,0,0,0}
  };

  // use start point (0, 0) rather then (1, 1).
  find_path(maze, 0, 0);
  PRINT_M_ELEMENTS(maze.path_points);
}

// TEST(DISABLED_Maze, Array10x10)
TEST(AlgoMaze, Array10x10)
{
  // when not support diagonal move
  // Maze maze(10, 10);
  // maze.input = {
  //   {0,1,0,1,0,1,0,0,0,1},
  //   {0,1,0,1,0,1,1,1,0,1},
  //   {0,0,0,0,0,1,0,0,0,1},
  //   {0,1,0,1,1,1,0,1,1,1},
  //   {0,1,2,1,0,0,0,0,0,1},
  //   {1,1,0,1,0,1,1,1,1,1},
  //   {0,1,0,0,0,0,0,1,0,1},
  //   {0,1,1,1,0,1,1,1,0,1},
  //   {0,0,0,0,0,0,0,0,0,1},
  //   {1,1,1,1,1,1,1,1,0,0}
  // };

  Maze maze(10, 10);
  maze.input = {
    {0,1,0,1,0,1,0,0,0,1},
    {0,1,0,1,0,1,1,1,0,1},
    {0,0,0,0,0,1,0,0,0,1},
    {0,1,0,1,1,1,0,1,1,1},
    {0,1,2,1,0,0,0,0,0,1},
    {1,1,0,1,0,1,1,1,1,1},
    {0,1,0,0,0,0,0,1,0,1},
    {0,1,1,1,0,1,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,0}
  };
 
  find_path(maze, 0, 0);
  PRINT_M_ELEMENTS(maze.path_points);
}

TEST(DISABLED_AlgoMaze, Array15x15)
{
  Maze maze(15, 15);
  maze.input = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  
    {0,1,0,1,0,0,0,1,0,1,0,1,0,1,0},
    {0,1,0,0,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,0,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
 
  find_path(maze, 0, 0);
  PRINT_M_ELEMENTS(maze.path_points);
}

// This input select the path which do have have "2" in. How to support this
// requirement?
//
// * change the traveled path to have the value and to keep only traveled points
// so far. So need to remove point when find_path() returns false so that
// traveled path only have points up to where it runs into
//
// * changed the end condition to see if the traveled path has the "2". if not
// return false so that can try other paths.


TEST(DISABLED_AlgoMaze, Array20x20)
{
  Maze maze(20, 20);
  maze.input = {

    {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, 
    {0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,0,1},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
    {0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1},
    {0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1},
    {0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1},
    {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0}
  };
 
  find_path(maze, 0, 0);
  PRINT_M_ELEMENTS(maze.path_points);
}


// ={=========================================================================
// algo-count-bits count same bits between two integers
//
// A = 35 = 10 0011
// B =  9 =    1001
//
// Ans = 2 because only counts bit positions which are valid position in both
// integers.
//
// From ansic, p50. 
// The function counts the number of 1 bits in its integer argument. 
//
// 1. The key is not to use sizeof operator
// 2. unsigned int
//
// *cxx-shift* Must use `unsigned` to do  `right-shift` in order to have
// guaranteed 0 values. 
//
// 3. use independent of type.


/* 191. Number of 1 Bits, Easy

Write a function that takes an unsigned integer and return the number of '1'
bits it has (also known as the Hamming weight).

Example 1:

Input: 00000000000000000000000000001011
Output: 3

Explanation: 
The input binary string 00000000000000000000000000001011 has a total of three
'1' bits.

Example 2:

Input: 00000000000000000000000010000000
Output: 1

Explanation: 
The input binary string 00000000000000000000000010000000 has a total of one '1'
bit.

Example 3:

Input: 11111111111111111111111111111101
Output: 31

Explanation: 
The input binary string 11111111111111111111111111111101 has a total of thirty
one '1' bits.
 
Note:

Note that in some languages such as Java, there is no unsigned integer type. In
this case, the input will be given as signed integer type and should not affect
your implementation, as the internal binary representation of the integer is the
same whether it is signed or unsigned. 

In Java, the compiler represents the signed integers using 2's complement
notation. Therefore, in Example 3 above the input represents the signed integer
-3.

*/

// namespace leetcode_easy_191
namespace algo_bit
{
  // as with itoa
  //
  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Number of
  // 1 Bits.
  //
  // Memory Usage: 8.1 MB, less than 70.42% of C++ online submissions for Number
  // of 1 Bits.

  int hammingWeight_1(uint32_t n)
  {
    int count{};

    while (n)
    {
      if (n % 2)
        count++;

      n /= 2;
    }

    return count;
  }

  // since input is unsigned int, can use >> shift

  int hammingWeight_2(uint32_t n)
  {
    int count{};

    while (n)
    {
      if (n & 0x01)
        count++;

      n >>= 1;
    }

    return count;
  }

  // page 51. exercise 2-9. In a two's complement number system, x &= (x-1)
  // deletes the rightmost 1-bit in x. Explain why. Use this observation to write
  // a 'faster' version of bitcount.
  //
  // Answer:
  // 
  // If x is odd, then (x-1) has the same bit representation as x except that the
  // rightmost 1-bit becomes a 0. In this case, (x & (x-1)) == (x-1).
  // 
  // x = 5: 5(101) & 4(100) = 100  // 101 -> 100 by having rightmost 1 to 0
  // 
  // If x is even, the end result of anding(&) x and x-1 has the rightmost 1 of x to 0.
  // 
  // x = 4: 4(100) & 3(11)  = 0    // 100 -> 0   by having rightmost 1 to 0
  //          ^ rightmost 1
  //
  // x = 6: 6(110) & 5(101) = 100  // 110 -> 100 by having rightmost 1 to 0
  //           ^ rightmost 1
  //
  // x = 8: 8(1000) & 7(111) = 0   // 1000 -> 0  by having rightmost 1 to 0
  // 
  // 000   0     All even numbers has tailing 0s and it becomes 1 when minus 1
  // 001   1
  // 010   2
  // 011   3
  // 100   4
  // 101   5
  // 110   6
  // 111   7
  // ...
  // 
  // note: This is about careful observation but not a mechanism of borrowing a
  // carry for example. For both odd and even case, has the effect of having
  // rightmost 1 to 0. So clear 1 from x one by one and no need to check on if
  // to count bits.
  // 
  // note: And(&) is faster than shift operation? Yes and also there is no `if`
  // in the loop.

  int count_bit(uint32_t n)
  {
    int count{};

    for (; n; n &= (n-1))
      count++;

    return count;
  }

} // namespace

TEST(AlgoBit, CountBits)
{
  using namespace algo_bit;

  {
    auto func = hammingWeight_1;

    // Input: 00000000000000000000000000001011, 11
    EXPECT_THAT(func(11), 3); 

    // Input: 00000000000000000000000010000000, 128
    EXPECT_THAT(func(128), 1); 

    // Input: 11111111111111111111111111111101, 4294967293
    EXPECT_THAT(func(4294967293), 31); 
  }
  {
    auto func = hammingWeight_2;

    // Input: 00000000000000000000000000001011, 11
    EXPECT_THAT(func(11), 3); 

    // Input: 00000000000000000000000010000000, 128
    EXPECT_THAT(func(128), 1); 

    // Input: 11111111111111111111111111111101, 4294967293
    EXPECT_THAT(func(4294967293), 31); 
  }
  {
    auto func = count_bit;

    // Input: 00000000000000000000000000001011, 11
    EXPECT_THAT(func(11), 3); 

    // Input: 00000000000000000000000010000000, 128
    EXPECT_THAT(func(128), 1); 

    // Input: 11111111111111111111111111111101, 4294967293
    EXPECT_THAT(func(4294967293), 31); 
  }
}


namespace algo_bit
{
  // note:
  // used to loop count running on two inputs to get common bit coutns for early
  // attempts. However, turns out it's not necessary.
  //
  // returns MSB position which starts from 1 since input >> is evalueated after
  // ++count but not 0th.

  uint32_t get_msb_pos_1(const uint32_t value)
  {
    uint32_t count{};
    uint32_t input = value;

    // do not need to check like: if (input &1) to increase count for every
    // interation since when runs out 1, input becomes 0 and the loop ends. 
    for (; input != 0; input >>= 1)
      ++count;

    return count;
  }

} // namespace


TEST(AlgoBit, GetMSBPosition)
{
  using namespace algo_bit;

  // A = 35 = 10 0011
  // B =  9 =    1001
  EXPECT_THAT(get_msb_pos_1(35), Eq(6));
  EXPECT_THAT(get_msb_pos_1(9), Eq(4));

  // count_bit() is different since has high MSB and has 0s in the lower bits.

  EXPECT_THAT(count_bit(35), 3);
  EXPECT_THAT(count_bit(9), 2);
}


namespace algo_bit
{
  // 2018.0619
  // 1. unsigned int
  int count_bits_18_0619(const unsigned int a, const unsigned int b)
  {
    // get min and max
    auto input = minmax(a, b);

    // take min value
    unsigned int min = input.first;
    unsigned int max = input.second;

    // get position of the pivot
    unsigned int num_of_bits = sizeof(min)*8;
    unsigned int pos_of_msb{};
    unsigned int pivot = min;

    for (unsigned int i = 0; i < num_of_bits; ++i)
    {
      if (pivot & 0x1)
        pos_of_msb = i;

      pivot >>= 1;
    }

    // // get mask value, mask max, and get xor'ed value
    // unsigned int mask_value{}, calculated_input{};

    // for (unsigned int i = 0; i <= pos_of_msb; ++i)
    //   mask_value |= (1 << i);

    // max = max & mask_value;

    unsigned int calculated_input = max^min;

    // get num of common bits
    unsigned int num_of_common_bits{};

    for (unsigned int i = 0; i <= pos_of_msb; ++i)
    {
      if (!(calculated_input & 0x1))
        ++num_of_common_bits;

      calculated_input >>= 1;
    }

    return num_of_common_bits;
  }

  // 2018.07
  int count_bits_18_0717(const unsigned int a, const unsigned int b)
  {
    // get min and max
    auto input = minmax(a, b);

    // take min value
    unsigned int min = input.first;
    unsigned int max = input.second;

    unsigned int num_of_common_bits{};

    // no differetn when use for (; min && min >>= 1;)
    for (; min; min >>= 1, max >>= 1)
    {
      // same bit, 0 or 1 between two numbers
      if ((min & 0x1) == (max &0x1u))
        ++num_of_common_bits;
    }

    return num_of_common_bits;
  }

  // 2019.03
  int count_common_bits(uint32_t a, uint32_t b)
  {
    uint32_t count{};

    for (; a && b; a >>= 1, b >>= 1)
    {
      if ((a & 0x1) == (b & 0x01))
        count++;
    }

    return count;
  }

} // namespce

TEST(AlgoBit, CommonBits)
{
  using namespace algo_bit;

  {
    //   9,   1001
    //  35, 100011,   mask, 15 (1111),  max, 3(0011)
    //                                    9,   1001
    //                                  xor,   1010
    //                                  ans, 2
    EXPECT_THAT(count_bits_18_0717(35, 9), 2);

    // 55 = 100111,  mask, 7 (0111),   max, 7(0111)
    //                                    5,    101
    //                                  xor, 2( 010)
    //                                  ans, 2 
    EXPECT_THAT(count_bits_18_0717(55, 5), 2);
  }
  {
    auto func = count_common_bits;
    EXPECT_THAT(func(35, 9), 2);
    EXPECT_THAT(func(55, 5), 2);
  }
}


/* 190. Reverse Bits, Easy

Reverse bits of a given 32 bits unsigned integer.

Example 1:

Input: 00000010100101000001111010011100
Output: 00111001011110000010100101000000

Explanation: 
The input binary string 00000010100101000001111010011100 represents the unsigned
integer 43261596, so return 964176192 which its binary representation is
00111001011110000010100101000000.

Example 2:

Input: 11111111111111111111111111111101
Output: 10111111111111111111111111111111

Explanation: 
The input binary string 11111111111111111111111111111101 represents the unsigned
integer 4294967293, so return 3221225471 which its binary representation is
10101111110010110010011101101001.
 
*/

namespace algo_bit
{
  // initial thought was to use mix of atoi and itoa

  // from discussion:
  //
  // O(1) bit operation C++ solution (8ms), tworuler
  //
  // for 8 bit binary number abcdefgh, the process is as follow:
  // abcdefgh -> efghabcd -> ghefcdab -> hgfedcba

  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Reverse
  // Bits.
  //
  // Memory Usage: 8 MB, less than 79.46% of C++ online submissions for Reverse
  // Bits.

  uint32_t reverseBits(uint32_t n) 
  {
    // abcdefgh -> efghabcd 
    n = (n >> 16) | (n << 16);

    // efghabcd -> ghefcdab
    // ef00ab00 >> 00ef00ab, 00gh00cd << gh00cd00
    n = ((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8);

    // ghefcdab -> hgfedcba
    n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);

    n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
    n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);

    return n;
  }

} // namespace

TEST(AlgoBit, ReverseBits)
{
  using namespace algo_bit;

  EXPECT_THAT(reverseBits(43261596), 964176192);
  EXPECT_THAT(reverseBits(4294967293), 3221225471);
}


// ={=========================================================================
// algo-equi

int equi_poor(int A[], int n)
{
  int start{}, index{};

  for (index = 0; index < n; ++index)
  {
    // have to reset them on every loop
    int left_sum{}, right_sum{};

    for (start = 0; start < index; ++start)
      left_sum += A[start];

    for (start = index+1; start < n; ++start)
      right_sum += A[start];

    if (left_sum == right_sum)
      return index;
  }

  return -1;
}

TEST(AlgoEquilbrium, EquiPoor)
{
  int coll[] = {-7, 1, 5, 2, -4, 3, 0};

  EXPECT_THAT(equi_poor(coll, 7), 3);
}

int equi_0620(int A[], int n)
{
  int index{};

  long long total_sum{};
  for (index = 0; index < n; ++index)
    total_sum += A[index];

  long long right_sum{total_sum};
  long long left_sum{};

  for (index = 0; index < n; ++index)
  {
    if (index-1 >= 0)
      left_sum += A[index-1];

    right_sum = total_sum - left_sum - A[index];

    if (left_sum == right_sum)
      return index;
  }

  return -1;
}

int equi_0620_better(int A[], int n)
{
  int index{};

  long long total_sum{};
  for (index = 0; index < n; ++index)
    total_sum += A[index];

  long long right_sum{total_sum};
  long long left_sum{};

  for (index = 0; index < n; ++index)
  {
    // if (index-1 >= 0)
    //   left_sum += A[index-1];

    right_sum = total_sum - left_sum - A[index];

    if (left_sum == right_sum)
      return index;

    left_sum += A[index];
  }

  return -1;
}

// do not use total_sum
int equi_do_not_use_total( int A[], int n )
{
  if( !n || !A )
    return -1;

  long long rsum = 0, lsum = 0;

  for(int i=0; i<n; i++)
    rsum += A[i];

  for(int i=0; i<n; i++)
  {
    rsum -= A[i];

    if( lsum == rsum )
      return i;

    lsum += A[i];
  }

  return -1;
}

TEST(AlgoEquilbrium, Equi_0620)
{
  int coll[] = {-7, 1, 5, 2, -4, 3, 0};

  EXPECT_THAT(equi_0620(coll, 7), 3);
  EXPECT_THAT(equi_0620_better(coll, 7), 3);
  EXPECT_THAT(equi_do_not_use_total(coll, 7), 3);
}


// ={=========================================================================
// algo-equi-tape

int tape_equi_0628(vector<int> &A)
{
  int abs_diff{};
  int saved_diff = numeric_limits<int>::max();

  long long right_sum{}, left_sum{};
  for (unsigned int i = 0; i < A.size(); ++i)
    right_sum += A[i]; 

  for (unsigned int i = 0; i < A.size()-1; ++i)
  {
    left_sum += A[i];
    right_sum -= A[i];

    abs_diff = abs(left_sum - right_sum);
    if (abs_diff < saved_diff)
    {
      saved_diff = abs_diff;
    }
  }

  return saved_diff;
}


int tape_equi_1(vector<int> &A)
{
  if(!A.size())
    return -1;

  // note:
  long long tsum = 0;

  // size N, [0, N-1]
  for(unsigned int i=0; i < A.size(); i++)
    tsum += A[i];

  long long rsum = 0, lsum = 0;
  int runabs = INT_MAX, curabs = 0;

  for(unsigned int i=0; i < A.size()-1; i++)
  {
    lsum += A[i];
    rsum = tsum - lsum;

    curabs = abs(lsum-rsum);

    if(runabs > curabs)
      runabs = curabs;
  }

  return runabs;
}

int tape_equi_2(vector<int> &A)
{
  if(!A.size())
    return -1;

  // note:
  long long rsum = 0;

  // size N, [0, N-1]
  for(unsigned int i=0; i < A.size(); i++)
    rsum += A[i];

  long long lsum = 0;
  int runabs = INT_MAX, curabs = 0;

  // [0, N-2]
  for(unsigned int i=0; i < A.size()-1; i++)
  {
    lsum += A[i];
    rsum -= A[i];

    curabs = abs(lsum-rsum);

    if(runabs > curabs)
      runabs = curabs;
  }

  return runabs;
}

int tape_equi_model(vector<int> &A) 
{
  // write your code in C++98
  if( !A.size() )
    return -1;
 
  long long sum = 0, rsum = 0, lsum = 0;
  int cmin = INT_MAX;
 
  for(size_t i=0; i<A.size(); i++)
    sum += A[i];
 
  lsum = A[0];
 
  // note: 
  // it is okay to use (n-1)th to calc lsum since not used anyway. WHY?
  for(size_t i=1; i<A.size(); i++)
  {
    rsum = sum - lsum;
 
    // cmin = abs cause warning of possible loss since assign from long long to int.
    if( abs(lsum-rsum) < cmin )
      cmin = abs(lsum-rsum);
 
    lsum += A[i];
  }
 
  return cmin;
}

TEST(AlgoEquilbrium, TapeEqui_0620)
{
  vector<int> coll{3, 2, 1, 4, 3};

  EXPECT_THAT(tape_equi_model(coll), 1);
  EXPECT_THAT(tape_equi_1(coll), 1);
  EXPECT_THAT(tape_equi_2(coll), 1);
  EXPECT_THAT(tape_equi_0628(coll), 1);
}


// ={=========================================================================
// algo-distinct-count

bool absLessThan(int a, int b)
{
  return abs(a) < abs(b);
}

bool absEqual(int a, int b)
{
  return abs(a) == abs(b);
}

int distinct_count_old_01(const vector<int> &A)
{
  if(!A.size())
    return -1;

  vector<int> ivec;
  int count = 0;

  for(size_t i = 0; i < A.size(); ++i)
    ivec.push_back(A[i]);

  sort( ivec.begin(), ivec.end(), absLessThan );
  auto it_end_unique = unique( ivec.begin(), ivec.end(), absEqual );

  auto it_begin = ivec.begin();

  while( it_begin != it_end_unique )
  {
    ++it_begin;
    ++count;
  }

  return count;
}

int distinct_count_old_02(const vector<int> &A) {
    // write your code in C++98
    int size = A.size();
    
    if( !size )
        return -1;
        
    std::set<int> iset;
    
    for( int i = 0; i <  size; i++ )
        iset.insert( abs(A[i]) );
        
    return iset.size();
}

int distinct_count_0621(const vector<int> &A)
{
  set<int> coll{};

  for (const auto e : A)
    coll.insert(abs(e));

  return coll.size();
}

TEST(AlgoDistinctCount, DistinctCount)
{
  vector<int> coll{-5, -3, -1, 0, 3, 6};

  EXPECT_THAT(distinct_count_old_01(coll), 5);
  EXPECT_THAT(distinct_count_old_02(coll), 5);
  EXPECT_THAT(distinct_count_0621(coll), 5);
}


// ={=========================================================================
// algo-water-volume

namespace algo_water_volume
{
  // two pass algo

  int water_volume_1(vector<int> const &A)
  {
    int max_value{};
    size_t max_index{};
    size_t volume{};

    // get the index of max
    for (size_t i = 0; i < A.size(); ++i)
    {
      if (max_value < A[i])
      {
        max_value = A[i];
        max_index = i;
      } 
    }

    // from left to the max
    int max_current{std::numeric_limits<int>::min()};

    for (size_t i = 0; i < max_index; ++i)
    {
      if (max_current <= A[i])
        max_current = A[i];
      else
        volume += max_current - A[i];
    }

    // from right to the max
    max_current = std::numeric_limits<int>::min();

    for (size_t i = A.size()-1; i > max_index; --i)
    {
      if (max_current <= A[i])
        max_current = A[i];
      else
        volume += max_current - A[i];
    }

    return volume;
  }

  // one pass

  int water_volume_2(vector<int> const &A)
  {
    int right_max{std::numeric_limits<int>::min()};
    int left_max{std::numeric_limits<int>::min()};

    size_t left_index{0};
    size_t right_index{A.size()-1};

    size_t volume{};

    while (left_index < right_index)
    {
      left_max = max(left_max, A[left_index]);
      right_max = max(right_max, A[right_index]);

      // cout << "li: " << left_index << ", ri: " << right_index
      //   << ", lm: " << left_max << ", rm: " << right_max << endl;

      // move left bar to right
      // note: should have "equal" case on either if. otherwise, loops goes
      // infinitive.

      if (left_max <= right_max)
      {
        if (A[left_index] < left_max)
        {
          volume += left_max - A[left_index];
          // cout << "vol += " << left_max << " - " 
          //   << A[left_index] << endl;
        }

        left_index++;
      }

      // move right bar to left
      if (left_max > right_max)
      {
        if (A[right_index] < right_max)
        {
          volume += right_max - A[right_index];
          // cout << "vol += " << right_max << " - " 
          //   << A[right_index] << endl;
        } 

        right_index--;
      }
    }

    return volume;
  }
} // namespace

TEST(AlgoWaterVolume, TwoPass)
{
  using namespace algo_water_volume;

  auto func = water_volume_1; 
  {
    vector<int> coll{2,5,1,2,3,4,7,7,6};
    EXPECT_THAT(func(coll), 10);
  }
  {
    vector<int> coll{2,5,1,3,1,2,1,7,7,6};
    EXPECT_THAT(func(coll), 17);
  }
  {
    vector<int> coll{2,5,4,3,4,7,6,5,4,5,7,9,5,4,3,4,5,6};
    EXPECT_THAT(func(coll), 21);
  }
}

TEST(AlgoWaterVolume, OnePass)
{
  using namespace algo_water_volume;

  auto func = water_volume_2; 
  {
    vector<int> coll{2,5,1,2,3,4,7,7,6};
    EXPECT_THAT(func(coll), 10);
  }
  {
    vector<int> coll{2,5,1,3,1,2,1,7,7,6};
    EXPECT_THAT(func(coll), 17);
  }
  {
    vector<int> coll{2,5,4,3,4,7,6,5,4,5,7,9,5,4,3,4,5,6};
    EXPECT_THAT(func(coll), 21);
  }
}


// ={=========================================================================
// algo-frog-jump

// this is not O(1)
int frog_jump_0622(int X, int Y, int D)
{
  int count{};
  for (long long i = X; i < Y; i += D)
    ++count;

  return count;
}

int frog_jump_0622_model(int X, int Y, int D)
{
  int quotient = (Y-X)/D;
  int remainder = (Y-X)%D;

  return remainder ? quotient+1 : quotient;
}

TEST(AlgoFrogJump, 0622)
{
  EXPECT_THAT(frog_jump_0622(10, 85, 30), 3);
  EXPECT_THAT(frog_jump_0622(10, 10, 30), 0);
  EXPECT_THAT(frog_jump_0622_model(10, 85, 30), 3);
  EXPECT_THAT(frog_jump_0622_model(10, 10, 30), 0);
}

// Have missed the condition which is O(1) and unit test cases. How to solve?

int frog_jump_old( int X, int Y, int D )
{
  int jump = -1;

  if( X == Y )
  {
    jump = 0;
  }
  else if (X < Y)
  {
    int diff = Y-X;

    if( (diff / D)  == 0)
    {
      jump = 1;
    }
    else if( ((diff/D) > 0) && ((diff%D) == 0) )
    {
      jump = diff/D;
    }
    else if( ((diff/D) > 0) && ((diff%D) != 0) )
    {
      jump = (diff/D)+1;
    }
  }

  return jump;
}

// score: 100 of 100. Detected time complexity:O(1)
// 
// X==Y : no jump
// X<Y  : ----------------------
//         X         Y   D         
//         
//    (Y-X)/D == 0. needs one jump.
//    (Y-X)/D > 0. needs more jump.
//       -----------------------
//         X         Y
//              D   D
//       (Y-X)%D == 0. fall exactly on Y.
//       (Y-X)%D != 0. +1 jump.
// 
// Lesson learned. Read the question carefully such as 'greater or equal', 'X <=
// Y', and O(1).

// There are three cases:
// 
//                   Y    Y   Y
// -------------- | ----- | ----- | ---------------------- 
//                       jumps == X + D*jump;

int frog_jump_2014_nov( int X, int Y, int D )
{
  if( X>Y || D==0 ) return -1;

  int jumps = (Y-X)/D;

  // Y >  X + jumps
  if( Y > (X + D*jumps) )
    return jumps+1;
  // Y <= X + jumps; covers when X == Y
  else
    return jumps;
}

int frog_jump_2014_dec( int X, int Y, int D )
{
  if( X>Y || D==0 ) return -1;

  int diff = (Y-X);
  int jump = diff/D;

  if( (diff % D) == 0 )
    return jump;
  else
    return jump+1;
}


// ={=========================================================================
// algo-minmax

namespace algo_min_max {

  bool AbsLess(int elem1, int elem2) {
    return abs(elem1) < abs(elem2);
  }

  using ITERATOR = std::deque<int>::iterator;

  pair<ITERATOR, ITERATOR> my_minmax(ITERATOR begin, ITERATOR end)
  {
    auto min = numeric_limits<int>::max();
    auto max = numeric_limits<int>::min();

    ITERATOR min_iter = begin;
    ITERATOR max_iter = begin;

    for(; begin != end; ++begin) {

      if (*begin < min) {
        min = *begin;
        min_iter = begin;
      }

      // add '=' to support the last max as minmax_element()
      if (max <= *begin) {
        max = *begin;
        max_iter = begin;
      }
    }

    return make_pair(min_iter, max_iter);
  }
}

TEST(AlgoMinMax, Stl)
{
  using namespace algo_min_max;

  deque<int> coll{2, 3, 4, 5, 6, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6};

  // If more than one minimum or maximum element exists, min_element() and
  // max_element() return `the first` found; minmax_element() returns the first
  // minimum but the last maximum element, so max_element() and minmax_element()
  // don’t yield the same maximum element.

  EXPECT_THAT(*min_element(coll.begin(), coll.end()), -3);

  EXPECT_THAT(*max_element(coll.begin(), coll.end()), 6);
  EXPECT_THAT(distance(coll.begin(),max_element(coll.begin(), coll.end())), 4);
 
  // return iterator pair
  // Note also that minmax_element() yields `the last maximum`, so the distance
  // 9.
  auto minmax = minmax_element(coll.begin(), coll.end());
  EXPECT_THAT(*(minmax.first), -3);   // first minimum
  EXPECT_THAT(*(minmax.second), 6);   // last maximum

  // last maximum is 6 which is the last element
  EXPECT_THAT(distance(coll.begin(), minmax.second), coll.size()-1);

  EXPECT_THAT(distance(minmax.first, minmax.second), 9);
  EXPECT_THAT(distance(
        min_element(coll.begin(), coll.end()),
        max_element(coll.begin(), coll.end()))
      , -1);

  // min/max of absolute values
  EXPECT_THAT(*min_element(coll.begin(), coll.end(), AbsLess), 0);
  EXPECT_THAT(*max_element(coll.begin(), coll.end(), AbsLess), 6);
}

namespace algo_min_max
{
  struct _Iter_less
  {
    template<typename _Iterator1, typename _Iterator2>
      bool operator()(_Iterator1 __it1, _Iterator2 __it2) const
      { return *__it1 < *__it2; }
  };

  template <typename _Iterator, typename _Compare>
    _Iterator my_max_element(_Iterator __first, _Iterator __last, 
        _Compare __comp)
    {
      // if thre is only one
      if (__first == __last)
        return __first;

      _Iterator __result = __first;

      // if *__result < *__first 
      while (++__first != __last)
        if (__comp(__result, __first))
          __result = __first;

      return __result;
    }

  // note: do by simply reversing comp()

  template <typename _Iterator, typename _Compare>
    _Iterator my_min_element(_Iterator __first, _Iterator __last,
        _Compare __comp)
    {
      if (__first == __last)
        return __first;

      _Iterator __result = __first;

      while (++__first != __last)
        // if (comp(__result, __first))
        if (__comp(__first, __result))
          __result = __first;

      return __result;
    }

} // namespace

TEST(AlgoMinMax, UseOwn)
{
  using namespace algo_min_max;

  {
    deque<int> coll{2, 3, 4, 5, 6, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6};

    // return iterator pair
    // Note also that minmax_element() yields `the last maximum`, so the distance
    // 9.
    auto minmax = my_minmax(coll.begin(), coll.end());
    EXPECT_THAT(*(minmax.first), -3);
    EXPECT_THAT(*(minmax.second), 6);
    EXPECT_THAT(distance(minmax.first, minmax.second), 9);
  }

  // on map
  {
    // sorted by key
    std::map<int, size_t> counts{
      {1, 2},
        {3, 2},
        {5, 3},
        {8, 3},
        {13, 1} 
    };

    auto e = max_element(counts.begin(), counts.end());
    EXPECT_THAT(*e, make_pair(13, 1));

    // ForwardIterator
    // max_element (ForwardIterator beg, ForwardIterator end, CompFunc op)
    // op is used to compare two elements:
    // op(elem1,elem2)
    // It should return true when the first element is less than the second
    // element.

    auto maxelem = std::max_element(
        std::begin(counts), std::end(counts),
        [](pair<int, size_t> const& e1, pair<int, size_t> const& e2)
        { return e1.second < e2.second; });

    EXPECT_THAT(*maxelem, make_pair(5, 3));
  }

  // multimap
  {
    // sorted by key and the order in the equal range are the order of input
    std::multimap<int, size_t> counts{
      {1, 2},
        {3, 9},
        {3, 8},
        {5, 3},
        {8, 3},
        {13, 2},
        {13, 4},
        {13, 12},
        {13, 1}
    };

    // for (auto &e : counts)
    //   cout << e.first << ", " << e.second << endl;

    // Q: how max_element() finds the max on the second?
    // see *cxx-pair-comparison*

    auto e = max_element(counts.begin(), counts.end());
    EXPECT_THAT(*e, make_pair(13, 12));
  }

  // max_element
  {
    // sorted by key
    std::map<int, size_t> counts{
      {1, 2},
        {3, 2},
        {5, 3},
        {8, 3},
        {13, 1} 
    };

    auto pos = my_max_element(counts.begin(), counts.end(),
        _Iter_less());

    EXPECT_THAT(*pos, make_pair(13, 1));
  }

  // min_element
  {
    // sorted by key
    std::map<int, size_t> counts{
      {1, 2},
        {3, 2},
        {5, 3},
        {8, 3},
        {13, 1} 
    };

    auto pos = my_min_element(counts.begin(), counts.end(),
        _Iter_less());

    EXPECT_THAT(*pos, make_pair(1, 2));
  }
}


// ={=========================================================================
// algo-find-missing

int find_missing_0623(const vector<int> &A)
{
  int N = A.size();
  int total_sum{};
  for (int i = 1; i <= N+1; ++i)
    total_sum += i;

  int local_sum{};
  for (auto e : A)
    local_sum += e;

  // cout << "total: " << total_sum << ", local: " << local_sum << endl;
  return total_sum - local_sum;
}

TEST(AlgoFindMissing, 0623)
{
  EXPECT_THAT(find_missing_0623({2,3,1,5}), 4);
  EXPECT_THAT(find_missing_0623({1,2,3,4}), 5);
  EXPECT_THAT(find_missing_0623({2,3,4,5}), 1);
  EXPECT_THAT(find_missing_0623({1,3,4,5}), 2);
  EXPECT_THAT(find_missing_0623({}), 1);
}

// fails when N=0.
//
// score: 90 of 100
// Detected time complexity: O(N)
//
// empty list and single element 	0.020 s. 	WRONG ANSWER got 0 expected 1
//
// This is about permutation. For example, {1,2,3,4,5} can have
// 
// {1,2,3,4} is missing 5
// {2,3,4,5} is missing 1
// {1,3,4,5} is missing 2
// 
// Reversely, 
// if N==3 inputs are given, then it's one of permutation of 4. [1,4]
// if N==2 inputs are given, then it's one of permutation of 3. [1,3]
// if N==1 inputs are given, then it's one of permutation of 2. [1,2]
// if N==0 inputs are given, then it's one of permutation of 1. [1] so the
// missing is always 1.


int find_missing_old(const vector<int> &A) 
{
  // write your code in C++98
  if( !A.size() )
    return 0;

  long long isum = 0;

  for( unsigned int i = 0; i < A.size(); i++ )
    isum += A[i];

  long long csum = 0;

  for( unsigned int i = 1; i <= A.size()+1; i++ )
    csum += i;

  return csum - isum;
}

TEST(AlgoFindMissing, find_missing_old)
{
  EXPECT_THAT(find_missing_old({2,3,1,5}), 4);
  EXPECT_THAT(find_missing_old({1,2,3,4}), 5);
  EXPECT_THAT(find_missing_old({2,3,4,5}), 1);
  EXPECT_THAT(find_missing_old({1,3,4,5}), 2);
  // fails
  // EXPECT_THAT(find_missing_old({}), 1);
}

int find_missing_old_fix(const vector<int> &A) 
{
  // write your code in C++98
  long long isum = 0;

  for( unsigned int i = 0; i < A.size(); i++ )
    isum += A[i];

  long long csum = 0;

  for( unsigned int i = 1; i <= A.size()+1; i++ )
    csum += i;

  return csum - isum;
}

TEST(AlgoFindMissing, find_missing_old_fix)
{
  EXPECT_THAT(find_missing_old_fix({2,3,1,5}), 4);
  EXPECT_THAT(find_missing_old_fix({1,2,3,4}), 5);
  EXPECT_THAT(find_missing_old_fix({2,3,4,5}), 1);
  EXPECT_THAT(find_missing_old_fix({1,3,4,5}), 2);
  EXPECT_THAT(find_missing_old_fix({}), 1);
}

int find_missing_old_two(const vector<int> &A) 
{
  // write your code in C++98
  long long isum = 0;

  for( unsigned int i = 0; i < A.size(); i++ )
    isum += A[i];

  // use the fact that sum{1..N} is N(N+1)/2 and take cauiton on integer
  // division. so not n*((n+1)/2)

  int n = A.size()+1;
  long long csum = (n*(n+1))/2;

  return csum - isum;
}

TEST(AlgoFindMissing, find_missing_old_two)
{
  EXPECT_THAT(find_missing_old_two({2,3,1,5}), 4);
  EXPECT_THAT(find_missing_old_two({1,2,3,4}), 5);
  EXPECT_THAT(find_missing_old_two({2,3,4,5}), 1);
  EXPECT_THAT(find_missing_old_two({1,3,4,5}), 2);
  EXPECT_THAT(find_missing_old_two({}), 1);
}


// ={=========================================================================
// algo-find-perm

// 2018.06.25
int find_perm_0625(const vector<int> &A)
{
  //  int N = A.size();
  int total_sum{};
  int input_max{};

  for (auto e : A)
  {
    if (e > input_max)
      input_max = e;

    total_sum += e;
  }

  int perm_sum = (input_max*(input_max+1))/2;

  cout << "total: " << total_sum << ", perm: " << perm_sum << endl;

  return total_sum == perm_sum;
}

TEST(AlgoFindPerm, 0625)
{
  EXPECT_THAT(find_perm_0625({4,1,3,2,1}), 0);
  EXPECT_THAT(find_perm_0625({1,4,1}), 0);
  // fails
  // EXPECT_THAT(find_perm_0625({9,5,7,3,2,7,3,1,10,8}),0);
}

// based on old tries. N's permutation and it downs to algo-unique in the end so
// if don't need to be defensive about input value, can return false as soon as
// see duplicates. 
//
// fails on:
// extreme_min_max 
// single element with minimal/maximal value
// large_range 
// sequence 1, 2, ..., N, N = ~100,000

int find_perm_0625_02(const vector<int> &A)
{
  int count{};
  int input_max{};

  vector<bool> lookup(A.size()+1);

  for (auto e : A)
  {
    if (e > input_max)
      input_max = e;

    if (lookup[e] == true)
      return false;
    else
    {
      lookup[e] = true;
      ++count;
    } 
  }

  // size_t perm_sum = (input_max*(input_max+1))/2;

  cout << "input_max: " << input_max << ", perm: " << count << endl;

  return count == input_max;
}

// so keys:
// no duplicate
// N, input max is A.size()
// all values <= N, input max
//
// 100% pass
// Detected time complexity: O(N * log(N)) or O(N)

int find_perm_0625_03(const vector<int> &A)
{
  int count{};
  int input_max = A.size();

  vector<bool> lookup(input_max+1);

  for (auto e : A)
  {
    if (e > input_max || lookup[e] == true)
      return false;
    else
    {
      lookup[e] = true;
      ++count;
    } 
  }

  // size_t perm_sum = (input_max*(input_max+1))/2;

  cout << "input_max: " << input_max << ", perm: " << count << endl;

  return count == input_max;
}

TEST(AlgoFindPerm, 0625_02)
{
  EXPECT_THAT(find_perm_0625_03({4,1,3,2,1}), 0);
  EXPECT_THAT(find_perm_0625_03({1,4,1}), 0);
  EXPECT_THAT(find_perm_0625_03({9,5,7,3,2,7,3,1,10,8}),0);
}


// nov 2014. both are O(n)
// 1. To get N, find the max value in the input and the sum of input in a single loop
// 2. If the sum is different from sum[1,N] then return false. 
// 3. If N is different from input size then return false.

// 80% pass
int find_perm_old_01( vector<int> &A )
{
  int max = 0;

  for( unsigned int i=0; i < A.size(); i++ )
  {
    if( max < A[i] )
      max = A[i];
  }

  return max == (int)A.size();
}


// <key> The problem is to understand question which is confusing. The question
// is that N is the size of array and also is the N for permutation. That is
// there shall be [1,N] elements in the input. If not, not a permutation. This
// becomes bit set problem to see if all elements are seen.

int find_perm_old_02( vector<int> &A )
{
  if( !A.size() )
    return 0;

  // default is false
  vector<bool> flag( A.size() );

  for( unsigned int i=0; i < A.size(); i++ )
  {
    // note: -1 since permutation starts from 1 but index starts from 0
    // note: 'unsigned' to handle possible negative input which will be caught
    // below if statement.

    unsigned int value = A[i]-1;

    // note: this covers values which are not in [1, N]
    if( value < A.size() )
      flag[value] = true;
    else
      return 0;
  }

  // note: if it is permutation then there is no flase in flag set
  return count( flag.cbegin(), flag.cend(), false ) == 0;
}

// The count() in the return which is a loop can be removed as below since can
// return 0 as soon as duplucates.

// 100% pass
// Detected time complexity: O(N * log(N)) or O(N)

int find_perm_old_03( vector<int> &A )
{
  if( !A.size() )
    return 0;

  // default is false
  vector<bool> flag( A.size() );

  for( unsigned int i=0; i < A.size(); i++ )
  {
    // note: -1 since permutation starts from 1 but index starts from 0
    // note: 'unsigned' to handle possible negative input which will be caught
    // below if statement.
    
    unsigned int value = A[i]-1;

    // note: this covers values which are not in [1, N]
    if( value < A.size() && !flag[value])
      flag[value] = true;
    else
      return 0;
  }

  return 1;
}


// ={=========================================================================
// algo-frog-river

// 2018.06.26
int find_frog_river_0626(int X, const vector<int> &A)
{
  // input check
  if (A.empty())
    return -1;

  vector<bool> lookup(A.size());
  int target_sum = (X*(X+1))/2;
  int input_sum{};

  for (size_t i = 0; i < A.size(); ++i)
  {
    if (lookup[A[i]] == false)
    {
      lookup[A[i]] = true;
      input_sum += A[i];

      if (target_sum == input_sum)
        return i;
    }
  }

  return -1;
}

TEST(AlgoFrogRiver, 0626_01)
{
  EXPECT_THAT(find_frog_river_0626(5, {1,3,1,4,2,3,5,4}), 6);
  // fails
  // EXPECT_THAT(find_frog_river_0626(1, {2,3,4,5,1,3,5,4}), 4);
}

int find_frog_river_0626_02(int X, const vector<int> &A)
{
  // input check
  if (A.empty())
    return -1;

  vector<bool> lookup(A.size());
  int target_sum = (X*(X+1))/2;
  int input_sum{};

  for (size_t i = 0; i < A.size(); ++i)
  {
    if (A[i] <= X && lookup[A[i]] == false)
    {
      lookup[A[i]] = true;
      input_sum += A[i];

      if (target_sum == input_sum)
        return i;
    }
  }

  return -1;
}

// although this version passed, it's wrong in size of lookup table.
TEST(AlgoFrogRiver, 0626_02)
{
  EXPECT_THAT(find_frog_river_0626_02(5, {1,3,1,4,2,3,5,4}), 6);
  EXPECT_THAT(find_frog_river_0626_02(1, {2,3,4,5,1,3,5,4}), 4);
  EXPECT_THAT(find_frog_river_0626_02(5, {}), -1);

  // fails
  // /usr/include/c++/4.9/debug/vector:357:error: attempt to subscript container 
  //     with out-of-bounds index 1, but container only holds 1 elements.

  // Objects involved in the operation:
  // sequence "this" @ 0x0x7fff25cb8fd0 {
  //   type = NSt7__debug6vectorIbSaIbEEE;
  // }
  // Aborted
  //
  // EXPECT_THAT(find_frog_river_0626_02(5, {1}), -1);
}

int find_frog_river_0626_03(int X, const vector<int> &A)
{
  // input check
  if (A.empty())
    return -1;

  vector<bool> lookup(A.size()+1);
  int target_sum = (X*(X+1))/2;
  int input_sum{};

  for (size_t i = 0; i < A.size(); ++i)
  {
    if (A[i] <= X && lookup[A[i]] == false)
    {
      lookup[A[i]] = true;
      input_sum += A[i];

      if (target_sum == input_sum)
        return i;
    }
  }

  return -1;
}

TEST(AlgoFrogRiver, 0626_03)
{
  EXPECT_THAT(find_frog_river_0626_03(5, {1,3,1,4,2,3,5,4}), 6);
  EXPECT_THAT(find_frog_river_0626_03(1, {2,3,4,5,1,3,5,4}), 4);
  EXPECT_THAT(find_frog_river_0626_03(5, {}), -1);
  EXPECT_THAT(find_frog_river_0626_03(5, {1}), -1);
  EXPECT_THAT(find_frog_river_0626_03(1, {2}), -1);
  EXPECT_THAT(find_frog_river_0626_03(1, {1}), 0);
}


int find_frog_river_old_01( int X, std::vector<int> &A )
{
  if( A.empty() || X==1 )
    return -1;

  bool *pbitset = new bool(X);
 
  int idx;                        
  int count=0;

  // bitset{0, X-1}
  for(idx=0; idx < X; idx++)
    pbitset[idx] = false;

  for(idx=0; idx < (int)A.size(); idx++)    // signed and unsigned warning.
  {
    // wasn't set before?
    if( pbitset[A[idx]-1] == false )
    {
      // set it and increase count
      pbitset[A[idx]-1] = true;
      count++;

      // are all position set?
      if( count == X )                 // signed and unsigned warning.
      {
        delete pbitset; return idx;
      }
    }
  }

  delete pbitset; return -1;
}

// Failed on 25%:
// 
// small_random1 3 random permutation, X = 50     0.020 s.     RUNTIME ERROR
// tested program terminated unexpectedly
//
// 1. signed and unsigned that complier warns mismatch between signed and
// unsigned.  No such error when run with GCC 4.6.3.
// 
// 2. this is wrong since it allocate a single bool but not array. Failed on
// other many test cases with the same error. But why no such error on GCC
// 4.6.3. This sites uses C++98 so may be new initialize way in C++11?


// Still failed with the same error.
int find_frog_river_old_02( int X, std::vector<int> &A )
{
  if( A.empty() || X==1 )
    return -1;

  bool *pbitset = new bool(X);
 
  int idx;                        
  int count=0;

  for(idx=0; idx < X; idx++)
    pbitset[idx] = false;

  for(idx=0; idx < (int)A.size(); idx++)
  {
    if( pbitset[A[idx]-1] == false )
    {
      pbitset[A[idx]-1] = true;
      count++;

      if( count == X )
      {
        delete[] pbitset; return idx;  // diff
      }
    }
  }

  delete[] pbitset; return -1;         // diff
}


int find_frog_river_old_03( int X, std::vector<int> &A )
{
  if( A.empty() || X==1 )
    return -1;

  bool *pbitset = new bool[X];   // diff
 
  int idx;                        
  int count=0;

  for(idx=0; idx < X; idx++)
    pbitset[idx] = false;

  for(idx=0; idx < (int)A.size(); idx++)
  {
    if( pbitset[A[idx]-1] == false )
    {
      pbitset[A[idx]-1] = true;
      count++;

      if( count == X )
      {
        delete[] pbitset; return idx;
      }
    }
  }

  delete[] pbitset; return -1;
}

// 90 out of 100 points. Detected time complexity: O(N). Failed on:
// 
// single single element     0.020 s.     WRONG ANSWER got -1 expected 0
// 
// note: think about when single element has big value(position)


int find_frog_river_old_04( int X, std::vector<int> &A )
{
  if( A.empty() || X==0 )           // diff
    return -1;

  bool *pbitset = new bool[X];
 
  int idx;                        
  int count=0;

  // bitset{0, X-1}
  for(idx=0; idx < X; idx++)
    pbitset[idx] = false;

  for(idx=0; idx < (int)A.size(); idx++)
  {
    // wasn't set before?
    if( (A[idx]-1 < X) && pbitset[A[idx]-1] == false )   // diff
    {
      // set it and increase count
      pbitset[A[idx]-1] = true;
      count++;

      // are all position set?
      if( count == X )
      {
        delete [] pbitset; return idx;
      }
    }
  }

  delete [] pbitset; return -1;
}

// 100 out of 100 points. Detected time complexity: O(N) 


int find_frog_river_old_05( int X, std::vector<int> &A )
{
  if( A.empty() || X==0 )           // diff
    return -1;

  bool *pbitset = new bool[X];
 
  int idx;                        
  int count=0;

  // bitset{0, X-1}
  for(idx=0; idx < X; idx++)
    pbitset[idx] = false;

  for(idx=0; idx < (int)A.size(); idx++)
  {
    int value = A[idx]-1;

    // wasn't set before?
    if( (value < X) && pbitset[value] == false )   // diff
    {
      // set it and increase count
      pbitset[value] = true;
      count++;

      // are all position set?
      if( count == X )
      {
        delete [] pbitset; return idx;
      }
    }
  }

  delete [] pbitset; return -1;
}

// The key idea is that it is about counting and to use counter to check if
// receives all inputs rather than using loops or function call like bitset.


int find_frog_river_old_06(int X, const vector<int> &A) {
  // write your code in C++11
  if( A.empty() || !X )
    return -1;

  vector<bool> flags(X);
  int count = 0;

  for(unsigned int i=0; i < A.size(); i++ )
  {
    int value = A[i]-1;

    if( value < X && flags[value] == false )
    {
      flags[value] = true;
      count++;
    }

    if( count == X )
      return i;
  }

  return -1;
}

// Detected time complexity: O(N)

TEST(AlgoFrogRiver, OldAttempts)
{
  EXPECT_THAT(find_frog_river_old_06(5, {1,3,1,4,2,3,5,4}), 6);
  EXPECT_THAT(find_frog_river_old_06(1, {2,3,4,5,1,3,5,4}), 4);
  EXPECT_THAT(find_frog_river_old_06(5, {}), -1);
  EXPECT_THAT(find_frog_river_old_06(5, {1}), -1);
  EXPECT_THAT(find_frog_river_old_06(1, {2}), -1);
  EXPECT_THAT(find_frog_river_old_06(1, {1}), 0);
}


// ={=========================================================================
// algo-find-missing-integer

// 2018.06.27
// 1. do not say about return value for errors
//
// 2. Allocate lookup table for 2,147,xxx,xxx x 2? No and not meet O(n) space as
// well. 
//  
// Since it's about `positive` minimal integer, inputs are 1..2,147,xxx,xxx.
// Since N size array could have the max input value which is N.
//
// So can reduce input value range to 1...N
//
// What if input is not sequential such as "{100, 200, 300, 340}"? Not a valid
// input since we are looking for `missing`, `not occurring` element. If that's
// valid input then what are the missing element? So many and not a valid input.


int find_missing_integer(const vector<int> &A)
{
  // input check
  if (A.empty())
    return -1;

  vector<bool> lookup(A.size());

  for (size_t i = 0; i < A.size(); ++i)
  {
    if (A[i] > 0 && lookup[A[i]-1] == false)
      lookup[A[i]-1] = true;
  }

  for (size_t i = 0; i < lookup.size(); ++i)
    if (lookup[i] == false)
      return i+1;

  return -1;
}

TEST(AlgoFindMissingInteger, 0627_01)
{
  EXPECT_THAT(find_missing_integer({1,3,6,4,1,2}), 5);
}


// O(N), 100%
//
// Use bool vector approach? The input element can be negative so ignore
// negegative inputs.  However, the problem is input value can be too big to
// have bool vector. how to solve?
// 
// The key is whatever the input value is the aim to find the minimum positive
// value which is missed. So have bool vector(N) and only consider inputs in 0 <
// x <= N. Since even if there is no input in the specificed range then it
// simply means that it misses the whole value of the range and need to get the
// first false in the bool vector. 
//
// If bool vector has all set then return N+1. ????

int find_missing_integer_old(const vector<int> &A)
{
  vector<bool> flags(A.size());

  for(unsigned int i=0; i < A.size(); i++)
  {
    int value = A[i];

    if( value > 0 && value <= (int)A.size() )
      flags[value-1] = true;
  }

  for(unsigned int i=0; i < flags.size(); i++)
    if( flags[i] == false )
      return i+1;

  return A.size()+1;
}

TEST(AlgoFindMissingInteger, OldTries)
{
  EXPECT_THAT(find_missing_integer_old({1,3,6,4,1,2}), 5);
}


// ={=========================================================================
// algo-max-counters

// 2018.06.27
// 
// A[M] array, N counters
// A[k], 1 =< A[k] =< N+1, 
//  if A[k] =< N, increase(A[k]). if A[k] == N+1, max_counter
// 1 =< N, M =< 100,000

vector<int> find_max_counters_0627(int N, vector<int> A)
{
  vector<int> result(N, 0);
  int max{};

  for(size_t i = 0; i < A.size(); ++i)
  {
    if (A[i] == N+1)
    {
      // fill_n(result, N, max);
      for(auto &e : result)
        e = max;
    }
    else if (A[i] >= 1 && A[i] <= N)
    {
      if(++result[A[i]-1] > max)
        max = result[A[i]-1];
    }
  }

  return result;
}

TEST(AlgoMaxCounters, 0627_01)
{
  EXPECT_THAT(find_max_counters_0627(5, {3,4,4,6,1,4,4}), 
      ElementsAre(3,2,2,4,2));
}


// when simply follows descriptions:
//
// The result is that 100% correctness and 40% performance.
//
// Therefore, can see that the problem is the max-all operation and as a worst
// case, when there are N max-all operations this will be O(N*M) but target is
// O(M+N). So the key is to find a way to have max-all effect without doing a
// loop. How?

vector<int> find_max_counters_old_01(int N, const vector<int>& A)
{
  vector<int> counters(N, 0);

  int current_max = 0;

  for( size_t i=0; i < A.size(); i++ )
  {
    // set current max to all
    if( A[i] >= N+1 )
    {
      for( size_t j=0; j < counters.size(); j++ )
        if( counters[j] > current_max )
          current_max = counters[j];

      for( size_t j=0; j < counters.size(); j++ )
        counters[j] = current_max;
    }
    // increment a counter
    else
      counters[A[i]-1] += 1;
  }

  return counters;
}

TEST(AlgoMaxCounters, OldTries)
{
  EXPECT_THAT(find_max_counters_old_01(5, {3,4,4,6,1,4,4}), 
      ElementsAre(3,2,2,4,2));
}

// The above has time O(N*M) for worst cases such as when input has all 6, max
// operations. So key is not to loop on N counter array for a max operation.
//
// The observation shows that max-all op is performance bottleneck.
//
// How to solve?
//
// {3,4,4,6,1,4,4}
//
// 0   0 0 0 0
// 0   0 1 0 0
// 0   0 1 1 0
// 0   0 1 2 0
// -   - - - - take a snapshot of current max rather than run max op
// 2+1 0 1 2 0
// 3   0 1 3 0
// 3   0 1 4 0
// then do lopp and update elements that are less than snapshot value so that
// avoid run loop every max op.

vector<int> find_max_counters_0627_02(int N, vector<int> A)
{
  vector<int> result(N, 0);
  int current_max{}, operation_max{};

  for (size_t i = 0; i < A.size(); ++i)
  {
    if (A[i] == N+1)
    {
      operation_max = current_max;
    }
    else if (A[i] >= 1 && A[i] <= N)
    {
      if (result[A[i]-1] < operation_max)
        result[A[i]-1] = operation_max + 1;
      else 
        result[A[i]-1] += 1;

      if(result[A[i]-1] > current_max)
        current_max = result[A[i]-1];
    }
  }

  for (auto &e : result)
  {
    if (e < operation_max)
      e = operation_max;
  }

  return result;
}

TEST(AlgoMaxCounters, 0627_02)
{
  EXPECT_THAT(find_max_counters_0627_02(5, {3,4,4,6,1,4,4}), 
      ElementsAre(3,2,2,4,2));
  EXPECT_THAT(find_max_counters_0627_02(5, {3,4,4,6,1,4,6}), 
      ElementsAre(3,3,3,3,3));
  EXPECT_THAT(find_max_counters_0627_02(5, {3,6,6,6,6,6,6}), 
      ElementsAre(1,1,1,1,1));
}


// solution from online.
// http://codility-lessons.blogspot.co.uk/2014/07/lesson-2maxcounters.html
// 
//     (0, 0, 1, 0, 0)        (0, 0, 1, 0, 0)
//     (0, 0, 1, 1, 0)        (0, 0, 1, 1, 0)
//     (0, 0, 1, 2, 0)        (0, 0, 1, 2, 0)
//     (2, 2, 2, 2, 2)        (-, -, -, -, -)   max=2, maxLastMaxOp = 2
//     (3, 2, 2, 2, 2)        (3, 0, 1, 2, 0)
//     (3, 2, 2, 3, 2)        (3, 0, 1, 3, 0)
//     (3, 2, 2, 4, 2)        (3, 0, 1, 4, 0)
//                            (3, 2, 2, 4, 2)   set maxLastMaxOp to all which are not increased since
//                            last max-all operation.
// 
// This approach use flags to know which is increased since the last max-all
// operation and set maxLastMaxOp to all which are not increased since the last
// max-all.
// 
// The key 'observation' is that max-all sets the 'base' for following increase
// operations. 
//
// This approach still however didn't meet performance goal. 88%. WHY? since do
// memset on every max op.


// comment out due to compile erros on Result struct
// struct Results find_max_counters_old_02(int N, int A[], int M) 
// {
//   struct Results result;
//   result.C = calloc(sizeof(int), N);
//   result.L = N;
// 
//   int* flg = alloca(sizeof(int) * N);
//   memset(flg, 0x00, sizeof(int) * N);
// 
//   int max = 0;
//   int maxAtTheLastMaxCntOp = 0;
// 
//   int i;
//   for (i = 0; i < M; i++){
//     int op = A[i];
//     //if the op is max counter.
//     if (op == N + 1){
//       maxAtTheLastMaxCntOp = max;
//       memset(flg, 0x00, sizeof(int) * N);    
//     }
//     //if the op is increase(x)
//     else {
//       //op is beweetn 1 to N, but the index for the array C 
//       //is between 0 and (N-1). Decrease op to adjust it.
//       op--; 
//       if (flg[op] == 1){
//         result.C[op]++;
//       }
//       else {
//         result.C[op] = maxAtTheLastMaxCntOp + 1;
//         flg[op] = 1;                
//       }
// 
//       if (result.C[op] > max){
//         max = result.C[op];
//       }
//     }
//   }
// 
//   //apply the 'max counter' operation
//   //to the slot(s) where it should be applied. 
//   int j;  
//   for (j = 0; j < N; j++){
//     if (flg[j] == 0){
//       result.C[j] = maxAtTheLastMaxCntOp;
//     }
//   }
//   return result;
// }

// the final solution from online.
// 
// This approach removes the use of flags. As with the above observation,
// max-all set the base that means any following increase should be based on
// 'hidden' base. So if result[op] < maxLastMaxOp then result[op] =
// maxLastMaxOp+1. Once done a loop, handle all which are not increased since
// the last max-all by checking less than maxLastMaxOp. 
//
// Verified 100% peformance mark.

vector<int> find_max_counters_old_03(int N, vector<int> &A) 
{
  // write your code in C++11
  vector<int> result(N,0);

  int maxLast =0, maxCurrent = 0;

  for(unsigned int i = 0; i < A.size(); i++ )
  {
    int op = A[i];

    if( op == N+1 )   // max-all op
      maxLast = maxCurrent;
    else              // inc op
    {
      op--;

      if( result[op] < maxLast )
        result[op] = maxLast+1;
      else
        result[op]++;

      if( result[op] > maxCurrent )
        maxCurrent = result[op];
    }
  }

  for( int i =0; i < N; i++ )
    if( result[i] < maxLast )
      result[i] = maxLast;

  return result;
}


// ={=========================================================================
// algo-perfix-sum

// counting prefix sums, O(n)
vector<int> make_prefix_sums(const vector<int> &A)
{
  // +1 since prefix sum has one more
  size_t size = A.size()+1;

  vector<int> prefix_sum(size);

  for (size_t i = 1; i < size; ++i)
    prefix_sum[i] = prefix_sum[i-1] + A[i-1];

  return prefix_sum;
}

TEST(AlgoPrefixSum, MakePrefixSum)
{
  EXPECT_THAT(make_prefix_sums({1,2,3,4,5,6}),
      ElementsAre(0,1,3,6,10,15,21));

  EXPECT_THAT(make_prefix_sums({2,3,7,5,1,3,9}),
      ElementsAre(0,2,5,12,17,18,21,30));
}

// Using prefix (or suffix) sums allows us to calculate the total of any slice
// of the array very quickly. For example, assume that you are asked about the
// totals of m slices [x..y] such that 0 <= x <= y < n, where the total is the
// sum ax + ax+1 + . . . + ay−1 + ay.

int count_total(const vector<int> &P, int x, int y)
{
  // since prefix sum index is +1 more then input index.
  return P[y+1] - P[x];
}

TEST(AlgoPrefixSum, SumAnySlice)
{
  //   {1,2, 3, 4, 5, 6}
  //         2     4
  // (0,1,3,[6,10,15],21)
  //
  // 15-3 = 12

  EXPECT_THAT(count_total(make_prefix_sums({1,2,3,4,5,6}), 2, 4), 12);

  // (0,2,5,[12,17,18],21,30)
  // 18-5 = 13

  EXPECT_THAT(count_total(make_prefix_sums({2,3,7,5,1,3,9}), 2, 4), 13);
}

// start: 4, moves: 6
// loop(0, 5), left:  4, shift:  6, right: 10 -> 10 ->  6, xresult: 13, result: 13
// loop(1, 5), left:  3, shift:  4, right:  8 ->  8 ->  6, xresult: 18, result: 18
// loop(2, 5), left:  2, shift:  2, right:  6 ->  6 ->  6, xresult: 25, result: 25
// loop(3, 5), left:  1, shift:  0, right:  4 ->  4 ->  4, xresult: 16, result: 25
// loop(4, 5), left:  0, shift: -2, right:  2 ->  4 ->  4, xresult: 18, result: 25
// 
// when move window to the left
//          0    1   2   3   4   5   6   7   8   9   10
//          2    3   7   5  [1   3   9   X   X   X   X]
//          2    3   7  [5   1   3   9   X   X]  X   X
//          2    3  [7   5   1   3   9]  X   X   X   X
//          2   [3   7   5   1]  3   9   X   X   X   X
//         [2    3   7   5   1]  3   9   X   X   X   X
//
// when move window to the right
// [X   X   2    3   7   5   1]  3   9   X   X   X   X
//     [X   2    3   7   5   1   3]  9   X   X   X   X
//          2    3  [7   5   1   3   9]  X   X   X   X
//
// when used a bug
// loop(0, 2), right:  4, shift:  6, left: -2 -> -2 ->  0, xresult: 18, result: 25
// loop(1, 2), right:  5, shift:  4, left:  0 ->  0 ->  0, xresult: 21, result: 25
//
// start: 4, moves: 6
// loop(0, 3), shift:  6, left: -2 -> -2 ->  0, right:  4, xresult: 18, result: 25
// loop(1, 3), shift:  4, left:  0 ->  0 ->  0, right:  5, xresult: 21, result: 25
// loop(2, 3), shift:  2, left:  2 ->  2 ->  2, right:  6, xresult: 25, result: 25
//
// from this observation, when shift to left to the start, right end get reduced
// by 2 since uses "move" twice when goes to left and right again.
// 
// After all, get possible max mushroom and moves windows which starts from
// start pos and ends with start pos.
//
// start: 8, moves: 5
// loop(0, 6), left:  8, shift:  5, right: 13 -> 13 -> 13, xresult: 21, result: 21
// loop(1, 6), left:  7, shift:  3, right: 11 -> 11 -> 11, xresult: 27, result: 27
// loop(2, 6), left:  6, shift:  1, right:  9 ->  9 ->  9, xresult: 23, result: 27
// loop(3, 6), left:  5, shift: -1, right:  7 ->  8 ->  8, xresult: 26, result: 27
// loop(4, 6), left:  4, shift: -3, right:  5 ->  8 ->  8, xresult: 30, result: 30
// loop(5, 6), left:  3, shift: -5, right:  3 ->  8 ->  8, xresult: 32, result: 32
//
//   0   1   2  3  4  5  6   7 *8* 9 10 11 12 13 14
//  13  12  11  2  4  6  8  10 [2  3  7  5  1  3] 9     (21)
//  13  12  11  2  4  6  8 [10  2  3  7  5] 1  3  9     (27)
//  13  12  11  2  4  6 [8  10  2  3] 7  5  1  3  9     (23)
//  13  12  11  2  4 [6  8  10  2] 3  7  5  1  3  9     (26)
//  13  12  11  2 [4  6  8  10  2] 3  7  5  1  3  9     (30)
//  13  12  11 [2  4  6  8  10  2] 3  7  5  1  3  9     (32)
//  ...
//
// start: 8, moves: 5
// loop(0, 6), shift:  5, left:  3 ->  3 ->  3, right:  8, xresult: 32, result: 32
// loop(1, 6), shift:  3, left:  5 ->  5 ->  5, right:  9, xresult: 29, result: 32
// loop(2, 6), shift:  1, left:  7 ->  7 ->  7, right: 10, xresult: 22, result: 32
// loop(3, 6), shift: -1, left:  9 ->  8 ->  8, right: 11, xresult: 17, result: 32
// loop(4, 6), shift: -3, left: 11 ->  8 ->  8, right: 12, xresult: 18, result: 32
// loop(5, 6), shift: -5, left: 13 ->  8 ->  8, right: 13, xresult: 21, result: 32

int mushroom_model(const vector<int> A, int start, int moves)
{
  int max_input_index = A.size()-1;
  int result{};
  auto prefix_sum = make_prefix_sums(A);
  int loop_max{};

  // cout << "start: " << start << ", moves: " << moves << endl;
  loop_max = min(start, moves) + 1;
  for (int prefix_index = 0; prefix_index < loop_max; ++prefix_index)
  {
    int left_pos = start - prefix_index;
    int shift_value = (moves-2*prefix_index);
    int possible_right_pos = start+shift_value;

    // max? since right end cannot be less than start
    int max_on_possible_right_pos = max(start, possible_right_pos);

    // min? since right end cannot be greater than max index of input.
    int right_pos = min(max_input_index, max_on_possible_right_pos);

    // collect mushroon only once? count_total use range and counts only once.
    auto xresult = count_total(prefix_sum, left_pos, right_pos);
    result = max(result, xresult);

    // cout << "loop(" << prefix_index << ", " << loop_max << ")" 
    //   << ", left: " << setw(2) << left_pos
    //   << ", shift: " << setw(2) << shift_value 
    //   << ", right: " << setw(2) << possible_right_pos << " -> " 
    //   << setw(2) << max_on_possible_right_pos << " -> " << setw(2) << right_pos 
    //   << ", xresult: " << xresult << ", result: " << result << endl;
  }

  // cout << "start: " << start << ", moves: " << moves << endl;

  // from codility code but looks wrong.
  // loop_max = min(moves+1, max_input_index-start);
  loop_max = min(moves, max_input_index-start)+1;
  for (int prefix_index = 0; prefix_index < loop_max; ++prefix_index)
  {
    int right_pos = start + prefix_index;
    // left_pos = max(0, min(K, K-(M-2*prefix_index)));
    int shift_value = (moves-2*prefix_index);
    int possible_left_pos = start-shift_value;

    // min? left end should be less than the start
    int min_on_possible_left_pos = min(start, possible_left_pos);

    // max? cannot not less than 0
    int left_pos = max(0, min_on_possible_left_pos);

    auto xresult = count_total(prefix_sum, left_pos, right_pos);
    result = max(result, xresult);

    // cout << "loop(" << prefix_index << ", " << loop_max << ")" 
    //   << ", shift: " << setw(2) << shift_value 
    //   << ", left: " << setw(2) << possible_left_pos << " -> " 
    //   << setw(2) << min_on_possible_left_pos << " -> " << setw(2) << left_pos
    //   << ", right: " << setw(2) << right_pos 
    //   << ", xresult: " << xresult << ", result: " << result << endl;
  }

  return result;
}

TEST(AlgoPrefixSum, MushroomPicker)
{
  EXPECT_THAT(mushroom_model({2,3,7,5,1,3,9}, 4, 6), 25);
  EXPECT_THAT(mushroom_model({13,12,11, 2, 4, 6, 8,10, 2, 3, 7, 5, 1, 3, 9}, 8, 5), 32);
}


int mushroom_0704(const vector<int> A, int start, int moves)
{
  int max_input_index = A.size()-1;
  int num_loop{};
  int result{};

  auto prefix_sum = make_prefix_sums(A);

  // moves a window to the left
  // since `start` is actually index, it's sure to have elements in [0,start] so
  // use it directly
  num_loop = min(start, moves)+1;
  for (int i = 0; i < num_loop; ++i)
  {
    int left_end = start - i;
    int shift_value = (moves - 2*i);
    int right_end = start + shift_value;

    // right_end should be in [start, max input index]
    int right_contained = min(max_input_index, right_end);
    int right_end_calculated = max(start, right_contained);

    int prefix_sum_result = count_total(prefix_sum, left_end, right_end_calculated);
    result = max(prefix_sum_result, result);
  }

  // moves a windows to the right
  // unlike `to the left` case, cannot use start and have to use max input
  // index.
  num_loop = min(moves, max_input_index-start)+1;
  for (int i = 0; i < num_loop; ++i)
  {
    int right_end = start + i;
    int shift_value = (moves - 2*i);
    int left_end = start - shift_value;

    // left_end should be in [0, start]
    int left_end_contained = max(0, left_end);
    int left_end_calculated = min(start, left_end_contained);

    int prefix_sum_result = count_total(prefix_sum, left_end_calculated, right_end);
    result = max(prefix_sum_result, result);
  }

  return result; 
}

TEST(AlgoPrefixSum, MushroomPicker_0704)
{
  EXPECT_THAT(mushroom_0704({2,3,7,5,1,3,9}, 4, 6), 25);
  EXPECT_THAT(mushroom_model({13,12,11, 2, 4, 6, 8,10, 2, 3, 7, 5, 1, 3, 9}, 8, 5), 32);
}


// ={=========================================================================
// algo-passing-car

// in sum, find (0, X) pair from the input. 
//
// if loop on input to find each (0, X) pair then cannot meet O(N)
//
// How to solve?
//
// the key is whenever see 1, it's counted more depending on preceding 0's
// number.
//
// {0, 1, 0, 1, 1}
//     *     *  *
//           *  *
// so sum is 5

int passing_car_0628_01(const vector<int> A)
{
  int pair_count{}, zero_count{};

  for(size_t i = 0; i < A.size(); ++i)
  {
    if (A[i] == 0)
      ++zero_count;
    else 
    {
      pair_count += zero_count;
    }
  }

  return pair_count;
}

TEST(AlgoPassingCar, 0628_01)
{
  EXPECT_THAT(passing_car_0628_01({0,1,0,1,1}), 5); 
}

// From http://codility-lessons.blogspot.co.uk/2014/07/lesson-3-passingcars.html.
// 
// The idea is that
// 
//    0 1 0 1 1
//    *------->
//        *--->

int passing_car_old_01( vector<int> &A )
{
  int count = 0, countEast = 0;

  for( int i=0; i < (int)A.size(); i++ )
  {
    if( A[i] == 0 )
      countEast++;
    else
    {
      count += countEast;

      if( count > 1000000000 )
        return -1;
    }
  }

  return count;
}


// ={=========================================================================
// algo-count-div

// Since time O(1), cannot use loop. 
//
// How to solve?
//
// when A is divisible by K:
//    B-S = diff. diff/K + 1 is the number of integers that can be divisible by
//    K. +1 since diff do not include A.
//
// 6,7,8,9,10,11,12, K=2
//
// 12-6=6. 6/2=3. 3+1=4
//
// when A is not diviaible by K:
//    cannot use loop either to find S(start value). so have to find the next
//    K*x element in the input. To do that, if A%K != 0, then S = (A/K + 1)*K.
//
//    B-S = diff. diff/K + 1.

// peformance 100%, correctness 50%
int count_div_0628_01(int A, int B, int K)
{
  int start{}, result{};

  if (A%K == 0)
    start = A;
  else
    start = (A/K+1)*K;

  return result = (B-start)/K + 1;
}

TEST(AlgoCountDiv, 0628_01)
{
  EXPECT_THAT(count_div_0628_01(6, 11, 2), 3); 
}


// failed from the report
// EXPECT_THAT(count_div_0628_01(1, 1, 11), 0); 
// 
// failed from the report
// EXPECT_THAT(count_div_0628_03(1, 1, 11), 0); 
// 
// fails since 0/K and 0%K are 0. WHY 1? Since 0 is still divisible.
// EXPECT_THAT(count_div_0628_03(0, 1, 11), 1); 
//
// why 1?
// EXPECT_THAT(count_div_0628_03(0, 0, 11), 1); 
// 
// fails
// EXPECT_THAT(count_div_0628_03(0, 14, 2), 8); 
//
// after all, missed to handle:
// 1. end case which is 0 on both A and B
// 2. 0/K and 0%K are 0.


// 100% pass
int count_div_0628_03(int A, int B, int K)
{
  int start{}, result{};

  if (A%K == 0)
    start = A;
  else
    start = (A/K+1)*K;

  if (B-start >= 0)
    result = (B-start)/K + 1;
  
  return result;
}

TEST(AlgoCountDiv, 0628_03)
{
  EXPECT_THAT(count_div_0628_03(6, 11, 2), 3); 
  EXPECT_THAT(count_div_0628_03(1, 1, 11), 0); 
  EXPECT_THAT(count_div_0628_03(0, 1, 11), 1); 
  EXPECT_THAT(count_div_0628_03(10, 10, 5), 1); 
  EXPECT_THAT(count_div_0628_03(10, 10, 7), 0); 
  EXPECT_THAT(count_div_0628_03(10, 10, 20), 0); 
  EXPECT_THAT(count_div_0628_03(0, 0, 11), 1); 
  EXPECT_THAT(count_div_0628_03(0, 14, 2), 8); 
}


// ={=========================================================================
// algo-count-identical-pairs

// {3,5,6,3,3,5}
//
// Why the answer is 4 but not 3? Use index and there are 4 pairs
//
// no mention of input value range and assumes 255, 0 <= A[x] <= 255
//
// the same as algo-passing_car

int count_identical_pairs_0629_01(const vector<int> &A)
{
  int count{};

  vector<pair<int, int>> map(256);

  for (auto e : A)
  {
    // not set before
    if (map[e].first == 0)
      map[e] = pair<int, int>(e, 0);

    // add count
    count += map[e].second;

    // update count
    map[e].second += 1;
  }

  return count;
}

TEST(AlgoCountIdenticalPairs, 0629_01)
{
  EXPECT_THAT(count_identical_pairs_0629_01({3,5,6,3,3,5}), 4); 
}


// ={=========================================================================
// algo-repairman

typedef long long lint;

int g_points[1000];
int g_weights[1000];

// the order of arg do not matter since it's abs. for example, abs(s-e) or abs(e-s)
#define ABS_DISTANCE(s,e) (abs(g_points[e] - g_points[s]))

lint sum_range(int start, int finish, lint start_distance = 0);
lint sum_range_with_start(int start, int begin, int end, lint start_distance =0);

// get weighted sum in (begin, end] from start offset. note that do not
// include start in its sum.
//
// * loop condition and distance math
// turns out have to have loop exit in the middle of loop as the orignial has.
// Other attemtps like these cause probblems:
//
// for (int i = begin + direction; i != end ; i += direction)
// cause to miss out the last in add so wrong result.
//
// for (int i = begin + direction; i != (end+1); i += direction)
// cause to seg fault when do the left direction, direction is -1. 
// 
// the proper is:
//
// for (int i = begin + direction; ; i += direction)
// {
//   start_distance += ABS_DISTANCE(i-direction, i);
//
//   if (i == end)
//    return weight_sum;
// }
//
// Why? In order to support both direction and not inclding the start, use
// clever way to do match. In particuler, the way `direction` is used.
//
// For example:
//
// sum_range(0,5)
//  not include start, 0.
//  direction = 1, i = 1, ABS(0,1), wi[1] 
//  direction = 1, i = 2, ABS(1,2), wi[2] 
//  direction = 1, i = 3, ABS(3,2), wi[3] 
//  direction = 1, i = 4, ABS(4,3), wi[4] 
//  direction = 1, i = 5, ABS(5,4), wi[5] 
//  exit
//
// sum_range(5,0)
//  not include start, 5.
//  direction = -1, i = 4, ABS(5,4), wi[4] 
//  direction = -1, i = 3, ABS(4,3), wi[3] 
//  direction = -1, i = 2, ABS(3,2), wi[2] 
//  direction = -1, i = 1, ABS(2,1), wi[1] 
//  direction = -1, i = 0, ABS(1,0), wi[0] 
//  exit
//
//
// running:
//
//  i   0    1    2     3     4     5
// wi   1    2    10    3     5     1
// -----+----+-----+-----+-----+-----+---
// xi   1    6    12    13    14    24
//      s--------------------------->   244
//      -----s
//      ---------------------------->   329
//      ----------s
//      ---------------------------->   246
//      ----------------s
//      ---------------------------->   196
//      ----------------------s
//      ---------------------------->   88
//      ...
//
// 86 is made of:
//
//                s-----*-----*
//      *----*----
//                            ------*
//
// where * denotes points used in sum.
//
//
// # DURATION     TID     FUNCTION
//             [   529] | search(0, 5) {
// 
//                          /* starts from 0 to 5 */
//                          /* sum(start, begin, end, dis); */
//    0.665 us [   529] |   sum(0, 5, 0, 5) = 244;
// 
//                          /* starts from 1 */
//                          /* sum(begin, end, dis); */
//             [   529] |   sum(1, 0, 5, 0) {
//    0.443 us [   529] |     sum(1, 0, 0, 0) = 5;
//    1.035 us [   529] |     sum(1, 5, 10, 5) = 329;
//    1.219 ms [   529] |   } = 334; /* sum */
// 
//                          /* starts from 2 */
//             [   529] |   sum(2, 0, 5, 0) {
//    0.514 us [   529] |     sum(2, 0, 0, 0) = 23;
//    0.499 us [   529] |     sum(2, 5, 22, 5) = 223;
//    1.225 ms [   529] |   } = 246; /* sum */
// 
//                          /* starts from 3 */
//             [   529] |   sum(3, 0, 5, 0) {
//    0.554 us [   529] |     sum(3, 0, 0, 0) = 36;
//    0.566 us [   529] |     sum(3, 5, 24, 5) = 160;
//    1.078 ms [   529] |   } = 196; /* sum */
// 
//                          /* update minimum sum and save start point */
// 
//                          /* starts from 4 */
//             [   529] |   sum(4, 0, 5, 0) {
//    0.493 us [   529] |     sum(4, 0, 0, 0) = 52;
//    0.431 us [   529] |     sum(4, 5, 26, 5) = 36;
//  190.644 us [   529] |   } = 88; /* sum */
// 
//                          /* update minimum sum and save start point */
// 
//                          /* search [0,5] ends and there are updates so search again [4,0] */
// 
//             [   529] |   search(4, 0) {
// 
//                          /* starts from 4 to 0 */
//    0.242 us [   529] |     sum(4, 0, 0, 0) = 52;
// 
//             [   529] |     sum(3, 4, 0, 0) {
//    0.202 us [   529] |       sum(3, 4, 0, 4) = 5;
//    0.394 us [   529] |       sum(3, 0, 2, 0) = 62;
//  280.063 us [   529] |     } = 67; /* sum */
// 
//             [   529] |     sum(2, 4, 0, 0) {
//    0.301 us [   529] |       sum(2, 4, 0, 4) = 13;
//    0.220 us [   529] |       sum(2, 0, 4, 0) = 35;
//   40.841 us [   529] |     } = 48; /* sum */
// 
//                          /* update minimum sum and save start point */
// 
//             [   529] |     sum(1, 4, 0, 0) {
//    0.217 us [   529] |       sum(1, 4, 0, 4) = 121;
//    0.211 us [   529] |       sum(1, 0, 16, 0) = 21;
//   37.859 us [   529] |     } = 142; /* sum */
// 
//                          /* search [4,0] ends and there are updates so search again [2,4] */
// 
//             [   529] |     search(2, 4) {
//    0.224 us [   529] |       sum(2, 4, 0, 4) = 13;
// 
//             [   529] |       sum(3, 2, 4, 0) {
//    3.156 us [   529] |         sum(3, 2, 0, 2) = 10;
//    0.221 us [   529] |         sum(3, 4, 2, 4) = 15;
//   44.632 us [   529] |       } = 25; /* sum */
// 
//    0.208 us [   529] |       sum(2, 4, 0, 4) = 13;
// 
//                              /* search [2,4] ends
//                                 add time[2, end 4], time +=2, time 2
//                                 <13, 2> = sum(ind:2, e:4, time:0)
//                              */
//
//                              /* means that start = 2 for minimul sum ! */ 
//
//   71.569 us [   529] |     } = 13; /* search */
// 
//                            /* still in search [4,0]
//                               ind was 2, add time[2, start 4], time +=2, time 4 
//                             */
// 
//    0.428 us [   529] |     sum(2, 0, 4, 0) = 35;
// 
//                            /* search [4,0] ends
//                               add time[2, start 0], time +=11, time 15
//                               <48, 15> = sum(ind:2, e:0, time:4)
//                            */
// 
//  955.074 us [   529] |   } = 48; /* search */
// 
//                            /* still in search [0,5]
//                               ind was 4, add time[4, start 0], time +=13, time 28 
//                             */
// 
//    0.235 us [   529] |   sum(4, 5, 28, 5) = 38;
// 
//                            /* search [0,5] ends
//                               add time[4, end 5], time +=10, time 38
//                               <86, 38> sum(ind:4, e:5, time:28)
//                            */
// 
//    6.085 ms [   529] | } = 86; /* search */
// 
//             [   529] | search(5, 0) {
//    0.239 us [   529] |   sum(5, 0, 0, 0) = 262;
//             [   529] |   sum(4, 5, 0, 0) {
//    0.206 us [   529] |     sum(4, 5, 0, 5) = 10;
//    0.223 us [   529] |     sum(4, 0, 20, 0) = 372;
//   49.363 us [   529] |   } = 382; /* sum */
//             [   529] |   sum(3, 5, 0, 0) {
//    0.300 us [   529] |     sum(3, 5, 0, 5) = 16;
//    0.295 us [   529] |     sum(3, 0, 22, 0) = 322;
//   50.948 us [   529] |   } = 338; /* sum */
//             [   529] |   sum(2, 5, 0, 0) {
//    0.287 us [   529] |     sum(2, 5, 0, 5) = 25;
//    0.305 us [   529] |     sum(2, 0, 24, 0) = 95;
//   52.524 us [   529] |   } = 120; /* sum */
//             [   529] |   sum(1, 5, 0, 0) {
//    0.254 us [   529] |     sum(1, 5, 0, 5) = 139;
//    0.249 us [   529] |     sum(1, 0, 36, 0) = 41;
//   42.398 us [   529] |   } = 180; /* sum */
//             [   529] |   search(2, 5) {
//    0.223 us [   529] |     sum(2, 5, 0, 5) = 25;
//             [   529] |     sum(3, 2, 5, 0) {
//    0.202 us [   529] |       sum(3, 2, 0, 2) = 10;
//    0.211 us [   529] |       sum(3, 5, 2, 5) = 28;
//   37.587 us [   529] |     } = 38; /* sum */
//             [   529] |     sum(4, 2, 5, 0) {
//    0.279 us [   529] |       sum(4, 2, 0, 2) = 23;
//    0.270 us [   529] |       sum(4, 5, 4, 5) = 14;
//   56.792 us [   529] |     } = 37; /* sum */
//    0.363 us [   529] |     sum(2, 5, 0, 5) = 25;
//  657.999 us [   529] |   } = 25; /* search */
//    0.286 us [   529] |   sum(2, 0, 24, 0) = 95;
//  954.739 us [   529] | } = 120; /* search */
//
//  86 = min(86, 120)
//
// Things to note:
//
// 1. Always calculate distance from the start point.
// 2. Do not count point which are already counted.
// 3. The start point do not included in the sum.


lint sum_range(int begin, int end, lint start_distance)
{
  if (begin == end)
    return 0;

  // forward or backward
  int direction = ((end > begin) ? 1: -1);

  lint weight_sum = 0;

  for (int i = begin + direction; ; i += direction)
  {
    start_distance += ABS_DISTANCE(i-direction, i);
    weight_sum += start_distance*(lint)g_weights[i];

    if (i == end)
      return weight_sum;
  }

  return weight_sum;
}


lint sum_range_with_start(int start, int begin, int end, lint start_distance)
{
	lint d1 = sum_range(start, begin, start_distance);

	// why 2? since used twice to make a turn
	lint d3 = sum_range(start, end, start_distance + 2*ABS_DISTANCE(begin, start));

	return d1 + d3;
}

// search(start index, end index)
pair<lint, lint> search(int begin, int end)
{
  lint current_sum = sum_range(begin, end, 0);
  int current_start = begin;

  if(begin == end)
    return make_pair(0, 0);

  int direction = ((end > begin)? 1:-1);

  lint calculated_sum = 0;
  lint calculated_distance = 0;

  for (int start = begin + direction; start != end; start += direction)
  {
    lint range_sum  = sum_range_with_start(start, begin, end);

    // found sum which is less than sum of [begin, end] and save start point.
    // so `ind` is saved start which shows the minimum sum so far.
    // 
    // redundant check
    // if (range_sum < current_sum && abs(start - begin) > abs(current_start - begin))

    if (range_sum < current_sum)
    {

      current_sum = range_sum;
      current_start = start;
    }
  }

  // when found start point which has lesser sum since current_start do not
  // change when there is no lesser sum found in the range.

  if (current_start != begin)
  {
    pair<lint, lint> result = search(current_start, begin);	
    calculated_sum += result.first;
    calculated_distance += result.second + ABS_DISTANCE(current_start, begin);		
  }

  calculated_sum += sum_range(current_start, end, calculated_distance);
  calculated_distance += ABS_DISTANCE(current_start, end);

  return make_pair(calculated_sum, calculated_distance);
}


TEST(AlgoRapairman, 0704)
{
  {
    vector<int> dcall{1, 6, 12, 13, 14, 24};
    vector<int> wcall{1, 2, 10, 3, 5, 1};

    for (size_t i = 0; i < dcall.size(); ++i)
    {
      g_points[i] = dcall[i];
      g_weights[i] = wcall[i];
    }

    int N = dcall.size();

    pair<lint, lint> res1 = search(0, N - 1);
    pair<lint, lint> res2 = search(N - 1, 0);

    EXPECT_THAT(min(res1.first, res2.first), 86);
  }

  {
    vector<int> dcall{5, 34, 45, 49, 51, 52, 53, 56, 63, 81, 84, 88, 93, 99, 106};
    vector<int> wcall{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    for (size_t i = 0; i < dcall.size(); ++i)
    {
      g_points[i] = dcall[i];
      g_weights[i] = wcall[i];
    }

    int N = dcall.size();

    pair<lint, lint> res1 = search(0, N - 1);
    pair<lint, lint> res2 = search(N - 1, 0);

    EXPECT_THAT(min(res1.first, res2.first), 630);
  }
}


// ={=========================================================================
// algo-atoi
//
// * input type? digits only? no space?
// * input size?
// * what base? 10 or 2?
// * sign support? 

// from ansic, p43.
//
// when base is 10.
//
// this is 'naive' implementation since no error handlings and return 0 when
// failed to convert. compare to strtol
//
// there is no check on the end of string input? '0' is not the same as
// 0(null) and when see any other than numbers, for loops ends.

namespace algo_conversion
{
  uint32_t atoi_navie(const char *str)
  {
    uint32_t value{0}, i{0};

    for (; str[i] >= '0' && str[i] <= '9'; ++i)
    {
      value = value*10 + (str[i] - '0');
    }

    return value;
  }

  // use isdigit()

  uint32_t atoi_isdigit(const char *str)
  {
    uint32_t value{0}, i{0};

    for (; isdigit(str[i]); ++i)
    {
      value = value*10 + (str[i] - '0');
    }

    return value;
  }

  // To support sign and leading space

  uint32_t atoi_sign(const char *str)
  {
    uint32_t value{0}, i{0}, sign{0};

    while (isspace(str[i]))
      ++i;

    // check sign but don't need to increase i
    sign = str[i] == '-' ? -1 : 1;

    // have to increase
    if (str[i] == '-' || str[i] == '+')
      ++i;

    for (; isdigit(str[i]); ++i)
    {
      value = value*10 + (str[i] - '0');
    }

    return sign*value;
  }

  // use base 2. value is right but not representation

  // Chapter 7: String Fundamentals, 207
  // #!/usr/bin/python
  //
  // B = '1101'
  // I = 0
  //
  // while B != '':
  //     I = I*2 + (ord(B[0]) - ord('0'))
  //     B = B[1:]
  //
  // print(I)

  uint32_t atoi_binary(const char *str)
  {
    uint32_t value{0}, i{0};

    for (; str[i] >= '0' && str[i] <= '9'; ++i)
    {
      value = value*2 + (str[i] - '0');
    }

    return value;
  }


  // use base 16. htoi
  //
  // From ansic, exercise 2-3. 
  //
  // Write the function htoi(s), which converts a string of hexadecimal digits
  // (including an 'optional' 0x or 0X) into its equivalent integer value. The
  // allowable digits are 0 through 9, a through f, and A through F.
  //
  // isxdigit()
  //        checks for hexadecimal digits, that is, one of
  //        0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F.

  // previous try
  //
  // int htoi(char s[])
  // {
  //   int n, i = 0, v = 0;
  // 
  //   // optional 0x or 0X
  //   if(s[0] == '0' && ( s[1] == 'x' || s[1] == 'X' ))
  //     i = 2;
  // 
  //   // isxdigit()
  //   // checks for a hexadecimal digits, that is, one of 
  //   // 0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F.
  //    
  //   for(n = 0; isxdigit(s[i]); i++)
  //   {
  //     if( s[i] >= '0' && s[i] <= '9' )
  //       v = s[i] - '0';
  //     else if( s[i] >= 'a' && s[i] <= 'f' )
  //       v = s[i] - 'a' + 10;
  //     else 
  //       v = s[i] - 'A' + 10;
  // 
  //     n = n*16 + v;
  //   }
  // 
  //   return n;
  // }

  uint32_t atoi_hex(const char *str)
  {
    uint32_t value{0}, i{0};
    const std::string hex{"0123456789abcdef"};

    for (; hex.find(std::tolower(str[i])) != std::string::npos; ++i)
    {
      value = value*16 + hex.find(std::tolower(str[i]));
    }

    return value;
  }
} // namespace

TEST(AlgoConversion, StringToInteger)
{
  using namespace algo_conversion;

  EXPECT_THAT(atoi_navie("123"), Eq(123));
  EXPECT_THAT(atoi_isdigit("123"), Eq(123));

  EXPECT_THAT(atoi_sign("123"), Eq(123));
  EXPECT_THAT(atoi_sign("-123"), Eq(-123));

  EXPECT_THAT(atoi_binary("1101"), Eq(13));

  EXPECT_THAT(atoi_hex("1a"), Eq(26));
  EXPECT_THAT(atoi_hex("1A"), Eq(26));
}


// ={=========================================================================
// algo-itoa
//
// * what base? 10 or 2?
// * sign support? 

// from ansic, p43.
//
// when base is 10.
//
// there is no check on the end of string input? '0' is not the same as
// 0(null) and when see any other than numbers, for loops ends.

namespace algo_conversion
{
  std::string itoa_navie(const int input)
  {
    int value{input};
    char letter{0};
    std::string result{};

    for (; value;)
    {
      // note: the order of statement does matter since it updtes "value"

      letter = '0' + (value % 10);
      result += letter;
      value /= 10;
    }

    // *algo-reverse*
    return std::string(result.crbegin(), result.crend());
  }

  std::string itoa_no_reverse(const int input)
  {
    std::string result{};
    std::string temp{};
    char letter{0};
    int value{input};

    for(; value;)
    {
      letter = '0' + (value % 10);
      result.insert( result.begin(), 1, letter );
      value /= 10;
    }

    return result;
  }

} // namespace

TEST(AlgoConversion, IntegerToString)
{
  using namespace algo_conversion;

  EXPECT_THAT(itoa_navie(123), Eq("123"));
  EXPECT_THAT(itoa_no_reverse(123), Eq("123"));
}


// ={=========================================================================
// algo-list

// The stack and `simple-list` are essentially the same implementation in terms
// of contiguous and linked implementation.
//
// o ListEntry can be any type.
// o No remove() to remove entry at random position since it's expensive
// operation as with other contiguous implementation; contiguous stack.

// class List
// {
//   public:
//     void create();
//     void clear();
//     bool empty();
//     bool full();
//     int size();
//     // as push_back();
//     void push(ListEntry const& entry);
//     std::vector<ListEntry> snap();
// };

namespace algo_list_contiguous
{
  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept 
      : row_(row), col_(col) {}

    int row_{};
    int col_{};
  };

  // cxx-operator-overload
  bool operator==(ListEntry const& lhs, ListEntry const& rhs)
  {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_) ? true : false;
  }

  bool operator!=(ListEntry const& lhs, ListEntry const& rhs)
  {
    return !(lhs == rhs);
  }

  class List
  {
    public:
      explicit List() : count_(0)
      {}

      void clear() 
      { count_ = 0;}

      bool empty()
      { return count_ == 0 ? true : false; }

      bool full()
      { return count_ >= MAX_ENTRY ? true : false; }

      int size()
      { return count_; }

      // as push_back();
      void push(ListEntry const& entry)
      {
        if (full())
          throw runtime_error("list is full");

        coll_[count_++] = entry;
      }

      std::vector<ListEntry> snap()
      {
        std::vector<ListEntry> coll;

        for (int i = 0; i < count_; ++i)
          coll.push_back(coll_[i]);

        return coll;
      }

    private:

      // cxx-static
      // error: invalid use of non-static data member ‘algo_list_contiguous::List::MAX_ENTRY’
      // const int MAX_ENTRY{5};

      static const int MAX_ENTRY{5};

      // count_ should be [0, 4] or [1, 5]? choose [1,5]
      int count_;

      ListEntry coll_[MAX_ENTRY];
  };
}

TEST(AlgoList, ContiguousSimple)
{
  using namespace algo_list_contiguous;

  std::vector<ListEntry> values{
      ListEntry(1,2), 
      ListEntry(2,3), 
      ListEntry(3,4), 
      ListEntry(4,5), 
      ListEntry(5,6)
  };

  List coll;

  for (auto &e : values)
    coll.push(e);

  EXPECT_THAT(coll.size(), 5);

  // requires cxx-operator-overload
  EXPECT_THAT(coll.snap(), values);

  EXPECT_THROW(coll.push(ListEntry(6,7)), runtime_error); 
}


namespace algo_list_linked
{
  // when node and entry are in a single structure and these can be different
  // structure such as ListEntry and ListNode

  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept 
      : row_(row), col_(col), next_(nullptr) 
    {}

    int row_{};
    int col_{};

    ListEntry* next_;
  };

  // cxx-operator-overload
  bool operator==(ListEntry const& lhs, ListEntry const& rhs)
  {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_) ? true : false;
  }

  bool operator!=(ListEntry const& lhs, ListEntry const& rhs)
  {
    return !(lhs == rhs);
  }

  class List
  {
    public:
      explicit List() noexcept
        : head_(nullptr)
        {}

      bool emptry()
      { return count_ == 0 ? true : false; }

      int size()
      { return count_; }

      // push_back()
      void push_old(ListEntry const& entry)
      {
        if (!head_)
          head_ = new ListEntry(entry);
        else
        {
          ListEntry* run = head_;

          // unlike clear(), snap(), run shall be before end() so that can
          // insert new one. Hence check run->next

          while (run->next_)
            run = run->next_;

          run->next_ = new ListEntry(entry);
        }
        
        ++count_;
      }

      // push_back()
      void push(ListEntry const& entry)
      {
        ListEntry* run{};

        // find node for insertion *algo-list-find-end*
        // works both when head_ is null and is not null

        for (run = head_; run && run->next_; run = run->next_)
          ;

        // first item
        if (!run)
          head_ = new ListEntry(entry);
        else
          run->next_ = new ListEntry(entry);

        ++count_;
      }

      void clear()
      {
        ListEntry* run = head_;
        ListEntry* prev{};

        while (run)
        {
          prev = run;
          run = run->next_;
          free(prev);
          --count_;
        }

        head_ = run;
      }

      std::vector<ListEntry> snap()
      {
        ListEntry* run = head_;
        std::vector<ListEntry> coll;

        while (run)
        {
          // ok as well
          // coll.push_back(ListEntry(*run));
          coll.push_back(*run);
          run = run->next_;
        }

        return coll;
      }

    private:
      int count_{};

      // can use ListEntry head_; which changes member implementation

      ListEntry* head_;
  };

} // namespace


TEST(AlgoList, LinkedSimple)
{
  using namespace algo_list_linked;

  std::vector<ListEntry> values{
    ListEntry(1,2), 
    ListEntry(2,3), 
    ListEntry(3,4), 
    ListEntry(4,5), 
    ListEntry(5,6)
  };

  List coll;

  for (auto &e : values)
    coll.push(e);

  EXPECT_THAT(coll.size(), 5);

  coll.push(ListEntry(6,7));
  EXPECT_THAT(coll.size(), 6);

  // requires cxx-operator-overload
  std::vector<ListEntry> expected{
    ListEntry(1,2), 
    ListEntry(2,3),
    ListEntry(3,4),
    ListEntry(4,5),
    ListEntry(5,6),
    ListEntry(6,7)
  };

  EXPECT_THAT(coll.snap(), expected);

  coll.clear();
  EXPECT_THAT(coll.size(), 0);
}


// TEST(AlgoList, LinkedSimpleReverse)
// {
//   using namespace algo_list_linked;
// 
//   {
//     auto values{
//       ListEntry(1,2), 
//         ListEntry(2,3), 
//         ListEntry(3,4), 
//         ListEntry(4,5), 
//         ListEntry(5,6)
//     };
// 
//     List coll;
// 
//     for (auto &e : values)
//       coll.push(e);
// 
//     EXPECT_THAT(coll.size(), 5);
// 
//     // now do not expect exception since there's no max
//     // EXPECT_THROW(coll.push(ListEntry(6,7)), runtime_error); 
// 
//     coll.push(ListEntry(6,7));
//     EXPECT_THAT(coll.size(), 6);
// 
//     EXPECT_THAT(coll.snap(), 
//         ElementsAre(
//           make_pair(1,2), 
//           make_pair(2,3), 
//           make_pair(3,4), 
//           make_pair(4,5), 
//           make_pair(5,6), 
//           make_pair(6,7))
//         );
// 
//     coll.reverse();
// 
//     EXPECT_THAT(coll.snap(), 
//         ElementsAre(
//           make_pair(6,7),
//           make_pair(5,6), 
//           make_pair(4,5), 
//           make_pair(3,4), 
//           make_pair(2,3), 
//           make_pair(1,2)) 
//         );
// 
//     coll.clear();
//     EXPECT_THAT(coll.size(), 0);
//   }
// }


// In order to exercise Divide(), have to have access to list structure but do
// not see any practical way to do it through class interface. so make `head_`
// public. same as algo_list_linked

namespace algo_list_linked_divide
{
  // when node and entry are in a single structure and these can be different
  // structure such as ListEntry and ListNode

  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept 
      : row_(row), col_(col), next_(nullptr) 
    {}

    int row_{};
    int col_{};

    ListEntry* next_;
  };

  // cxx-operator-overload
  bool operator==(ListEntry const& lhs, ListEntry const& rhs)
  {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_) ? true : false;
  }

  bool operator!=(ListEntry const& lhs, ListEntry const& rhs)
  {
    return !(lhs == rhs);
  }

  class List
  {
    public:
      explicit List() noexcept
        : head_(nullptr)
        {}

      bool emptry()
      { return count_ == 0 ? true : false; }

      int size()
      { return count_; }

      // push_back()
      void push_old(ListEntry const& entry)
      {
        if (!head_)
          head_ = new ListEntry(entry);
        else
        {
          ListEntry* run = head_;

          // unlike clear(), snap(), run shall be before end() so that can
          // insert new one. Hence check run->next

          while (run->next_)
            run = run->next_;

          run->next_ = new ListEntry(entry);
        }
        
        ++count_;
      }

      // push_back()
      void push(ListEntry const& entry)
      {
        ListEntry* run{};

        // find node for insertion *algo-list-find-end*
        // works both when head_ is null and is not null

        for (run = head_; run && run->next_; run = run->next_)
          ;

        // first item
        if (!run)
          head_ = new ListEntry(entry);
        else
          run->next_ = new ListEntry(entry);

        ++count_;
      }

      void clear()
      {
        ListEntry* run = head_;
        ListEntry* prev{};

        while (run)
        {
          prev = run;
          run = run->next_;
          free(prev);
          --count_;
        }

        head_ = run;
      }

      std::vector<ListEntry> snap()
      {
        ListEntry* run = head_;
        std::vector<ListEntry> coll;

        while (run)
        {
          // ok as well
          // coll.push_back(ListEntry(*run));
          coll.push_back(*run);
          run = run->next_;
        }

        return coll;
      }

    public:
      int count_{};

      // can use ListEntry head_; which changes member implementation
      ListEntry* head_;
  };


  // o slow and ffast starts from same place, begin()
  // o slow goes 1 and ffast goes 2
  // o this is implementation of {algo-list-tortoise-and-hare}

  bool detect_cycle_01(List const &list)
  {
    ListEntry *slow;
    ListEntry *fast;
    ListEntry *ffast;

    for (slow = fast = ffast = list.head_;
        slow && (fast = ffast->next_) && (ffast = fast->next_);)
    {
      if (slow == fast || slow == ffast)
        return true;

      slow = slow->next_;
    }
    
    return false;
  }


  // o use single fast
  // o see *cxx-for-while*
  //
  // algo-leetcode
  //
  // 141. Linked List Cycle
  //
  // Runtime: 12 ms, faster than 99.74% of C++ online submissions for Linked
  // List Cycle.
  //
  // Memory Usage: 9.7 MB, less than 73.40% of C++ online submissions for Linked
  // List Cycle.

  bool detect_cycle_02(List const &list)
  {
    ListEntry *slow;
    ListEntry *fast;

    for (slow = fast = list.head_;
        slow && (fast = fast->next_) && (fast = fast->next_);)
    {
      if (slow == fast)
        return true;

      slow = slow->next_;
    }
    
    return false;
  }

  bool detect_cycle_03(List const &list)
  {
    ListEntry *slow;
    ListEntry *fast;

    // do not check when head_ is null
    for (slow = list.head_, fast = slow->next_;
        fast && slow != fast; )
    {
      fast = fast->next_;

      if (fast)
      {
        fast = fast->next_;
        slow = slow->next_;
      }
    }

    // loop ends when fast is null or when there is cycle so need to check if it
    // was when there is a cycle

    return fast != slow ? false : true;
  }

} // namespace

TEST(AlgoList, DetectCycle)
{
  using namespace algo_list_linked_divide;

  {
    std::vector<ListEntry> values{
      ListEntry(1,2), 
        ListEntry(2,3), 
        ListEntry(3,4), 
        ListEntry(4,5), 
        ListEntry(5,6)
    };

    List coll;

    for (auto &e : values)
      coll.push(e);

    EXPECT_THAT(detect_cycle_01(coll), false);
    EXPECT_THAT(detect_cycle_02(coll), false);
    EXPECT_THAT(detect_cycle_03(coll), false);
  }

  {
    std::vector<ListEntry> values{
      ListEntry(1,2), 
        ListEntry(2,3), 
        ListEntry(3,4), 
        ListEntry(4,5), 
        ListEntry(5,6)
    };

    List coll;

    for (auto &e : values)
      coll.push(e);

    // to make a cycle
    ListEntry *current;
    ListEntry *next;

    current = coll.head_;

    // find end()-1
    for (next = current; next; next = next->next_)
    {
      current = next;
    }

    // makes a cycle
    current->next_ = coll.head_;

    EXPECT_THAT(detect_cycle_01(coll), true);
    EXPECT_THAT(detect_cycle_02(coll), true);
    EXPECT_THAT(detect_cycle_03(coll), true);
  }
}

namespace algo_list_linked_divide
{
  // o devide the first list in two as evenly as possible
  // o when the size of the first is odd, the divided first will have one more
  //   than the second
  // o as with detect_cycle_03(), slow starts from 0 and fast starts from 1.
 
  void divide_list_01(List &first, List &second)
  {
    ListEntry *slow;
    ListEntry *fast;

    // do not check if slow is null or not

    for (slow = first.head_, fast = slow->next_; 
        fast; )
    {
      fast = fast->next_;

      if (fast)
      {
        fast = fast->next_;
        slow = slow->next_;
      }
    }

    // `slow` is the end of divided first list
    second.head_ = slow->next_;
    slow->next_ = nullptr;
  }

  // as with detect_cycle_01(), both starts from the same pos

  void divide_list_02(List &first, List &second)
  {
    ListEntry *slow;
    ListEntry *fast;

    for (slow = fast = first.head_;
        slow && (fast = fast->next_) && (fast = fast->next_); )
    {
      slow = slow->next_;
    }

    // `slow` is the end of divided first list
    second.head_ = slow->next_;
    slow->next_ = nullptr;
  }
} // namespace

TEST(AlgoList, Divide)
{
  using namespace algo_list_linked_divide;

  std::vector<ListEntry> values{
    ListEntry(1,2), 
    ListEntry(2,3), 
    ListEntry(3,4), 
    ListEntry(4,5), 
    ListEntry(5,6),
    ListEntry(6,7)
  };

  {
    const auto func = divide_list_01;

    List coll1;

    for (auto &e : values)
      coll1.push(e);

    // requires cxx-operator-overload
    std::vector<ListEntry> divided_first{
      ListEntry(1,2), 
      ListEntry(2,3),
      ListEntry(3,4)
    };

    std::vector<ListEntry> divided_second{
      ListEntry(4,5),
      ListEntry(5,6),
      ListEntry(6,7)
    };

    // note:
    // since divide_list_01() do not change `count_`, count of simple_list,
    // simple_second will not be correct after this.

    List coll2;

    func(coll1, coll2);

    EXPECT_THAT(coll1.snap(), divided_first);
    EXPECT_THAT(coll2.snap(), divided_second);

    coll1.clear();
    coll2.clear();
  }

  {
    const auto func = divide_list_02;

    List coll1;

    for (auto &e : values)
      coll1.push(e);

    // requires cxx-operator-overload
    std::vector<ListEntry> divided_first{
      ListEntry(1,2), 
      ListEntry(2,3),
      ListEntry(3,4)
    };

    std::vector<ListEntry> divided_second{
      ListEntry(4,5),
      ListEntry(5,6),
      ListEntry(6,7)
    };

    // note:
    // since divide_list_01() do not change `count_`, count of simple_list,
    // simple_second will not be correct after this.

    List coll2;

    func(coll1, coll2);

    EXPECT_THAT(coll1.snap(), divided_first);
    EXPECT_THAT(coll2.snap(), divided_second);

    coll1.clear();
    coll2.clear();
  }

  // TEST(List, SpliceAndDivide)
  {
    std::list<int> coll{26, 33, 35, 29, 19, 12, 22};
    auto slow = coll.begin();
    auto fast = next(slow);

    for(;fast != coll.end();)
    {
      ++fast;

      if (fast != coll.end())
      {
        ++fast;
        ++slow;
      }
    }

    list<int> coll1;
    list<int> coll2;

    // due to open end of iterator, increase one more compared to C version.
    EXPECT_THAT(*slow, 29);
    ++slow;

    // c.splice(pos,c2, c2beg,c2end) 
    // Moves all elements of the range [c2beg,c2end) in c2 in
    // front of pos of list c (c and c2 may be identical)

    coll1.splice(coll1.begin(), coll, coll.begin(), slow);
    coll2.splice(coll2.begin(), coll, slow, coll.end());

    EXPECT_THAT(coll1, ElementsAre(26,33,35,29));
    EXPECT_THAT(coll2, ElementsAre(19,12,22));
  }
}


// namespace list_simple_linked_list_public_two
// {
//   // C version list from the reference
// 
//   typedef int ListEntry;
// 
//   typedef struct node {
//     int       key;
//     struct node *pnext;
//   } ListNode;
// 
//   typedef struct {
//     ListNode *header;
//     int count;
//   } List;
// 
//   void CombineList(List *first, List *last, List *result)
//   {
//     // handle when one of lists is empty
//     if( !first->header )
//     {
//       *result = *last;
//       return;
//     }
//     else if ( !last->header ) 
//     {
//       *result  = *first;
//       return;
//     }
// 
//     // handle first comparison
//     ListNode *pfirst = first->header, *psecond = last->header;
//     ListNode *psorted;
// 
//     if(pfirst->key < psecond->key)
//     {
//       result->header = pfirst;
//       pfirst = pfirst->pnext;
//     }
//     else
//     {
//       result->header = psecond;
//       psecond = psecond->pnext;
//     }
// 
//     psorted = result->header;
// 
//     // sort until finish one of lists because first and second is alreay sorted itself
//     while( pfirst && psecond )
//     {
//       if(pfirst->key < psecond->key)
//       {
//         psorted->pnext = pfirst;
//         psorted = pfirst;
//         pfirst = pfirst->pnext;
//       }
//       else
//       {
//         psorted->pnext = psecond;
//         psorted = psecond;
//         psecond = psecond->pnext;
//       }
//     }
// 
//     // when one of lists are finished, simply append the other list to the sorted
//     if(!pfirst)
//       psorted->pnext = psecond;
//     else
//       psorted->pnext = pfirst;
//   }
// } // namespace


// ={=========================================================================
// algo-stack

// class Stack
// {
//   public:
//     void create();
//     void clear();
//     bool empty();
//     bool full();
//     int size();
//
//     // as push_back();
//     void push(ListEntry const& entry);
//     void pop();
//     ListEntry top();
//
//     // as traverse()
//     std::vector<ListEntry> snap();
// };

namespace algo_stack_contiguous
{
  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept 
      : row_(row), col_(col) {}

    int row_{};
    int col_{};
  };

  bool operator==(ListEntry const& lhs, ListEntry const& rhs)
  {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_) ? true : false;
  }

  bool operator!=(ListEntry const& lhs, ListEntry const& rhs)
  {
    return !(lhs == rhs);
  }

  class Stack
  {
    public:
      void create()
      { top_ = 0; }

      void clear() 
      { top_ = 0;}

      bool empty()
      { return top_ == 0 ? true : false; }

      bool full()
      { return top_ >= MAX_ENTRY ? true : false; }

      int size()
      { return top_; }

      // as push_back();
      void push(ListEntry const& entry)
      {
        if (full())
          throw runtime_error("list is full");

        coll_[top_++] = entry;
      }

      void pop()
      {
        --top_;
      }

      ListEntry top()
      {
        if (empty())
          throw runtime_error("list is empty");

        return coll_[top_ - 1];
      }

      std::vector<ListEntry> snap()
      {
        std::vector<ListEntry> coll;

        for (int i = 0; i < top_; ++i)
          coll.push_back(coll_[i]);

        return coll;
      }

    private:

      // cxx-static
      // error: invalid use of non-static data member ‘algo_list_contiguous::List::MAX_ENTRY’
      // const int MAX_ENTRY{5};

      static const int MAX_ENTRY{5};

      // top_ is next positon to push
      int top_;

      ListEntry coll_[MAX_ENTRY];
  };
}

TEST(AlgoStack, ContiguousSimple)
{
  using namespace algo_stack_contiguous;

  std::vector<ListEntry> values{
      ListEntry(1,2), 
      ListEntry(2,3), 
      ListEntry(3,4), 
      ListEntry(4,5), 
      ListEntry(5,6)
  };

  Stack coll;
  coll.create();

  for (auto &e : values)
    coll.push(e);

  EXPECT_THAT(coll.size(), 5);
  EXPECT_THROW(coll.push(ListEntry(6,7)), runtime_error); 

  EXPECT_THAT(coll.top(), ListEntry(5,6));

  coll.pop();
  coll.pop();
  coll.pop();

  EXPECT_THAT(coll.size(), 2);

  // requires cxx-operator-overload
  auto expected{
    ListEntry(1,2), 
    ListEntry(2,3)
  };

  EXPECT_THAT(coll.snap(), expected);
}


// ={=========================================================================
// algo-sort-insert

// reference code, ascending sort
void sort_insertion_01(vector<int> &coll)
{
  // start from 1 since one entry is always sorted.
  int size = (int) coll.size(); 

  for (int unsorted_index = 1; unsorted_index < size; ++unsorted_index)
  {
    int sorted_index = unsorted_index-1;

    // pick the first from `unsorted` and that is less than the last of the
    // sorted. so have to place it in the sorted area.
    
    // if (coll[unsorted_index] < coll[sorted_index]) 
    if (coll[sorted_index] > coll[unsorted_index]) 
    { 
      int unsorted_entry = coll[unsorted_index];
      int current_index = sorted_index;

      // find the place of picked unsorted in the sorted until see that 
      // sorted[current-1] < the entry in question by:
      //
      // move the last sorted down one which makes a space, and check the one
      // before the current.
      //
      // must have "index == 0" check since have a check on current-1 to see if
      // or not contine searching.

      for (; 0 <= current_index; --current_index)
      {
        // move one entry down
        coll[current_index+1] = coll[current_index]; 

        if (current_index == 0 || coll[current_index-1] < unsorted_entry)
          break;
      }

      coll[current_index] = unsorted_entry;
    } 
  } 
}


// removes to check `current-1` from the current in searching the place in the
// sorted and then no need to check on -1 index.

void sort_insertion_02(vector<int> &coll)
{
  int size = (int) coll.size(); 

  for (int unsorted_index = 1; unsorted_index < size; ++unsorted_index)
  {
    int sorted_index = unsorted_index-1;

    if (coll[unsorted_index] < coll[sorted_index]) 
    { 
      int unsorted_entry = coll[unsorted_index];
      int current_index = sorted_index;

      // diffs from 01
      for (; 0 <= current_index; --current_index)
      {
        if (coll[current_index] < unsorted_entry)
          break;

        coll[current_index+1] = coll[current_index]; 
      }

      coll[current_index+1] = unsorted_entry;
    } 
  } 
}

// `current` starts from the unsorted and uses swap
void sort_insertion_03(vector<int> &coll)
{
  int size = (int) coll.size(); 

  for (int unsorted_index = 1; unsorted_index < size; ++unsorted_index)
    for (int current_index = unsorted_index;
        0 < current_index && coll[current_index] < coll[current_index-1];
        --current_index)
    {
      // swap current and current-1
      swap(coll[current_index], coll[current_index-1]);
    }
}

// https://www.codeproject.com/Articles/854127/Top-Beautiful-Cplusplus-std-Algorithms-Examples
void sort_insertion_04(vector<int> &coll)
{
  auto first = coll.begin();

  for (auto run = first; run != coll.end(); ++run)
    rotate(upper_bound(first, run, *run), run, next(run));
}

// From Programming Pearl 11.1
//
// for i = [1, n)
//  // invariant: x[0..i-1] is sorted
//  // shift x[i] down to its proper place in x[0..i]
//
// From the swap version which do two operations, move down elements and put the
// saved back when loop ends as 01/02 version. So one update rather than doing
// it every time which runs faster.

void sort_insertion_05(vector<int> &coll)
{
  int size = (int) coll.size(); 

  for (int unsorted_index = 1; unsorted_index < size; ++unsorted_index)
  {
    int unsorted = coll[unsorted_index];
    int current_index = unsorted_index;

    for (; 0 < current_index && unsorted < coll[current_index-1];
        --current_index)
    {
      // swap current and current-1
      // swap(coll[current_index], coll[current_index-1]);
      coll[current_index] = coll[current_index-1];
    }

    coll[current_index] = unsorted;

#ifdef SORT_INSERT_DEBUG
    cout << "coll(" << unsorted_index << ", " << current_index << "): ";

    for (int i = 0; i < size; ++i)
      cout << coll[i] << ", ";

    cout << endl;
#endif // SORT_INSERT_DEBUG
  }
}

// 2018.12.06
void sort_insertion_06(vector<int> &coll)
{
  size_t sorted{0}, unsorted{0};
  int target{0}, source{0};

  for (unsorted = 1; unsorted < coll.size(); ++unsorted, sorted = unsorted-1)
  {
    for (target = sorted, source = unsorted; target >= 0; --target, --source)
    {
      // ascending order
      if (coll[target] > coll[source])
        std::swap(coll[target], coll[source]);
      else
        break;
    }
  }
}

// when move comparison in for loop, can remove `else` as sort_insertion_03

void sort_insertion_07(vector<int> &coll)
{
  size_t sorted{0}, unsorted{0};
  int target{0}, source{0};

  for (unsorted = 1; unsorted < coll.size(); ++unsorted, sorted = unsorted-1)
  {
    // ascending order
    for (target = sorted, source = unsorted; 
        target >= 0 && coll[target] > coll[source]; --target, --source)
    {
      std::swap(coll[target], coll[source]);
    }
  }
}

template <typename T, typename F = std::greater<T>>
void sort_insertion_08(vector<T> &coll, F f)
{
  size_t sorted{0}, unsorted{0};
  int target{0}, source{0};

  for (unsorted = 1; unsorted < coll.size(); ++unsorted, sorted = unsorted-1)
  {
    // ascending order
    for (target = sorted, source = unsorted; 
        target >= 0 && f(coll[target], coll[source]); --target, --source)
    {
      std::swap(coll[target], coll[source]);
    }
  }
}

TEST(AlgoSort, Insertion)
{
  {
    vector<int> coll{ 33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3 };
    sort_insertion_01(coll); 
    EXPECT_THAT(coll, 
        ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }
  {
    vector<int> coll{ 33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3 };
    sort_insertion_02(coll);
    EXPECT_THAT(coll, 
        ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }
  {
    vector<int> coll{ 33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3 };
    sort_insertion_03(coll);
    EXPECT_THAT(coll, 
        ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }
  {
    vector<int> coll{ 33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3 };
    sort_insertion_04(coll);
    EXPECT_THAT(coll, 
        ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }
  {
    vector<int> coll{ 33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3 };
    sort_insertion_05(coll);
    EXPECT_THAT(coll, 
        ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }
  {
    vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};
    sort_insertion_06(coll); 
    EXPECT_THAT(coll, 
        ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }
  {
    vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};
    sort_insertion_07(coll); 
    EXPECT_THAT(coll, 
        ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }
  {
    vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};
    sort_insertion_08(coll, std::less<int>()); 
    EXPECT_THAT(coll, 
        ElementsAreArray({33, 31, 30, 29, 17, 15, 13, 12, 10, 6, 5, 3, 2}));
  }
}


// ={=========================================================================
// algo-partition algo-sort-insert a

// algo-partition which uses the same grouping trick as algo-sort-insert

namespace algo_partition
{
  // Re-arrange the portfolio between (begin, end)  in such a way that all the
  // stocks with quantity <= maxQuantity precede all those with quantity >
  // maxQuantity Return the iterator to the first element with quantity >
  // maxQuantity

  using PortfolioIterator = vector<unsigned int>::iterator;

  // replace Stock to simple int 
  // std::vector<Stock> PortfolioGreater, PortfolioLesser;

  // 2N space and 2N time(2 pass)

  PortfolioIterator rearrangeByQuantity_1(PortfolioIterator begin,
      PortfolioIterator end,
      unsigned int maxQuantity)
  {
    // implement me
    std::vector<unsigned int> PortfolioGreater, PortfolioLesser;
    PortfolioIterator run = begin;

    for( ; run != end; ++run)
    {
      // greater
      // if( run->quantity > maxQuantity )
      if( *run > maxQuantity )
        PortfolioGreater.push_back(*run);
      else
        PortfolioLesser.push_back(*run);
    }

    run = begin;
    for(const auto& elem : PortfolioLesser) {
      *run++ = elem;
    }
    begin = run;

    for(const auto& elem : PortfolioGreater) {
      *run++ = elem;
    }

    return begin;
  }

  // less space but still 2 pass

  PortfolioIterator rearrangeByQuantity_2(PortfolioIterator begin,
      PortfolioIterator end, unsigned int max_quanity)
  {
    // how to get T of coll such as algo-remove? here, assumes that we know T
    vector<unsigned int> coll;

    PortfolioIterator start = begin;
    PortfolioIterator current{};

    // one pass to filter <=

    for (; start != end; ++start)
    {
      // not use push_back() since void push_back()
      if (*start <= max_quanity)
        current = coll.insert(coll.end(), *start);
    }

    start = begin;

    // second pass to filter >

    for (; start != end; ++start)
    {
      if (*start > max_quanity)
        coll.push_back(*start);
    }

    // copy it back
    copy(coll.begin(), coll.end(), begin);

    // *cxx-vector-reallocation* *cxx-iter-invalidated*
    // *cxx-iter-singular* means invalidated iterator since there is no guarantee
    // that current is valid after second pass push_back due to relocation
    //
    // /usr/include/c++/6/debug/safe_iterator.h:298:
    // Error: attempt to increment a singular iterator.
    // 
    // Objects involved in the operation:
    //     iterator "this" @ 0x0x7ffdeb5ea9a0 {
    //       type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<unsigned int*, std::__cxx1998::vector<unsigned int, std::allocator<unsigned int> > >, 
    //          std::__debug::vector<unsigned int, std::allocator<unsigned int> > > (mutable iterator);
    //       state = singular;
    //       references sequence with type 'std::__debug::vector<unsigned int, std::allocator<unsigned int> >' @ 0x0x7ffdeb5eaa00
    //     }
    // Aborted
    //
    // return ++current;

    return current;
  }

  namespace algo_code
  {
    // /usr/include/c++/4.9.2/bits/stl_algo.h

    /**
     *  @brief Move elements for which a predicate is true to the beginning
     *         of a sequence.
     *  @ingroup mutating_algorithms
     *  @param  __first   A forward iterator.
     *  @param  __last    A forward iterator.
     *  @param  __pred    A predicate functor.
     *  @return  An iterator @p middle such that @p __pred(i) is true for each
     *  iterator @p i in the range @p [__first,middle) and false for each @p i
     *  in the range @p [middle,__last).
     *
     *  @p __pred must not modify its operand. @p partition() does not preserve
     *  the relative ordering of elements in each group, use
     *  @p stable_partition() if this is needed.
     */
    template<typename _ForwardIterator, typename _Predicate>
      inline _ForwardIterator
      partition(_ForwardIterator __first, _ForwardIterator __last,
          _Predicate   __pred)
      {
        return std::__partition(__first, __last, __pred,
            std::__iterator_category(__first));
      }

    /// This is a helper function...
    template<typename _ForwardIterator, typename _Predicate>
      _ForwardIterator
      __partition(_ForwardIterator __first, _ForwardIterator __last,
          _Predicate __pred, forward_iterator_tag)
      {
        if (__first == __last)
          return __first;

        while (__pred(*__first))
          if (++__first == __last)
            return __first;

        _ForwardIterator __next = __first;

        while (++__next != __last)
          if (__pred(*__next))
          {
            std::iter_swap(__first, __next);
            ++__first;
          }

        return __first;
      }
  } // namespace


  // same as algo-partition in /usr/include/c++/4.9.2/bits/stl_algo.h
  //
  // o is to find the first unmatched

  template <typename _Iterator, typename _Compare>
    _Iterator partition_1(_Iterator begin, _Iterator end, _Compare comp)
    {
      if (begin == end)
        return begin;

      // skip matched elements and begin becomes the first unmatched item.
      // begin is "start of the unmatched"
      // note that begin is increased in "if"
 
      while (comp(*begin))
        if (++begin == end)
          return begin;

      // do the same
      // _Iterator first = begin;
      // for (; first != end; ++first)
      //   if (!comp(*first))
      //     break;

      _Iterator run = begin;

      // increase first since knows *run is already unmatched.
      while (++run != end)
      {
        // see matched and move it to matched group
        if (comp(*run))
        {
          // cannot use "=" since it's not algo-remove
          std::iter_swap(run, begin);
          ++begin;
        }
      }

      return begin;
    }

} // namespace


TEST(AlgoPartition, Stl)
{
  using namespace algo_partition;

  // | matched | unmatched |
  //
  // algo-partition returns an iterator to the first element where the
  // predicate is not true, or the end of the range if all elements satisfy
  // the predicate. so first odd element:

  {
    vector<int> coll1;
    vector<int> coll2;

    // INSERT_ELEMENTS(coll1, 1, 9);
    coll1 = {1,2,3,4,5,6,7,8,9};
    EXPECT_THAT(coll1, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

    auto pos1 = partition(coll1.begin(), coll1.end(),    // range
        [](int elem)
        {
        return elem %2 == 0;
        });

    EXPECT_THAT(coll1, ElementsAre(8, 2, 6, 4, 5, 3, 7, 1, 9));

    EXPECT_EQ(*pos1, 5);

    // INSERT_ELEMENTS(coll2, 1, 9);
    coll2 = {1,2,3,4,5,6,7,8,9};
    EXPECT_THAT(coll2, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

    auto pos2 = stable_partition(coll2.begin(), coll2.end(),
        [](int elem)
        {
        return elem %2 == 0;
        });
    EXPECT_THAT(coll2, ElementsAre(2, 4, 6, 8, 1, 3, 5, 7, 9));

    // first odd element:
    EXPECT_EQ(*pos2, 1);
  }

  // works like algo-partition-stable_partition
  {
    vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

    const auto func = rearrangeByQuantity_1;

    auto it = func(coll.begin(), coll.end(), 25);

    // 43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23,
    // 6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,37,48,26,41,30,
    //                                ^^

    EXPECT_THAT(coll, 
        ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,37,48,26,41,30}));

    EXPECT_THAT(*it, 43);
  }

  {
    vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

    const auto func = rearrangeByQuantity_2;

    auto it = func(coll.begin(), coll.end(), 25);

    // 43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23,
    // 6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,37,48,26,41,30,
    //                                ^^

    EXPECT_THAT(coll, 
        ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,37,48,26,41,30}));

    // this now fails since `current` is iterator of internal coll but not input
    // coll. Have to work out one.
    // EXPECT_THAT(*it, 43);
  }

  {
    vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

    // this prevents cxx-template-deduction
    // const auto func = partition_1;

    auto it = partition_1(coll.begin(), coll.end(), 
        [](unsigned int value)
        { return value <= 25; }
        );

    EXPECT_THAT(coll, 
        ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,26,41,30,37,48}));
    EXPECT_THAT(distance(coll.begin(), it), 11);
    EXPECT_THAT(*it, 43);
  }


  // Q: why partiton_xxx() make different order from partition() when use the
  // same logic? but distance is the same.

  {
    vector<unsigned int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

    auto it = partition(coll.begin(), coll.end(), 
        [](unsigned int value)
        { return value <= 25; }
        );

    // EXPECT_THAT(coll, 
    //     ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,26,41,30,37,48}));

    EXPECT_THAT(coll, 
        ElementsAreArray({23,6,11,6,13,23,21,19,20,15,24,48,37,34,29,26,41,30,42,43}));

    EXPECT_THAT(distance(coll.begin(), it), 11);

    // EXPECT_THAT(*it, 43);
    EXPECT_THAT(*it, 48);
  }
}


// ={=========================================================================
// algo-remove algo-sort-insert 

namespace algo_remove
{
  namespace algo_code 
  {
    // /usr/include/c++/4.9/bits/predefined_ops.h

    // check if when predicate is called, what arg iter refers to equals to the
    // value.

    template<typename _Value>
      struct _Iter_equals_val
      {
        _Value& _M_value;

        _Iter_equals_val(_Value& __value)
          : _M_value(__value)
        { }

        template<typename _Iterator>
          bool
          operator()(_Iterator __it)
          { return *__it == _M_value; }
      };

    template<typename _Value>
      inline _Iter_equals_val<_Value>
      __iter_equals_val(_Value& __val)
      { return _Iter_equals_val<_Value>(__val); }

    // /usr/include/c++/4.9/bits/stl_algo.h

    /**
     *  @brief Remove elements from a sequence.
     *  @ingroup mutating_algorithms
     *  @param  __first  An input iterator.
     *  @param  __last   An input iterator.
     *  @param  __value  The value to be removed.
     *  @return   An iterator designating the end of the resulting sequence.
     *
     *  All elements equal to @p __value are removed from the range
     *  @p [__first,__last).
     *
     *  remove() is stable, so the relative order of elements that are
     *  not removed is unchanged.
     *
     *  Elements between the end of the resulting sequence and @p __last
     *  are still present, but their value is unspecified.
     */
    template<typename _ForwardIterator, typename _Tp>
      inline _ForwardIterator
      remove(_ForwardIterator __first, _ForwardIterator __last,
          const _Tp& __value)
      {
        return std::__remove_if(__first, __last,
            __gnu_cxx::__ops::__iter_equals_val(__value));
      }

    template<typename _ForwardIterator, typename _Predicate>
      _ForwardIterator
      __remove_if(_ForwardIterator __first, _ForwardIterator __last,
          _Predicate __pred)
      {
        __first = std::__find_if(__first, __last, __pred);
        if (__first == __last)
          return __first;

        _ForwardIterator __result = __first;
        ++__first;
        for (; __first != __last; ++__first)
          if (!__pred(__first))
          {
            // note
            // this is `assign` but not `swap` which make a difference to own
            // remove does.

            *__result = _GLIBCXX_MOVE(*__first);
            ++__result;
          }
        return __result;
      }

    // /usr/include/c++/4.9/bits/predefined_ops.h
    template<typename _Predicate>
      struct _Iter_pred
      {
        _Predicate _M_pred;

        _Iter_pred(_Predicate __pred)
          : _M_pred(__pred)
        { }

        template<typename _Iterator>
          bool
          operator()(_Iterator __it)
          { return bool(_M_pred(*__it)); }
      };

    template<typename _Predicate>
      inline _Iter_pred<_Predicate>
      __pred_iter(_Predicate __pred)
      { return _Iter_pred<_Predicate>(__pred); }

    template<typename _ForwardIterator, typename _Predicate>
      inline _ForwardIterator
      remove_if(_ForwardIterator __first, _ForwardIterator __last,
          _Predicate __pred)
      {
        return std::__remove_if(__first, __last,
            __gnu_cxx::__ops::__pred_iter(__pred));
      }
  } // namespace


  // same as partition_1() but | unmatched | matched |

  template <typename _Iterator, typename _T>
    _Iterator remove_1(_Iterator begin, _Iterator end, _T value)
    {
      if (begin == end)
        return begin;

      // skip *not* matched elements and begin becomes the first unmatched item.
      // begin is "start of the unmatched"
      // note that begin is increased in "if"
 
      while (*begin != value)
        if (++begin == end)
          return begin;

      _Iterator run = begin;

      // increase first since knows *run is already unmatched.
      while (++run != end)
      {
        // see matched and move it to matched group
        if (*run != value)
        {
          // cannot use "=" since it's not algo-remove
          // std::iter_swap(run, begin);
          *begin = *run;
          ++begin;
        }
      }

      return begin;
    }

  // 1. do the same as algo-partition but from the end. 
  // 2. therefore, do not care about the order of unmatched group.

  template <typename _Iterator, typename _T>
    _Iterator remove_2(_Iterator __begin, _Iterator __end, _T __value)
    {
      _Iterator run = __end - 1;
      _Iterator start_of_remove = __end;

      for (; run > __begin; --run)
      {
        // swap only when element has the same value and swap is necessary
        if ((*run == __value) && (run != start_of_remove-1))
          swap(*run, *(--start_of_remove));
      }

      // swap only when element has the same value and swap is necessary
      if ((*run == __value) && (run != start_of_remove-1))
        swap(*run, *(--start_of_remove));

      return start_of_remove;
    }

} // namespace


TEST(AlgoRemove, Stl)
{
  using namespace algo_remove;

  // algo-remove which is opposite from algo-partition
  // | unmatched | matched |
  
  // coll.erase() delete elements but algo-remove do not.
  {
    std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

    for (auto it = coll.begin(); it != coll.end(); ++it)
    {
      if (*it == 2)
        it = coll.erase(it);
    }

    EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
  }

  {
    std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

    auto end = remove(coll.begin(), coll.end(), 2);

    EXPECT_THAT(distance(end, coll.end()), 4);
    EXPECT_THAT(coll, 
        ElementsAreArray({1,3,4,5,6,7,8,9,2,8,2,9}));

    coll.erase(end, coll.end());
    EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
  }

  // show that algo-remove() do not remove elements
  {
    std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

    remove(coll.begin(), coll.end(), 2);

    // std::vector<int> coll{1,3,4,5,6,7,8,9,2,8,2,9};
    //                                       ^^^^^^^ 

    EXPECT_THAT(coll, ElementsAreArray({1,3,4,5,6,7,8,9,2,8,2,9}));
  }

  // show that remove_if() returns end if not found
  {
    std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

    auto it = remove_if(coll.begin(), coll.end(), 
        [](int value)
        { return value == 10; }
        );

    EXPECT_THAT(it, coll.end());
  }

  // own remove
  {
    std::vector<int> coll{1,2,3,4,5,6,2,7,2,8,2,9};

    auto end = remove_1(coll.begin(), coll.end(), 2);

    EXPECT_THAT(distance(end, coll.end()), 4);
    EXPECT_THAT(coll, 
        ElementsAreArray({1,3,4,5,6,7,8,9,2,8,2,9}));

    coll.erase(end, coll.end());
    EXPECT_THAT(coll, ElementsAre(1,3,4,5,6,7,8,9));
  }
}


// ={=========================================================================
// algo-unique
//
// | matched(unique)  | unmatched |
//                  ^dest
//
// use the dest which is the end of the matched group

namespace algo_unique 
{
  using ITERATOR = vector<int>::iterator;

  // when see two consequtive equal items, return a iterator to the first.
  ITERATOR adjacent_find(ITERATOR first, ITERATOR last)
  {
    if (first == last)
      return last;

    ITERATOR next = first;
    while (++next != last)
    {
      if (*first == *next)
        return first;
      first = next;
    }

    return last;
  }

  // /usr/include/c++/4.9/bits/stl_algo.h

  ITERATOR unique_1(ITERATOR first, ITERATOR last)
  {
    first = adjacent_find(first, last);
    if (first == last)
      return last;

    ITERATOR dest = first;
    ++first;
    while (++first != last)
    {
      // not equal and assign(overwrite). so if equals, keep increase first.
      if (*dest != *first)
        *++dest = *first;
    }

    // one after from the last unique
    return ++dest;
  }

  template <typename _Iterator>
    _Iterator unique_2(_Iterator first, _Iterator last)
    {
      // first = adjacent_find(first, last);
      // if (first == last)
      //   return last;

      _Iterator dest = first;
      // ++first;
      while (++first != last)
      {
        // not equal and assign(overwrite). so if equals, keep increase first.
        if (*dest != *first)
          *++dest = *first;
      }

      // one after from the last unique
      return ++dest;
    }
} // namespace

TEST(AlgoUnique, Unique)
{
  using namespace algo_unique;
  
  // o Both forms collapse `consecutive equal elements` by removing the
  // following duplicates.
  {
    vector<int> coll{1, 4, 4, 6};
    auto pos = unique(coll.begin(), coll.end());
    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 6}));
  }
  {
    vector<int> coll{1, 4, 4, 4, 6};
    auto pos = unique(coll.begin(), coll.end());
    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 6}));
  }

  // o algo-unique() is not perferct
  // The first form removes from the range [beg,end) all elements that are equal
  // to `the previous elements.` Thus, only when 
  //
  // the elements in the sequence are sorted, or at least when all elements of
  // the same value are adjacent, 
  //
  // does it remove all duplicates.
  //
  // o sorted input is not assumed

  {
    list<int> coll{1,2,3,1,2,3,4,4,6,1,2,2,3,1,6,6,6,4,4};

    auto pos = unique(coll.begin(), coll.end());
    EXPECT_THAT(coll, 
        ElementsAreArray(
          {1,2,3,1,2,3,4,6,1,2,3,1,6,4,6,6,6,4,4}));

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, 
        ElementsAreArray(
          {1,2,3,1,2,3,4,6,1,2,3,1,6,4}));
  }

  {
    list<int> coll{1,2,3,1,2,3,4,4,6,1,2,2,3,1,6,6,6,4,4};

    auto pos = unique_2(coll.begin(), coll.end());
    EXPECT_THAT(coll, 
        ElementsAreArray(
          {1,2,3,1,2,3,4,6,1,2,3,1,6,4,6,6,6,4,4}));

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, 
        ElementsAreArray(
          {1,2,3,1,2,3,4,6,1,2,3,1,6,4}));
  }

  // o The second form removes all elements that follow an element e and for
  // which the binary predicate op(e,elem) yields true. In other words, the
  // predicate is not used to compare an element with its predecessor; the
  // element is compared with the previous element that was not removed (see the
  // following examples).

  // For example, the first 6 is greater than the following 1, 2, 2, 3, and 1,
  // so all these elements are removed. In other words, the predicate is not
  // used to compare an element with its predecessor; the element is compared
  // with the previous element that was not removed 
  {
    list<int> coll{1, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 5, 7, 5, 4, 4};

    auto pos = unique(coll.begin(), coll.end(), greater<int>());
    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 4, 6, 6, 6, 6, 7}));
  }

  {
    string input{"1   2  3            4           "};
    EXPECT_THAT(input, "1   2  3            4           ");

    auto new_end = unique(input.begin(), input.end(), [](const char &x, const char &y) {
      return x == y and x == ' ';
    });

    input.erase(new_end, input.end());
    EXPECT_THAT(input, "1 2 3 4 ");
  }


  // o Both forms collapse `consecutive equal elements` by removing the
  // following duplicates.
  {
    vector<int> coll{1, 4, 4, 6};
    auto pos = unique_1(coll.begin(), coll.end());
    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 6}));
  }
  {
    vector<int> coll{1, 4, 4, 4, 6};
    auto pos = unique_1(coll.begin(), coll.end());
    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, ElementsAreArray({1, 4, 6}));
  }

  // o sorted input is not assumed
  {
    vector<int> coll{1, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 5, 7, 5, 4, 4};

    auto pos = unique_1(coll.begin(), coll.end());
    EXPECT_THAT(coll, 
        ElementsAreArray({1, 4, 6, 1, 2, 3, 1, 6, 5, 7, 5, 4, 5, 7, 5, 4, 4}));

    coll.erase(pos, coll.end());
    EXPECT_THAT(coll, 
        ElementsAreArray({1, 4, 6, 1, 2, 3, 1, 6, 5, 7, 5, 4}));
  }
}


// ={=========================================================================
// algo-remove algo-leetcode-8
/*
26. Remove Duplicates from Sorted Array, Easy

Given a sorted array nums, remove the duplicates in-place such that each element
appear only once and return *the new length.*

Do not allocate extra space for another array, you must do this by modifying the
input array in-place with O(1) extra memory.

Example 1:

Given nums = [1,1,2],

Your function should return length = 2, with the first two elements of nums
being 1 and 2 respectively.

It doesn't matter what you leave beyond the returned length.

Example 2:

Given nums = [0,0,1,1,1,2,2,3,3,4],

Your function should return length = 5, with the first five elements of nums
being modified to 0, 1, 2, 3, and 4 respectively.

It doesn't matter what values are set beyond the returned length.

Clarification:

Confused why the returned value is an integer but your answer is an array?

Note that the input array is passed in by reference, which means modification to
the input array will be known to the caller as well.

Internally you can think of this:

// nums is passed in by reference. (i.e., without making a copy)
int len = removeDuplicates(nums);

// any modification to nums in your function would be known by the caller.
// using the length returned by your function, it prints the first len elements.

for (int i = 0; i < len; i++) {
    print(nums[i]);
}

*/

namespace leetcode_easy_008
{
  // input num is sorted (ascending)
  // the idea is to move the duplicates to the end of right using swap
  // don't need to check if run see smaller value since it's sorted input

  int RemoveDuplicates_01(vector<int> &nums)
  {
    if (nums.empty())
      return 0;

    int value = nums[0];
    size_t i{};

    for (i = 1; i < nums.size(); ++i)
    {
      int run = nums[i];

      // cout << "for i: " << i << ", run: " << run 
      //   << ", value: " << value << endl;

      // update current max when current value is bigger
      if (run > value)
        value = run;
      // ends when see smaller and means reaches the the new end
      else if(run < value)
      {
        // cout << "break i: " << i << endl;
        break;
      }
      // when run == value, swap it to tne end.
      else
      {
        for (size_t s = i; s < nums.size()-1; ++s)
          swap(nums[s], nums[s+1]);

        // since nums[i] is swapped, it may be bigger

        if (nums[i] > value)
          value = nums[i];
      }
    }

    // cout << "return i: " << i << endl;

    return i;
  }

  // o the key idea is to swap to the left
  // o no repeated swap until see the new end. single swap is enough 
  // o swap() should be done after updating current_max
  //
  // o end is index but shold return len so +1 -> revised. As algo-partition,
  // end represet start of not-interested, that is end of interested group + 1.
  // so no need to +1. have to think about swap on the same index
  //
  // | ... |end ...|

  // Runtime: 24 ms, faster than 100.00% of C++ online submissions for Remove
  // Duplicates from Sorted Array.
  //
  // Memory Usage: 11 MB, less than 24.81% of C++ online submissions for Remove
  // Duplicates from Sorted Array.

  int RemoveDuplicates_02(vector<int> &nums)
  {
    if (nums.empty())
      return 0;

    int current_max = nums[0];
    size_t end{1};

    for (size_t i = 1; i < nums.size(); ++i)
    {
      if (nums[i] > current_max)
      {
        current_max = nums[i];

        // to avoid swap on the same index
        if (end != i)
          swap(nums[end], nums[i]);

        ++end;
      }
    }

    return end;
  }
  
  using ITERATOR = vector<int>::iterator;

  ITERATOR adjacent_find(ITERATOR first, ITERATOR last)
  {
    if (first == last)
      return last;

    auto next = first;
    while (++next != last)
    {
      // found two consecutive items
      if (*first == *next)
        return first;

      first = next;
    }

    // no two consecutive items found.
    return last;
  }

  // algo-unique, same as unique_1(), works on not-sorted input.
  // `end` fro adjacent_find() is end of the interested group
  // | ... end| ...|

  int RemoveDuplicates_03(vector<int> &nums)
  {
    auto first = nums.begin();
    auto last = nums.end();

    auto end = adjacent_find(first, last);

    // means empty or no duplicates
    if (end == last)
      return 0;

    auto run = end;

    while (++run != last)
    {
      // see different item
      if (*end != *run)
        *++end = *run;
    }

    return distance(first, end) + 1;
  }

  int RemoveDuplicates_04(vector<int> &nums)
  {
    if (nums.empty())
      return 0;

    size_t unique_end{0};
    size_t current{1};
    size_t size = nums.size() - 1;

    while (current <= size)
    {
      if (nums[unique_end] == nums[current])
      {
        ++current;
      }
      else
      {
        ++unique_end;

        // to avoid assign on the same index
        if (unique_end != current)
          nums[unique_end] = nums[current];

        ++current;
      }
    }

    // +1 since unique_end is index but return length
    return unique_end + 1;
  }

} // namespace

TEST(AlgoUnique, LeetCode_Easy_008_RemoveDuplicates)
{
  using namespace leetcode_easy_008;

  // okay
  {
    const auto func = RemoveDuplicates_01;

    vector<int> coll{1,1,2};
    auto len = func(coll);

    EXPECT_THAT(len, 2);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1,2));
  }

  // fails
  {
    const auto func = RemoveDuplicates_01;

    vector<int> coll{0,0,1,1,1,2,2,3,3,4};
    auto len = func(coll);

    EXPECT_THAT(len, Not(5));

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, Not(ElementsAre(0,1,2,3,4)));
  }

  {
    const auto func = RemoveDuplicates_02;

    vector<int> coll{1,1,2};
    auto len = func(coll);

    EXPECT_THAT(len, 2);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1,2));
  }

  {
    const auto func = RemoveDuplicates_02;

    vector<int> coll{0,0,1,1,1,2,2,3,3,4};
    auto len = func(coll);

    EXPECT_THAT(len, 5);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(0,1,2,3,4));
  }

  // same ex as AlgoUnique
  {
    const auto func = RemoveDuplicates_03;

    vector<int> coll{1, 4, 4, 6};
    auto len = func(coll);

    EXPECT_THAT(len, 3);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1,4,6));
  }
  {
    const auto func = RemoveDuplicates_03;

    vector<int> coll{1, 4, 4, 4, 6};
    auto len = func(coll);

    EXPECT_THAT(len, 3);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(1,4,6));
  }
  {
    const auto func = RemoveDuplicates_03;

    vector<int> coll{1, 4, 4, 6, 1, 2, 2, 3, 1, 6, 6, 6, 5, 7, 5, 4, 4};

    auto len = func(coll);

    EXPECT_THAT(coll, 
        ElementsAreArray({1,4,6,1,2,3,1,6,5,7,5,4,5,7,5,4,4}));
    EXPECT_THAT(len, 12);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, 
        ElementsAreArray({1,4,6,1,2,3,1,6,5,7,5,4}));
  }
}


// ={=========================================================================
// algo-remove algo-leetcode-9
/*
27. Remove Element, Easy

Given an array nums and a value val, remove all instances of that value in-place
and return the new length.

Do not allocate extra space for another array, you must do this by modifying the
input array in-place with O(1) extra memory.

The order of elements can be changed. It doesn't matter what you leave beyond
the new length.

Example 1:

Given nums = [3,2,2,3], val = 3,

Your function should return length = 2, with the first two elements of nums
being 2.

It doesn't matter what you leave beyond the returned length.

Example 2:

Given nums = [0,1,2,2,3,0,4,2], val = 2,

Your function should return length = 5, with the first five elements of nums
containing 0, 1, 3, 0, and 4.

Note that the order of those five elements can be arbitrary.

It doesn't matter what values are set beyond the returned length.

Clarification:

Confused why the returned value is an integer but your answer is an array?

Note that the input array is passed in by reference, which means modification to
the input array will be known to the caller as well.

Internally you can think of this:

// nums is passed in by reference. (i.e., without making a copy)
int len = removeElement(nums, val);

// any modification to nums in your function would be known by the caller.
// using the length returned by your function, it prints the first len elements.
for (int i = 0; i < len; i++) {
    print(nums[i]);
}

*/

namespace leetcode_easy_009
{
  // o Unlike RemoveDuplicates_02, end is not index but index+1

  // Runtime: 4 ms, faster than 100.00% of C++ online submissions for Remove
  // Element.
  //
  // Memory Usage: 9.3 MB, less than 56.68% of C++ online submissions for Remove
  // Element.

  int RemoveIf_01(vector<int> &nums, int val)
  {
    if (nums.empty())
      return 0;

    size_t end{};

    for (size_t i = 0; i < nums.size(); ++i)
    {
      if (nums[i] != val)
      {
        if (end != i)
          swap(nums[end], nums[i]);

        ++end;
      }
    }

    return end;
  }

} // namespace

TEST(AlgoRemove, LeetCode_Easy_009_RemoveIf)
{
  using namespace leetcode_easy_009;

  {
    const auto func = RemoveIf_01;

    vector<int> coll{3,2,2,3};
    auto len = func(coll, 3);

    EXPECT_THAT(len, 2);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(2,2));
  }

  {
    const auto func = RemoveIf_01;

    vector<int> coll{0,1,2,2,3,0,4,2};
    auto len = func(coll, 2);

    EXPECT_THAT(len, 5);

    vector<int> result{};

    for (int i = 0; i < len; ++i)
      result.push_back(coll[i]);

    EXPECT_THAT(result, ElementsAre(0,1,3,0,4));
  }
}


// ={=========================================================================
// algo-rotate, algo-slide, algo-reverse

TEST(AlgoRotate, Rotate)
{
  vector<int> coll{1,2,3,4,5,6,7,8};

  // rotate one to the left
  // before *cxx-11* void rotate() so comment out 
  // auto pos = rotate(

  rotate(
    coll.begin(),     // begin  
    coll.begin()+1,   // new begin
    coll.end()        // end
  );
  EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));

  // return the new position of the (pervious) first element.
  // EXPECT_THAT(*pos, 1);

  // pos = rotate(

  rotate(
    coll.begin(),
    coll.end()-2,
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(8,1,2,3,4,5,6,7));
  // EXPECT_THAT(*pos, 2);

  // rotate so that 4 is the beginning
  // pos = rotate(

  rotate(
    coll.begin(),
    find(coll.begin(), coll.end(), 4),
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(4,5,6,7,8,1,2,3));
  // EXPECT_THAT(*pos, 8);
}


// 1. do not use additional space
// 2. slide down sub group, [ne, e}
// 3. use of for loop count
//
// /usr/include/c++/4.9.2/bits/stl_algo.h
//
// /// This is a helper function for the rotate algorithm.
// template<typename _ForwardIterator>
//   _ForwardIterator
//   __rotate(_ForwardIterator __first,
//      _ForwardIterator __middle,
//      _ForwardIterator __last,
//      forward_iterator_tag)
// {}

//         ne           e
// 1  2  3 [4  5  6  7]     #1 move
//       4  3
//          5  3
//             6  3
//       ne       7  3
//    ne[4  5  6  7] 3      #2 move
//   [4  5  6  7] 2  3      #3 move
// 4  5  6  7] 1  2  3
// 
// use reverse:
//
// 1  2  3 [4  5  6  7]
// 3  2  1 [7  6  5  4]
// 4  5  6  7  1  2  3


namespace algo_rotate 
{
  template <typename _Iterator>
    void rotate_1(_Iterator __begin, _Iterator __new_begin, _Iterator __end)
    {
      if ((__begin == __new_begin) || (__end == __new_begin))
        return;

      auto num_swap = std::distance(__new_begin, __end);

      for (;__new_begin != __begin; --__new_begin)
      {
        _Iterator start = __new_begin;

        for (int i = 0; i < num_swap; ++i)
        {
          swap(*start, *(start-1));
          ++start;
        }
      }
    }

  // algo-rotate that use algo-reverse()
  // void
  // reverse (BidirectionalIterator beg, BidirectionalIterator end)

  template <typename _Iterator>
    void rotate_2(_Iterator begin, _Iterator new_begin, _Iterator end)
    {
      std::reverse(begin, new_begin);
      std::reverse(new_begin, end);
      std::reverse(begin, end);
    }
} // namespace

TEST(AlgoRotate, Rotate_1)
{
  using namespace algo_rotate; 

  {
    vector<int> coll{1,2,3,4,5,6,7,8};

    // cannot use this since it's template
    // auto func = rotate_1;

    // rotate one to the left
    rotate_1(
        coll.begin(),     // begin  
        coll.begin()+1,   // new begin
        coll.end()        // end
        );
    EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));

    rotate_1(
        coll.begin(),
        coll.end()-2,
        coll.end()
        );
    EXPECT_THAT(coll, ElementsAre(8,1,2,3,4,5,6,7));

    rotate_1(
        coll.begin(),
        find(coll.begin(), coll.end(), 4),
        coll.end()
        );
    EXPECT_THAT(coll, ElementsAre(4,5,6,7,8,1,2,3));
  }

  {
    vector<int> coll{1,2,3,4,5,6,7,8};
    // auto func = rotate_2;

    // rotate one to the left
    rotate_2(
        coll.begin(),     // begin  
        coll.begin()+1,   // new begin
        coll.end()        // end
        );
    EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));

    rotate_2(
        coll.begin(),
        coll.end()-2,
        coll.end()
        );
    EXPECT_THAT(coll, ElementsAre(8,1,2,3,4,5,6,7));

    rotate_2(
        coll.begin(),
        find(coll.begin(), coll.end(), 4),
        coll.end()
        );
    EXPECT_THAT(coll, ElementsAre(4,5,6,7,8,1,2,3));
  }
}


// algo-leetcode-189
/* 189. Rotate Array, Easy

Given an array, rotate the array to the right by k steps, where k is
non-negative.

Example 1:

Input: [1,2,3,4,5,6,7] and k = 3
Output: [5,6,7,1,2,3,4]

Explanation:
rotate 1 steps to the right: [7,1,2,3,4,5,6]
rotate 2 steps to the right: [6,7,1,2,3,4,5]
rotate 3 steps to the right: [5,6,7,1,2,3,4]

Example 2:

Input: [-1,-100,3,99] and k = 2
Output: [3,99,-1,-100]

Explanation: 
rotate 1 steps to the right: [99,-1,-100,3]
rotate 2 steps to the right: [3,99,-1,-100]

Note:
Try to come up as many solutions as you can, there are at least 3 different ways
to solve this problem.  Could you do it in-place with O(1) extra space?

*/

namespace leetcode_easy_189
{
  // k poins to new end which is size()-k in pos

  void rotate(vector<int>& nums, int k) 
  {
    int new_begin = nums.size() - k;

    // note that must be > 0 but not >= 0
    for(; new_begin > 0; --new_begin)
    {
      for(int index = new_begin, i = 0; i < k; ++i, ++index)
        swap(nums[index-1], nums[index]);
    }
  }

  // index :  0   1   2   3   4   5   6
  // arr   :  1   2   3   4   5   6   7
  // k     :  7   6   5   4   3   2   1
  //          14  13  12  11  10  9   8
  //          21  20  19  18  17  16  15
  //
  // so except cases that falls to index 0 (% makes 0), others works with "%
  // size"
  // 
  // However, it's too slow

  void rotate_1(vector<int>& nums, int k) 
  {
    // as with condition check in algo-rotate, no need to when it rotates to
    // itself.

    int count = k % nums.size();

    if (count == 0)
      return;

    int new_begin = nums.size() - count;

    // note that must be > 0 but not >= 0
    for(; new_begin > 0; --new_begin)
    {
      for(int index = new_begin, i = 0; i < count; ++i, ++index)
        swap(nums[index-1], nums[index]);
    }
  }

  // why faster? less swap() operations.
  //
  // Runtime: 16 ms, faster than 100.00% of C++ online submissions for Rotate
  // Array.
  //
  // Memory Usage: 9.6 MB, less than 35.88% of C++ online submissions for Rotate
  // Array.

  void rotate_2(vector<int>& nums, int k) 
  {
    // as with condition check in algo-rotate, no need to when it rotates to
    // itself.

    int count = k % nums.size();

    if (count == 0)
      return;

    int new_begin = nums.size() - count;

    std::reverse(nums.begin(), nums.begin()+new_begin);
    std::reverse(nums.begin()+new_begin, nums.end());
    std::reverse(nums.begin(), nums.end());
  }
} // namespace


TEST(AlgoRotate, LeetCode_Easy_189)
{
  using namespace leetcode_easy_189;

  {
    vector<int> coll{1,2,3,4,5,6,7,8};

    // cannot use this since it's template
    // auto func = rotate_1;

    // rotate one to the left
    // rotate( 
    //     coll.begin(),     // begin  
    //     coll.begin()+1,   // new begin
    //     coll.end()        // end
    //     );
    rotate(coll, 7); 
    EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));

    // rotate(
    //     coll.begin(),
    //     coll.end()-2,
    //     coll.end()
    //     );
    rotate(coll, 2);
    EXPECT_THAT(coll, ElementsAre(8,1,2,3,4,5,6,7));

    // rotate(
    //     coll.begin(),
    //     find(coll.begin(), coll.end(), 4),
    //     coll.end()
    //     );
    rotate(coll, 4);
    EXPECT_THAT(coll, ElementsAre(4,5,6,7,8,1,2,3));
  }

  {
    vector<int> coll{1,2,3,4,5,6,7};
    rotate(coll, 3);
    EXPECT_THAT(coll, ElementsAre(5,6,7,1,2,3,4));
  }
  {
    vector<int> coll{-1,-100,3,99};
    rotate(coll, 2);
    EXPECT_THAT(coll, ElementsAre(3,99,-1,-100));
  }

  // fails since assumes new begin is within [begin, end)
  {
    vector<int> coll{1,2};
    rotate(coll, 3);
    EXPECT_THAT(coll, Not(ElementsAre(2,1)));
  }
  {
    vector<int> coll{1,2};
    rotate_1(coll, 3);
    EXPECT_THAT(coll, ElementsAre(2,1));
  }
  {
    vector<int> coll{1,2};
    rotate_1(coll, 99);
    EXPECT_THAT(coll, ElementsAre(2,1));
  }

  {
    vector<int> coll{1,2,3,4,5,6,7,8};
    rotate_2(coll, 7); 
    EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));
  }
}


// ={=========================================================================
// algo-sort-quick

namespace algo_sort_quick
{

// #define SORT_QUICK_DEBUG

int build_partition(vector<int> &coll, int first, int last)
{
  int pivot_pos = (first + last) / 2;
  int pivot_value = coll[pivot_pos];

#ifdef SORT_QUICK_DEBUG
    cout << "pivot_pos(" << pivot_pos << ") build before(" << first << ", " << last << "): ";

    for (int i = first; i <= last; ++i)
      cout << coll[i] << ", ";

    cout << endl;
#endif // SORT_QUICK_DEBUG

  // move the pivot to the first pos
  swap(coll[first], coll[pivot_pos]);

  int last_small = first;

  for (int inspect = first + 1; inspect <= last; ++inspect)
  {
    // *cxx-less*
    if (coll[inspect] < pivot_value)
    {
      ++last_small;

      // last_small == inspect case does happens and it is enhancement from the
      // previous code

      if (last_small != inspect)
      {
        cout << "swap(coll[" << coll[last_small] << "], coll[" << coll[inspect] << "]);" << endl;
        swap(coll[last_small], coll[inspect]);
      }
    }
  }

  // move the pivot back
  swap(coll[first], coll[last_small]);

#ifdef SORT_QUICK_DEBUG
    cout << "pivot_pos(" << pivot_pos << ") build after (" << first << ", " << last << "): ";

    for (int i = first; i <= last; ++i)
      cout << coll[i] << ", ";

    cout << endl;
#endif // SORT_QUICK_DEBUG

  return last_small;
}

int build_partition_descending(vector<int> &coll, int first, int last)
{
  int pivot_pos = (first + last) / 2;
  int pivot_value = coll[pivot_pos];

  // move the pivot to the first pos
  swap(coll[first], coll[pivot_pos]);

  int last_small = first;

  for (int inspect = first + 1; inspect <= last; ++inspect)
  {
    // *cxx-greater*
    if (coll[inspect] > pivot_value)
    {
      ++last_small;

      // last_small == inspect case does happens and it is enhancement from the
      // previous code

      if (last_small != inspect)
      {
        swap(coll[last_small], coll[inspect]);
      }
    }
  }

  // move the pivot back
  swap(coll[first], coll[last_small]);

  return last_small;
}

void sort_quick_01(vector<int> &coll, int first, int last)
{
  int last_small{};

  if (first < last)
  {
    last_small = build_partition(coll, first, last); 
    sort_quick_01(coll, first, last_small-1);
    sort_quick_01(coll, last_small+1, last);
  }
}

void sort_quick_01_descending(vector<int> &coll, int first, int last)
{
  int last_small{};

  if (first < last)
  {
    last_small = build_partition_descending(coll, first, last); 
    sort_quick_01_descending(coll, first, last_small-1);
    sort_quick_01_descending(coll, last_small+1, last);
  }
}

// from ansic, p87. exactly same way.
void sort_quick_02(vector<int> &v, int left, int right)
{
  int i, last;

  // do nothing if array contains fewer than two elements
  if( left >= right )
    return;

  // move partition elem
  // swap( v, left, (left+right)/2 );
  swap( v[left], v[(left+right)/2]);

  last = left;  // to v[0]

  // partition
  for(i = left+1; i <= right; i++)
    if( v[i] < v[left] )
    {
      // swap( v, ++last, i );   // shall ++last
      swap(v[++last], v[i]);   // shall ++last
    }

  // restore partition elem
  // swap(v, left, last);
  swap(v[left], v[last]);

  sort_quick_02( v, left, last-1 );
  sort_quick_02( v, last+1, right );
}

} // namespace

TEST(AlgoSort, Quick)
{
  using namespace algo_sort_quick;

  // pivot_pos(3) build before(0, 6): 29, 33, 35, |26|, 19, 12, 22,
  // pivot_pos(3) build after (0, 6): 22, 19, 12, 26, 33, 35, 29,
  // pivot_pos(1) build before(0, 2): 22, |19|, 12,
  // pivot_pos(1) build after (0, 2): 12, 19, 22,
  // pivot_pos(5) build before(4, 6):                 33, |35|, 29,
  // pivot_pos(5) build after (4, 6):                 29, 33, 35,
  // pivot_pos(4) build before(4, 5):                 |29|, 33,
  // pivot_pos(4) build after (4, 5):                 29, 33,
  //                                  12  19  22  26  29  33  35

  {
    vector<int> coll{29, 33, 35, 26, 19, 12, 22};

    sort_quick_01(coll, 0, coll.size()-1);

    EXPECT_THAT(coll, 
        ElementsAre(12, 19, 22, 26, 29, 33, 35));
  }

  {
    vector<int> coll{29, 33, 35, 26, 19, 12, 22};

    sort_quick_01_descending(coll, 0, coll.size()-1);

    EXPECT_THAT(coll, 
        ElementsAre(35, 33, 29, 26, 22, 19, 12));
  }

  // build i(0, 12): 30, 2, 31, 5, 33, 6, 12, 10, 13, 15, 17, 29, 6,
  // build o(0, 12): 6, 2, 5, 6, 10, 12, 30, 33, 13, 15, 17, 29, 31,
  // build i(0, 4): 6, 2, 5, 6, 10,
  // build o(0, 4): 2, 5, 6, 6, 10,
  // build i(2, 4): 6, 6, 10,
  // build o(2, 4): 6, 6, 10,
  // build i(3, 4): 6, 10,
  // build o(3, 4): 6, 10,
  // build i(6, 12): 30, 33, 13, 15, 17, 29, 31,
  // build o(6, 12): 13, 15, 33, 30, 17, 29, 31,
  // build i(8, 12): 33, 30, 17, 29, 31,
  // build o(8, 12): 17, 30, 33, 29, 31,
  // build i(9, 12): 30, 33, 29, 31,
  // build o(9, 12): 31, 30, 29, 33,
  // build i(9, 11): 31, 30, 29,
  // build o(9, 11): 29, 30, 31,

  {
    vector<int> coll{30, 2, 31, 5, 33, 6, 12, 10, 13, 15, 17, 29, 6};

    sort_quick_01(coll, 0, coll.size()-1);

    EXPECT_THAT(coll, 
        ElementsAreArray({2, 5, 6, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }

  // when input is already sorted
  //
  // build i(0, 12): 2, 5, 6, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33,
  // build o(0, 12): 2, 5, 6, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33,
  // build i(0, 5): 2, 5, 6, 6, 10, 12,
  // build o(0, 5): 2, 5, 6, 6, 10, 12,
  // build i(0, 1): 2, 5,
  // build o(0, 1): 2, 5,
  // build i(3, 5): 6, 10, 12,
  // build o(3, 5): 6, 10, 12,
  // build i(7, 12): 15, 17, 29, 30, 31, 33,
  // build o(7, 12): 15, 17, 29, 30, 31, 33,
  // build i(7, 8): 15, 17,
  // build o(7, 8): 15, 17,
  // build i(10, 12): 30, 31, 33,
  // build o(10, 12): 30, 31, 33,

  {
    vector<int> coll{2, 5, 6, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};

    sort_quick_01(coll, 0, coll.size()-1);

    EXPECT_THAT(coll, 
        ElementsAreArray({2, 5, 6, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }
}


// ={=========================================================================
// algo-sort-heap

namespace algo_sort_heap
{
  // From 'Programming Pearls' by Jon Bentley, 14 Heap
  //
  // pre  : heap(1, n-1)
  // post : heap(1, n)
  // 
  // void siftup(n)
  // {
  //   int i = n;
  // 
  // loop:
  //   // invariant: heap(1, n) except between i and its parent
  //   
  //   // terminates when reaches the root
  //   if (i == 1)
  //     break;
  //
  //   p = i / 2;
  //
  //   // terminates when it meets heap property
  //   if (x[i] >= x[p])
  //     break;
  // 
  //   swap(i, p);
  // 
  //   i = p;
  // }


  // while version
  void siftup_01(vector<int> &coll, int n)
  {
    int i = n;
    int p = i / 2;

    for (; (i != 1) && (coll[i] <= coll[p]);)
    {
      swap(coll[i], coll[p]);
      i = p;
      p = i / 2;
    }
  }

  void siftup_02(vector<int> &coll, int n)
  {
    int i = n;
    int p = i / 2;

    while (i != 1 && coll[i] <= coll[p])
    {
      swap(coll[i], coll[p]);
      i = p;
      p = i / 2;
    }
  }

  // // for version, (cxx-error) 
  // void siftup_01(vector<int> &coll, int n)
  // {
  //   int i = n;
  //   int p = i / 2;
  // 
  //   for (; (i != 1) && (coll[i] <= coll[p]); i = p)
  //   {
  //     swap(coll[i], coll[p]);
  //     p = i / 2;
  //   }
  // 
  //   // effectively this and should update p after i so this is an error
  //   //
  //   // for (; (i != 1) && (coll[i] <= coll[p]);)
  //   // {
  //   //   swap(coll[i], coll[p]);
  //   //   p = i / 2;
  //   //   i = p;
  //   // }
  // }

  // // for version, (cxx-error) 
  // void siftup_01_error(vector<int> &coll, int n)
  // {
  //   int i = n;
  //   int p = i / 2;
  // 
  //   // for (; (i != 1) && (coll[i] <= coll[p]); i = p)
  //   // {
  //   //   p = i / 2;
  //   //   swap(coll[i], coll[p]);
  //   // }
  // 
  //   // effectively this and cause error since has different pair of i and p in
  //   // check of the lopp.
  //   for (; (i != 1) && (coll[i] <= coll[p]);)
  //   {
  //     p = i / 2;
  //     swap(coll[i], coll[p]);
  //     i = p;
  //   }
  // }

  // for version from text
  void siftup_min_text(vector<int> &coll, int n)
  {
    int i, p;
    for (i = n; i > 1 && coll[p = i/2] > coll[i]; i = p)
      swap(coll[i], coll[p]);
  }


  // pre  : heap(2, n)
  // post : heap(1, n)
  // 
  // void siftdown(n)
  // {
  //   int i = 1;
  // 
  // loop:
  //   // invariant: heap(1, n) except between i and its children(0, 1, or 2)
  //   
  //   // c is index of the left child
  //   c = i * 2;
  //
  //   // terminates when no children
  //   if (c > n)
  //     break;
  // 
  //   // when the right child is lesser
  //   if (c+1 <= n)
  //    if (x[c] > x[c+1])
  //      c++;
  //
  //   // terminates when it meets heap property
  //   if (x[i] <= x[c])
  //     break;
  // 
  //   swap(i, c);
  // 
  //   i = c;
  // }

  void siftdown_01(vector<int> &coll, int n)
  {
    int i = 1;
    int c = i * 2;

    if (c+1 <= n)
      if (coll[c] > coll[c+1])
        c++;

    while ((c < n) && (coll[i] > coll[c]))
    {
      swap(coll[i], coll[c]);

      i = c;

      c = i * 2;

      if (c+1 <= n)
        if (coll[c] > coll[c+1])
          c++;
    }
  }

  // void siftdown_01(vector<int> &coll, int n)
  // {
  //   int i = 1;
  //   int c = i * 2;
  // 
  //   if (c+1 <= n)
  //     if (coll[c] > coll[c+1])
  //       c++;
  // 
  //   for (; (c < n) && (coll[i] > coll[c]); i = c)
  //   {
  //     swap(coll[i], coll[c]);
  // 
  //     if (c+1 <= n)
  //       if (coll[c] > coll[c+1])
  //         c++;
  // 
  //     c = i * 2;
  //   }
  // }


  void siftdown_min_text(vector<int> &coll, int n)
  {
    int c;

    for (int i = 1; (c = i*2) <= n; i = c)
    {
      if (c+1 <= n && coll[c] > coll[c+1])
        c++;

      if (coll[i] <= coll[c])
        break;

      swap(coll[i], coll[c]);
    }
  }
} // namespace

TEST(AlgoSort, HeapSiftUp)
{
  using namespace algo_sort_heap;

  {
    // not use coll[0], 13 elements 
    vector<int> coll{0, 12, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    // +1, 14 elements
    coll.push_back(13);

    // range is [1, 13]
    siftup_01(coll, 13);

    //                   {0, 12,/20, 13,/29, 23, 15, 22,/35, 40, 26, 51, 19, 17}
    EXPECT_THAT(coll, 
        ElementsAreArray({0, 12, 20, 13, 29, 23, 15, 22, 35, 40, 26, 51, 19, 17}));
  }

  {
    // not use coll[0], 13 elements 
    vector<int> coll{0, 12, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    // +1, 14 elements
    coll.push_back(13);

    siftup_min_text(coll, 13);

    //                   {0, 12,/20, 13,/29, 23, 15, 22,/35, 40, 26, 51, 19, 17}
    EXPECT_THAT(coll, 
        ElementsAreArray({0, 12, 20, 13, 29, 23, 15, 22, 35, 40, 26, 51, 19, 17}));
  }
}

TEST(AlgoSort, HeapSiftDown)
{
  using namespace algo_sort_heap;

  {
    // when did x[1] = 18
    vector<int> coll{0, 18, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    siftdown_01(coll, coll.size()-1);
    EXPECT_THAT(coll,
        ElementsAreArray({0,15,20,17,29,23,18,22,35,40,26,51,19}));
  }
  {
    // when did x[1] = 18
    vector<int> coll{0, 18, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    siftdown_min_text(coll, coll.size()-1);
    EXPECT_THAT(coll,
        ElementsAreArray({0,15,20,17,29,23,18,22,35,40,26,51,19}));
  }
}

namespace algo_sort_heap
{
  // template <typename T>
  // class PriQueue {
  //   public:
  //     PriQueue(int m)
  //     {
  //       maxsize_ = m;
  //       x_ = new T[maxsize_ +1];
  //       n_ = 0;
  //     }
  // 
  //     void insert(T t)
  //     {
  //       if (n_ >= maxsize_)
  //         error;
  // 
  //       x_[++n] = t;
  // 
  //       // pre : heap(1, n-1)
  //       siftup();
  //       // post: heap(1, n)
  //     }
  // 
  //     int extractmin()
  //     {
  //       T t;
  // 
  //       if (n_ < 1)
  //         error;
  // 
  //       t = x_[1];
  // 
  //       x_[1] = x[n--];
  // 
  //       // pre : heap(2, n)
  //       siftdown();
  //       // post: heap(1, n)
  // 
  //       return t;
  //     }
  // 
  //   private:
  //     int n_, maxsize_;
  //     T *x_;
  // };

  template <typename T>
    class PriQueue {
      public:
        PriQueue(int size)
        {
          maxsize_ = size;
          coll_ = new T[maxsize_ +1];
          n_ = 0;
        }
        ~PriQueue()
        {
          if (coll_)
            delete coll_;
          n_ = 0;
        }

        void insert(T value)
        {
          int parent{};

          // error
          if (n_ >= maxsize_)
            return;

          coll_[++n_] = value;

          // heap(1, n-1)
          // siftup()
          // heap(1, n)
          // siftup(n) 
          for (int i = n_; i > 1 && coll_[i] < coll_[parent = i/2]; i = parent) 
            swap(coll_[i], coll_[parent]);
        }

        int extractmin()
        {
          T min_value{};
          int child{};

          // error
          if (n_ < 1)
            return -1;

          min_value = coll_[1];

          coll_[1] = coll_[n_--];

          // heap(2, n)
          // siftdown(n)
          // heap(1,n)

          for (int i = 1; (child = i*2) <= n_; i = child)
          {
            if (child+1 <= n_ && coll_[child] > coll_[child+1])
              child++;

            if (coll_[i] <= coll_[child])
              break;

            swap(coll_[i], coll_[child]);
          }

          return min_value;
        }

      private:
        int n_, maxsize_;
        T *coll_;
    };


  // simply use `opposite` comparisons
  template <typename T>
    class PriQueueMax {
      public:
        PriQueueMax(int size)
        {
          maxsize_ = size;
          coll_ = new T[maxsize_ +1];
          n_ = 0;
        }
        ~PriQueueMax()
        {
          if (coll_)
            delete coll_;
          n_ = 0;
        }

        void insert(T value)
        {
          int parent{};

          // error
          if (n_ >= maxsize_)
            return;

          coll_[++n_] = value;

          // heap(1, n-1)
          // siftup()
          // heap(1, n)
          // siftup(n) 
          for (int i = n_; i > 1 && coll_[i] > coll_[parent = i/2]; i = parent) 
            swap(coll_[i], coll_[parent]);
        }

        int extractmin()
        {
          T min_value{};
          int child{};

          // error
          if (n_ < 1)
            return -1;

          min_value = coll_[1];

          coll_[1] = coll_[n_--];

          // heap(2, n)
          // siftdown(n)
          // heap(1,n)

          for (int i = 1; (child = i*2) <= n_; i = child)
          {
            if (child+1 <= n_ && coll_[child] < coll_[child+1])
              child++;

            if (coll_[i] >= coll_[child])
              break;

            swap(coll_[i], coll_[child]);
          }

          return min_value;
        }

      private:
        int n_, maxsize_;
        T *coll_;
    };


  template <typename T, typename F = std::less<T>>
    class PriQueueTemplate {
      public:
        PriQueueTemplate(int size)
        {
          maxsize_ = size;
          coll_ = new T[maxsize_ +1];
          n_ = 0;
          f_ = F();
        }
        ~PriQueueTemplate()
        {
          if (coll_)
            delete coll_;
          n_ = 0;
        }

        void insert(T value)
        {
          int parent{};

          // error
          if (n_ >= maxsize_)
            return;

          coll_[++n_] = value;

          // heap(1, n-1)
          // siftup()
          // heap(1, n)
          // siftup(n) 
          for (int i = n_; i > 1 && f_(coll_[i], coll_[parent = i/2]); i = parent) 
            swap(coll_[i], coll_[parent]);
        }

        int extractmin()
        {
          T min_value{};
          int child{};

          // error
          if (n_ < 1)
            return -1;

          min_value = coll_[1];

          coll_[1] = coll_[n_--];

          // heap(2, n)
          // siftdown(n)
          // heap(1,n)

          for (int i = 1; (child = i*2) <= n_; i = child)
          {
            if (child+1 <= n_ && !f_(coll_[child], coll_[child+1]))
              child++;

            if (f_(coll_[i], coll_[child]) || coll_[i] == coll_[child])
              break;

            swap(coll_[i], coll_[child]);
          }

          return min_value;
        }

      private:
        int n_, maxsize_;
        T *coll_;
        F f_;
    };

} // namespace


TEST(AlgoSort, HeapPriQueue)
{
  using namespace algo_sort_heap;

  // min
  {
    vector<int> coll;

    int n = 10;
    PriQueue<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(i);

    for (int i = 0; i < n; ++i)
      coll.push_back(pq.extractmin());

    EXPECT_THAT(coll, ElementsAreArray({0,1,2,3,4,5,6,7,8,9}));
  }
  {
    vector<int> coll{4,5,6,7,8,9,1,2,3,0};
    vector<int> collo;

    int n = 10;
    PriQueue<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({0,1,2,3,4,5,6,7,8,9}));
  }

  // max
  {
    vector<int> coll;

    int n = 10;
    PriQueueMax<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(i);

    for (int i = 0; i < n; ++i)
      coll.push_back(pq.extractmin());

    EXPECT_THAT(coll, ElementsAreArray({9,8,7,6,5,4,3,2,1,0}));
  }
  {
    vector<int> coll{4,5,6,7,8,9,1,2,3,0};
    vector<int> collo;

    int n = 10;
    PriQueueMax<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({9,8,7,6,5,4,3,2,1,0}));
  }

  // min, template
  {
    vector<int> coll;

    int n = 10;
    PriQueueTemplate<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(i);

    for (int i = 0; i < n; ++i)
      coll.push_back(pq.extractmin());

    EXPECT_THAT(coll, ElementsAreArray({0,1,2,3,4,5,6,7,8,9}));
  }
  {
    vector<int> coll{4,5,6,7,8,9,1,2,3,0};
    vector<int> collo;

    int n = 10;
    PriQueueTemplate<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({0,1,2,3,4,5,6,7,8,9}));
  }

  // max, template
  {
    vector<int> coll;

    int n = 10;
    PriQueueTemplate<int, std::greater<int>> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(i);

    for (int i = 0; i < n; ++i)
      coll.push_back(pq.extractmin());

    EXPECT_THAT(coll, ElementsAreArray({9,8,7,6,5,4,3,2,1,0}));
  }
  {
    vector<int> coll{4,5,6,7,8,9,1,2,3,0};
    vector<int> collo;

    int n = 10;
    PriQueueTemplate<int, std::greater<int>> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({9,8,7,6,5,4,3,2,1,0}));
  }
}


/*

heap is one-based array; the easiest approach is to declare x[n+1] and waste
x[0]. Can use [0]  so that heapsort can be used to sort any input without
consdiering [0]? NO since "root == 1" affects how child and parent index is
calculated.

The simple one uses two arrays, one for the priority queue and one for the
element to be sorted.

Heapsort saves space by using just one. Two stage process:

1. build input into a heap
establish heap(1, n), by sifting elements to move up front in the array, the one
single array.

  | heap -> | ?  |
  1         i    n

[0] is not used and [1] so put from [2] into heap. 

2. the next n step extract the elements in `decreasing` order and build the
sorted

  | heap | <- sorted  |
  1      i            n

*/

namespace algo_sort_heap
{
  void sort_heap_min(vector<int> &coll)
  {
    int n = coll.size() - 1;

    // convert input to heap
    for (int i = 2; i <= n; ++i)
      siftup_min_text(coll, i);

    // // convert heap to sorted
    // for (int i = n; i > 1; --i)
    // {
    //   swap(coll[1], coll[i]);
    //   siftdown_min_text(coll, i-1);
    // }

    // convert heap to sorted
    for (int i = n; i > 1;)
    {
      swap(coll[1], coll[i]);
      siftdown_min_text(coll, --i);
    }
  }
} // namespace

TEST(AlgoSort, HeapSort)
{
  using namespace algo_sort_heap;

  {
    // add the dummpy at 0 to make heap
    vector<int> coll{0,4,5,6,7,8,9,1,2,3,0};

    sort_heap_min(coll);

    // descending order
    EXPECT_THAT(coll, ElementsAreArray({0,9,8,7,6,5,4,3,2,1,0}));
  }
  
  // max verison will make ascending order
  //
  // {
  //   // add the dummpy at 0 to make heap
  //   vector<int> coll{0,4,5,6,7,8,9,1,2,3,0};
  //
  //   sort_heap_02(coll);
  //
  //   EXPECT_THAT(coll, ElementsAreArray({0,0,1,2,3,4,5,6,7,8,9}));
  // }
  // {
  //   vector<int> coll{ 0, 33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3 };
  //   sort_heap_02(coll); 
  //   EXPECT_THAT(coll, 
  //       ElementsAreArray({0, 2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33 }));
  // }
}


// ={=========================================================================
// algo-sort-merge

// merge(combine) sorted input list

TEST(AlgoSort, MergeSortedList)
{
  std::list<int> coll1{26, 33, 35, 29};
  std::list<int> coll2{9, 12, 22};
  std::list<int> result{};

  auto first = coll1.begin();
  auto second = coll2.begin();

  for (; first != coll1.end() && (second != coll2.end());)
  {
    // ascending order
    if (*first <= *second)
    {
      result.push_back(*first);
      ++first;
    }
    else
    {
      result.push_back(*second);
      ++second;
    }
  }

  // second has some left
  if (first == coll1.end() && second != coll2.end())
    result.splice(result.end(), coll2, second, coll2.end());
  // first has some left
  else if (first != coll1.end() && second == coll2.end())
    result.splice(result.end(), coll1, first, coll1.end());
  else
  {
    // no left from the both
  }

  // combined
  EXPECT_THAT(result, ElementsAre(9, 12, 22, 26, 33, 35, 29));
}


// ={=========================================================================
// algo-search-binary-search

namespace algo_binary_search
{
  // greater-than-version

  // o should have empty input check? not necessary since do not run for loop
  //   anyway.
  //
  // o since there is no match check in the loop, have to check if it found the
  //   match and return index or iterator found or return something to mean `not
  //   found`. 
  //
  // o iterator or index? 
  //
  // When use iterator, there's no more element when first and last are the
  // same. When use array index, there is one element and (last-first) == -1
  // when express there is no element. 
  //
  // Whether or not use iterator or array, have the same number of comparison
  // tree and exit the loop when there is no more comparison to do.
  //
  // o end while loop when begin == end or begin > end
  
  template <typename _Iterator, typename _T> 
    _Iterator binary_search_1(_Iterator begin, _Iterator end, _T const key) 
    { 
      _Iterator result = end;

      while (begin < end)
      {
        auto middle = std::distance(begin, end)/2;

        // cout << "while: middle : " << middle << endl;

        if (key > *(begin + middle))
          begin = begin + middle + 1;
        else
          end = begin + middle;
      }

      // cout << "while: distance : " << std::distance(begin, end) << endl;

      return key == *begin ? begin : result;
    }


  // returns index either found or to insert than boolean
  // must use `begin` to the right return value

  template <typename _Iterator, typename _T> 
    _T binary_search_2(_Iterator begin, _Iterator end, _T const key) 
    { 
      _Iterator result = begin;

      while (begin < end)
      {
        auto middle = std::distance(begin, end)/2;

        if (key > *(begin + middle))
          begin = begin + middle + 1;
        else
          end = begin + middle;
      }

      return std::distance(result, begin);
    }

  template <typename _Coll, typename _T> 
    _T binary_search_3(_Coll coll, _T const key) 
    { 
      auto begin = coll.begin();
      auto end = coll.end();

      while (begin < end)
      {
        auto middle = std::distance(begin, end)/2;

        if (key > *(begin + middle))
          begin = begin + middle + 1;
        else
          end = begin + middle;
      }

      return std::distance(coll.begin(), begin);
    }


  // `equality-version`
  // note that while loop has `=` now

  template <typename _Iterator, typename _T> 
    _Iterator binary_search_4(_Iterator begin, _Iterator end, _T const key) 
    { 
      _Iterator result = end;

      while (begin <= end)
      {
        auto middle = std::distance(begin, end)/2;

        // cout << "while: middle : " << middle << endl;

        if (key == *(begin + middle))
          return begin + middle;
        else if (key > *(begin + middle))
          begin = begin + middle + 1;
        else
          end = begin + middle -1;
      }

      // when not found only
      return result;
    }


  // note: do not work as expected for:
  //
  // EXPECT_THAT(binary_search_5(coll.begin(), coll.end(), 7), 4);
  // EXPECT_THAT(binary_search_5(coll.begin(), coll.end(), 0), 0);
  //
  // since end or begin go past each end and fails on std::distance
  //
  // why gt-version works? since gt-version do not have "begin == end" case in
  // the loop so do not go past

  template <typename _Iterator, typename _T> 
    _T binary_search_5(_Iterator begin, _Iterator end, _T const key) 
    { 
      _Iterator saved_begin = begin;
      _Iterator saved_end = end;

      while (begin <= end)
      {
        auto middle = std::distance(begin, end)/2;

        // cout << "while: middle : " << middle << endl;

        if (key == *(begin + middle))
          return std::distance(saved_begin, begin + middle);
        else if (key > *(begin + middle))
          begin = begin + middle + 1;
        else
          end = begin + middle -1;
      }

      // when not found only
      return std::distance(saved_begin, begin);
    }

  // ansic, p58 `equality-version`
  // cracking the coding interview, p120
  // Programming Pearl, p46
  //
  // note:
  //
  // it can cause `overflow` when array is huge so can use:
  // 
  // mid = (high-low)/2 + low;
  // 
  // it has the same as distance() in iterator version or can use
  // length approach as stl version.
  
  int binary_search_6(vector<int> &coll, int key)
  {
    // *cxx-undefined* since can be negative
    // size_t low{}; size_t high{}; size_t mid{};

    int low{};
    int high{};
    int mid{};

    low = 0;
    high = coll.size()-1;

    while (low <= high)
    {
      mid = (low + high)/2;

      if (key == coll[mid])
        return mid;
      else if (key < coll[mid])
        high = mid - 1;
      else
        low = mid + 1;
    }
   
    // to return index
    return low;

    // return -1;
  }

  // gt-version, stl version
  //
  // Repeat:
  // Whether or not use iterator or array, have the same number of
  // comparison tree and exit the loop when there is no more comparison to do;
  // either found or not found when first/end is 0 or end.
  //
  // to return bool
  // return (first != last && *first == key);
  //
  // Q: if return iterator, when not found, first can be either end() or the
  // first. Is it right to return the first to mean that not found?
  //
  // template<typename _ForwardIterator, typename _Tp>
  //   bool
  //   binary_search(_ForwardIterator __first, _ForwardIterator __last,
  //       const _Tp& __val)
  //   {
  //     typedef typename iterator_traits<_ForwardIterator>::value_type
  //       _ValueType;
  //
  //     _ForwardIterator __i
  //       = std::__lower_bound(__first, __last, __val,
  //           __gnu_cxx::__ops::__iter_less_val());
  //     return __i != __last && !(__val < *__i);
  //   }
  //
  // // lower_bound moved to stl_algobase.h
  //
  // /**
  //  *  @brief Finds the first position in which @p __val could be inserted
  //  *         without changing the ordering.
  //  *  @ingroup binary_search_algorithms
  //  *  @param  __first   An iterator.
  //  *  @param  __last    Another iterator.
  //  *  @param  __val     The search term.
  //  *  @param  __comp    A functor to use for comparisons.
  //  *  @return An iterator pointing to the first element <em>not less
  //  *           than</em> @p __val, or end() if every element is less
  //  *           than @p __val.
  //  *  @ingroup binary_search_algorithms
  //  *
  //  *  The comparison function should have the same effects on ordering as
  //  *  the function used for the initial sort.
  // */
  // 
  // template<typename _ForwardIterator, typename _Tp>
  //   inline _ForwardIterator
  //   lower_bound(_ForwardIterator __first, _ForwardIterator __last,
  //       const _Tp& __val)
  //   {
  //     return std::__lower_bound(__first, __last, __val,
  //         __gnu_cxx::__ops::__iter_less_val());
  //   }
  //
  // As with lower_bound(), it reutns a value "equal to or greater than" and
  // for "0" scale, goes path "<=" since key is 1.
  //
  // {
  //   vector<int> coll{2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};
  //
  //   // lower_bound() returns the position of the first element that has a value
  //   // equal to or greater than value. This is the first position where an
  //   // element with value value could get inserted without breaking the sorting
  //   // of the range [beg,end).
  //
  //   auto first = lower_bound(coll.cbegin(), coll.cend(), 1);
  //   EXPECT_THAT(*first, 2);
  // }

  namespace algo_code
  {
    // /usr/include/c++/4.9/bits/stl_algobase.h
    template<typename _ForwardIterator, typename _Tp, typename _Compare>
      _ForwardIterator
      __lower_bound(_ForwardIterator __first, _ForwardIterator __last,
          const _Tp& __val, _Compare __comp)
      {
        typedef typename iterator_traits<_ForwardIterator>::difference_type
          _DistanceType;

        _DistanceType __len = std::distance(__first, __last);

        while (__len > 0)
        {
          _DistanceType __half = __len >> 1;
          _ForwardIterator __middle = __first;
          std::advance(__middle, __half);
          if (__comp(__middle, __val))
          {
            __first = __middle;
            ++__first;
            __len = __len - __half - 1;
          }
          else
            __len = __half;
        }
        return __first;
      }
  } // namespace

  template <typename _Iterator, typename _T> 
    _T binary_search_7(_Iterator begin, _Iterator end, _T const key) 
    {
      auto saved_begin = begin;
      auto length  = std::distance(begin, end);

      while (0 < length)
      {
        auto half = length >> 1;

        // cxx-advance
        _Iterator middle = begin;
        std::advance(middle, half);

        // error since: void advance().
        // ITERATOR middle = advance(first, half);

        if (*middle < key)
        {
          begin = ++middle;
          length = length - half - 1;
        }
        else
        {
          length = half;
        }
      }

      return std::distance(saved_begin, begin);
    }

  int binary_search_8(std::vector<int> &coll, int key)
  {
    int begin{};
    int length = coll.size();

    while (0 < length)
    {
      auto half = length >> 1;
      auto middle = begin + half;

      if (key > coll[middle])
      { 
        // update begin and work towards the right
        begin = ++middle;

        // note that length is length but not index
        length = length - half -1;
      }
      else
      {
        // work towards the left
        length = half;
      }
    }

    return begin;
  }

} // namespace

TEST(AlgoSearch, BinarySearch)
{
  using namespace algo_binary_search;

  // cxx-binary-search, stl version
  //
  // 11.10 Sorted-Range Algorithms
  //
  // The following algorithms search certain values in sorted ranges.  Checking
  // Whether One Element Is Present
  //
  // bool binary_search (ForwardIterator beg, ForwardIterator end, const T&
  // value)
  //
  // bool binary_search (ForwardIterator beg, ForwardIterator end, const T&
  // value, BinaryPredicate op)

  {
    //               0  1  2  3   4   5   6   7   8   9  10  11  12
    vector<int> coll{2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};
    EXPECT_THAT(binary_search(coll.begin(), coll.end(), 15), true);
    EXPECT_THAT(binary_search(coll.begin(), coll.end(), 32), false);
  }


  // gt-version
  {
    //               0  1  2  3   4   5   6   7   8   9  10  11  12
    vector<int> coll{2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};

    auto it = binary_search_1(coll.begin(), coll.end(), 15);
    EXPECT_THAT(std::distance(coll.begin(), it), 7);

    EXPECT_THAT(binary_search_1(coll.begin(), coll.end(), 32), 
        coll.end());
  }

  {
    //               0 1 2 3
    vector<int> coll{1,3,5,6};

    EXPECT_THAT(binary_search_1(coll.begin(), coll.end(), 2), 
        coll.end());
  }

  // gt-version that return index
  {
    //               0 1 2 3
    vector<int> coll{1,3,5,6};
    EXPECT_THAT(binary_search_2(coll.begin(), coll.end(), 5), 2);
    EXPECT_THAT(binary_search_2(coll.begin(), coll.end(), 2), 1);
    EXPECT_THAT(binary_search_2(coll.begin(), coll.end(), 7), 4);
    EXPECT_THAT(binary_search_2(coll.begin(), coll.end(), 0), 0);
  }
  {
    //               0 1 2 3
    vector<int> coll{1,3,5,6};
    EXPECT_THAT(binary_search_3(coll, 5), 2);
    EXPECT_THAT(binary_search_3(coll, 2), 1);
    EXPECT_THAT(binary_search_3(coll, 7), 4);
    EXPECT_THAT(binary_search_3(coll, 0), 0);
  }

  // equality-version
  {
    //               0  1  2  3   4   5   6   7   8   9  10  11  12
    vector<int> coll{2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};

    auto it = binary_search_4(coll.begin(), coll.end(), 15);
    EXPECT_THAT(std::distance(coll.begin(), it), 7);

    EXPECT_THAT(binary_search_4(coll.begin(), coll.end(), 32), 
        coll.end());
  }
  {
    //               0 1 2 3
    vector<int> coll{1,3,5,6};
    EXPECT_THAT(binary_search_5(coll.begin(), coll.end(), 5), 2);
    EXPECT_THAT(binary_search_5(coll.begin(), coll.end(), 2), 1);
  }

  {
    //               0 1 2 3
    vector<int> coll{1,3,5,6};
    EXPECT_THAT(binary_search_6(coll, 5), 2);
    EXPECT_THAT(binary_search_6(coll, 2), 1);
    EXPECT_THAT(binary_search_6(coll, 7), 4);
    
    // when use size_t
    // /usr/include/c++/4.9/debug/vector:357:error: attempt to subscript container
    //     with out-of-bounds index 9223372036854775807, but container only holds 4
    //      elements.

    EXPECT_THAT(binary_search_6(coll, 0), 0);
  }
}

TEST(AlgoSearch, BinarySearchStl)
{
  using namespace algo_binary_search;

  // AlgoUpperLowerBound
  {
    vector<int> coll{1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9};

    //  0  1  2  3  4  5  6  7  8  9 10 11 12
    // {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9};
    //                          ^^^^^^^
    // lower_bound() returns the position of the first element that has a value
    // equal to or greater than value. This is the first position where an
    // element with value could get inserted without breaking the sorting of the
    // range [beg,end).

    // : error: no matching function for call to 
    // ‘distance(std::vector<int>::iterator, __gnu_cxx::__normal_iterator<const int*, std::vector<int> >&)’
    // since lower_bound() uses cbegin().
    // EXPECT_THAT(distance(coll.begin(), first), 8);

    auto first = lower_bound(coll.cbegin(), coll.cend(), 5);
    EXPECT_THAT(*first, 5);
    EXPECT_THAT(distance(coll.cbegin(), first), 8);

    auto pos = binary_search_7(coll.begin(), coll.end(), 5);
    EXPECT_THAT(pos, 8);
  }
  {
    vector<int> coll{2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33};

    auto first = lower_bound(coll.cbegin(), coll.cend(), 1);
    EXPECT_THAT(*first, 2);

    EXPECT_THAT(binary_search_7(coll.begin(), coll.end(), 1), 0);
  }
  {
    //               0 1 2 3
    vector<int> coll{1,3,5,6};
    EXPECT_THAT(binary_search_7(coll.begin(), coll.end(), 5), 2);
    EXPECT_THAT(binary_search_7(coll.begin(), coll.end(), 2), 1);
    EXPECT_THAT(binary_search_7(coll.begin(), coll.end(), 7), 4);
    EXPECT_THAT(binary_search_7(coll.begin(), coll.end(), 0), 0);
  }
  {
    //               0 1 2 3
    vector<int> coll{1,3,5,6};
    EXPECT_THAT(binary_search_8(coll, 5), 2);
    EXPECT_THAT(binary_search_8(coll, 2), 1);
    EXPECT_THAT(binary_search_8(coll, 7), 4);
    EXPECT_THAT(binary_search_8(coll, 0), 0);
  }
}


// algo-binary-search algo-leetcode-17
/*
69. Sqrt(x), Easy

Implement int sqrt(int x).

Compute and return the square root of x, where x is guaranteed to be a
non-negative integer.

Since the return type is an integer, the decimal digits are truncated and only
the integer part of the result is returned.

Example 1:
Input: 4
Output: 2

Example 2:
Input: 8
Output: 2
Explanation: The square root of 8 is 2.82842..., and since 
             the decimal part is truncated, 2 is returned.
*/

namespace algo_binary_search
{
namespace leetcode_easy_017
{

  // https://www.geeksforgeeks.org/square-root-of-an-integer/
  // binary search version
  //
  // O(Log x)
  //
  //  0   1       mid   sqrt(x)                 x = sqrt(x)*sqrt(x)
  //  |---|--------|-------|--------------------|
  //
  //  actually, trying to find sqrt(x) from [1, x] and x is ^2 domain and big
  //  value but used 

  int floor_sqrt_1(int x)
  {
    // base cases
    if (x == 0 || x == 1)
      return x;

    // starts from 1 since it's covered in base cases
    int start{1};
    int end{x-1};
    int ans{};

    while (start <= end)
    {
      int mid = (start + end) / 2;

      // equality; perfect square
      if (mid * mid == x)
        return mid;
      else if (mid * mid < x)
      {
        // so discard [1, mid], update start and move closer to sqrt(x)
        start = mid + 1;

        // we need floor answer so update ans  
        ans = mid;
      }
      // discard [mid, x]
      else
        end = mid - 1;
    }

    // return floor value rather than `not found`
    return ans;
  }

  // Note: The Binary Search can be further optimized to start with ‘start’ = 0
  // and ‘end’ = x/2. Floor of square root of x cannot be more than x/2 when x >
  // 1.
  //
  // cxx-error-overflow
  // Line 18: Char 15: runtime error: signed integer overflow: 536848899 *
  // 536848899 cannot be represented in type 'int' (solution.cpp)

  int floor_sqrt_2(int x)
  {
    // // base cases
    // if (x == 0 || x == 1)
    //   return x;

    // starts from 1 since it's covered in base cases
    int start{0};
    int end{x/2};
    int ans{};

    while (start <= end)
    {
      int mid = (start + end) / 2;

      // equality; perfect square
      if (mid * mid == x)
        return mid;
      else if (mid * mid < x)
      {
        // so discard [1, mid], update start and move closer to sqrt(x)
        start = mid + 1;

        // we need floor answer so update ans  
        ans = mid;
      }
      // discard [mid, x]
      else
        end = mid - 1;
    }

    // return floor value rather than `not found`
    return ans;
  }

  // Runtime: 24 ms, faster than 21.88% of C++ online submissions for Sqrt(x).
  //
  // Memory Usage: 13.9 MB, less than 49.57% of C++ online submissions for
  // Sqrt(x).

  int floor_sqrt_3(int x)
  {
    // // base cases
    if (x == 0 || x == 1)
      return x;

    // starts from 1 since it's covered in base cases
    int start{0};
    int end{x/2};
    int ans{};

    while (start <= end)
    {
      long long mid = (start + end) / 2;
      long long sqare = mid * mid;

      // equality; perfect square
      if (sqare == x)
        return mid;
      else if (sqare < x)
      {
        // so discard [1, mid], update start and move closer to sqrt(x)
        start = mid + 1;

        // we need floor answer so update ans  
        ans = mid;
      }
      // discard [mid, x]
      else
        end = mid - 1;
    }

    // return floor value rather than `not found`
    return ans;
  }


  // having square variable for mid * mid causes performance penalty?
  //
  // Runtime: 12 ms, faster than 99.18% of C++ online submissions for Sqrt(x).
  //
  // Memory Usage: 13.8 MB, less than 84.46% of C++ online submissions for
  // Sqrt(x).

  int floor_sqrt_4(int x)
  {
    // // base cases
    if (x == 0 || x == 1)
      return x;

    // starts from 1 since it's covered in base cases
    int start{0};
    int end{x/2};
    int ans{};

    long long mid{};

    while (start <= end)
    {
      mid = (start + end) / 2;

      // equality; perfect square
      if (mid * mid == x)
        return mid;
      else if (mid * mid < x)
      {
        // so discard [1, mid], update start and move closer to sqrt(x)
        start = mid + 1;

        // we need floor answer so update ans  
        ans = mid;
      }
      // discard [mid, x]
      else
        end = mid - 1;
    }

    // return floor value rather than `not found`
    return ans;
  }

  // code discussion forum
  //
  // Runtime: 12 ms, faster than 99.18% of C++ online submissions for Sqrt(x).
  //
  // Memory Usage: 13.8 MB, less than 73.75% of C++ online submissions for
  // Sqrt(x).

  int floor_sqrt_5(int x)
  {
    long long l=1,r=x,mid;

    if(x==0)
      return 0;

    while(l<=r)
    {
      mid = l+(r-l)/2;

      if( mid*mid==x)
        return mid;
      else if( mid*mid>x)
        r=mid-1;
      else
      {
        l=mid+1;
        if(l*l>x)
          return mid;
      }
    }

    // just to avoid warning
    return mid;
  }
} // namespace
} // namespace


TEST(AlgoSearch, LeetCode_Easy_017_Sqrt)
{
  using namespace algo_binary_search::leetcode_easy_017;

  {
    // #include <math.h>
    // double sqrt(double x);
    // float sqrtf(float x);
    // long double sqrtl(long double x);

    // 2
    // 2.82843
    // 3.16228
    // 4

    EXPECT_DOUBLE_EQ(sqrt(4), 2);
    EXPECT_NEAR(sqrt(8), 2.82843, 0.00001);

    // Expected equality of these values:
    //   sqrt(10)
    //     Which is: 3.1622776601683795
    //   3.16228
    // EXPECT_DOUBLE_EQ(sqrt(10), 3.16228);

    EXPECT_NEAR(sqrt(10), 3.16228, 0.00001);
    EXPECT_DOUBLE_EQ(sqrt(16), 4);
  }

  {
    auto func = floor_sqrt_1;

    EXPECT_THAT(func(4), 2);
    EXPECT_THAT(func(8), 2);
    EXPECT_THAT(func(10), 3);
    EXPECT_THAT(func(16), 4);
  }
  {
    auto func = floor_sqrt_2;

    EXPECT_THAT(func(4), 2);
    EXPECT_THAT(func(8), 2);
    EXPECT_THAT(func(10), 3);
    EXPECT_THAT(func(16), 4);
  }
  {
    // >>> 46339*46339, floor
    // 2,147,302,921
    //
    // 2,147,395,599
    //
    // >>> 46340*46340, ceiling
    // 2,147,395,600
    
    auto func = floor_sqrt_3;
    EXPECT_THAT(func(2147395599), 46339);
    EXPECT_NEAR(sqrt(2147395599), 46340, 0.1);
  }

  {
    // >>> 46339*46339, floor
    // 2,147,302,921
    //
    // 2,147,395,599
    //
    // >>> 46340*46340, ceiling
    // 2,147,395,600
    
    auto func = floor_sqrt_4;
    EXPECT_THAT(func(1), 1);
    EXPECT_THAT(func(2147395599), 46339);
    EXPECT_NEAR(sqrt(2147395599), 46340, 0.1);
  }
}


// ={=========================================================================
// algo-queue

namespace queue_circular_vacant
{
  class CircularQueue
  {
    public:

      // *cxx-vector-ctor*
      // CircularQueue() : coll_(MAX_SIZE, 0) {}

      CircularQueue() {}

      bool empty() { return head_ == tail_; }

      bool full() { return (head_ + 1) % MAX_SIZE == tail_; }

      size_t size() { return (head_ - tail_ + MAX_SIZE) % MAX_SIZE; }

      void push(int value)
      {
        if (full())
          throw std::runtime_error("queue is full");

        // to see where exception happens since gmock do not show where it
        // throws
        //
        // if (full())
        // {
        //   cout << "queue is full, value = " << value << endl;
        //   return;
        // }

        head_ = (head_ + 1) % MAX_SIZE;
        coll_[head_] = value;
      }

      int pop()
      {
        if (empty())
          throw std::runtime_error("queue is empty");

        tail_ = (tail_ + 1) % MAX_SIZE;
        return coll_[tail_];
      }

      std::vector<int> snap()
      {
        std::vector<int> result{};

        // do not work like this
        //
        // for (auto run = tail_ + 1; run <= head_; ++run)
        //   result.push_back(coll_[run]);
        //
        // care about start value and <= condition. However, cannot use comparison
        // on head and tail since it warps around after all.
        //
        // for (int i = tail_ + 1; i <= head_; i = (i + 1) % MAX_SIZE)
        //     coll.push_back(coll_[i]);

        auto run = (tail_ + 1) % MAX_SIZE;
        for (size_t i = 0; i < size(); ++i)
        {
          result.push_back(coll_[run]);
          run = (run + 1) % MAX_SIZE;
        }

        return result;
      }

    private:
      static size_t const MAX_SIZE{10};

      // if MAX_SIZE is not static
      // std::vector<int> coll_;
      // and use ctor
      
      // if MAX_SIZE is not static
      // std::array<int, 10> coll_;
      
      // if MAX_SIZE is static
      std::array<int, MAX_SIZE> coll_;

      // if MAX_SIZE is static but still error
      // std::vector<int> coll_(MAX_SIZE, 0);

      // they are indexes
      size_t head_{};
      size_t tail_{};
  };

} // namespace

TEST(AlgoQueue, CircularVacant)
{
  using namespace queue_circular_vacant;

  CircularQueue cq;

  cq.push(10);
  cq.push(11);
  cq.push(12);

  EXPECT_THAT(cq.size(), 3);
  EXPECT_THAT(cq.snap(), ElementsAre(10, 11, 12));

  cq.push(13);
  cq.push(14);
  cq.push(15);

  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(10, 11, 12, 13, 14, 15));

  EXPECT_THAT(cq.pop(), 10);
  EXPECT_THAT(cq.pop(), 11);
  EXPECT_THAT(cq.pop(), 12);

  EXPECT_THAT(cq.size(), 3);

  cq.push(100);
  cq.push(101);
  cq.push(102);

  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(13, 14, 15, 100, 101, 102));

  EXPECT_THAT(cq.pop(), 13);
  EXPECT_THAT(cq.pop(), 14);

  cq.push(300);
  cq.push(301);
  cq.push(302);

  EXPECT_THAT(cq.size(), 7);
  EXPECT_THAT(cq.snap(), ElementsAre(15, 100, 101, 102, 300, 301, 302));

  cq.push(400);
  cq.push(401);
  EXPECT_THROW(cq.push(402), std::runtime_error);   // full, exception
  EXPECT_THROW(cq.push(403), std::runtime_error);   // full, exception
  EXPECT_THROW(cq.push(404), std::runtime_error);   // full, exception

  // since it is vacant version
  EXPECT_THAT(cq.size(), 9);
  EXPECT_THAT(cq.full(), true);

  EXPECT_THAT(cq.pop(), 15);
  EXPECT_THAT(cq.pop(), 100);
  EXPECT_THAT(cq.pop(), 101);

  // since it is vacant version
  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(102, 300, 301, 302, 400, 401));

  EXPECT_THAT(cq.full(), false);
}


namespace queue_circular_count
{

class CircularQueue
{
  public:
    // *cxx-vector-ctor*
    CircularQueue() 
      : coll_(MAX_SIZE, 0)
    {
    }

    bool empty() const 
    { return count_ == 0; }

    bool full() const
    { return count_ >= MAX_SIZE; }

    int size() const 
    { return count_; }

    void push(int value) 
    {
      if (full())
        return;

      head_ = head_ % MAX_SIZE;
      coll_[head_++] = value;
      count_++;
    }

    int pop() 
    {
      int value{};

      if (empty())
        return -1;

      tail_ = tail_ % MAX_SIZE;
      value = coll_[tail_++];
      count_--;

      return value;
    }

    std::vector<int> snap()
    {
      std::vector<int> coll;

      int start = tail_;

      for (int i = 0; i < size(); ++i)
      {
        coll.push_back(coll_[start]);
        start = (start+1) % MAX_SIZE;
      }

      return coll;
    }

  private:
    const int MAX_SIZE{10};
    int head_{};
    int tail_{};
    int count_{};
    std::vector<int> coll_; // {MAX_SIZE, 0};
}; 

} // namespace

TEST(AlgoQueue, CircularCount)
{
  using namespace queue_circular_count;

  CircularQueue cq;

  cq.push(10);
  cq.push(11);
  cq.push(12);

  EXPECT_THAT(cq.size(), 3);
  EXPECT_THAT(cq.snap(), ElementsAre(10, 11, 12));

  cq.push(13);
  cq.push(14);
  cq.push(15);

  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(10, 11, 12, 13, 14, 15));

  EXPECT_THAT(cq.pop(), 10);
  EXPECT_THAT(cq.pop(), 11);
  EXPECT_THAT(cq.pop(), 12);

  EXPECT_THAT(cq.size(), 3);

  cq.push(100);
  cq.push(101);
  cq.push(102);

  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(13, 14, 15, 100, 101, 102));

  EXPECT_THAT(cq.pop(), 13);
  EXPECT_THAT(cq.pop(), 14);

  cq.push(300);
  cq.push(301);
  cq.push(302);

  EXPECT_THAT(cq.size(), 7);
  EXPECT_THAT(cq.snap(), ElementsAre(15, 100, 101, 102, 300, 301, 302));

  cq.push(400);
  cq.push(401);
  cq.push(402);
  cq.push(403);
  cq.push(404);

  EXPECT_THAT(cq.size(), 10);
  EXPECT_THAT(cq.full(), true);

  EXPECT_THAT(cq.pop(), 15);
  EXPECT_THAT(cq.pop(), 100);
  EXPECT_THAT(cq.pop(), 101);

  EXPECT_THAT(cq.size(), 7);
  EXPECT_THAT(cq.snap(), ElementsAre(102, 300, 301, 302, 400, 401, 402));

  EXPECT_THAT(cq.full(), false);
}


// from Problem 46, circular buffer, the modern c++ challenge
// 1. use size(count) and head only
// 2. in push, no full check since it overwrites and in pop, it simply
// calculates first from head substracing size.
// 3. no iterator support is needed if not use begin()/end()

// no need to have `count` since std::vector() has size() to get current size
// and which is different from queue array implementation

namespace queue_circular_count_iterator
{
  template <typename T>
    class circular_buffer_iterator;

  template <typename T>
    class circular_buffer
    {
      typedef circular_buffer_iterator<T> const_iterator;
      friend class circular_buffer_iterator<T>;

      public:
      circular_buffer() = delete;
      explicit circular_buffer(size_t const size) : coll_(size) {}

      bool empty() const noexcept
      { return size_ == 0; }

      bool full() const noexcept
      { return size_ >= coll_.size(); }

      size_t capacity() const noexcept
      { return size_; }

      void clear() noexcept
      { head_ = -1, size_ = 0; }

      T pop()
      {
        if (empty())
          throw std::runtime_error("buffer is empty");

        auto pos = first_pos();

        #ifdef QUEUE_CIRCULAR_DEBUG
        cout << "pop: pos: " << pos 
          << ", coll_[]: " << coll_[pos] << endl;
        #endif // QUEUE_CIRCULAR_DEBUG

        size_--;
        return coll_[pos];
      }

      void push(T const item)
      {
        // this is how the text is implemented and this allows overwrites
        // this make head and tail changed
        //
        // if (full())
        //   throw std::runtime_error("buffer is full");

        head_ = next_pos();
        coll_[head_] = item;

        #ifdef QUEUE_CIRCULAR_DEBUG
        cout << "push: head_: " << head_ 
          << ", coll_[]: " << coll_[head_] << endl;
        #endif // QUEUE_CIRCULAR_DEBUG

        // due to overwrite feature
        if (size_ < coll_.size())
          size_++;
      }

      // iterators
      const_iterator begin() const
      { return const_iterator(*this, first_pos(), empty()); }

      const_iterator end() const
      { return const_iterator(*this, next_pos(), true); }

      private:
      // same as `count`
      size_t size_{};

      // ?, set max value
      // size_t head_{-1};

      // to aviod narrowing warning
      int head_{-1};

      std::vector<T> coll_;

      // return `head` pos to push and the reason of having size_t == 0 is
      // that `head` starts from -1.

      size_t next_pos() const noexcept
      {
        return size_ == 0 ? 0 
          : ((head_ + 1 ) % coll_.size());
      }

      // return `tail` pos to pop and get tail from head and size
      // as with vacant case, normalise and +1 since no vacant item.

      size_t first_pos() const noexcept
      {
        return size_ == 0 ? 0 
          : (head_ - size_ + 1 + coll_.size()) % coll_.size();
      }
    };

  template <typename T>
    class circular_buffer_iterator
    {
      typedef circular_buffer_iterator    self_type;
      typedef T const&                    const_reference;

      public:
      explicit circular_buffer_iterator(circular_buffer<T> const& buffer,
          size_t position, bool is_last)
        : buffer_(buffer), position_(position), is_last_(is_last)
      {}

      // cxx-operator-prefix
      self_type& operator++()
      {
        if (is_last_)
          throw std::out_of_range("past the end");

        position_ = (position_ + 1) % buffer_.coll_.size();

        // although it's circular queue which wraps around, iterator moves
        // around [tail, head] range. If increased pos is the same as head
        // then it reaches to the end.
        //
        // is_last_ get set either from ctor or ++()

        is_last_ = (position_ == buffer_.next_pos());

        return *this;
      }

      // cxx-operator-postfix which use prefix version
      self_type& operator++(int)
      {
        auto temp = *this;
        ++*this();
        return temp;
      }

      bool operator==(self_type const& other) const
      {
        // & address? since buffer do not support operator==() 
        return &buffer_ == &other.buffer_
          && position_ == other.position_
          && is_last_ == other.is_last_;
      }

      bool operator!=(self_type const& other) const
      { return !(*this == other); }

      const_reference operator*() const
      {
        return buffer_.coll_[position_];
      }

      private:
      circular_buffer<T> const& buffer_;
      size_t position_;
      bool is_last_;
    };

  template <typename T>
    std::vector<T> print(circular_buffer<T> & buf)
    {
      std::vector<T> coll{};

      for (auto & e : buf)
        coll.push_back(e);

      return coll;
    }

} // namespace


TEST(AlgoQueue, CircularCountIterator)
{
  using namespace queue_circular_count_iterator;

  {
    circular_buffer<int> cbuf(5);   // {0, 0, 0, 0, 0} -> {}

    cbuf.push(1);                   // {1, 0, 0, 0, 0} -> {1}
    cbuf.push(2);                   // {1, 2, 0, 0, 0} -> {1, 2}
    cbuf.push(3);                   // {1, 2, 3, 0, 0} -> {1, 2, 3}

    auto item = cbuf.pop();         // {1, 2, 3, 0, 0} -> {X, 2, 3}
    EXPECT_THAT(item, 1);

    cbuf.push(4);                   // {1, 2, 3, 4, 0} -> {X, 2, 3, 4}
    cbuf.push(5);                   // {1, 2, 3, 4, (5)} -> {X, 2, 3, 4, 5}

    // see that it overwrites
    cbuf.push(6);                   // {(6), 2, 3, 4, 5} -> {2, 3, 4, 5, 6}
    cbuf.push(7);                   // {6, (7), 3, 4, 5} -> {3, 4, 5, 6, 7}
    cbuf.push(8);                   // {6, 7, (8), 4, 5} -> {4, 5, 6, 7, 8}

    item = cbuf.pop();              // {6, 7, 8, 4, 5} -> {5, 6, 7, 8}
    EXPECT_THAT(item, 4);
    item = cbuf.pop();              // {6, 7, 8, 4, 5} -> {6, 7, 8}
    EXPECT_THAT(item, 5);
    item = cbuf.pop();              // {6, 7, 8, 4, 5} -> {7, 8}
    EXPECT_THAT(item, 6);

    cbuf.pop();                     // {6, 7, 8, 4, 5} -> {8}
    cbuf.pop();                     // {6, 7, 8, 4, 5} -> {}
    cbuf.push(9);                   // {6, 7, 8, 9, 5} -> {9}
  }

  // to exercise iterator feature
  {
    circular_buffer<int> cbuf(5);

    cbuf.push(1);
    cbuf.push(2);
    cbuf.push(3);
    cbuf.push(4);
    cbuf.push(5);
    cbuf.push(6);
    cbuf.push(7);
    EXPECT_THAT(print(cbuf), ElementsAre(3,4,5,6,7));
  }
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

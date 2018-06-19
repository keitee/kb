#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <bitset>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

template <typename T>
void PRINT_ELEMENTS_MAP(T col, const string mesg, const string sep = ", ")
{
    cout << "=" << mesg << endl;

    for ( auto &e : col)
        cout << "{" << e.first << ", " << e.second << "}" << sep;
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
// algo-find a number seen odd times
unsigned int t_algo_find_a_number_01(const vector<unsigned int> &input)
{
  unsigned int uireturn{};

  for (const auto e : input)
    uireturn ^= e;

  return uireturn;
}

TEST(AlgoFindOdd, FindNumberSeenOddTimes)
{
    const vector<unsigned int> input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};
    EXPECT_THAT(t_algo_find_a_number_01(input), 12);
}

unsigned int t_algo_find_a_number_02(const vector<unsigned int> &input)
{
  size_t uiresult;
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

  uiresult = (pos_return != imap.cend()) ? pos_return->first : 0;
  return uiresult;
}

TEST(AlgoFindOdd, FindNumberSeenOddTimesUseMap)
{
    const vector<unsigned int> input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};
    EXPECT_THAT(t_algo_find_a_number_02(input), 12);
}

unsigned int t_algo_find_a_number_03(const vector<unsigned int> &input)
{
  size_t uireturn{};
  multiset<size_t> imset{input.cbegin(), input.cend()};

  // put them into a map<key, count>
  for (const auto e : imset)
  {
    if ( imset.count(e) % 2)
    {
      uireturn = e;
      break;
    }
  }

  return uireturn;
}

TEST(AlgoFindOdd, FindNumberSeenOddTimesUseSet)
{
    const vector<unsigned int> input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};
    EXPECT_THAT(t_algo_find_a_number_02(input), 12);
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
// * use in/out parameter
// * cstring but efficient
// * strlen()-1 since array indexing is always [0, size-1], or [0,size) and in
//   this code, [begin, end] but not [begin, end)

void reverse_string_01(char *input)
{
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

TEST(AlgoReverse, ReverseCstring)
{
    char input[] = "REVERSE IT";
    reverse_string_01(input);
    EXPECT_THAT(input, StrEq("TI ESREVER"));
}

// ansic, p62

void reverse_string_02(char *input)
{
    int str{}, end{};
    char temp{}; 

    for (end = strlen(input)-1; str < end; ++str, --end)
    {
        // swap(str, end)
        temp = input[str], input[str] = input[end], input[end] = temp;
    }
}

TEST(AlgoReverse, ReverseCstringAnsiVersion)
{
    char input[] = "REVERSE IT";
    reverse_string_02(input);
    EXPECT_THAT(input, StrEq("TI ESREVER"));
}


// * if can use std::string and can return although c version is better.
// * use *cpp-reverse-iterator*

std::string reverse_string_03(const std::string &input)
{
    return std::string(input.crbegin(), input.crend());
}

TEST(AlgoReverse, ReverseCppStringUseRevrseIterator)
{
    std::string input{"REVERSE IT"};
    EXPECT_THAT(reverse_string_03(input), Eq("TI ESREVER"));
}


// when want to modify input itself

void reverse_string_04(std::string &input)
{
    std::reverse(input.begin(), input.end());
}

TEST(AlgoReverse, ReverseCppStringUseAlgorithm)
{
    std::string input{"REVERSE IT"};
    reverse_string_04(input);
    EXPECT_THAT(input, Eq("TI ESREVER"));
}

std::string reverse_string_05(const std::string &input)
{
    std::string reversed{};

    for (auto len = input.size(); len > 0; --len)
        reversed.push_back(input[len-1]);

    return reversed;
}

TEST(AlgoReverse, ReverseCppStringUsePushBack)
{
    std::string input{"REVERSE IT"};
    EXPECT_THAT(reverse_string_05(input), Eq("TI ESREVER"));
}


// Write a program to reverse a string with all its duplicates removed. Only the
// last instance of a character in the reverse string has to appear. Also, the
// following conditions are to be satisfied: Assume only Capital Letters.
//
// o. assume that input is ASCII and is all upper case chars. so can use
// unsigned int to check if it's a duplicate or not. if needs more range to
// cover then need to use something else.
//
// o. from the net

std::string reverse_string_06(const std::string &input)
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

TEST(AlgoReverse, ReverseCppStringRemoveDuplicates)
{
    std::string input{"JTVAKAVISHAAAL"};
    EXPECT_THAT(reverse_string_06(input), Eq("LHSIKAVTJ"));
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
// algo-find the longest sequence of input char array

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
pair<char, size_t> find_longest_01(const string &input)
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


TEST(AlgoLongestSequence, FindLongestSequence)
{
    const string input1{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"};
    EXPECT_THAT(find_longest_01(input1), 
            EqPair(pair<char, size_t>('F', 18)));

    const string input2{"AAABBCCCCDDD"};
    EXPECT_THAT(find_longest_01(input2), 
            EqPair(pair<char, size_t>('C', 4)));

    const string input3{"AAAAAAAAAAAA"};
    EXPECT_THAT(find_longest_01(input3), 
            EqPair(pair<char, size_t>('A', 12)));
}


// Firstly, looks better but it fails when the input has one long sequence.
pair<char, size_t> find_longest_02(const string &input)
{
    char current_char{}, longest_char{};
    size_t current_occurance{}, longest_occurance{};

    for (auto letter : input)
    {
        // if see the different char. use XOR and looks fancy?
        if (current_char^letter)
        {
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

TEST(AlgoLongestSequence, FindLongestSequenceFail)
{
    // fails on this case
    const string input3{"AAAAAAAAAAAA"};
    EXPECT_THAT(find_longest_02(input3), 
            NotEqPair(pair<char, size_t>('A', 12)));
}

pair<unsigned char, size_t> algo_find_longest_sequence_0618(const string &input)
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

TEST(AlgoLongestSequence, FindLongestSequence_0618)
{
    const string input1{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"};
    EXPECT_THAT(algo_find_longest_sequence_0618(input1), 
            EqPair(pair<char, size_t>('F', 18)));

    const string input2{"AAABBCCCCDDD"};
    EXPECT_THAT(algo_find_longest_sequence_0618(input2), 
            EqPair(pair<char, size_t>('C', 4)));

    const string input3{"AAAAAAAAAAAA"};
    EXPECT_THAT(algo_find_longest_sequence_0618(input3), 
            EqPair(pair<char, size_t>('A', 12)));
}


// To do better than O(n), can skip some chars in searching a sequence.
pair<char, size_t> t_algo_find_longest_02(const string &input)
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

TEST(AlgoLongestSequence, FindLongestSequenceBetter)
{
    const string input1{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"};
    EXPECT_THAT(t_algo_find_longest_02(input1), 
            EqPair(pair<char, size_t>('F', 18)));

    const string input2{"AAABBCCCCDDD"};
    EXPECT_THAT(t_algo_find_longest_02(input2), 
            EqPair(pair<char, size_t>('C', 4)));

    const string input3{"AAAABCBBBBBCCCCDDD"};
    EXPECT_THAT(t_algo_find_longest_02(input3), 
            EqPair(pair<char, size_t>('B', 5)));
}

TEST(AlgoLongestSequence, FindLongestSequenceBetterFail)
{
    const string input3{"AAAABCCCCCCCC"};
    EXPECT_THAT(t_algo_find_longest_02(input3), 
            NotEqPair(pair<char, size_t>('C', 8)));
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


TEST(RomanConvert, ConvertToRomansFirstAttempt) {

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

TEST(RomanConvert, ConvertToRomansTDD) 
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
// 3. use independent of type.

// returns MSB position which starts from 1th since input >> is evalueated after
// ++count but not 0th.

uint32_t get_msb_pos_01(const uint32_t value)
{
    uint32_t count{};
    uint32_t input = value;

    // do not need to check like: if (input &1) to increase count for every
    // interation since when runs out 1, input becomes 0 and the loop ends. 
    for (; input != 0; input >>= 1)
        ++count;

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
// x = 6: 6(110) & 5(101) = 100  // 110 -> 100 by having rightmost 1 to 0
//           ^ rightmost 1
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
// rightmost 1 to 0. So clear 1 from x one by one and no need to check on if to
// count bits.
// 
// note: And(&) is faster than shift operation? Yes and also there is no `if` in
// the loop.
// 
// int bitcount(unsigned x)
// {
//   int b;
// 
//   for (b = 0; x != 0; x &= (x-1))
//     b++;
//   return b;
// }

uint32_t get_msb_pos_02(const uint32_t value)
{
    uint32_t count{};
    uint32_t input = value;

    for (; input != 0; input &= (input-1))
        ++count;

    return count;
}


TEST(BitPattern, GetMSBPosition)
{
    // A = 35 = 10 0011
    // B =  9 =    1001
    EXPECT_THAT(get_msb_pos_01(35), Eq(6));
    EXPECT_THAT(get_msb_pos_01(9), Eq(4));

    // // but this fails. WHY?
    // EXPECT_THAT(get_msb_pos_02(35), Eq(6));
    // EXPECT_THAT(get_msb_pos_02(9), Eq(4));
}

uint32_t find_same_number_of_bits(const uint32_t first, const uint32_t second)
{
  // get the smaller between inputs
  uint32_t small = first > second ? second : first;

  // same as get_msg_pos()
  uint32_t top_pos{};

  for (; small && (small >>=1);)
    ++top_pos;

  // xor
  uint32_t diff = first^second;
  uint32_t count{};

  for(uint32_t i = 0; i <= top_pos; ++i)
  {
    if(!(diff & 1u))
      ++count;

    diff >>= 1;
  }

  return count;
}

TEST(BitPattern, FindNumberOfBitsBetweenTwoIntegers)
{
    // 35 = 10 0011
    // 9 =     1001
    //          ^ ^
    EXPECT_THAT(find_same_number_of_bits(35, 9), Eq(2));

    // 55 = 10 0111
    // 5 =      101
    //          ^ ^
    EXPECT_THAT(find_same_number_of_bits(55, 5), Eq(2));
}

// 2018.0619
// assumes:
// 1. unsigned int
int get_number_of_bits_between_two_integers(const unsigned int a, const unsigned int b)
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

TEST(BitPattern, FindNumberOfBitsBetweenTwoIntegers_0619)
{
  //  35, 100011,   mask, 15 (1111),  max, 3(0011)
  //                                    9,   1001
  //                                  xor,   1010
  //                                  ans, 2
  EXPECT_THAT(get_number_of_bits_between_two_integers(35, 9), 2);

  // 55 = 10 0111,  mask, 7 (0111),   max, 7(0111)
  //                                    5,    101
  //                                  xor, 2( 010)
  //                                  ans, 2 
  EXPECT_THAT(get_number_of_bits_between_two_integers(55, 5), 2);
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


TEST(CxxAlgoAtoiTest, RunWithVariousValues)
{
    EXPECT_THAT(atoi_navie("123"), Eq(123));
    EXPECT_THAT(atoi_isdigit("123"), Eq(123));

    EXPECT_THAT(atoi_sign("123"), Eq(123));
    EXPECT_THAT(atoi_sign("-123"), Eq(-123));

    EXPECT_THAT(atoi_binary("1101"), Eq(13));

    EXPECT_THAT(atoi_hex("1a"), Eq(26));
    EXPECT_THAT(atoi_hex("1A"), Eq(26));
}


// ={=========================================================================
// itoa
//
// * input type? digits only? no space?
// * input size?
// * what base? 10 or 2?
// * sign support? 

// from ansic, p43.
//
// when base is 10.
//
// there is no check on the end of string input? '0' is not the same as
// 0(null) and when see any other than numbers, for loops ends.

std::string itoa_navie(const int input)
{
    int value{input};
    char letter{0};
    std::string result{};

    for (; value;)
    {
        letter = '0' + (value % 10);
        result += letter;
        value /= 10;
    }

    return std::string(result.crbegin(), result.crend());
}

std::string itoa_no_reverse(const int input)
{
    std::string result{};
    std::string temp;
    char letter{0};
    int value{input};

    for(; value;)
    {
        letter = '0' + (value % 10);
        temp += letter;
        result.insert( 0, temp );
        value /= 10;
    }

    return result;
}


// t_algo.cpp:1664: Failure
// Value of: itoa_no_reverse(123)
// Expected: is equal to 0x46e830 pointing to "123"
//   Actual: "321323" (of type std::string)
// [  FAILED  ] CxxAlgoItoaTest.RunWithVariousValues (0 ms)

TEST(DISABLED_CxxAlgoItoaTest, RunWithVariousValues)
{
    EXPECT_THAT(itoa_navie(123), Eq("123"));
    EXPECT_THAT(itoa_no_reverse(123), Eq("123"));
}


int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

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

const string roman_numeral_0711(unsigned int value)
{
  const auto roman_table = {
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

TEST(RomanConvert, 0711) 
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


// ={=========================================================================
// algo-recursion-factorial

// factorial, n!, is the product of all `positive` integers less than or equals
// to n.

// CPR 227
int factorial(int value)
{
  // CodeComplete 440, #19
  if (1 < value)
    return factorial(value-1)*value;

  return 1;
}

TEST(AlgoRecursion, FactorialRecursion) 
{
  EXPECT_THAT(factorial(5), 120);
}

// CodeComplete 397, #17.2,
// Don’t use recursion for factorials or Fibonacci numbers

int factorial_iteration(int value)
{
  int result{1};

  // for (int i = 1; i <= value; ++i)

  for (int i = 2; i <= value; ++i)
  {
    result *= i;
  }

  return result;
}

TEST(AlgoRecursion, FactorialIteration) 
{
  EXPECT_THAT(factorial_iteration(5), 120);
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

size_t water_volume_0621(const vector<int> &A)
{
  size_t last_high{}, last_high_index{};
  size_t prev{}, curr{};
  size_t volume{}, high{}, width{};

  // cout << "--------" << endl;

  for (size_t i = 0; i < A.size(); ++i)
  {
    curr = A[i];

    // trigger condition to compute volume. that is when it goes up(prev < curr)
    // and last_high is not null.
    
    if (prev < curr && last_high)
    {
      high = min(curr, last_high) - prev;
      width = (i - last_high_index) - 1;
      volume += (high*width);

      // cout << "i:" << i << " c:" << curr << " p:" << prev 
      //   << " lh:" << last_high << " lhi:" << last_high_index
      //   << " h:" << high << " w:" << width << " v:" << volume << endl;
    }

    // when it goes down and see bigger than before
    if(curr < prev && last_high < prev)
    {
      last_high = prev;
      last_high_index = i-1;
      // cout << "i:" << i << " u lh:" << last_high << endl;
    }

    prev = curr;
  }

  return volume;
}

TEST(AlgoWaterVolume, WaterVolume_0612)
{
  vector<int> coll1{2,5,1,2,3,4,7,7,6};
  EXPECT_THAT(water_volume_0621(coll1), 10);

  // fails
  // vector<int> coll2{2,5,1,3,1,2,1,7,7,6};
  // EXPECT_THAT(water_volume_0621(coll2), 17);
}

// * move the lesser max towards the opposite and by doing this removes the need
// to get the highest high before. saves one pass.
//
// * calculats volume while updating high which removes needs up/down trigger
// considerations.
//
// so calculates volume this way is okay since ww know there is other high point
// than this and we move the lesser one.

// 2018.06.22
// Cannot figure out the answer and see the model answer.

int water_volume_0621_model(const vector<int> &A)
{
  int left{}, left_high{};
  int right = A.size()-1, right_high{};
  int volume{};

  while (left < right)
  {
    if (A[left] > left_high)
      left_high = A[left];

    if (A[right] > right_high)
      right_high = A[right];

    // should have equal case since have to have one of max high.
    if (left_high >= right_high)
    {
      volume += right_high - A[right];
      --right;
    }
    else
    {
      volume += left_high - A[left];
      ++left;
    }
  }

  return volume;
}

TEST(AlgoWaterVolume, WaterVolume_0612_model)
{
  vector<int> coll1{2,5,1,2,3,4,7,7,6};
  EXPECT_THAT(water_volume_0621_model(coll1), 10);

  vector<int> coll2{2,5,1,3,1,2,1,7,7,6};
  EXPECT_THAT(water_volume_0621_model(coll2), 17);

  vector<int> coll3{2,5,4,3,4,7,6,5,4,5,7,9,5,4,3,4,5,6};
  EXPECT_THAT(water_volume_0621_model(coll3), 21);
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

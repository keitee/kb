#include "gmock/gmock.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>


// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::StrEq;
using testing::FloatEq;


// ={=========================================================================
// MakeTonen
//
// int main( ) {
//    string tmp;
//    MakeToken_0529 st("Name|Address|Phone");
//    cout << "there are " << st.countTokens( ) << " tokens.\n";
//    while (st.hasMoreTokens( )) {
//        st.nextToken(tmp);
//        cout << "token = " << tmp << '\n';
//    }
// }
//
// 1. countTokens() count number of tokens, cache it, and return it. Then how
// about hasMoreTokens()? This means that don't need to keep input_ and change
// it whenever nextToken() gets called. So count number of tokens whenever it
// gets called and keep a contailer which has tokens.
//
// int main( ) {
//    string tmp;
//    MakeToken st("Name|Address|Phone");
//    cout << "there are " << st.countTokens( ) << " tokens.\n";
//    while (st.countTokens( )) {
//        st.nextToken(tmp);
//        cout << "token = " << tmp << '\n';
//    }
// }

// TDD on 2018.05.29
// * Like cxx-boost-split, "||Name" has {"", "", "Name"}
// * Has token array member so remove hadMoreToken() and countToken() do not
// calculate count everytime when gets called.

class MakeToken_0529
{
  public:
    MakeToken_0529(const string &input, const string &delim = "|"):
      input_(input), delim_(delim)
      {
        size_t i{};
        size_t found = input_.find_first_of(delim_, i);

        while (found != string::npos)
        {
          tokens_.push_back(input_.substr(i, found-i));
          i = ++found;
          found = input_.find_first_of(delim_, i);

        }

        if (found == string::npos)
          tokens_.push_back(input_.substr(i, found));
      }

    size_t countTokens()
    {
      return tokens_.size();
    }

    void nextToken(string &token)
    {
      auto it = tokens_.begin();
      if (it != tokens_.end())
      {
        token = *it;
        tokens_.erase(it);
      }
    }

  private:
    const string input_;
    const string delim_;
    size_t number_of_tokens_{};
    vector<string> tokens_{};
};

TEST(MakeToken_0529, UseCountTokens)
{
  MakeToken_0529 st("Name|Address|Phone");

  EXPECT_THAT(st.countTokens(), Eq(3));
  EXPECT_THAT(st.countTokens(), Eq(3));
}

TEST(MakeToken_0529, UseGetTokens)
{
  vector<string> coll{};
  MakeToken_0529 st("Name|Address|Phone");
  string token;

  while (st.countTokens())
  {
    st.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
}

TEST(MakeToken_0529, UseVariousInputs)
{
    std::string token{};
    std::vector<std::string> svec{};

    // 1. "Name|Address|Phone"
    MakeToken_0529 mt1("Name|Address|Phone");
    EXPECT_EQ(3, mt1.countTokens());

    while (mt1.countTokens())
    {
        mt1.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name", "Address", "Phone"));

    // 2. "Name|Address"
    token.clear();
    svec.clear();
    MakeToken_0529 mt2("Name|Address");
    EXPECT_EQ(2, mt2.countTokens());

    while (mt2.countTokens())
    {
        mt2.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name", "Address"));

    // 3. "Name"
    token.clear();
    svec.clear();
    MakeToken_0529 mt3("Name");
    EXPECT_EQ(1, mt3.countTokens());

    while (mt3.countTokens())
    {
        mt3.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name"));

    // 4. "Name|"
    token.clear();
    svec.clear();
    MakeToken_0529 mt4("Name|");
    EXPECT_EQ(2, mt4.countTokens());

    while (mt4.countTokens())
    {
        mt4.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name", ""));

    // 5. "Name||Address"
    token.clear();
    svec.clear();
    MakeToken_0529 mt5("Name||Address");
    EXPECT_EQ(3, mt5.countTokens());

    while (mt5.countTokens())
    {
        mt5.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name", "", "Address"));

    // 6. "||Name"
    // exception where end > begin
    token.clear();
    svec.clear();
    MakeToken_0529 mt6("||Name");
    EXPECT_EQ(3, mt6.countTokens());

    while (mt6.countTokens())
    {
        mt6.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("", "", "Name"));
}


// ={=========================================================================
// StringTokenizer_0606
//
// int main( ) {
//    string s = " razzle dazzle giddyup ";
//    string tmp;
//    StringTokenizer st(s);
//    cout << "there are " << st.countTokens( ) << " tokens.\n";
//    while (st.hasMoreTokens( )) {
//        st.nextToken(tmp);
//        cout << "token = " << tmp << '\n';
//    }
// }
//
// class StringTokenizer {
//
//   private:
//     StringTokenizer() {}
//     string delim_;
//     string str_;
//     size_t count_{};
//     size_t begin_{};
//     size_t end_{};
// };
// 
// * not boost-split style which means do not support null token.

class StringTokenizer_0606
{
  public:
    StringTokenizer_0606(const string &input, const string &delim = "|")
      : input_(input), delim_(delim)
    {
      pos_token_ = input_.find_first_not_of(delim_, 0);
      pos_delim_ = input_.find_first_of(delim_, pos_token_);
    }

    bool hasMoreTokens()
    {
      return pos_token_ != pos_delim_;
    }

    void nextToken(string &token)
    {
      token = input_.substr(pos_token_, pos_delim_ - pos_token_);

      pos_token_ = input_.find_first_not_of(delim_, pos_delim_);
      pos_delim_ = input_.find_first_of(delim_, pos_token_);
    }

    size_t countTokens()
    {
      if (count_)
        return count_;

      size_t pos_start = input_.find_first_not_of(delim_, 0);
      size_t pos_delim{};
      
      while( (pos_delim = input_.find_first_of(delim_, pos_start)) != string::npos)
      {
        pos_start = input_.find_first_not_of(delim_, pos_delim);
        ++count_;
      }

      if (pos_start != string::npos && pos_delim == string::npos)
        ++count_;

      return count_;
    }

  private:
    StringTokenizer_0606() {}
    string input_;
    string delim_;
    size_t count_{};
    size_t pos_token_{};
    size_t pos_delim_{};
};

TEST(StringTokenizer_0606, UseCountTokens)
{
  StringTokenizer_0606 st1("Name|Address|Phone");
  EXPECT_EQ(st1.countTokens(), 3);

  StringTokenizer_0606 st2("Name|Address");
  EXPECT_EQ(2, st2.countTokens());

  StringTokenizer_0606 st3("Name");
  EXPECT_EQ(1, st3.countTokens());

  StringTokenizer_0606 st4("Name||Address");
  EXPECT_EQ(2, st4.countTokens());

  StringTokenizer_0606 st5("||Name");
  EXPECT_EQ(1, st5.countTokens());

  StringTokenizer_0606 st6("|||");
  EXPECT_EQ(0, st6.countTokens());
}

TEST(StringTokenizer_0606, ParseTokensFromVariosInputs)
{
  string token{};
  vector<string> coll{};

  StringTokenizer_0606 st1("Name|Address|Phone");
  EXPECT_EQ(st1.countTokens(), 3);

  while (st1.hasMoreTokens())
  {
    st1.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));

  // 2. "Name|Address"
  token.clear();
  coll.clear();
  StringTokenizer_0606 st2("Name|Address");
  EXPECT_EQ(2, st2.countTokens());

  while (st2.hasMoreTokens())
  {
    st2.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name", "Address"));

  // 3. "Name"
  token.clear();
  coll.clear();
  StringTokenizer_0606 st3("Name");
  EXPECT_EQ(1, st3.countTokens());

  while (st3.hasMoreTokens())
  {
    st3.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name"));

  // 4. "Name|"
  token.clear();
  coll.clear();
  StringTokenizer_0606 st4("Name|");
  EXPECT_EQ(1, st4.countTokens());

  while (st4.hasMoreTokens())
  {
    st4.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name"));

  // 5. "Name||Address"
  token.clear();
  coll.clear();
  StringTokenizer_0606 st5("Name||Address");
  EXPECT_EQ(2, st5.countTokens());

  while (st5.hasMoreTokens())
  {
    st5.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name", "Address"));

  // 6. "||Name"
  // exception where end > begin
  token.clear();
  coll.clear();
  StringTokenizer_0606 st6("||Name");
  EXPECT_EQ(1, st6.countTokens());

  while (st6.hasMoreTokens())
  {
    st6.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name"));
}


TEST(StringStream, X)
{
  size_t number_of_entries{};
  cin >> number_of_entries;

  map<string, string> phone_map{};

  for (auto i = 0; i < number_of_entries; ++i)
  {
    // string line;
    // getline(cin >> ws, line);
    // stringstream ss(line);

    string name, number;
    ss >> name;
    ss >> number;

    phone_map[name] = number;
  }

  string query;

  while (cin >> query)
  {
    if (phone_map.find(query) != phone_map.end())
      cout << query << "=" << phone_map[query] << endl;
    else
      cout << "Not found" << endl;
  }
}


// ={=========================================================================

int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
    }

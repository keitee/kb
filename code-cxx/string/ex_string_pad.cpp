#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <bitset>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::StrEq;
using testing::FloatEq;


namespace tokenizer_2018_11_split {

  // use split()

  // o '|' is default delim
  //
  // int main( ) {
  //    string s = " razzle dazzle giddyup ";
  //    string tmp;
  //    StringTokenizer st(s, "|");
  //    cout << "there are " << st.countTokens( ) << " tokens.\n";
  //    while (st.hasMoreTokens( )) {
  //        st.nextToken(tmp);
  //        cout << "token = " << tmp << '\n';
  //    }
  // }

  class StringTokenizer
  {
    public:
    explicit StringTokenizer(const string &s, const char delim = '|')
      : delim_(delim)
    {
      split(s, delim);
    }

    int countTokens() { return token_.size(); }

    // meaning of empty()
    bool hasMoreTokens() { return !token_.empty(); }

    // have to remove one
    void nextToken(string &next) 
    { 
      // since vector don't have interface to pop from the front
      next = token_.front();
      token_.erase(token_.begin());
    }

    private:

    void split(const string &s, const char delim)
    {
      auto length = s.length();
      auto end = s.find(delim);
      decltype(end) start{};
      string token;

      while (end < length)
      {
        token = s.substr(start, end - start);
        if (!token.empty())
          token_.push_back(token);

        // this fails on caes which has empty token like "||Name" so have to
        // have to check before inserting and this is improvement from
        // *string-split*
        //
        // token_.push_back(s.substr(start, end - start));

        start = end + 1;
        end = s.find(delim, start);
      }

      // // this covers either last or single split
      // token_.push_back(s.substr(start, end - start));

      // this covers either last or single split
      token = s.substr(start, end - start);
      if (!token.empty())
        token_.push_back(token);
    }

    vector<string> token_;
    char delim_;
  };

} // namespace


// cases:
//
// "Name|Address|Phone"
// "Name|Address"
// "Name"
// "Name|"
// "Name||Address"
// "||Name"
//  "||"

TEST(StringTokenizer, 2018_11_split)
{
  using namespace tokenizer_2018_11_split;

  // "Name|Address|Phone"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|Address|Phone");

    EXPECT_EQ(3, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
  }

  // "Name|Address"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|Address");

    EXPECT_EQ(2, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address"));
  }

  // "Name"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  // "Name|"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  // "Name||Address"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name||Address");

    EXPECT_EQ(2, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address"));
  }

  // "||Name"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("||Name");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  //  "||"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("||");

    EXPECT_EQ(0, st.countTokens());
  }

  // this fails
  //
  // //  ""
  // {
  //   std::string token{};
  //   std::vector<std::string> coll{};

  //   StringTokenizer st("");

  //   EXPECT_EQ(0, st.countTokens());

  //   for (int i = 0; i < 5; ++i)
  //   {
  //     st.nextToken(token);
  //     if (!token.empty())
  //       coll.push_back(token);
  //   }

  //   EXPECT_TRUE(coll.empty());
  // }
}

namespace tokenizer_2018_11_find {

  // use find() family
  // do not use internal coll such as vector which means that do everything on
  // the fly when requested

  // o '|' is default delim
  // o countToken() returns # of tokens of input
  //
  // int main( ) {
  //    string s = " razzle dazzle giddyup ";
  //    string tmp;
  //    StringTokenizer st(s, "|");
  //    cout << "there are " << st.countTokens( ) << " tokens.\n";
  //    while (st.hasMoreTokens( )) {
  //        st.nextToken(tmp);
  //        cout << "token = " << tmp << '\n';
  //    }
  // }

  class StringTokenizer
  {
    public:
      StringTokenizer(const string &s, const char delim = '|')
      : input_(s), delim_(delim)
      {
        count_ = 0;
        start_ = input_.find_first_not_of(delim_);
        end_ = input_.find_first_of(delim_, start_);
      }

      void nextToken(string &s)
      {
        // works for all except when call nextToken() without checking on
        // hasMore(). crashes.
        //
        // s = input_.substr(start_, end_ - start_);
        // start_ = input_.find_first_not_of(delim_, end_);
        // end_ = input_.find_first_of(delim_, start_);

        if (start_ != string::npos && end_ != string::npos)
        {
          s = input_.substr(start_, end_ - start_);
          start_ = input_.find_first_not_of(delim_, end_);
          end_ = input_.find_first_of(delim_, start_);
        }
        else if (start_ != string::npos && end_ == string::npos)
        {
          s = input_.substr(start_, end_ - start_);
          start_ = input_.find_first_not_of(delim_, end_);
        }
      }

      bool hasMoreTokens()
      {
        return start_ != end_;
      }

      size_t countTokens()
      {
        if (count_)
          return count_;

        // use two variables:
        //
        // decltype(start_) start{}, end{};
        // 
        // start = input_.find_first_not_of(delim_);
        // end = input_.find_first_of(delim_);
        //
        // while (end < string::npos)
        // {
        //   ++count_;
        //   start = input_.find_first_not_of(delim_, end);
        //   end = input_.find_first_of(delim_, start);
        // }


        // do not support cases which has only delims:
        //
        // decltype(start_) run{};
        //
        // run = input_.find_first_of(delim_);
        //
        // while (run < string::npos)
        // {
        //   ++count_;
        //   run = input_.find_first_not_of(delim_, run);
        //   run = input_.find_first_of(delim_, run);
        // }
        //
        // return ++count_;


        // same approach as the book. the difference is that
        // o do find_first_not_of() and this makes no need to increase count out
        // of while loop which enables to return 0.
        //
        // o there is at least one token whenever see a char which is not delim,
        // so find_first_not_of()

        decltype(start_) run{};

        while (run < string::npos)
        {
          if ((run = input_.find_first_not_of(delim_, run)) == string::npos)
            break;

          ++count_;
          run = input_.find_first_of(delim_, run);
        }

        return count_;
      }

    private:
      string input_;
      char delim_;
      size_t start_, end_;
      decltype(start_) count_;
  };

} // namespace


TEST(StringTokenizer, 2018_11_find)
{
  using namespace tokenizer_2018_11_find;

  // "Name|Address|Phone"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|Address|Phone");

    EXPECT_EQ(3, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
  }

  // "Name|Address"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|Address");

    EXPECT_EQ(2, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address"));
  }

  // "Name"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  // "Name|"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  // "Name||Address"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name||Address");

    EXPECT_EQ(2, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address"));
  }

  // "||Name"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("||Name");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  //  "||"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("||");

    EXPECT_EQ(0, st.countTokens());
  }

  //  ""
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("");

    EXPECT_EQ(0, st.countTokens());

    for (int i = 0; i < 5; ++i)
    {
      st.nextToken(token);
      if (!token.empty())
        coll.push_back(token);
    }

    EXPECT_TRUE(coll.empty());
  }
}


// ={=========================================================================
int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

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


// use split()

namespace tokenizer_2018_11 {

  class StringTokenizer
  {
    public:
    explicit StringTokenizer(const string &s, const char delim = '|')
      : delim_(delim)
    {
      split(s, delim);
    }

    int countTokens() { return token_.size(); }
    bool hasMoreTokens() { return !token_.empty(); }
    void nextToken(string &next) 
    { 
      next = token_.front();
      token_.erase(token_.begin());
    }

    private:

    void split(const string &s, const char delim)
    {
      auto length = s.length();
      auto end = s.find(delim);
      decltype(end) start{};
      string x;

      while (end < length)
      {
        token_.push_back(s.substr(start, end - start));

        start = end + 1;
        end = s.find(delim, start);
      }

      // this covers either last or single split
      token_.push_back(s.substr(start, end - start));
    }

    vector<string> token_;
    char delim_;
  };

} // namespace


TEST(StringTokenizer, 2018_11)
{
  using namespace tokenizer_2018_11;

  std::string token{};
  std::vector<std::string> svec{};

  StringTokenizer st("Name|Address|Phone");

  EXPECT_EQ(3, st.countTokens());

  while (st.hasMoreTokens())
  {
    st.nextToken(token);
    svec.push_back(token);
  }

  EXPECT_THAT(svec, ElementsAre("Name", "Address", "Phone"));
}


// ={=========================================================================
int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

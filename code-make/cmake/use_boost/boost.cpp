#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <bitset>
#include <boost/lexical_cast.hpp>
#include <regex>

#include <gmock/gmock.h>

// g++ -g -std=c++0x t_override.cpp

using namespace std;

// ={=========================================================================
// string-pos

TEST(Boost, BooxtLexicalCast)
{
  EXPECT_THAT(boost::lexical_cast<std::string>(11), "11");
  EXPECT_THAT(boost::lexical_cast<std::string>(3301), "3301");

  EXPECT_THAT(boost::lexical_cast<int>("11"), 11);
  EXPECT_THAT(boost::lexical_cast<int>("3301"), 3301);
}


// ={=========================================================================
int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

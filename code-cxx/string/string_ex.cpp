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


// ={=========================================================================
// string-pos

TEST(StringIterator, PosEnd)
{
  string coll{"12345"};

  EXPECT_EQ(coll.size(), 5);
  EXPECT_EQ(coll.length(), 5);

  // this is error since iterator is not int
  // EXPECT_EQ(coll.end(), 5);
  
  EXPECT_EQ(*coll.end(), '\0');
}

// ={=========================================================================
int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

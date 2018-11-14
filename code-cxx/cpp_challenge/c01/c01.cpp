#include "gmock/gmock.h"

#include <iostream>
#include <cctype>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

2. Greatest common divisor

The greatest common divisor (gcd in short) of two or more non-zero integers,
also known as the greatest common factor (gcf), highest common factor (hcf),
greatest common measure (gcm), or highest common divisor, is the greatest
positive integer that divides all of them. There are several ways the gcd could
be computed; an efficient method is Euclid's algorithm. For two integers, the
algorithm is:

gcd(a,0) = a
gcd(a,b) = gcd(b, a mod b)

*/

unsigned int gcd_recursive(unsigned int const a, unsigned int const b)
{
  return (b == 0) ? a : gcd_recursive(b, a % b);
}

unsigned int gcd_iteratitive(unsigned int a, unsigned int b)
{
  while (b != 0)
  {
    unsigned int r = a % b;
    a = b;
    b = r;
  }
  return a;
}

TEST(CH3,GreatestCommonDivisor)
{
  const auto inputs{ 
    make_pair(3, 12), 
    make_pair(2, 34), 
    make_pair(13, 245)
  };

  unsigned int r1{}, r2{};

  for (const auto e : inputs)
  {
    r1 = gcd_recursive(e.first, e.second);
    r2 = gcd_iteratitive(e.first, e.second);

    // cout << "r1: " << r1 << ", r2: " << r2 << endl;

    EXPECT_EQ(r1, r2);
  }
}


/*

14. Validating ISBNs                                                        

The International Standard Book Number (ISBN) is a unique numeric identifier for
books. Currently, a 13-digit format is used. However, for this problem, you are
to validate the former format that used 10 digits. 

The last of the 10 digits is a checksum. This digit is chosen so that the sum of
all the ten digits, each multiplied by its (integer) weight, descending from 10
to 1, is a multiple of 11.

The validate_isbn_10 function, shown as follows, takes an ISBN as a string, and
returns true if the length of the string is 10, all ten elements are digits, and
the sum of all digits multiplied by their weight (or position) is a multiple of
11:

You can take it as a further exercise to improve this function to also correctly
validate ISBN-10 numbers that include hyphens, such as 3-16-148410-0. Also, you
can write a function that validates ISBN-13 numbers.

*/

bool validate_isbn_10(const string isbn)
{
  int sum{};

  // why compile error?
  // if (10 == isbn.size() &&
  //     10 == count_if(isbn.begin(), isbn.end(), std::isdigit))

  if (10 == isbn.size() &&
      10 == count_if(isbn.begin(), isbn.end(), [](const char c){return isdigit(c);}))
  {
    int weight=10;
    sum = accumulate(isbn.begin(), isbn.end(), 0,
        [&](int init, char elem)
        {
          return init + weight--*(elem-'0');
        });
  }

  return (sum % 11) ? false : true;
}

TEST(CH3,ValidateISBN)
{
  auto result = validate_isbn_10("7500558088");
  EXPECT_EQ(result, false);
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

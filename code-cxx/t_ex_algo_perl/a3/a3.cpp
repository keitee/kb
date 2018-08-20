#include <iostream>
#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;

// ={=========================================================================

// Raw sizeof:
// sizeof(char)      = 1
// sizeof(short)     = 2
// sizeof(int)       = 4
// sizeof(float)     = 4
// sizeof(structc *) = 8
// sizeof(long)      = 8
// sizeof(double)    = 8
//
// Measure macro:
//             int    4 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//         structc    1 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//        structic    8 :   32 :   32 :  208 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//        structip   16 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//        structdc   16 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//        structcd   16 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structcdc   24 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structiii   12 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structiic   12 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structc12   12 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structc13   13 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structc28   28 :  112 :  992 : 3248 :   48 :   48 :   48 :   48 :   48 :   48 :   48
//       structc29   29 :   48 :   48 :   48 :   48 :   48 :   48 :   48 :   48 :   48 :   48
//       structc44   44 :  944 : 5216 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64
//       structc45   45 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64
//       structc60   60 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80
//       structc61   61 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80
//
// Measure template:
//  measure: int   4
//  measure: structc       1
//  measure: structic      8

/* 
Copyright (C) 1999 Lucent Technologies
From 'Programming Pearls' by Jon Bentley
spacemod.cpp -- simple model for C++ space
*/

#define MEASURE(T, text) { \
  cout << setw(15) << text; \
  cout << setw(5) << sizeof(T); \
  int lastp = 0; \
  for (int i = 0; i < 11; ++i) { \
    T *p = new T; \
    int thisp = (int) p; \
    if (lastp != 0) \
      cout << " : " << setw(4) << thisp - lastp; \
    lastp = thisp; \
  } \
  cout << "\n"; \
}


// must use macros since template give funny answers.
// this seems okay now. 

template <typename T>
void measure(char *text)
{
  cout << " measure: " << text << "\t";
  cout << sizeof(T) << "\n";
}

struct structc { char c; };
struct structic { int i; char c; };
struct structip { int i; structip *p; };
struct structdc { double d; char c; };
struct structcd { char c; double d; };
struct structcdc { char c1; double d; char c2; };
struct structiii { int i1; int i2; int i3; };
struct structiic { int i1; int i2; char c; };
struct structc12 { char c[12]; };
struct structc13 { char c[13]; };
struct structc28 { char c[28]; };
struct structc29 { char c[29]; };
struct structc44 { char c[44]; };
struct structc45 { char c[45]; };
struct structc60 { char c[60]; };
struct structc61 { char c[61]; };

TEST(Appendix3, SpaceMod)
{
  cout << "Raw sizeof: \n"
    << "sizeof(char)      = " << sizeof(char) << endl
    << "sizeof(short)     = " << sizeof(short) << endl
    << "sizeof(int)       = " << sizeof(int) << endl
    << "sizeof(float)     = " << sizeof(float) << endl
    << "sizeof(structc *) = " << sizeof(structc *) << endl
    << "sizeof(long)      = " << sizeof(long) << endl
    << "sizeof(double)    = " << sizeof(double) << endl;

  cout << endl << "Measure macro: \n";
  MEASURE(int, "int");
  MEASURE(structc, "structc");
  MEASURE(structic, "structic");
  MEASURE(structip, "structip");
  MEASURE(structdc, "structdc");
  MEASURE(structcd, "structcd");
  MEASURE(structcdc, "structcdc");
  MEASURE(structiii, "structiii");
  MEASURE(structiic, "structiic");
  MEASURE(structc12, "structc12");
  MEASURE(structc13, "structc13");
  MEASURE(structc28, "structc28");
  MEASURE(structc29, "structc29");
  MEASURE(structc44, "structc44");
  MEASURE(structc45, "structc45");
  MEASURE(structc60, "structc60");
  MEASURE(structc61, "structc61");
  
  cout << endl << "Measure template: \n";
  measure<int>("int");
  measure<structc>("structc");
  measure<structic>("structic");
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

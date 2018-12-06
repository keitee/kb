#include <iostream>
#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;

// ={=========================================================================

// Copyright (C) 1999 Lucent Technologies
// From 'Programming Pearls' by Jon Bentley
// spacemod.cpp -- simple model for C++ space
// 
// (Linux kt-office-debian 4.9.0-7-686-pae #1 SMP Debian 4.9.110-3+deb9u1 (2018-08-03) 
// i686 GNU/Linux) 
//
// Raw sizeof:
// sizeof(char)      = 1
// sizeof(short)     = 2
// sizeof(int)       = 4
// sizeof(float)     = 4
// sizeof(structc *) = 4
// sizeof(long)      = 4
// sizeof(double)    = 8
// 
// Measure macro:
//             int    4 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16
//         structc    1 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16
//        structic    8 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16
//        structip    8 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16
//        structdc   12 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16
//        structcd   12 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16
//       structcdc   16 :   24 :   24 :   24 :   24 :   24 :   24 :   24 :   24 :   24 :   24
//       structiii   12 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16
//       structiic   12 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16
//       structc12   12 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16 :   16
//       structc13   13 :   24 :   24 :   24 :   24 :   24 :   24 :   24 :   24 :   24 :   24
//       structc28   28 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structc29   29 :   40 :   40 :   40 :   40 :   40 :   40 :   40 :   40 :   40 :   40
//       structc44   44 :   48 :   48 :   48 :   48 :   48 :   48 :   48 :   48 :   48 :   48
//       structc45   45 :   56 :   56 :   56 :   56 :   56 :   56 :   56 :   56 :   56 :   56
//       structc60   60 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64
//       structc61   61 :   72 :   72 :   72 :   72 :   72 :   72 :   72 :   72 :   72 :   72
// 
// Measure template:
//  measure: int   4
//  measure: structc       1
//  measure: structic      8
//
// (VM, Linux kit-debian64 3.16.0-4-amd64 #1 SMP Debian 3.16.36-1+deb8u1 (2016-09-03) x86_64 GNU/Linux)
//
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
//
// (Linux st-castor-03.cisco.com 3.10.0-514.26.2.el7.x86_64 #1 SMP Tue Jul 4 15:04:05 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux)
//
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
//             int    4 :   32 : -1136 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//         structc    1 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//        structic    8 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :  608 :   32
//        structip   16 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//        structdc   16 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//        structcd   16 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structcdc   24 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structiii   12 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structiic   12 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structc12   12 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structc13   13 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32 :   32
//       structc28   28 :   48 : -336 : 3440 :   48 :   48 :   48 :   48 :   48 :   48 :   48
//       structc29   29 :   48 :   48 :   48 :   48 :   48 :   48 :   48 :   48 :   48 :   48
//       structc44   44 :   64 : 3904 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64
//       structc45   45 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64 :   64
//       structc60   60 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80
//       structc61   61 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80 :   80
// 
// Measure template:
//  measure: int   4
//  measure: structc       1
//  measure: structic      8

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

TEST(Appendix3, SpaceCostModel)
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


/*
 Copyright (C) 1999 Lucent Technologies
 From 'Programming Pearls' by Jon Bentley
 timemod.cpp -- simple model for C++ run time costs


 (VM, Linux kit-debian64 3.16.0-4-amd64 #1 SMP Debian 3.16.36-1+deb8u1 (2016-09-03) 
 x86_64 GNU/Linux)

 C Time Cost Model, n=5000
 
 Integer Arithmetic(n=5000)
  {}                         36797     36729     35255     35179     35139       1
  k++                        49811     49684     48032     48518     49267       2
  k = i + j                  40464     40432     38640     38963     39150       2
  k = i - j                  39693     40412     38841     38921     39535       2
  k = i * j                  39111     38745     37284     37214     37793       2
  k = i / j                  55687     55335     53961     55504     53681       2
  k = i % j                  56043     55277     55272     54311     53333       2
  k = i & j                  39602     40539     38743     39598     38617       2
  k = i | j                  39654     39877     38924     39506     38434       2
 
 Floating Point Arithmetic(n=5000)
  fj=j;                      57166     57057     57058     56929     55879       2
  fj=j; fk = fi + fj;        46843     46379     45374     46174     45746       2
  fj=j; fk = fi - fj;        49856     46376     46492     46366     48340       2
  fj=j; fk = fi * fj;        47819     46584     46612     46887     45862       2
  fj=j; fk = fi / fj;        46528     44958     43810     46752     49824       2
 
 Array Operations(n=5000)
  k = i + j                  39115     39279     38530     37596     37984       2
  k = x[i] + j               41939     40577     39627     39737     39490       2
  k = i + x[j]               49330     48821     47019     49139     50191       2
  k = x[i] + x[j]            55689     52794     51838     52385     52561       2
 
 Comparisons(n=5000)
  if (i < j) k++             42400     41887     40565     40890     41131       2
  if (x[i] < x[j]) k++      107669    102913    102090    102485    103682       4
 
 Array Comparisons and Swaps(n=5000)
  k = (x[i]<x[k]) ? -1:1     37253     37922     36369     35770     37140       1
  k = intcmp(x+i, x+j)      122573    119309    126385    136072    119631       5
  swapmac(i, j)              62142     61524     59777     59922     59717       2
  swapfunc(i, j)            117157    114001    115626    117863    114425       5
 
 Max Function, Macro and Inline(n=5000)
  k = (i > j) ? i : j        49272     47541     47194     46313     46994       2
  k = maxmac(i, j)           48781     48097     46721     47788     46927       2
  k = maxfunc(i, j)         107599    102703    104602    109470    117149       4
 
 Math Functions(n=1000)
  fk = j+fi;                  1604      1664      1549      1723      1500       2
  k = rand();                23308     21475     21629     21416     20340      22
  fk = sqrt(j+fi)             4377      4276      4218      4486      4312       4
  fk = sin(j+fi)             19498     18466     18478     18086     18083      19
  fk = sinh(j+fi)            35833     38332     35108     34404     35250      36
  fk = asin(j+fi)            29286     31486     28412     29238     29298      30
  fk = cos(j+fi)             18042     19361     19236     18553     19163      19
  fk = tan(j+fi)            228162    220731    221708    221563    223463     223
 
 Memory Allocation(n=500)
  free(malloc(16));           8529      8328      8476      9696      9157      35
  free(malloc(100));         10416     10867      9707     10160      9297      40
  free(malloc(2000));        11163     10934     12934     10745     11737      46
 
 Memory Allocation - malloc/free(n=500)
  free(malloc(sizeof(structc12)))      9289      8363      8632      9224      8999      36
  free(malloc(sizeof(structc13)))      8433      9255      8419      9635      8864      36
  free(malloc(sizeof(structc28)))      9465      8860      8307     10570      8371      36
  free(malloc(sizeof(structc29)))      9392      8215      8942     10547      8787      37
  free(malloc(sizeof(structc44)))      8658      8235      8329      9214      8865      35
  free(malloc(sizeof(structc45)))      8507      8669      8247      8501     10739      36
  free(malloc(sizeof(structc60)))      8958      8892      8400      9506     10322      37
  free(malloc(sizeof(structc61)))      9031      8795     11371      9443     10132      39
 
 Memory Allocation - new/delete(n=500)
  delete(new structc12)      16235     13919     14321     12987     12597      56
  delete(new structc13)      12949     12805     14786     13563     12831      54
  delete(new structc28)      12787     12166     13438     13130     13259      52
  delete(new structc29)      12968     13382     14429     13003     12834      53
  delete(new structc44)      12671     13852     13652     13849     12834      53
  delete(new structc45)      12739     14240     13389     12750     12897      53
  delete(new structc60)      12815     13391     12964     13081     13502      53
  delete(new structc61)      13320     12894     14239     14814     12952      55
  Secs: 10.33


(Linux st-castor-03.cisco.com 
 3.10.0-514.26.2.el7.x86_64 #1 SMP Tue Jul 4 15:04:05 UTC 2017 
 x86_64 x86_64 x86_64 GNU/Linux)

C Time Cost Model, n=5000                                                                                                                                                                             [19/1657]

Integer Arithmetic(n=5000)
 {}                         60000     50000     60000     50000     50000       2
 k++                        70000     60000     60000     70000     60000       3
 k = i + j                  70000     60000     70000     70000     70000       3
 k = i - j                  60000     60000     70000     70000     70000       3
 k = i * j                  50000     60000     50000     50000     50000       2
 k = i / j                  80000     70000     80000     70000     70000       3
 k = i % j                  70000     70000     80000     70000     80000       3
 k = i & j                  60000     70000     60000     70000     60000       3
 k = i | j                  70000     70000     60000     70000     70000       3

Floating Point Arithmetic(n=5000)
 fj=j;                      50000     60000     60000     50000     60000       2
 fj=j; fk = fi + fj;       100000     90000    100000     90000    100000       4
 fj=j; fk = fi - fj;       100000     90000    100000    100000    100000       4
 fj=j; fk = fi * fj;       110000    110000    110000    120000    110000       4
 fj=j; fk = fi / fj;       180000    180000    180000    180000    180000       7

Array Operations(n=5000)
 k = i + j                  60000     70000     60000     70000     60000       3
 k = x[i] + j               60000     70000     60000     70000     60000       3
 k = i + x[j]               60000     60000     50000     60000     50000       2
 k = x[i] + x[j]            60000     60000     60000     60000     60000       2

Comparisons(n=5000)
 if (i < j) k++             50000     60000     60000     60000     50000       2
 if (x[i] < x[j]) k++      130000    120000    110000    120000    120000       5

Array Comparisons and Swaps(n=5000)
 k = (x[i]<x[k]) ? -1:1     50000     50000     40000     50000     50000       2
 k = intcmp(x+i, x+j)       80000     90000     90000     90000     80000       3
 swapmac(i, j)              70000     80000     70000     70000     80000       3
 swapfunc(i, j)            100000    100000    100000    100000    100000       4

Max Function, Macro and Inline(n=5000)
 k = (i > j) ? i : j        60000     60000     60000     60000     70000       2
 k = maxmac(i, j)           60000     60000     70000     60000     60000       2
 k = maxfunc(i, j)          80000     70000     80000     70000     80000       3

Math Functions(n=1000)
 fk = j+fi;                     0     10000         0         0         0       2
 k = rand();                10000     10000     10000     10000     10000      10
 fk = sqrt(j+fi)            10000     10000     10000     10000     10000      10
 fk = sin(j+fi)             40000     30000     30000     30000     30000      32
 fk = sinh(j+fi)            20000     20000     20000     10000     20000      18
 fk = asin(j+fi)            10000     10000     10000     10000     10000      10
 fk = cos(j+fi)             40000     30000     40000     40000     30000      36
 fk = tan(j+fi)            260000    230000    230000    240000    230000     238

Memory Allocation(n=500)
 free(malloc(16));          10000     10000         0     10000         0      24
 free(malloc(100));         10000         0     10000         0     10000      24
 free(malloc(2000));        10000     10000         0     10000     10000      32

Memory Allocation - malloc/free(n=500)
 free(malloc(sizeof(structc12)))         0     10000         0     10000         0      16
 free(malloc(sizeof(structc13)))     10000     10000         0     10000         0      24
 free(malloc(sizeof(structc28)))     10000         0     10000         0     10000      24
 free(malloc(sizeof(structc29)))     10000         0     10000         0     10000      24
 free(malloc(sizeof(structc44)))         0     10000         0     10000         0      16
 free(malloc(sizeof(structc45)))     10000         0     10000     10000         0      24
 free(malloc(sizeof(structc60)))     10000         0     10000         0     10000      24
 free(malloc(sizeof(structc61)))         0     10000         0     10000         0      16

Memory Allocation - new/delete(n=500)
 delete(new structc12)      10000     10000         0     10000     10000      32
 delete(new structc13)          0     10000         0     10000     10000      24
 delete(new structc28)          0     10000         0     10000     10000      24
 delete(new structc29)          0     10000         0     10000     10000      24
 delete(new structc44)          0     10000         0     10000     10000      24
 delete(new structc45)          0     10000         0     10000     10000      24
 delete(new structc60)          0     10000     10000         0     10000      24
 delete(new structc61)          0     10000     10000         0     10000      24
 Secs: 12.61


(Linux kt-office-debian 4.9.0-7-686-pae #1 SMP Debian 4.9.110-3+deb9u1 (2018-08-03) 
i686 GNU/Linux) 

C Time Cost Model, n=5000

Integer Arithmetic(n=5000)
 {}                         49754     49069     49329     49408     49196       2
 k++                        56924     56908     56908     56904     56904       2
 k = i + j                  61400     61391     61379     61531     61371       2
 k = i - j                  63604     63602     63601     63602     63597       3
 k = i * j                  63616     63650     63601     63612     63600       3
 k = i / j                  77444     77438     77444     77452     77440       3
 k = i >> j                 68434     67582     68487     68167     68052       3
 k = i % j                  77454     77444     77446     77449     77448       3
 k = i & j                  62780     61798     61910     61787     62196       2
 k = i | j                  63594     63590     63594     63602     63599       3

Floating Point Arithmetic(n=5000)
 fj=j;                      57184     56919     56935     56914     56917       2
 fj=j; fk = fi + fj;        66965     66965     66969     66966     66964       3
 fj=j; fk = fi - fj;        66965     66965     66960     66964     66964       3
 fj=j; fk = fi * fj;        66989     66981     66980     66985     66981       3
 fj=j; fk = fi / fj;       183729    183733    183728    183727    183731       7

Array Operations(n=5000)
 k = i + j                  61463     61368     61369     61406     61371       2
 k = x[i] + j               55676     55365     55365     55361     55438       2
 k = i + x[j]               58774     58771     58770     58845     58782       2
 k = x[i] + x[j]            63739     63732     63731     63733     63736       3

Comparisons(n=5000)
 if (i < j) k++             65165     65001     65136     65130     65128       3
 if (x[i] < x[j]) k++      117402    117397    117404    117392    117397       5

Array Comparisons and Swaps(n=5000)
 k = (x[i]<x[k]) ? -1:1     62769     62756     62758     62958     62779       3
 k = intcmp(x+i, x+j)      133261    131906    132740    133142    132273       5
 swapmac(i, j)              93827     93817     93836     93818     93821       4
 swapfunc(i, j)            177569    177658    177587    177601    177663       7

Max Function, Macro and Inline(n=5000)
 k = (i > j) ? i : j        70632     70587     70588     70583     70620       3
 k = maxmac(i, j)           71486     71435     70847     70957     71190       3
 k = maxfunc(i, j)         140236    140248    140231    140232    140305       6

Math Functions(n=1000)
 fk = j+fi;                  1886      1848      1855      1848      1855       2
 k = rand();                14998     15001     15007     15000     14995      15
 fk = sqrt(j+fi)            11748     11738     11736     11736     11736      12
 fk = sin(j+fi)             26410     26409     26400     26408     26410      26
 fk = sinh(j+fi)           138529    140887    142813    142813    142712     142
 fk = asin(j+fi)           105416    105415    105410    105414    105435     105
 fk = cos(j+fi)             26586     26603     26639     26633     26637      27
 fk = tan(j+fi)            658495    658682    657875    657161    657684     658

Memory Allocation(n=500)
 free(malloc(16));           8993      8978      8990      8982      8976      36
 free(malloc(100));         12658     12666     12665     12666     12688      51
 free(malloc(2000));        12303     12405     12442     12289     12392      49

Memory Allocation - malloc/free(n=500)
 free(malloc(sizeof(structc12)))      8967      8965      8958      8956      8957      36
 free(malloc(sizeof(structc13)))      8964      8957      8957      8957      8962      36
 free(malloc(sizeof(structc28)))      8958      8956      8957      8957      8962      36
 free(malloc(sizeof(structc29)))      8957      8957      8957      8962      8957      36
 free(malloc(sizeof(structc44)))      8958      8957      8962      8956      8958      36
 free(malloc(sizeof(structc45)))      8957      8962      8956      8959      8957      36
 free(malloc(sizeof(structc60)))      8962      8957      8957      8957      8963      36
 free(malloc(sizeof(structc61)))     12972     12992     13057     12968     12905      52

Memory Allocation - new/delete(n=500)
 delete(new structc12)      10384     10384     10383     10376     10324      41
 delete(new structc13)      10380     10382     10383     10377     10382      42
 delete(new structc28)      10376     10391     10294     10358     10346      41
 delete(new structc29)      10406     10407     10377     10383     10382      42
 delete(new structc44)      10415     10453     10397     10299     10299      41
 delete(new structc45)      10377     10382     10378     10387     10384      42
 delete(new structc60)      10378     10382     10348     10382     10315      41
 delete(new structc61)      14727     14699     14743     14809     14728      59
 Secs: 17.20

*/

#define MAXN 100000
int x[MAXN];
int startn = 5000;
int n = MAXN;

// functions to be timed
int intcmp(int *i, int *j)
{
  return *i - *j;
}

#define swapmac(i, j) \
{ \
  t = x[i]; x[i] = x[j]; x[j] = t; \
}

void swapfunc(int i, int j)
{
  int t = x[i];
  x[i] = x[j];
  x[j] = t;
}

#define maxmac(a, b) ((a) > (b) ? (a) : (b))

int maxfunc(int a, int b)
{
  return a > b ? a : b;
}

// workhorse

// "nans" is the average time cost in nanoseconds per operation.

#define T(s) cout << endl << s << "(n=" << n << ")" << endl
#define TRIALS 5
#define M(op) \
  printf(" %-22s", #op); \
  k = 0; \
  timesum = 0; \
  for (ex = 0; ex < TRIALS; ++ex) \
  { \
    start = clock(); \
    for (i = 1; i <= n; ++i) \
    { \
      fi = (float) i; \
      for (j = 1; j <= n; ++j) \
      { \
        op; \
      } \
    } \
    t = clock() - start; \
    printf("%10d", t); \
    timesum += t; \
  } \
  nans = 1e9 * timesum / ((double) n*n * TRIALS * CLOCKS_PER_SEC); \
  printf("%8.0f\n", nans);


TEST(Appendix3, TimeCostModel)
{ 
  int i, j, k;
  float fi, fj, fk;
  int t, ex, timesum, start, globalstart;
  double nans;

  globalstart = clock();
  for (i = 0; i < n; i++)
    x[i] = rand();
  n = startn;
  printf("C Time Cost Model, n=%d\n", n);

  T("Integer Arithmetic");

  M({});
  M(k++);
  M(k = i + j);
  M(k = i - j);
  M(k = i * j);
  M(k = i / j);
  M(k = i >> j);
  M(k = i % j);
  M(k = i & j);
  M(k = i | j);

  T("Floating Point Arithmetic");
  M(fj=j;);
  M(fj=j; fk = fi + fj;);
  M(fj=j; fk = fi - fj;);
  M(fj=j; fk = fi * fj;);
  M(fj=j; fk = fi / fj;);

  T("Array Operations");
  M(k = i + j);
  M(k = x[i] + j);
  M(k = i + x[j]);
  M(k = x[i] + x[j]);

  T("Comparisons");
  M(if (i < j) k++);
  M(if (x[i] < x[j]) k++);

  T("Array Comparisons and Swaps");
  M(k = (x[i]<x[k]) ? -1:1);
  M(k = intcmp(x+i, x+j));
  M(swapmac(i, j));
  M(swapfunc(i, j));

  T("Max Function, Macro and Inline");
  M(k = (i > j) ? i : j);
  M(k = maxmac(i, j));
  M(k = maxfunc(i, j));
  n = startn / 5;

  T("Math Functions");
  M(fk = j+fi;);
  M(k = rand(););
  M(fk = sqrt(j+fi));
  M(fk = sin(j+fi));
  M(fk = sinh(j+fi));
  M(fk = asin(j+fi));
  M(fk = cos(j+fi));
  M(fk = tan(j+fi));
  n = startn / 10;

  T("Memory Allocation");
  M(free(malloc(16)););
  M(free(malloc(100)););
  M(free(malloc(2000)););

  T("Memory Allocation - malloc/free");

  M(free(malloc(sizeof(structc12))));
  M(free(malloc(sizeof(structc13))));
  M(free(malloc(sizeof(structc28))));
  M(free(malloc(sizeof(structc29))));
  M(free(malloc(sizeof(structc44))));
  M(free(malloc(sizeof(structc45))));
  M(free(malloc(sizeof(structc60))));
  M(free(malloc(sizeof(structc61))));

  T("Memory Allocation - new/delete");

  M(delete(new structc12));
  M(delete(new structc13));
  M(delete(new structc28));
  M(delete(new structc29));
  M(delete(new structc44));
  M(delete(new structc45));
  M(delete(new structc60));
  M(delete(new structc61));

  /* Possible additions: input, output, malloc */
  printf(" Secs: %4.2f\n",
      ((double) clock()-globalstart)
      / CLOCKS_PER_SEC);
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

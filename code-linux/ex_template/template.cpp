#include "gmock/gmock.h"

#include <iostream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

// #define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from <stdio.h> */
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

// ={=========================================================================

/*
template
*/

TEST(IPV4, 20181015)
{
  cout << "tid : " << syscall(SYS_gettid) << endl;
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

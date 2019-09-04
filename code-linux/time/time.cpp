#include "gmock/gmock.h"

#include <iostream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

// #define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from <stdio.h> */
//
// ibc.cpp: In member function ‘virtual void Syscall_GetPidThroughSyscall_Test::TestBody()’:
// libc.cpp:605:13: error: ‘SYS_getpid’ was not declared in this scope
//      syscall(SYS_getpid);
//              ^
// libc.cpp: In function ‘sanitizer_syscall::uptr sanitizer_syscall::internal_stat(const char*, void*)’:
// libc.cpp:699:23: error: ‘__NR_stat’ was not declared in this scope
//  #define SYSCALL(name) __NR_##name
//                        ^
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>

// ={=========================================================================
// time

TEST(Time, Localtime)
{
  auto now = time(0);
  cout << "the local date and time: " << ctime(&now) << endl;

  {
    // tm *localtm = localtime(&now);
    auto localtm = localtime(&now);
    cout << "The local date and time: " << asctime(localtm) << endl;
  }

  {
    // tm *gmtm = gmtime(&now);
    auto gmtm = gmtime(&now);
    if (gmtm != nullptr)
    {
      cout << "The UTC date and time: " << asctime(gmtm) << endl;
    }
  }
}


/*
 To build C and embedded 

#include <stdio.h>
#include <time.h>

{
  struct timespec tp;

  clock_gettime(CLOCK_REALTIME, &tp);

  // sec and us (micro sec)
  printf("time %.010lu.%.06lu\n", 
      (unsigned long)tp.tv_sec, (unsigned long) (tp.tv_nsec / 1000));

  struct tm* localtm = localtime(&tp.tv_sec);
  printf("The local date and time: %s\n", asctime(localtm));

  return 0;
}

*/

TEST(Time, Realtime)
{
  {
    timespec tp;

    clock_gettime(CLOCK_REALTIME, &tp);

    // sec and us (micro sec)
    printf("time %.010lu.%.06lu\n", 
        (unsigned long)tp.tv_sec, (unsigned long) (tp.tv_nsec / 1000));

    auto localtm = localtime(&tp.tv_sec);
    cout << "The local date and time: " << asctime(localtm) << endl;
  }
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

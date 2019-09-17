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
#include <sys/time.h>

#include "lpi_error.h"

#define LOG_ENABLE

#ifdef LOG_ENABLE

#define LOG_ERROR(fmt...) errMsg(fmt...)
#define LOG_EXIT_ERROR(fmt...) errExit(fmt...)

#else

#define LOG_ERROR(fmt...)
#define LOG_EXIT_ERROR(fmt...)

#endif


// ={=========================================================================
// log-printf

TEST(Log, useErrorMessage)
{
  int value{10};
  std::string message{"errMsg"};

  errMsg("this is error message from %s and value %d", 
      message.c_str(), value);
  LOG_ERROR("this is error message from %s and value %d", 
      message.c_str(), value);
}

TEST(Log, useErrorExit)
{
  errMsg("this is error message from errExit");
  LOG_EXIT_ERROR("this is error message from errExit");
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

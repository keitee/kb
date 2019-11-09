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
#include "slog.h"

#define LOG_ENABLE

#ifdef LOG_ENABLE

// #define LOG_ERROR(fmt...) errMsg(fmt...)
// #define LOG_EXIT_ERROR(fmt...) errExit(fmt...)

#define LOG_ERROR errMsg
#define LOG_EXIT_ERROR errExit

#else

#define LOG_ERROR(fmt...)
#define LOG_EXIT_ERROR(fmt...)

#endif


// ={=========================================================================
/* LPI log

[----------] 2 tests from LPILog
[ RUN      ] LPILog.useLog
ERROR [?UNKNOWN? Success], this is error message from errMsg and value 10
ERROR [?UNKNOWN? Success], this is error message from errMsg and value 10
[       OK ] LPILog.useLog (0 ms)
[ RUN      ] LPILog.useLogExit
ERROR [?UNKNOWN? Success], this is error message from errExit
[       OK ] LPILog.useLogExit (0 ms)
[----------] 2 tests from LPILog (0 ms total)

*/

TEST(LPILog, useLog)
{
  int value{10};
  std::string message{"errMsg"};

  errMsg("this is error message from %s and value %d", 
      message.c_str(), value);
  LOG_ERROR("this is error message from %s and value %d", 
      message.c_str(), value);
}

TEST(LPILog, useLogExit)
{
  errMsg("this is error message from errExit");

  // since it do exit()
  // LOG_EXIT_ERROR("this is error message from errExit");
}

/*

[----------] 2 tests from SimpleLog
[ RUN      ] SimpleLog.useLog
LOG| F:test_log.cpp C:virtual void SimpleLog_useLog_Test::TestBody() L:00073 :
this is error message from errMsg and value 10 [       OK ] SimpleLog.useLog (0
ms) [ RUN      ] SimpleLog.showCorrectLineNumbers LOG| F:test_log.cpp C:void
func1() L:00093 : this is func1() LOG| F:test_log.cpp C:void func2() L:00087 :
this is func2() LOG| F:test_log.cpp C:void func3() L:00082 : this is func3() [
OK ] SimpleLog.showCorrectLineNumbers (0 ms)

*/

TEST(SimpleLog, useLog)
{
  int value{10};
  std::string message{"errMsg"};

  LOG_MSG("this is error message from %s and value %d", 
      message.c_str(), value);

  // since it do exit()
  // LOG_ERR("this is error message from %s and value %d", 
  //     message.c_str(), value);
}

void func3()
{
  LOG_MSG("this is func3()");
}

void func2()
{
  LOG_MSG("this is func2()");
  func3();
}

void func1()
{
  LOG_MSG("this is func1()");
  func2();
}

TEST(SimpleLog, showCorrectLineNumbers)
{
  func1();
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

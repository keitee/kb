#include <fcntl.h>
#include <iostream>

#include "gmock/gmock.h"

using namespace std;
using namespace testing;

// #define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from
// <stdio.h> */
//
// ibc.cpp: In member function ‘virtual void
// Syscall_GetPidThroughSyscall_Test::TestBody()’: libc.cpp:605:13: error:
// ‘SYS_getpid’ was not declared in this scope
//      syscall(SYS_getpid);
//              ^
// libc.cpp: In function ‘sanitizer_syscall::uptr
// sanitizer_syscall::internal_stat(const char*, void*)’: libc.cpp:699:23:
// error: ‘__NR_stat’ was not declared in this scope
//  #define SYSCALL(name) __NR_##name
//                        ^
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>

#include "lpi_error.h"
#include "rlog.h"
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

/* LPI log
// ={=========================================================================

1. LPI log supports `errno`

[ RUN      ] LPILog.useLog
ERROR [ Success], this is error message from errMsg and value 10
ERROR [ Success], this is error message from errMsg and value 10
ERROR [ENOENT No such file or directory], failed to open file
[       OK ] LPILog.useLog (0 ms)


2. this shows `errno` remains

[ RUN      ] LPILog.useLogExit
ERROR [ENOENT No such file or directory], this is error message from errExit
[       OK ] LPILog.useLogExit (0 ms)


[ RUN      ] Log.lpi_usage_error
Usage: signal pid num-sigs sig-num [sig-num-2]
$

*/

TEST(Log, lpi_err_msg)
{
  {
    int value{10};
    std::string message{"errMsg"};

    errMsg("this is error message from %s and value %d",
           message.c_str(),
           value);
    LOG_ERROR("this is error message from %s and value %d",
              message.c_str(),
              value);
  }

  // shows errno use
  {
    int fd;

    fd = open("startup", O_RDONLY);
    if (fd == -1)
    {
      errMsg("failed to open file");
    }
  }
}

TEST(Log, lpi_err_exit)
{
  errMsg("this is error message from errExit");

  // since it do exit()
  // LOG_EXIT_ERROR("this is error message from errExit");
}

TEST(Log, lpi_usage_error)
{
  usageErr("%s pid num-sigs sig-num [sig-num-2]\n", "signal");
}

TEST(Log, lpi_print_message)
{
  prnMsg("%s pid num-sigs sig-num [sig-num-2]\n", "signal");
}

/*

1. simple log do not supports `errno`
2. simple log provides LOG_xxx defines

[ RUN      ] SimpleLog.useLog
LOG| F:test_log.cxx C:virtual void SimpleLog_useLog_Test::TestBody() L:00099 :
this is error message from errMsg and value 10 [       OK ] SimpleLog.useLog (0
ms)

[ RUN      ] SimpleLog.showCorrectLineNumbers
LOG| F:test_log.cxx C:void func1() L:00119 : this is func1()
LOG| F:test_log.cxx C:void func2() L:00113 : this is func2()
LOG| F:test_log.cxx C:void func3() L:00108 : this is func3()
[       OK ] SimpleLog.showCorrectLineNumbers (0 ms)

*/

TEST(Log, simple_useMessage)
{
  int value{10};
  std::string message{"errMsg"};

  LOG_MSG("this is error message from %s and value %d", message.c_str(), value);

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

TEST(Log, simple_showCorrectLineNumbers)
{
  func1();
}

// ={=========================================================================
/* rlog

  Return Value
  On success, sd_bus_new() returns 0 or a positive integer. On failure, it
  returns a negative errno-style error code.

  rc = sd_bus_attach_event(bus, eventLoop.handle(), SD_EVENT_PRIORITY_NORMAL);
  if (rc < 0)
  {
    logSysError(-rc, "failed to attach bus to event loop");
    sd_bus_unref(bus);
  }

*/

// (gdb) b
// Rlog_useCategory_Test::TestBody()

// [----------] 1 test from Rlog
// [ RUN      ] Rlog.useCategory
// 0001200307.527447 ERR: < M:test_log.cxx F:TestBody L:181 > failed to open file (2-No such file or directory)
// [       OK ] Rlog.useCategory (0 ms)

TEST(Log, rlog_useCategory)
{
  {
    int fd;

    errno = 0;

    fd = open("startup", O_RDONLY);
    if (fd == -1)
    {
      logSysError(errno, "failed to open file");
    }
  }

  // use macro
  {
    int fd;

    errno = 0;

    fd = open("startup", O_RDONLY);
    if (fd == -1)
    {
      LOG_SERR(errno, "failed to open file");
    }
  }
}

int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

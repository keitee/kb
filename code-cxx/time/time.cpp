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
#include <unistd.h>

#include <string.h>

#define LOG_ERROR(fmt) \
  std::cout << fmt << std::endl

#define LOG_EXIT_ERROR(fmt) \
  { std::cout << fmt << std::endl; \
    ASSERT_THAT(true, false); \
  }


// ={=========================================================================
// time-gettimeofday

namespace ostime
{
} // namespace

TEST(Time, Gettimeofday)
{
  struct timeval current;
  
  if (gettimeofday(&current, NULL) < 0)
    LOG_ERROR("failed on gettimeofday");
  
  std::cout << "sec: " << current.tv_sec 
    << ", micro sec: " << current.tv_usec << std::endl;
}


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
// time-timer
//
// LPI-23 Listing 23-1: Using a real-time timer
// timers/real_timer.c
//
// struct timeval {
//     time_t tv_sec;         /* Seconds since 00:00:00, 1 Jan 1970 UTC */
//     suseconds_t tv_usec;   /* Additional `microseconds` (long int) */
// };
//
// struct itimerval {
//   struct timeval it_interval; /* Interval for periodic timer */
//   struct timeval it_value;    /* Current value (time until next expiration) */
// };
//
// #include <sys/time.h>
// int setitimer(int which , const struct itimerval * new_value ,
// struct itimerval * old_value );
// Returns 0 on success, or –1 on error

namespace ostime
{
  static volatile sig_atomic_t got_alarm = 0;

  static void sigalmHandler(int sig)
  {
    got_alarm = 1;
  }

  void displayTimes(const char *msg, bool include_timer)
  {
    itimerval timer;
    static timeval start;
    timeval current;

    // number of calls to this function
    static int call_count{};

    // set start time once and that's why start is static
    if (call_count == 0)
    {
      if (gettimeofday(&start, NULL) == -1)
        LOG_EXIT_ERROR("failed on gettimeofday");
    }

    if (call_count % 20 == 0)
    {
      std::cout << "    Elapsed     Value   Interval" << std::endl;
    }

    if (gettimeofday(&current, NULL) == -1)
      LOG_EXIT_ERROR("failed on gettimeofday");

    std::cout << msg << ", " << (current.tv_sec - start.tv_sec + (current.tv_usec - start.tv_usec)/ 1000000.0);

    if (include_timer)
    {
      if (getitimer(ITIMER_REAL, &timer) == -1)
        LOG_EXIT_ERROR("failed on getitimer");

      // show itimer value in seconds
      std::cout << " " <<
      timer.it_value.tv_sec + timer.it_value.tv_usec / 1000000.0 << 
      timer.it_interval.tv_sec + timer.it_interval.tv_usec / 1000000.0;
    }

    std::cout << std::endl;

    call_count++;
  }
} // namespace

TEST(Time, SetTimer)
{
  using namespace ostime;

  struct itimerval timer{};

  // number of signals(expiration) to catch before exiting
  int max_signals{};

  // number of signals so far caught
  int sig_count{};

  struct sigaction sa;

  // if (argc > 1 && strcmp(argv[1], "--help") == 0)
  //  usageErr("%s [secs [usecs [int-secs [int-usecs]]]]\n", argv[0]);

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = sigalmHandler;
  if (sigaction(SIGALRM, &sa, NULL) == -1)
  {
    LOG_EXIT_ERROR("failed on sigaction");
  }

  // exit after 3 expirations
  max_signals = 3;

  displayTimes("START:", false);

  // set timer value from command line but here set them up
  // Initial value 1.8 seconds, interval 1 second
  // $ ./real_timer 1 800000 1 0 

  timer.it_value.tv_sec = 1;
  timer.it_value.tv_usec = 800000; // 800.000
  timer.it_interval.tv_sec = 1;
  timer.it_interval.tv_usec = 0;

  if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
  {
    LOG_EXIT_ERROR("failed on settimer");
  }

  // process time of clock_t
  clock_t prev_process_time = clock();

  sig_count = 0;

  for (;;)
  {
    // inner loop consumes at least 0.5 seconds CPU time. that is, update
    // process time at >= 0.5 interval and check if signal is raised at < 0.5
    
    // divide by CLOCKS_PER_SEC makes seconds. multifly by 10 so that can use 5
    // to mean 0.5 seconds than using 0.5 in a statement.

    while(((clock() - prev_process_time) * 10 / CLOCKS_PER_SEC) < 5)
    {
      if (got_alarm)
      {
        got_alarm = 0;
        displayTimes("ALARM:", true);

        sig_count++;

        if (sig_count >= max_signals)
        {
          LOG_ERROR("that's all folks");
          // exit(EXIT_SUCCESS);
          return;
        }
      }
    } // while

    prev_process_time = clock();
    displayTimes("MAIN:", true);

  } // for
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

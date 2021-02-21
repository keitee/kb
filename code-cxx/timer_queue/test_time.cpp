#include <cstdarg>

#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>

#include <condition_variable>
#include <mutex>
#include <thread>
#include <variant>

#include <sys/prctl.h>
#include <sys/time.h>

#include "gmock/gmock.h"

using namespace std;
using namespace testing;

namespace
{
  // from case
  // takes a printf style format string and args. returns std::string
  // string_formatter(const char *fmt, ...);

  void __attribute__((format(printf, 1, 2))) print(const char *fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);

    char *s{nullptr};

    // DESCRIPTION
    //
    // The functions asprintf() and vasprintf() are analogs of sprintf(3)
    // and vsprintf(3), except that they allocate a string large enough to
    // hold the output including the terminating null byte ('\0'), and
    // return a pointer to it via the first argument.  This pointer should
    // be passed to free(3) to release the allocated storage when it is no
    // longer needed.

    int n = vasprintf(&s, fmt, ap);
    va_end(ap);

    std::string str{};

    if (nullptr != s)
    {
      if (n > 0)
        str.assign(s, n);

      free(s);
    }

    // return str;
    std::cout << str;
  }
}

/*
// ={=========================================================================
cxx-time-timer-signal

23.1 Interval Timers

the classical UNIX APIs for setting interval timers (setitimer() and alarm()) to
notify a process when a certain amount of time has passed;

The setitimer() system call establishes an interval timer, which is a timer that
expires at a future point in time and (optionally) at regular intervals after
that.

The default disposition of all of the timer signals is to terminate the process.
Unless this is the desired result, we must to establish a handler for the signal
delivered by the timer.

LPI-23 Listing 23-1: Using a real-time timer

timers/real_timer.c

struct timeval {
    time_t tv_sec;         // Seconds since 00:00:00, 1 Jan 1970 UTC
    suseconds_t tv_usec;   // Additional `microseconds` (long int)
};

struct itimerval {
  struct timeval it_interval; // Interval for periodic timer
  struct timeval it_value;    // Current value (time until next expiration)
};

#include <sys/time.h>

int setitimer(
  int which , const struct itimerval * new_value ,
  struct itimerval * old_value );

Returns 0 on success, or –1 on error


As a timer progresses, it "counts down" from the initial value (it_value) toward
0.  When the timer reaches 0, the corresponding signal is sent to the process,
and then, if the interval (it_interval) is "nonzero", the timer value (it_value)
is reloaded, and counting down toward 0 recommences.

At any time, we can use getitimer() to retrieve the current state of the timer
in order to see how much time is left before it next expires.


SIGALRM

The kernel generates this signal upon the expiration of a real-time timer set by
a call to alarm() or setitimer(). A real-time timer is one that counts according
to wall clock time (i.e., the human notion of elapsed time). For further
details, see Section 23.1.


[ RUN      ] CxxTime.os_set_timer
    Elapsed     Value   Interval
START:    0.00
MAIN:     0.50   2.30   1.00
MAIN:     1.00   1.80   1.00
MAIN:     1.50   1.30   1.00
MAIN:     2.00   0.80   1.00
MAIN:     2.50   0.30   1.00
ALARM:    2.80   1.00   1.00  // after 2.80
MAIN:     3.00   0.80   1.00
MAIN:     3.50   0.30   1.00
ALARM:    3.80   1.00   1.00  // then +1
MAIN:     4.00   0.80   1.00
MAIN:     4.50   0.30   1.00
ALARM:    4.80   1.00   1.00  // then +1
that's all folks
[       OK ] CxxTime.os_set_timer (4800 ms)

As can see, after the first time expired, it uses interval time.

*/

namespace cxx_time
{
  static volatile sig_atomic_t got_alarm = 0;

  // on receipt of SIGALARM
  static void sigalmHandler(int sig) { got_alarm = 1; }

  // "include_timer" to show iterval timer time or not
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
        std::cout << "failed on gettimeofday\n";
    }

    if (call_count % 20 == 0)
    {
      std::cout << "    Elapsed     Value   Interval" << std::endl;
    }

    // get current
    if (gettimeofday(&current, NULL) == -1)
      std::cout << "failed on gettimeofday\n";

    // msg and elapsed
    print("%-7s %6.2f",
          msg,
          (current.tv_sec - start.tv_sec +
           (current.tv_usec - start.tv_usec) / 1000000.0));

    if (include_timer)
    {
      if (getitimer(ITIMER_REAL, &timer) == -1)
        std::cout << "failed on gettimeofday\n";

      // show itimer value in seconds
      print(" %6.2f %6.2f", timer.it_value.tv_sec + timer.it_value.tv_usec / 1000000.0,
                timer.it_interval.tv_sec + timer.it_interval.tv_usec / 1000000.0);
    }

    std::cout << std::endl;

    call_count++;
  }
} // namespace cxx_time

// ={=========================================================================
TEST(CxxTime, os_set_timer_1)
{
  using namespace cxx_time;

  struct itimerval timer{};

  // number of signals(expiration) to catch before exiting
  int max_signals{};

  // number of signals so far caught
  int sig_count{};

  struct sigaction sa;

  // install signal handler
  sigemptyset(&sa.sa_mask);
  sa.sa_flags   = 0;
  sa.sa_handler = sigalmHandler;
  if (sigaction(SIGALRM, &sa, NULL) == -1)
  {
    std::cout << "failed on sigaction\n";
  }

  // exit after 3 expirations
  max_signals = 3;

  displayTimes("START:", false);

  // set timer value from command line but here set them up
  // Initial value 1.8 seconds, interval 1 second
  //
  // if (argc > 1 && strcmp(argv[1], "--help") == 0)
  //  usageErr("%s [secs [usecs [int-secs [int-usecs]]]]\n", argv[0]);
  //
  // $ ./real_timer 1 800000 1 0
  //
  // The it_value substructure of the new_value argument specifies the delay
  // until the timer is to expire. The it_interval substructure specifies
  // whether this is to be a periodic timer. If both fields of it_interval are
  // set to 0, then the timer is expires just once, at the time given by
  // it_value. If one or both of the it_interval fields are nonzero, then, after
  // each expiration of the timer, the timer will be reset to expire again at
  // the specified interval.

  timer.it_value.tv_sec     = 2;      // 2 secs
  timer.it_value.tv_usec    = 800000; // us. 800.000 ms. 0.8 sec
  timer.it_interval.tv_sec  = 1;
  timer.it_interval.tv_usec = 0;

  if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
  {
    std::cout << "failed on settimer\n";
  }

  // process time of clock_t
  clock_t prev_process_time = clock();

  sig_count = 0;

  for (;;)
  {
    // inner loop consumes at least 0.5 seconds CPU time. that is, looping while
    // < 0.5 check if signal is raised at < 0.5 and when exit loop, update
    // process time at >= 0.5 interval

    // divide by CLOCKS_PER_SEC makes seconds. multifly by 10 so that can use 5
    // to mean 0.5 seconds than using 0.5 in a statement.

    while (((clock() - prev_process_time) * 10 / CLOCKS_PER_SEC) < 5)
    {
      if (got_alarm)
      {
        // clear "got_alarm"
        got_alarm = 0;

        displayTimes("ALARM:", true);

        sig_count++;

        if (sig_count >= max_signals)
        {
          std::cout << "that's all folks\n";
          return;
        }
      }
    } // while

    prev_process_time = clock();
    displayTimes("MAIN:", true);

  } // for

  exit(EXIT_SUCCESS);
}

/*
// ={=========================================================================
NOTE: what if not set interval time? becomes single shot timer

RUN      ] CxxTime.os_set_timer_2
START:    4.80
MAIN:     5.30   2.30   0.00
MAIN:     5.80   1.80   0.00
MAIN:     6.30   1.30   0.00
MAIN:     6.80   0.80   0.00
MAIN:     7.30   0.30   0.00
ALARM:    7.60   0.00   0.00
    Elapsed     Value   Interval
MAIN:     7.80   0.00   0.00
MAIN:     8.30   0.00   0.00
MAIN:     8.80   0.00   0.00
MAIN:     9.30   0.00   0.00
MAIN:     9.80   0.00   0.00
MAIN:    10.30   0.00   0.00
MAIN:    10.80   0.00   0.00
MAIN:    11.30   0.00   0.00
MAIN:    11.80   0.00   0.00
MAIN:    12.30   0.00   0.00
MAIN:    12.80   0.00   0.00
MAIN:    13.30   0.00   0.00
MAIN:    13.80   0.00   0.00
MAIN:    14.30   0.00   0.00
MAIN:    14.80   0.00   0.00
MAIN:    15.30   0.00   0.00
MAIN:    15.80   0.00   0.00
MAIN:    16.30   0.00   0.00
MAIN:    16.80   0.00   0.00
MAIN:    17.30   0.00   0.00
    Elapsed     Value   Interval
MAIN:    17.80   0.00   0.00
MAIN:    18.30   0.00   0.00
MAIN:    18.80   0.00   0.00
MAIN:    19.30   0.00   0.00
MAIN:    19.80   0.00   0.00
MAIN:    20.30   0.00   0.00
MAIN:    20.80   0.00   0.00
MAIN:    21.30   0.00   0.00
MAIN:    21.80   0.00   0.00
MAIN:    22.30   0.00   0.00
MAIN:    22.80   0.00   0.00
MAIN:    23.30   0.00   0.00
MAIN:    23.80   0.00   0.00
MAIN:    24.30   0.00   0.00
MAIN:    24.80   0.00   0.00
MAIN:    25.30   0.00   0.00
MAIN:    25.80   0.00   0.00
MAIN:    26.30   0.00   0.00
MAIN:    26.80   0.00   0.00
MAIN:    27.30   0.00   0.00
    Elapsed     Value   Interval
MAIN:    27.80   0.00   0.00
...

*/

TEST(CxxTime, os_set_timer_2)
{
  using namespace cxx_time;

  struct itimerval timer{};

  // number of signals(expiration) to catch before exiting
  int max_signals{};

  // number of signals so far caught
  int sig_count{};

  struct sigaction sa;

  // install signal handler
  sigemptyset(&sa.sa_mask);
  sa.sa_flags   = 0;
  sa.sa_handler = sigalmHandler;
  if (sigaction(SIGALRM, &sa, NULL) == -1)
  {
    std::cout << "failed on sigaction\n";
  }

  // exit after 3 expirations
  max_signals = 3;

  displayTimes("START:", false);

  // set timer value from command line but here set them up
  // Initial value 1.8 seconds, interval 1 second
  //
  // if (argc > 1 && strcmp(argv[1], "--help") == 0)
  //  usageErr("%s [secs [usecs [int-secs [int-usecs]]]]\n", argv[0]);
  //
  // $ ./real_timer 1 800000 1 0
  //
  // The it_value substructure of the new_value argument specifies the delay
  // until the timer is to expire. The it_interval substructure specifies
  // whether this is to be a periodic timer. If both fields of it_interval are
  // set to 0, then the timer is expires just once, at the time given by
  // it_value. If one or both of the it_interval fields are nonzero, then, after
  // each expiration of the timer, the timer will be reset to expire again at
  // the specified interval.

  timer.it_value.tv_sec     = 2;      // 2 secs
  timer.it_value.tv_usec    = 800000; // us. 800.000 ms. 0.8 sec

  // NOTE: what if not set interval time?
  //
  // timer.it_interval.tv_sec  = 1;
  // timer.it_interval.tv_usec = 0;

  if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
  {
    std::cout << "failed on settimer\n";
  }

  // process time of clock_t
  clock_t prev_process_time = clock();

  sig_count = 0;

  for (;;)
  {
    // inner loop consumes at least 0.5 seconds CPU time. that is, looping while
    // < 0.5 check if signal is raised at < 0.5 and when exit loop, update
    // process time at >= 0.5 interval

    // divide by CLOCKS_PER_SEC makes seconds. multifly by 10 so that can use 5
    // to mean 0.5 seconds than using 0.5 in a statement.

    while (((clock() - prev_process_time) * 10 / CLOCKS_PER_SEC) < 5)
    {
      if (got_alarm)
      {
        // clear "got_alarm"
        got_alarm = 0;

        displayTimes("ALARM:", true);

        sig_count++;

        if (sig_count >= max_signals)
        {
          std::cout << "that's all folks\n";
          return;
        }
      }
    } // while

    prev_process_time = clock();
    displayTimes("MAIN:", true);

  } // for

  exit(EXIT_SUCCESS);
}

/*
// ={=========================================================================
TODO: add tests for timerfd?
*/

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

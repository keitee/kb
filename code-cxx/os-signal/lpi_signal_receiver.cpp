// #include "lpi_lib.h"
#include "lpi_error.h"
#include <chrono>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string.h> // strsignal()
#include <thread>
#include <unistd.h>

/*
20.12 Signals Are Not Queued

Listing 20-7: Catching and counting signals

NOTE: the receiver do get signals as expected regardless whether runs both on
the same terminal or not.

*/

/* Print list of signals within a signal set */
void printSigSet(FILE *of, const char *prefix, const sigset_t *sigset)
{
  int sig{}, cnt{};

  for (sig = 1; sig < NSIG; ++sig)
  {
    if (sigismember(sigset, sig))
    {
      ++cnt;
      fprintf(of, "%s%d (%s)\n", prefix, sig, strsignal(sig));
    }
  }

  if (cnt == 0)
    fprintf(of, "%s<empty signal set>\n", prefix);
}

/* Print mask of blocked signals for this process */
int printSigMask(FILE *of, const char *msg)
{
  sigset_t currMask;

  if (nullptr != msg)
    fprintf(of, "%s", msg);

  if (sigprocmask(SIG_BLOCK, nullptr, &currMask) == -1)
    return -1;

  printSigSet(of, "\t\t", &currMask);

  return 0;
}

/* Print signals currently pending for this process */
int printPendingSigs(FILE *of, const char *msg)
{
  sigset_t pendingSigs;

  if (nullptr != msg)
    fprintf(of, "%s", msg);

  if (sigpending(&pendingSigs) == -1)
    return -1;

  printSigSet(of, "\t\t", &pendingSigs);

  return 0;
}

// counts deliveries of each signal
static int sigCnt[NSIG];
static volatile sig_atomic_t gotSignal = 0;

static void sigHandler(int sig)
{
  static int count{};

  errMsg("sigHandler: SIG{%d} delivered\n", sig);

  if (SIGINT == sig)
  {
    gotSignal = 1;
  }
  else
  {
    sigCnt[sig]++;
  }
}

int main(int argc, char **argv)
{
  int n, numSecs;

  // signal mask
  sigset_t pending, blocking, empty;

  pid_t pid;

  // send signals to receiver
  prnMsg("%s: PID is %ld\n", argv[0], getpid());

  // The program sets up a single handler to catch all signals w. (It isn’t
  // possible to catch SIGKILL and SIGSTOP , but we ignore the error that occurs
  // when trying to establish a handler for these signals.) For most types of
  // signals, the handler q simply counts the signal using an array. If SIGINT
  // is received, the handler sets a flag (gotSigint) that causes the program to
  // exit its main loop (the while loop described below). (We explain the use of
  // the volatile qualifier and the sig_atomic_t data type used to declare the
  // gotSigint variable in Section 21.1.3.)

  // same handler for all signals and ignore errors
  for (n = 1; n < NSIG; ++n)
    signal(n, sigHandler);

  // If a command-line argument was supplied to the program, then the program
  // blocks all signals for the number of seconds specified by that argument,
  // and then, prior to unblocking the signals, displays the set of pending
  // signals e. This allows us to send signals to the process before it
  // commences the following step.

  if (argc > 1)
  {
    std::stringstream{argv[1]} >> numSecs;

    // initialise blocking mask and set it to the current process. so blocks
    // all.
    sigfillset(&blocking);
    if (sigprocmask(SIG_SETMASK, &blocking, nullptr) == -1)
      errExit("sigprocmask");

    prnMsg("%s: sleeping for %ld seconds\n", argv[0], numSecs);
    std::this_thread::sleep_for(std::chrono::seconds(numSecs));

    // get pending mask
    if (sigpending(&pending) == -1)
      errExit("sigpending");

    prnMsg("%s: pending signals are: \n", argv[0]);
    printSigSet(stdout, "\t\t", &pending);

    // unblock all signals
    sigfillset(&empty);
    if (sigprocmask(SIG_SETMASK, &empty, nullptr) == -1)
      errExit("sigprocmask");
  }

  // loop until SIGINT is delivered
  prnMsg("%s: sleeping ends and looping on gotSignal\n", argv[0]);
  while (!gotSignal)
    continue;

  prnMsg("%s: looping ends and prints out signals received \n", argv[0]);
  // display number of signals received
  for (n = 1; n < NSIG; n++)
  {
    if (sigCnt[n] != 0)
    {
      prnMsg("%s: signal %d caught %d time%s\n",
             argv[0],
             n,
             sigCnt[n],
             (sigCnt[n] == 1) ? "" : "s");
    }
  }

  exit(EXIT_SUCCESS);
}

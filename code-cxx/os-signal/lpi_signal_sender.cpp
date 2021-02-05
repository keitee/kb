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

$ ./sig_sender PID num-sigs sig-num [sig-num-2]

The first argument is the process ID of a process to which the program should
send signals. The second argument specifies the number of signals to be sent to
the tar- get process. The third argument specifies the signal number that is to
be sent to the target process. If a signal number is supplied as the fourth
argument, then the program sends one instance of that signal after sending the
signals specified by the previous arguments.

#include <signal.h>

int kill(pid_t pid , int sig );

Returns 0 on success, or –1 on error

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

static void sigHandler(int sig)
{
  static int count{};

  errMsg("sigHandler: SIG{%d} delivered\n", sig);

  printSigMask(stdout, "blocked: ");
  printSigMask(stdout, "pending: ");

  if (SIGINT == sig)
  {
    count++;
    // errMsg("caught SIGINT and count{%d}\n", count);
    return;
  }

  errMsg("caught SIGQUIT\n");
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
  int numSigs, sig, j, sig_num;
  pid_t pid;

  if (argc < 4 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pid num-sigs sug-num [sig-num]\n", argv[0]);

  std::stringstream{argv[1]} >> pid;
  std::stringstream{argv[2]} >> numSigs;
  std::stringstream{argv[3]} >> sig;
  std::stringstream{argv[4]} >> sig_num;

  // send signals to receiver
  prnMsg("%s: sending signal %d to process %ld %d times",
         argv[0],
         sig,
         pid,
         numSigs);

  for (j = 0; j < numSigs; ++j)
    if (kill(pid, sig) == -1)
      errExit("kill 1");

  if (argc > 4)
    if (kill(pid, sig_num) == -1)
      errExit("kill 2");

  exit(EXIT_SUCCESS);
}

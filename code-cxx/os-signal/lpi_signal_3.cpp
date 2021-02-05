// #include "lpi_lib.h"
#include "lpi_error.h"
#include <chrono>
#include <iostream>
#include <signal.h>
#include <string.h> // strsignal()
#include <thread>
#include <unistd.h>

/*
20.9 Signal Sets

Listing 20-4: Functions for displaying signal sets

The sigemptyset() function initializes a signal set to contain no members. The
sigfillset() function initializes a set to contain all signals (including all
realtime signals).

After initialization, individual signals can be added to a set using sigaddset()
and removed using sigdelset().

#include <signal.h>

int sigaddset(sigset_t * set , int sig );
int sigdelset(sigset_t * set , int sig );

Both return 0 on success, or –1 on error

For both sigaddset() and sigdelset(), the sig argument is a signal number.

int sigismember(const sigset_t * set , int sig );

Returns 1 if sig is a member of set, otherwise 0

The sigismember() function returns 1 (true) if sig is a member of set, and 0
(false) otherwise.


$ man sigisemptyset

   Glibc extensions
       If the _GNU_SOURCE feature test macro is defined, then <signal.h>
       exposes three other functions for manipulating signal sets:

       int sigisemptyset(const sigset_t *set);
       int sigorset(sigset_t *dest, const sigset_t *left,
                     const sigset_t *right);
       int sigandset(sigset_t *dest, const sigset_t *left,
                     const sigset_t *right);


  sigisemptyset() returns 1 if set contains no signals, and 0 otherwise.


This function uses the NSIG constant, which is defined in <signal.h> to be one
greater than the highest signal number. We use NSIG as the upper bound in a loop
that tests all signal numbers for membership of a set.


keitee@kit-ubuntu:~/git/kb/code-cxx/os-signal/build$ ./lpi_signal_3
loops
^CERROR [ Success], sigHandler: SIG{2} delivered

blocked:                2 (Interrupt)
pending:                2 (Interrupt)
loops
^CERROR [EINTR Interrupted system call], sigHandler: SIG{2} delivered

blocked:                2 (Interrupt)
pending:                2 (Interrupt)
loops
^\ERROR [EINTR Interrupted system call], sigHandler: SIG{3} delivered

blocked:                3 (Quit)
pending:                3 (Quit)
ERROR [EINTR Interrupted system call], caught SIGQUIT

keitee@kit-ubuntu:~/git/kb/code-cxx/os-signal/build$


20.10 The Signal Mask (Blocking Signal Delivery)

For each process, the kernel maintains a signal mask—a set of signals whose
delivery to the process is currently blocked. If a signal that is blocked is
sent to a process, delivery of that signal is delayed until it is unblocked by
being removed from the process signal mask. (In Section 33.2.1, we’ll see that
the signal mask is actually a per-thread attribute, and that each thread in a
multithreaded process can indepen- dently examine and modify its signal mask
using the pthread_sigmask() function.) A signal may be added to the signal mask
in the following ways:

o The sigprocmask() system call can be used at any time to explicitly add
  signals to, and remove signals from, the signal mask.

#include <signal.h>

int sigprocmask(int how , const sigset_t * set , sigset_t * oldset );

Returns 0 on success, or –1 on error

The how argument determines the changes that sigprocmask() makes to the signal
mask:

SIG_BLOCK

The signals specified in the signal set pointed to by set are added to the
signal mask. In other words, the signal mask is set to the union of its current
value and set.

SIG_UNBLOCK

The signals in the signal set pointed to by set are removed from the signal
mask. Unblocking a signal that is not currently blocked doesn’t cause an error
to be returned.

SIG_SETMASK
The signal set pointed to by set is assigned to the signal mask. In each case,
if the oldset argument is not NULL , it points to a sigset_t buffer that is used
to return the previous signal mask.

If we want to retrieve the signal mask without changing it, then we can specify
NULL for the set argument, in which case the how argument is ignored.

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
  int j;

  if (signal(SIGINT, sigHandler) == SIG_ERR)
    errExit("signal error for SIGINT");

  if (signal(SIGQUIT, sigHandler) == SIG_ERR)
    errExit("signal error for SIGQUIT");

  // loops forever, waiting for signals
  for (;;)
  {
    std::cout << "loops " << std::endl;
    pause();
    // std::this_thread::sleep_for(std::chrono::seconds(3));
  }
}

// #include "lpi_lib.h"
#include "lpi_error.h"
#include <chrono>
#include <iostream>
#include <signal.h>
#include <string.h> // strsignal()
#include <thread>
#include <unistd.h>

/*
Listing 20-2: Establishing the same handler for two different signals

When we run the program in Listing 20-1, we see the following:
$ ./ouch
0
Type Control-C
Ouch!
1
2
Main program loops, displaying successive integers
Signal handler is executed, and returns
Control has returned to main program
Type Control-C again
Ouch!
3
Type Control-\ (the terminal quit character)
Quit (core dumped)


keitee@kit-ubuntu:~/git/kb/code-cxx/os-signal/build$ ./lpi_signal_2
loops
^CERROR [ Success], sigHandler: SIG{2} delivered

ERROR [ Success], caught SIGINT and count{1}

loops
^CERROR [EINTR Interrupted system call], sigHandler: SIG{2} delivered

ERROR [EINTR Interrupted system call], caught SIGINT and count{2}

loops
^\ERROR [EINTR Interrupted system call], sigHandler: SIG{3} delivered

ERROR [EINTR Interrupted system call], caught SIGQUIT

keitee@kit-ubuntu:~/git/kb/code-cxx/os-signal/build$


20.14 Waiting for a Signal: pause()

Calling pause() suspends execution of the process until the call is
interrupted by a signal handler (or until an unhandled signal terminates the
    process).

NAME
       pause - wait for signal

SYNOPSIS
       #include <unistd.h>

       int pause(void);

DESCRIPTION
       pause() causes the calling process (or thread) to sleep until a signal
       is delivered that either terminates the process or causes the
       invocation of a signal-catching function.


20.8 Displaying Signal Descriptions

keitee@kit-ubuntu:~/git/kb/code-cxx/os-signal/build$ ./lpi_signal_2
loops
^CERROR [ Success], sigHandler: SIG{2} delivered

ERROR [ Success], sigHandler: strsignal{Interrupt}

from psignal: Interrupt
ERROR [ Success], caught SIGINT and count{1}

loops
^\ERROR [EINTR Interrupted system call], sigHandler: SIG{3} delivered

ERROR [EINTR Interrupted system call], sigHandler: strsignal{Quit}

from psignal: Quit
ERROR [EINTR Interrupted system call], caught SIGQUIT


*/

static void sigHandler(int sig)
{
  static int count{};

  errMsg("sigHandler: SIG{%d} delivered\n", sig);

  errMsg("sigHandler: strsignal{%s}\n", strsignal(sig));
  psignal(sig, "from psignal");

  if (SIGINT == sig)
  {
    count++;
    errMsg("caught SIGINT and count{%d}\n", count);
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

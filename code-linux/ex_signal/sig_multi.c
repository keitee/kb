#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Listing 20-2: Establishing the same handler for two different signals

static void sigHandler(int sig)
{
  static int count = 0;
  /* UNSAFE: This handler uses non-async-signal-safe functions
     (printf(), exit(); see Section 21.1.2) */
  if (sig == SIGINT) {
    count++;
    printf("Caught SIGINT (%d)\n", count);
    return; /* Resume execution at point of interruption */
  }
  /* Must be SIGQUIT - print a message and terminate the process */
  printf("Caught SIGQUIT - that's all folks!\n");
  exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  /* Establish same handler for SIGINT and SIGQUIT */
  if (signal(SIGINT, sigHandler) == SIG_ERR)
    printf("error: signal");
  if (signal(SIGQUIT, sigHandler) == SIG_ERR)
    printf("error: signal");

  for (;;) /* Loop forever, waiting for signals */
    pause(); /* Block until a signal is caught */
}

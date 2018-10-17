#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Listing 20-1: Installing a handler for SIGINT

static void sigHandler(int sig)
{
  (void) sig;
  printf("Ouch!\n"); /* UNSAFE (see Section 21.1.2) */
}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  int j;
  /* Establish same handler for SIGINT and SIGQUIT */
  if (signal(SIGINT, sigHandler) == SIG_ERR)
    printf("error: signal");

  for (j = 0; ; j++) {
    printf("%d\n", j);
    sleep(3); /* Loop slowly... */
  }
}

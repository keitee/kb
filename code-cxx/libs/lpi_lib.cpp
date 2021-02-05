#include "lpi_lib.h" /* Declares function defined here */
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

/*
Listing 10-2: currTime
*/
#define BUF_SIZE 1000

/* Return a string containing the current time formatted according to
   the specification in 'format' (see strftime(3) for specifiers).
   If 'format' is NULL, we use "%c" as a specifier (which gives the'
   date and time as for ctime(3), but without the trailing newline).
   Returns NULL on error. */

char *currTime(const char *format)
{
  static char buf[BUF_SIZE]; /* Nonreentrant */
  time_t t;
  size_t s;
  struct tm *tm;

  t  = time(NULL);
  tm = localtime(&t);
  if (tm == NULL)
    return NULL;

  s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);

  return (s == 0) ? NULL : buf;
}

// lib/print_wait_status.c
/* Examine a wait() status using the W* macros */
void printWaitStatus(const char *msg, int status)
{
  if (msg != NULL)
    printf("%s", msg);

  if (WIFEXITED(status))
  {
    printf("child exited, status=%d\n", WEXITSTATUS(status));
  }
  else if (WIFSIGNALED(status))
  {
    printf("child killed by signal %d (%s)",
           WTERMSIG(status),
           strsignal(WTERMSIG(status)));
#ifdef WCOREDUMP        /* Not in SUSv3, may be absent on some systems */
        if (WCOREDUMP(status))
            printf(" (core dumped)");
#endif
        printf("\n");

    } else if (WIFSTOPPED(status)) {
        printf("child stopped by signal %d (%s)\n",
                WSTOPSIG(status), strsignal(WSTOPSIG(status)));

#ifdef WIFCONTINUED     /* SUSv3 has this, but older Linux versions and
                           some other UNIX implementations don't */
    } else if (WIFCONTINUED(status)) {
        printf("child continued\n");
#endif

    } else {            /* Should never happen */
        printf("what happened to this child? (status=%x)\n",
                (unsigned int) status);
    }
}

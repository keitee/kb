#include "lpi_error.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> // exit()
// #include <string.h>
#include <unistd.h>

/*

Listing 27-6 demonstrates the manipulation of the close-on-exec flag.
Depending on the presence of a command-line argument (any string), this
program first sets the close-on-exec flag for standard output and then execs
the ls program. Here is what we see when we run the program:

$ ./closeonexec   // Exec ls without closing standard output
-rwxr-xr-x 1 mtk users 28098 Jun 15 13:59 closeonexec

$ ./closeonexec n // Sets close-on-exec flag for standard output
ls: write error: Bad file descriptor


$ ./test_process_closeonexec
-rwxr-xr-x 1 keitee keitee 83640 Feb  1 17:13 ./test_process_closeonexec

$ ./test_process_closeonexec x
have set CLOEXEC flag
ls: write error: Bad file descriptor


Listing 27-6: Setting the close-on-exec flag for a file descriptor
–––––––––––––––––––––––––––––––––––––––––––––––––––– procexec/closeonexec.c
*/

int main(int argc, char **argv)
{
  int flags{};

  if (argc > 1)
  {
    // fetch flags
    flags = fcntl(STDOUT_FILENO, F_GETFD);
    if (flags < 0)
      errExit("fcntl - F_GETFD");

    flags |= FD_CLOEXEC;

    if (-1 == fcntl(STDOUT_FILENO, F_SETFD, flags))
      errExit("fcntl - F_SETFD");

    printf("have set CLOEXEC flag\n");
  }

  // Since it replaces the program that called it, a successful execve() never
  // returns. that is, return from the "argv[1]", test_process_envargs.

  execlp("ls", "ls", "-l", argv[0], nullptr);

  printf("if we get here, someting wrong.\n");

  exit(EXIT_FAILURE);
}

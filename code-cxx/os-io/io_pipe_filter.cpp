#include "lpi_error.h"
#include "slog.h"
#include <cerrno>
#include <cstdlib> // exit
#include <fcntl.h> // Defines O_* constants
#include <sys/file.h>
#include <sys/wait.h> // Defines mode constants
#include <unistd.h>

#define LOG_EXIT LOG_MSG

/*

LPI-44.4 Using Pipes to Connect Filters

When a pipe is created, the file descriptors used for the two ends of the pipe
are the next lowest-numbered descriptors available. Since, in normal
circumstances, descriptors 0, 1, and 2 are already in use for a process, some
higher-numbered descriptors will be allocated for the pipe. 

So how do we bring about the situation shown in Figure 44-1, where two filters
(i.e., programs that read from stdin and write to stdout) are connected using
a pipe, such that the standard output of one program is directed into the pipe
and the standard input of the other is taken from the pipe? 

And in particular, how can we do this without modifying the code of the
filters themselves?

$ ls | wc -l

process 1                                               process 2
ls                       pipe                           wc
(stdout, fd 1)    ->     unidirectional byte stream  -> (stdin, fd 0)
               `write end`                             `read end`
                of pipe                                 of pipe


*call-dup* *call-dup2*
The answer is to use the techniques described in Section 5.5 for duplicating
file descriptors. Traditionally, the following series of calls was used to
accomplish the desired result:

int pfd[2];

// Allocates (say) file descriptors 3 and 4 for pipe. 

// Other steps here, e.g., fork()

pipe(pfd); 

// Free file descriptor stdout, 1
close(STDOUT_FILENO); 

// Duplication uses lowest free file descriptor, i.e., fd 1
dup(pfd[1]); 

OR,

Note that these steps depend on the assumption that file descriptors 0, 1, and
2 for a process are already open. (The shell normally ensures this for each
    program it executes.) 

If file descriptor 0 was already closed prior to the above steps, then we
would erroneously bind the process’s standard input to the write end of the
pipe.  

To avoid this possibility, we can replace the calls to close() and dup() with
the *call-dup2* which allows us to `explicitly specify the descriptor` to be
bound to the pipe end:

// Close descriptor 1, and reopen bound to write end of pipe
dup2(pfd[1], STDOUT_FILENO);

After duplicating pfd[1], we now have two file descriptors referring to the
write end of the pipe: descriptor 1 and pfd[1]. Since unused pipe file
descriptors should be closed, after the dup2() call, we close the superfluous
descriptor: 

close(pfd[1]);

The end result of the above steps is that the process’s standard output is
bound to the write end of the pipe. A corresponding set of calls can be used
to bind a process’s standard input to the read end of the pipe.


Listing 44-4: Using a pipe to connect ls and wc

The program in Listing 44-4 uses the techniques described in this section to
bring about the setup shown in Figure 44-1. After building a pipe, this program
creates two child processes. The first child binds its standard output to the
write end of the pipe and then execs ls. The second child binds its standard
input to the read end of the pipe and then execs wc.

Verify the results using shell commands

ls | wc -l
16

./io_pipe_filter
16

./io_pipe_filter
parent: close read pipe
parent: close write pipe
child 2: stdout is duplicated with write pipe
child 2: runs wc
23
parent: children are joined and ends parent
$

*/

int main(int argc, char *argv[])
{
  // pipe fds
  int pfd[2];

  // create pipe
  if (pipe(pfd) == -1)
    errExit("pipe");

  switch (fork())
  {
    case -1:
      errExit("fork");

    case 0:
      // First child: exec 'ls' to write to pipe
      // Read end is unused so close it
      if (close(pfd[0]) == -1)
        errExit("close 1");

      // Duplicate stdout on write end of pipe; close duplicated descriptor,
      // which is write pipe.
      //
      // o Since it's duplciated, stdout refers to the same open file
      // description of write pipe so writing on stdout goes to write pipe
      // even after closing the write pipe fd.
      //
      // o no need to save the fd from dup2() call since stdout is implicit.
      //
      // Defensive check

      if (pfd[1] != STDOUT_FILENO)
      {
        if (dup2(pfd[1], STDOUT_FILENO) == -1)
          errExit("dup2 1");

        if (close(pfd[1]) == -1)
          errExit("close 2");

        printf("child 1: stdout is duplicated with write pipe\n");
      }

      printf("child 1: runs ls\n");

      // writes to pipe
      execlp("ls", "ls", (char *)NULL);

      printf("child 1: no return from execlp\n");

      errExit("execlp ls");

    // Parent falls through to create "next child"
    default:
      break;
  }

  // "next child"
  switch (fork())
  {
    case -1:
      errExit("fork");
    case 0:
      // Second child: exec 'wc' to read from pipe
      // Write end is unused
      if (close(pfd[1]) == -1)
        errExit("close 3");

      // Duplicate stdin on read end of pipe; close duplicated descriptor
      // Defensive check
      if (pfd[0] != STDIN_FILENO)
      {
        if (dup2(pfd[0], STDIN_FILENO) == -1)
          errExit("dup2 2");

        if (close(pfd[0]) == -1)
          errExit("close 4");

        printf("child 2: stdout is duplicated with write pipe\n");
      }

      printf("child 2: runs wc\n");

      // Reads from pipe
      execlp("wc", "wc", "-l", (char *)NULL);

      printf("child 2: no return from execlp\n");

      errExit("execlp wc");

    // Parent falls through
    default:
      break;
  }

  // Parent closes unused file descriptors for pipe, and waits for children
  if (close(pfd[0]) == -1)
    errExit("close 5");

  printf("parent: close read pipe\n");

  if (close(pfd[1]) == -1)
    errExit("close 6");

  printf("parent: close write pipe\n");

  if (wait(NULL) == -1)
    errExit("wait 1");

  if (wait(NULL) == -1)
    errExit("wait 2");

  printf("parent: children are joined and ends parent\n");

  exit(EXIT_SUCCESS);
}

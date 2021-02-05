#include "lpi_lib.h"
#include "slog.h"
#include <cerrno>
#include <cstdlib> // exit
#include <fcntl.h> // Defines O_* constants
#include <sys/file.h>
#include <sys/wait.h> // Defines mode constants
#include <unistd.h>

#define LOG_EXIT LOG_MSG

/*
Listing 44-2: Using a pipe to communicate between a parent and child process

The program in Listing 44-2 demonstrates the use of a pipe for communication
between parent and child processes. This example demonstrates the byte-stream
nature of pipes referred to earlier—the parent writes its data in a single
operation, while the child reads data from the pipe in small blocks.

The main program calls pipe() to create a pipe, and then calls fork() to create
a child. 

After the fork(), the parent process closes its file descriptor for the read end
of the pipe, and writes the string given as the program’s command-line argument
to the write end of the pipe. 

The parent then closes the read end of the pipe, and calls wait() to wait for
the child to terminate. After closing its file descriptor for the write end of
the pipe, the child process enters a loop where it reads blocks of data (of up
to BUF_SIZE bytes) from the pipe and writes them to standard output. 

When the child encounters end-of-file on the pipe, it exits the loop, writes a
trailing newline character, closes its descriptor for the read end of the pipe,
and terminates.

Here’s an example of what we might see when running the program in Listing 44-2:

$ ./simple_pipe 'It was a bright cold day in April, '\
'and the clocks were striking thirteen.'
It was a bright cold day in April, and the clocks were striking thirteen.


./io_simple_pipe 'It was a bright cold day in April, and the clocks were striking thirteen.'
parent (PID 14469) started 05:18:10
parent (PID 14469) ready to go 05:18:10
parent (PID 14469) waits 05:18:10
It was a bchild (PID 14470): reading 10 at 05:18:12
right coldchild (PID 14470): reading 10 at 05:18:14
 day in Apchild (PID 14470): reading 10 at 05:18:16
ril, and tchild (PID 14470): reading 10 at 05:18:18
he clocks child (PID 14470): reading 10 at 05:18:20
were strikchild (PID 14470): reading 10 at 05:18:22
ing thirtechild (PID 14470): reading 10 at 05:18:24
en.child (PID 14470): reading 3 at 05:18:26
LOG| F:io_simple_pipe.cpp C:int main(int, char**) L:00111 : read 0 and break

child (PID 14470): exits at 05:18:26
$

*/

int main(int argc, char **argv)
{
  int pfd[2];
  char buf[10];
  ssize_t numRead;

  if (argc < 2 || strcmp(argv[1], "--help") == 0)
  {
    printf("%s string\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // make stdout *unbuffered*, since we terminate child with _exit(). ???
  setbuf(stdout, NULL);

  printf("parent (PID %ld) started %s\n", (long)getpid(), currTime("%T"));

  // #include <unistd.h>
  // int pipe(int filedes[2]);        Returns 0 on success, or -1 on error
  //
  // As with any file descriptor, we can use the read() and write() system calls
  // to perform I/O on the pipe.
  //
  // one for the read end of the pipe (filedes[0]) and one for the write end
  // (filedes[1]).

  if (pipe(pfd) == -1)
    LOG_ERR("parent pipe failed");

  switch (fork())
  {
    case -1:
      LOG_ERR("fork failed");
      break;

      // child reads from pipe
    case 0:
    {
      // write end is unused so close it
      if (close(pfd[1]) == -1)
        LOG_ERR("close(write) failed");

      for (;;)
      {
        // read data from pipe, echo on stdout
        numRead = read(pfd[0], buf, 10);

        if (numRead == -1)
          LOG_ERR("read failed");

        // EOF
        if (numRead == 0)
        {
          LOG_MSG("read 0 and break");
          break;
        }

        // write to stdout
        if (write(STDOUT_FILENO, buf, numRead) != numRead)
          LOG_ERR("child - partial/failed write");

        // child dose some work, and let parent know it's done
        sleep(2);

        printf("child (PID %ld): reading %ld at %s\n",
               (long)getpid(),
               numRead,
               currTime("%T"));
      } // for end

      // read end
      write(STDOUT_FILENO, "\n", 1);

      // close it
      if (close(pfd[0]) == -1)
        LOG_ERR("close(read) failed");

      // child now carries on to do other things

      printf("child (PID %ld): exits at %s\n", (long)getpid(), currTime("%T"));

      _exit(EXIT_SUCCESS);
    }
    break;

      // parent write to pipe in a single operation.
    default:
    {
      printf("parent (PID %ld) ready to go %s\n",
             (long)getpid(),
             currTime("%T"));

      // read end is unused so close it
      if (close(pfd[0]) == -1)
        LOG_ERR("parent close(read) failed");

      if (write(pfd[1], argv[1], strlen(argv[1])) != strlen(argv[1]))
        LOG_ERR("parent - partial/failed write");

      // write end is unused
      if (close(pfd[1]) == -1)
        LOG_ERR("parent close(write) failed");

      printf("parent (PID %ld) waits %s\n", (long)getpid(), currTime("%T"));

      wait(NULL);

      exit(EXIT_SUCCESS);
    }
    break;
  } // switch ends
}

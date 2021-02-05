#include "lpi_error.h"
// #include "slog.h"
#include <cerrno>
#include <cstdlib> // exit
#include <fcntl.h> // Defines O_* constants
// #include <sys/file.h>
// #include <sys/wait.h> // Defines mode constants
// #include <unistd.h>
#include <cstring>
#include <sstream>

/*
63.2.1 The select() System Call

The select() system call blocks until one or more of a set of file descriptors
becomes ready.

#include <sys/select.h>
#include <sys/time.h> // For portability

int select(
  int nfds , fd_set * readfds , fd_set * writefds , fd_set * exceptfds,
  struct timeval * timeout );

Returns number of ready file descriptors, 0 on timeout, or –1 on error

The nfds, readfds, writefds, and exceptfds arguments specify the file
descriptors that select() is to monitor. 

The nfds argument must be set one greater than the highest file descriptor
number included in any of the three file descriptor sets. This argument allows
select() to be more efficient, since the kernel then knows not to check whether
file descriptor numbers higher than this value are part of each file descriptor
set.


o readfds is the set of file descriptors to be tested to see if input is
  possible

o writefds is the set of file descriptors to be tested to see if output is
  possible

o exceptfds is the set of file descriptors to be tested to see if an exceptional
  condition has occurred.

Typically, the fd_set data type is implemented as a bit mask. However, we don’t
need to know the details, since all manipulation of file descriptor sets is done
via four macros: FD_ZERO(), FD_SET(), FD_CLR(), and FD_ISSET().

#include <sys/select.h>

void FD_ZERO(fd_set * fdset );
void FD_SET(int fd , fd_set * fdset );
void FD_CLR(int fd , fd_set * fdset );
int FD_ISSET(int fd , fd_set * fdset );

Returns true (1) if fd is in fdset, or false (0) otherwise

These macros operate as follows:

o FD_ZERO() initializes the set pointed to by fdset to be empty.
o FD_SET() adds the file descriptor fd to the set pointed to by fdset.
o FD_CLR() removes the file descriptor fd from the set pointed to by fdset.
o FD_ISSET() returns true if the file descriptor fd is a member of the set
  pointed to by fdset.

A file descriptor set has a maximum size, defined by the constant FD_SETSIZE .
On Linux, this constant has the value 1024.

the fd_set structures pointed to by these arguments must be initialized (using
FD_ZERO() and FD_SET() ) to contain the set of file descriptors of interest. The
select() call modifies each of these structures so that, on return, they contain
the set of file descriptors that are ready.


The timeout argument can be used to set an upper limit on the time for which
select() will block.

The timeout argument controls the blocking behavior of select(). It can be
specified either as NULL , in which case select() blocks indefinitely, or as a
pointer to a timeval structure:


*os-time-timeval*
struct timeval {
    time_t tv_sec;         // Seconds since 00:00:00, 1 Jan 1970 UTC
    suseconds_t tv_usec;   // Additional `microseconds` (long int)
};

If both fields of timeout are 0, then select() doesn’t block; it simply polls
the specified file descriptors to see which ones are ready and returns
immediately. Otherwise, timeout specifies an upper limit on the time for which
select() is to wait.


On Linux, if select() returns because one or more file descriptors became ready,
and if timeout was non-NULL , then select() updates the structure to which
timeout points to indicate how much time remained until the call would have
timed out. However, this behavior is implementation-specific.


Return value from select()

As its function result, select() returns one of the following:

o A return value of –1 indicates that an error occurred. 
  Possible errors include EBADF and EINTR. EBADF indicates that one of the file
  descriptors in readfds, writefds, or exceptfds is invalid (e.g., not currently
      open). 
  EINTR, indicates that the call was interrupted by a signal handler. (As noted
      in Section 21.5, select() is never automatically restarted if interrupted
      by a signal handler.)

o A return value of 0 means that the call "timed out" before any file descriptor
  became ready. In this case, each of the returned file descriptor sets will be
  empty.

o A positive return value indicates that one or more file descriptors is ready.
  The return value is "the number of ready descriptors." In this case, each of
  the returned file descriptor sets must be examined (using FD_ISSET()) in
  order to find out which I/O events occurred. If the same file descriptor is
  specified in more than one of readfds, writefds, and exceptfds, it is counted
  multiple times if it is ready for more than one event. In other words,
  select() returns the total number of file descriptors marked as ready in all
  three returned sets.


Example program

The program in Listing 63-1 demonstrates the use of select(). Using command-line
arguments, we can specify the timeout and the file descriptors that we wish to
monitor. 

The first command-line argument specifies the timeout for select(), in seconds.
If a hyphen (-) is specified here, then select() is called with a timeout of
NULL , meaning block indefinitely. Each of the remaining command-line arguments
specifies the number of a file descriptor to be monitored, followed by letters
indicating the operations for which the descriptor is to be checked. The letters
we can specify here are r (ready for read) and w (ready for write).

Listing 63-1: Using select() to monitor multiple file descriptors

In the following shell session log, we demonstrate the use of the program in
Listing 63-1. In the first example, we make a request to monitor file
descriptor 0 for input with a 10-second timeout:

$ ./t_select 10 0r
Press Enter, so that a line of input is available on file descriptor 0
ready = 1
0: r
timeout after select(): 8.003
$       //  Next shell prompt is displayed


keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$ ./io_select 10 0r
FD_SETSIZE is {1024}
ok, call select
back from call select
ready = 0
0:
timeout after select(): 0.000
keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$

keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$ ./io_select 10 0r
FD_SETSIZE is {1024}
ok, call select

back from call select
ready = 1
0: r
timeout after select(): 5.935
keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$

*/

static void usageError(const char *progName)
{
  fprintf(stderr, "Usage: %s {timeout|-} fd-num[rw]...\n", progName);
  fprintf(stderr, "    - means infinite timeout; \n");
  fprintf(stderr, "    r = monitor for read\n");
  fprintf(stderr, "    w = monitor for write\n\n");
  fprintf(stderr, "    e.g.: %s - 0rw 1w\n", progName);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
  struct timeval timeout;
  struct timeval *pto;
  int timeout_value{};
  int fd{};

  if (argc < 2 || strcmp(argv[1], "--help") == 0)
  {
    usageError(argv[0]);
  }

  // timeout for select() is specified in argv[1]
  if (strcmp(argv[1], "-") == 0)
  {
    pto = nullptr;
  }
  else
  {
    pto = &timeout;

    std::stringstream{argv[1]} >> timeout_value;

    timeout.tv_sec  = timeout_value;
    timeout.tv_usec = 0;
  }

  // process remaning args to build file descriptor sets
  int nfds{};
  fd_set read_fds, write_fds;
  char buf[10];

  // clear fds
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);

  for (int j = 2; j < argc; ++j)
  {
    // read "timeout" and "read/write"
    auto num_read = sscanf(argv[2], "%d%2[rw]", &fd, buf);
    if (num_read != 2)
      usageError(argv[0]);

    // A file descriptor set has a maximum size, defined by the constant
    // FD_SETSIZE . On Linux, this constant has the value 1024.

    prnMsg("FD_SETSIZE is {%d}\n", FD_SETSIZE);

    // error message and exit
    if (fd >= FD_SETSIZE)
      cmdLineErr("file descriptor exceeds limit (%d)\n", FD_SETSIZE);

    // max fd is +1
    if (fd >= nfds)
      nfds = fd + 1;

    // add it to read set
    if (nullptr != strchr(buf, 'r'))
      FD_SET(fd, &read_fds);

    // add it to write set
    if (nullptr != strchr(buf, 'w'))
      FD_SET(fd, &write_fds);
  }

  // ok, have built all of the args. call select()

  prnMsg("ok, call select\n");
  auto ready = select(nfds, &read_fds, &write_fds, nullptr, pto);
  if (-1 == ready)
    errExit("select");

  prnMsg("back from call select\n");

  // display result of select
  prnMsg("ready = %d\n", ready);
  for (fd = 0; fd < nfds; ++fd)
  {
    prnMsg("%d: %s%s\n",
           fd,
           FD_ISSET(fd, &read_fds) ? "r" : "",
           FD_ISSET(fd, &write_fds) ? "w" : "");
  }

  // to print times remain.
  if (nullptr != pto)
  {
    prnMsg("timeout after select(): %ld.%03ld\n",
           (long)timeout.tv_sec,
           (long)timeout.tv_usec / 1000);
  }

  exit(EXIT_SUCCESS);
}

#include "lpi_error.h"
#include <cerrno>
#include <cstdlib> // exit
#include <cstring>
#include <fcntl.h> // Defines O_* constants
#include <sstream>
#include <sys/epoll.h>
#include <unistd.h> // pipe()

/*
Example program

Listing 63-5 demonstrates the use of the epoll API. As command-line arguments,
this program expects the pathnames of one or more terminals or FIFOs. The
program performs the following steps:

Open each of the files named on the command line for input and add the resulting
file descriptor to the interest list of the epoll instance, specifying the set
of events to be monitored as EPOLLIN .

Execute a loop that calls epoll_wait() to monitor the interest list of the epoll
instance and handles the returned events from each call. Note the following
points about this loop:

o After the epoll_wait() call, the program checks for an EINTR return, which may
  occur if the program was stopped by a signal in the middle of the epoll_wait()
  call and then resumed by SIGCONT . (Refer to Section 21.5.) If this occurs,
  the program restarts the epoll_wait() call.

o It the epoll_wait() call was successful, the program uses a further loop to
  check each of the ready items in evlist. For each item in evlist, the program
  checks the events field for the presence of not just EPOLLIN, but also
  EPOLLHUP and EPOLLERR. These latter events can occur if the other end of a
  FIFO was closed or a terminal hangup occurred. If EPOLLIN was returned, then
  the program reads some input from the corresponding file descriptor and
  displays it on standard output. Otherwise, if either EPOLLHUP or EPOLLERR
  occurred, the program closes the corresponding file descriptor and decrements
  the counter of open files (numOpenFds).

o The loop terminates when all open file descriptors have been closed (i.e.,
  when numOpenFds equals 0).


The following shell session logs demonstrate the use of the program in Listing
63-5.

Terminal window 1
$ mkfifo p q
$ ./epoll_input p q

(here epoll_input block on open(). since: Each open of a FIFO for reading by
this program will complete only after another process has opened the FIFO for
writing as described in Section 44.7. "cat > p" unblock it.)

                                          Terminal window 2
                                          $ cat > p

Opened "p" on fd 4
                                          Type Control-Z to suspend cat
                                          [1]+ Stopped cat >p
(blocks on open)
                                          $ cat > q

Opened "q" on fd 5

About to epoll_wait()

Type Control-Z to suspend the epoll_input program
[1]+ Stopped ./epoll_input p q


Above, we suspended our monitoring program so that we can now generate input on
both FIFOs, and close the write end of one of them:

                                          qqq
                                          Type Control-D to terminate “cat > q”
                                          $ fg %1
                                          cat >p
                                          ppp

Now we resume our monitoring program by bringing it into the foreground, at
which point epoll_wait() returns two events:

$ fg
./epoll_input p q
About to epoll_wait()
Ready: 2
  fd=4; events: EPOLLIN
  read 4 bytes: ppp

  fd=5; events: EPOLLIN EPOLLHUP
  read 4 bytes: qqq

  closing fd 5

About to epoll_wait()
$

The two blank lines in the above output are the newlines that were read by the
instances of cat, written to the FIFOs, and then read and echoed by our
monitoring program.

Now we type Control-D in the second terminal window in order to terminate the
remaining instance of cat, which causes epoll_wait() to once more return, this
time with a single event:

Ready: 1
fd=4; events: EPOLLHUP
closing fd 4
All file descriptors closed; bye


the first run:

keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$ ./io_epoll_input p q
epoll created
tries to open
opened "p" on fd 4
tries to open
opened "q" on fd 5
about to epoll_wait()
^Z
[1]+  Stopped                 ./io_epoll_input p q

keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$ fg
./io_epoll_input p q
about to epoll_wait()
ready: 2
 fd = 5; events: EPOLLIN EPOLLHUP x
         read 4 bytes: qqq

 fd = 4; events: EPOLLIN xx
         read 4 bytes: ppp

about to epoll_wait()
ready: 1
 fd = 5; events: xEPOLLHUP x
         close fd 5
about to epoll_wait()
ready: 1                        // when Type Control-D to terminate “cat >p”
 fd = 4; events: xEPOLLHUP x
         close fd 4
ok, all file desc closed
keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$

the second run without stopping:

keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$ ./io_epoll_input p q
epoll created
tries to open
opened "p" on fd 4
tries to open
opened "q" on fd 5
about to epoll_wait()
ready: 1
 fd = 5; events: EPOLLIN xx
         read 4 bytes: [qqq
]
about to epoll_wait()
ready: 1
 fd = 5; events: xEPOLLHUP x
         close fd 5
about to epoll_wait()
ready: 1
 fd = 4; events: EPOLLIN xx
         read 4 bytes: [ppp
]
about to epoll_wait()
ready: 1                        // when Type Control-D to terminate “cat >p”
 fd = 4; events: xEPOLLHUP x
         close fd 4
ok, all file desc closed
keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$


Listing 63-5: Using the epoll API
––––––––––––––––––––––––––––––––––––––––––––––––––––––– altio/epoll_input.c

*/

static void usageError(const char *progName)
{
  fprintf(stderr, "Usage: %s {num-pipes [num-writes]\n", progName);
  fprintf(stderr, "    e.g.: %s 10 3\n", progName);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
  const int MAX_EVENTS{5};
  const int MAX_BUF{1000};
  char buf[MAX_BUF];

  if (argc < 2 || strcmp(argv[1], "--help") == 0)
  {
    usageError(argv[0]);
  }

  // do not use size in epoll_crate(size) so use any
  auto epfd = epoll_create(argc - 1);
  if (-1 == epfd)
    errExit("epoll_create");

  printf("epoll created\n");

  // open each file on command line, and add it to the "interest list"
  struct epoll_event ev;

  for (int i = 1; i < argc; ++i)
  {
    printf("tries to open\n");

    auto fd = open(argv[i], O_RDONLY);
    if (-1 == fd)
      errExit("open");

    printf("opened \"%s\" on fd %d\n", argv[i], fd);

    // only interested in input event:
    // POLLIN      Data other than high-priority data can be read
    ev.events  = EPOLLIN;
    ev.data.fd = fd;

    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev))
      errExit("epoll_ctl");
  }

  // counter of open files
  auto num_open_fds = argc - 1;

  struct epoll_event evlist[MAX_EVENTS];

  while (num_open_fds > 0)
  {
    printf("about to epoll_wait()\n");

    // If timeout equals -1, block until an event occurs for one of the file
    // descriptors in the interest list for epfd or until a signal is caught.

    auto ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
    if (-1 == ready)
    {
      // restart if interrupted by signal
      if (EINTR == errno)
        continue;
      else
        errExit("epoll_wait");
    }

    printf("ready: %d\n", ready);

    // deal with returned list of events
    //
    // On success, epoll_wait() returns the number of items that have been
    // placed in the array evlist, or 0 if no file descriptors were ready within
    // the interval specified by timeout. On error, epoll_wait() returns -1,
    // with errno set to indicate the error.

    for (int i = 0; i < ready; ++i)
    {
      printf(" fd = %d; events: %s%s%s\n",
             evlist[i].data.fd,
             (evlist[i].events & EPOLLIN) ? "EPOLLIN " : "x",
             (evlist[i].events & EPOLLHUP) ? "EPOLLHUP " : "x",
             (evlist[i].events & EPOLLERR) ? "EPOLLERR " : "x");

      if (evlist[i].events & EPOLLIN)
      {
        auto num_read = read(evlist[i].data.fd, buf, MAX_BUF);
        if (-1 == num_read)
          errExit("read");

        printf("\t read %d bytes: [%.*s]\n", (int)num_read, (int)num_read, buf);
      }
      else if (evlist[i].events & (EPOLLHUP | EPOLLERR))
      {
        // if EPOLLHUP and EPOLLERR were both set, then there might be more
        // than MAX_BUF bytes to read. Therefore, close the file desc only if
        // EPOLLIN was not set. we'll read further bytes after the next
        // epoll_wait.
        printf("\t close fd %d\n", evlist[i].data.fd);
        if (-1 == close(evlist[i].data.fd))
          errExit("close");

        --num_open_fds;
      }
    }
  } // while

  printf("ok, all file desc closed\n");

  exit(EXIT_SUCCESS);
}

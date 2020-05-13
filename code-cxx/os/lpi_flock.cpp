#include "lpi_lib.h"
#include "slog.h"
#include <cerrno>
#include <cstdlib> // exit
#include <fcntl.h> // Defines O_* constants
#include <string>
#include <sys/file.h>
#include <sys/stat.h> // Defines mode constants
#include <unistd.h>

#define LOG_EXIT LOG_MSG

/*
55.2 File Locking with flock()

Although fcntl() provides a superset of the functionality provided by flock(),
we nevertheless describe flock() because it is still used in some applications,
and because it differs from fcntl() in some of the semantics of inheritance and
release of locks.  

The flock() system call places a single lock on an entire file. The file to be
locked is specified via an open file descriptor passed in fd. The operation
argument specifies one of the values LOCK_SH, LOCK_EX, or LOCK_UN, which are
described in Table 55-1.

A process can place a shared or exclusive lock *regardless of the access mode*
(read, write, or read-write) of the file


example

We begin by creating a file, and then start an instance of our program that sits
in the background and holds a shared lock for 60 seconds:

$ touch tfile

keitee@kit-ubuntu:~/git/kb/code-cxx/os/build$ ./lpi-flock tfile s 120 &
[1] 26308
PID 26308: requesting LOCK_SH at 23:06:05
PID 26308: granted LOCK_SH at 23:06:05

we start another instance of the program that successfully requests a shared
lock and then releases it:

keitee@kit-ubuntu:~/git/kb/code-cxx/os/build$ ./lpi-flock tfile s 2
PID 26325: requesting LOCK_SH at 23:06:09
PID 26325: granted LOCK_SH at 23:06:09
PID 26325: releasing LOCK_SH at 23:06:11

However, when we start another instance of the program that makes a nonblocking
requests for an exclusive lock, the request immediately fails:

keitee@kit-ubuntu:~/git/kb/code-cxx/os/build$ ./lpi-flock tfile xn
PID 26365: requesting LOCK_EX at 23:06:20
LOG| F:lpi_flock.cpp C:int main(int, char**) 
  L:00058 [EAGAIN/EWOULDBLOCK Resource temporarily unavailable] PID 26365: already locked - bye!

When we start another instance of the program that makes a blocking request for
an exclusive lock, the program blocks. When the background process that was
holding a shared lock for 60 seconds releases its lock, the blocked request is
granted:

keitee@kit-ubuntu:~/git/kb/code-cxx/os/build$ ./lpi-flock tfile x
PID 26413: requesting LOCK_EX at 23:06:34
PID 26308: releasing LOCK_SH at 23:08:05
PID 26413: granted LOCK_EX at 23:08:05
PID 26413: releasing LOCK_EX at 23:08:15
[1]+  Done                    ./lpi-flock tfile s 120


55.2.2 Limitations of flock()

Placing locks with flock() suffers from a number of limitations:

o Only whole files can be locked. Such coarse locking limits the potential for
concurrency among cooperating processes. If, for example, we have multiple
processes, each of which would like to simultaneously access different parts of
the same file, then locking via flock() would needlessly prevent these processes
from operating concurrently.

o We can place only advisory locks with flock().

o Many NFS implementations don’t recognize locks granted by flock().

All of these limitations are addressed by the locking scheme implemented by
fcntl(), which we describe in the next section.

*/

int main(int argc, char **argv)
{
  int fd, lock;
  const char *lname;

  // char *strchr(const char *s, int c);
  // that is, if not found a char, argv[2][0], in "sx".

  if (argc < 3 || strcmp(argv[1], "--help") == 0 ||
      strchr("sx", argv[2][0]) == NULL)
  {
    printf("%s file lock [sleep-time]\n"
           " 'lock' is 's' (shared) or 'x' (exclusive)\n"
           " optionally followed by 'n' (nonblocking)\n"
           " 'secs' specifies time to hold lock\n",
           argv[0]);
    exit(EXIT_FAILURE);
  }

  // LOCK_SH, Place a shared lock on the file referred to by fd
  // LOCK_EX, Place an exclusive lock on the file referred to by fd

  lock = (argv[2][0] == 's') ? LOCK_SH : LOCK_EX;

  // LOCK_NB Make a nonblocking lock request
  //
  // By default, flock() blocks if another process already holds an incompatible
  // lock on a file. If we want to prevent this, we can OR (|) the value into
  // operation. In this case, if another process already holds an incompatible
  // lock on the file, flock() doesn’t block, but instead returns –1, with errno
  // set to EWOULDBLOCK.

  if (argv[2][1] == 'n')
    lock |= LOCK_NB;

  fd = open(argv[1], O_RDONLY);
  if (fd == -1)
  {
    LOG_ERR("open failed");
  }

  lname = (lock & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";

  printf("PID %ld: requesting %s at %s\n",
         (long)getpid(),
         lname,
         currTime("%T"));

  if (flock(fd, lock) == -1)
  {
    if (errno == EWOULDBLOCK)
      LOG_ERR("PID %ld: already locked - bye!", (long)getpid());
    else
      LOG_ERR("flock (PID = %ld)", (long)getpid());
  }

  printf("PID %ld: granted %s at %s\n", (long)getpid(), lname, currTime("%T"));

  // sleep((argc > 3) ? getInt(argv[3], GN_NONNEG, "sleep-time") : 10);

  sleep((argc > 3) ? std::stoi(argv[3]) : 10);

  printf("PID %ld: releasing %s at %s\n",
         (long)getpid(),
         lname,
         currTime("%T"));

  // LOCK_UN, Unlock the file referred to by fd
  if (flock(fd, LOCK_UN) == -1)
    LOG_ERR("flock failed");

  exit(EXIT_SUCCESS);
}

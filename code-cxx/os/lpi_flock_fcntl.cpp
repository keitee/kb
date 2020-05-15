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
55.3 Record Locking with fcntl()

Using fcntl() (Section 5.2), we can place a lock on any part of a file, ranging
from a single byte to the entire file. 

The general form of the fcntl() call used to create or remove a file lock is as
follows:

struct flock flockstr;

// Set fields of 'flockstr' to describe lock to be placed or removed

fcntl(fd, cmd, &flockstr); // Place lock defined by 'fl'


The flock structure

The flock structure defines the lock that we wish to acquire or remove. It is
defined as follows:

struct flock {
 short l_type;    // Lock type: F_RDLCK, F_WRLCK, F_UNLCK
 short l_whence;  // How to interpret 'l_start': SEEK_SET, SEEK_CUR, SEEK_END
 off_t l_start;   // Offset where the lock begins
 off_t l_len;     // Number of bytes to lock; 0 means "until EOF"
 pid_t l_pid;     // Process preventing our lock (F_GETLK only)
};

The l_type field indicates the type of lock we want to place. It is specified as
one of the values in Table 55-3.

Table 55-3: Lock types for fcntl() locking

Lock type   Description
F_RDLCK     Place a read lock
F_WRLCK     Place a write lock
F_UNLCK     Remove an existing lock

shared and exclusive

Semantically, read (F_RDLCK) and write (F_WRLCK) locks correspond to the shared
and exclusive locks applied by flock(), and they follow the same compatibility
rules (Table 55-2): any number of processes can hold read locks on a file
region, but only one process can hold a write lock, and that lock excludes read
and write locks by other processes. Specifying l_type as F_UNLCK is analogous to
the flock() LOCK_UN operation

In order to place a read lock on a file, the file must be open for reading.
Similarly, to place a write lock, the file must be open for writing. To place
both types of locks, we open the file read-write (O_RDWR). Attempting to place a
lock that is incompatible with the file access mode results in the error EBADF.


Specifying 0 in l_len has the special meaning “lock all bytes from the point
specified by l_start and l_whence through to the end of the file, no matter how
large the file grows.” This is convenient if we don’t know in advance how many
bytes we are going to add to a file. To lock the entire file, we can specify
l_whence as SEEK_SET and both l_start and l_len as 0.


The cmd argument

When working with file locks, three possible values may be specified for the cmd
argument of fcntl(). The first two are used for acquiring and releasing locks:

F_SETLK

Acquire (l_type is F_RDLCK or F_WRLCK) or release (l_type is F_UNLCK) a lock on
the bytes specified by flockstr. If an incompatible lock is held by another
process on any part of the region to be locked, fcntl() *fails* with the error
EAGAIN. On some UNIX implementations, fcntl() fails with the error EACCES in
this case. SUSv3 permits either possibility, and a portable application should
test for both values.

F_SETLKW

This is the same as F_SETLK, except that if another process holds an
incompatible lock on any part of the region to be locked, then the call *blocks*
until the lock can be granted. 


As with flock example, do the same

gets shared lock for 120 secs

keitee@kit-ubuntu:~/git/kb/code-cxx/os/build$ ./lpi-flock-fcntl tfile s 120 &
[1] 29158
PID 29158: requesting SH F_RDLCK at 22:49:30
cmd F_SETLKW
PID 29158: granted SH F_RDLCK at 22:49:30

gets shared lock for 2

keitee@kit-ubuntu:~/git/kb/code-cxx/os/build$ ./lpi-flock-fcntl tfile s 2
PID 29255: requesting SH F_RDLCK at 22:49:55
cmd F_SETLKW
PID 29255: granted SH F_RDLCK at 22:49:55
PID 29255: releasing SH F_RDLCK at 22:49:57

tries exclusive lock and blocked until the first process releases it

keitee@kit-ubuntu:~/git/kb/code-cxx/os/build$ ./lpi-flock-fcntl tfile x 20
PID 29464: requesting EX F_WRLCK at 22:50:47
cmd F_SETLKW
PID 29158: releasing SH F_RDLCK at 22:51:30
PID 29464: granted EX F_WRLCK at 22:51:30
PID 29464: releasing EX F_WRLCK at 22:51:50
[1]+  Done                    ./lpi-flock-fcntl tfile s 120


keitee@kit-ubuntu:~/git/kb/code-cxx/os/build$ ./lpi-flock-fcntl tfile s 120 &
[1] 29838
PID 29838: requesting SH F_RDLCK at 22:52:21
cmd F_SETLKW
PID 29838: granted SH F_RDLCK at 22:52:21

nonblock shows errors immidiately

keitee@kit-ubuntu:~/git/kb/code-cxx/os/build$ ./lpi-flock-fcntl tfile xn 20
PID 29869: requesting EX F_WRLCK at 22:52:28
cmd F_SETLK
LOG| F:lpi_flock_fcntl.cpp C:int main(int, char**) L:00167 [EAGAIN/EWOULDBLOCK Resource temporarily unavailable] fcntlflock (PID = 29869)
keitee@kit-ubuntu:~/git/kb/code-cxx/os/build$

TODO: not tried LPI example which shows record locking.

*/

int main(int argc, char **argv)
{
  int fd{};

  struct flock flock
  {};
  const char *flocktype;
  int flockcmd{};

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

  // shared or exclusive?

  flock.l_type = (argv[2][0] == 's') ? F_RDLCK : F_WRLCK;

  // shared lock. unlike flock, access mod should match to lock type.
  if (flock.l_type == F_RDLCK)
  {
    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
      LOG_ERR("open to read failed");
    }

    flocktype = "SH F_RDLCK";
  }
  // if (flock.l_type != F_RDLCK)
  else
  {
    fd = open(argv[1], O_WRONLY);
    if (fd == -1)
    {
      LOG_ERR("open to write failed");
    }

    flocktype = "EX F_WRLCK";
  }

  // blocking or nonblocking? n is nonblocking.

  flockcmd = (argv[2][1] == 'n') ? F_SETLK : F_SETLKW;

  printf("PID %ld: requesting %s at %s\n",
         (long)getpid(),
         flocktype,
         currTime("%T"));

  if (flockcmd == F_SETLK)
  {
    printf("cmd F_SETLK\n");
  }
  else
  {
    printf("cmd F_SETLKW\n");
  }

  if (fcntl(fd, flockcmd, &flock) == -1)
  {
    LOG_ERR("fcntlflock (PID = %ld)", (long)getpid());
  }

  // means it comes out of fcntl() above.
  printf("PID %ld: granted %s at %s\n",
         (long)getpid(),
         flocktype,
         currTime("%T"));

  sleep((argc > 3) ? std::stoi(argv[3]) : 10);

  printf("PID %ld: releasing %s at %s\n",
         (long)getpid(),
         flocktype,
         currTime("%T"));

  // unlock
  flock.l_type = F_UNLCK;
  if (fcntl(fd, flockcmd, &flock) == -1)
  {
    LOG_ERR("fcntl(unlock) failed");
  }

  exit(EXIT_SUCCESS);
}

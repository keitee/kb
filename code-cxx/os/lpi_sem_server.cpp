#include "slog.h"
#include <cerrno>
#include <cstdlib> // exit
#include <fcntl.h> // Defines O_* constants
#include <semaphore.h>
#include <sys/stat.h> // Defines mode constants
#include <unistd.h>

#define LOG_EXIT LOG_MSG

/*

Listing 53-1: Using sem_open() to open or create a POSIX named semaphore
––––––––––––––––––––––––––––––––––––––––––––––––––––––– psem/psem_create.c


The following shell session log demonstrates the use of this program. We first
use the umask command to deny all permissions to users in the class other. We
then exclusively create a semaphore and examine the contents of the
Linux-specific virtual directory that contains named semaphores.

$ umask 007

$ ./psem_create -cx /demo 666
 666 means read+write for all users

$ ls -l /dev/shm/sem.*
-rw-rw---- 1 mtk users 16 Jul 6 12:09 /dev/shm/sem.demo


#include <fcntl.h> // Defines O_* constants
#include <semaphore.h>
#include <sys/stat.h> // Defines mode constants

sem_t *sem_open(const char *name, int oflag, ...
// mode_t mode,
// unsigned int value
);

Returns pointer to semaphore on success, or SEM_FAILED on error


The sem_wait() function decrements (decreases by 1) the value of the semaphore
referred to by sem.

If the semaphore currently has a value greater than 0, sem_wait() returns
immediately. If the value of the semaphore is currently 0, sem_wait() blocks
until the semaphore value rises above 0; at that time, the semaphore is then
decremented and sem_wait() returns.

If a blocked sem_wait() call is interrupted by a signal handler, then it fails
with the error EINTR , regardless of whether the SA_RESTART flag was used when
establish- ing the signal handler with sigaction(). (On some other UNIX
implementations, SA_RESTART does cause sem_wait() to automatically restart.) The
program in Listing 53-3 provides a command-line interface to the

Open named semaphores are also automatically closed on process termination or if
the process performs an exec().

Closing a semaphore does not delete it. For that purpose, we need to use
sem_unlink().


*/

int main(int argc, char **argv)
{
  int flags{};
  int opt{};
  mode_t perms;
  unsigned int value{};
  sem_t *sem;

  flags |= O_CREAT;
  flags |= O_EXCL;

  perms = 0777;
  value = 0;

  // create and open `named semaphore`
  sem = sem_open("/demo", flags, perms, value);
  if (sem == SEM_FAILED)
  {
    LOG_MSG("sem_open() failed");
    exit(EXIT_FAILURE);
  }

  LOG_MSG("sem is created and wait on /demo");

  if (sem_wait(sem) == -1)
    LOG_MSG("sem_wait() failed");

  LOG_MSG("sem_wait() succeeded and server ends");

  if (sem_unlink("/demo") == -1)
    LOG_MSG("sem_unlink() failed");

  exit(EXIT_SUCCESS);
}

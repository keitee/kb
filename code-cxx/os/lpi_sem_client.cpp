#include "slog.h"
#include <cerrno>
#include <cstdlib> // exit
#include <fcntl.h> // Defines O_* constants 
#include <sys/stat.h> // Defines mode constants
#include <semaphore.h>
#include <unistd.h>

#define LOG_EXIT LOG_MSG

/*
Listing 57-2: Header file for us_xfr_sv.c and us_xfr_cl.c
––––––––––––––––––––––––––––––––––––––––––––––––––––––––– sockets/us_xfr.h
*/

#include <sys/socket.h>
#include <sys/un.h> // sockaddr_un

#define SV_SOCK_PATH "/tmp/us_xfr"

#define BUF_SIZE (100)

/*
Listing 57-3: A simple UNIX domain stream socket server
––––––––––––––––––––––––––––––––––––––––––––––––––––––sockets/us_xfr_sv.c
*/

#define BACKLOG (5)

/*

53.3.2

Posting a Semaphore

The sem_post() function increments (increases by 1) the value of the semaphore
referred to by sem.


If the value of the semaphore was 0 before the sem_post() call, and some other
process (or thread) is blocked waiting to decrement the semaphore, then that
process is awoken, and its sem_wait() call proceeds to decrement the semaphore.
If multiple processes (or threads) are blocked in sem_wait(), then, if the
processes are being scheduled under the default round-robin time-sharing policy,
it is indeterminate which one will be awoken and allowed to decrement the
semaphore.

*/

int main(int argc, char **argv)
{
  int flags{};
  int opt{};
  mode_t perms;
  unsigned int value{};
  sem_t *sem;

  perms = 666;
  value = 0;

  sem = sem_open("/demo", 0);
  if (sem == SEM_FAILED)
  {
    LOG_MSG("sem_open() failed");
    exit(EXIT_FAILURE);
  }

  if (sem_post(sem) == -1)
    LOG_MSG("sem_post() failed");

  LOG_MSG("sem_post() succeed");

  exit(EXIT_SUCCESS);
}

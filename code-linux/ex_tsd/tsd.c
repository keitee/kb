//
// LPI-31.4
// Listing 31-3: A thread-safe implementation of strerror() using thread-specific data
// –––––––––––––––––––––––––––––––––––––––––––––––––––– threads/strerror_tsd.c

// kyoupark@kit-debian64:~/git/kb/code-linux/ex_tsd$ ./tsd_test_out
// own_strerror(22)
// pid:11269, tid:11269, key: 0
// own_strerror: key: 0, buff:0x1f24010
// Main thread  (pid:11269, tid:11269) has called strerror()
// Other thread (pid:11269, tid:11270) about to call strerror()
// own_strerror(1)
// own_strerror: key: 0, buff:0x7f0ec00008c0
// Other thread: str (0x7f0ec00008c0) = Operation not permitted
// Main thread:  str (0x1f24010) = Invalid argument


#define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from <stdio.h> */
#include <stdio.h>
#include <string.h> /* Get declaration of strerror() */
#include <pthread.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerrorKey;


/* Maximum length of string in per-thread buffer returned by strerror() */
#define MAX_ERROR_LEN 256 

/* Free thread-specific data buffer */
static void destructor(void *buf)
{
  free(buf);
}


/* One-time key creation function */
static void createKey(void)
{
  int s;
  /* Allocate a unique thread-specific data key and save the address
     of the destructor for thread-specific data buffers */
  s = pthread_key_create(&strerrorKey, destructor);
  if (s != 0)
    printf("error: pthread_key_create");
  else
    printf("pid:%ld, tid:%ld, key: %ld\n", getpid(), syscall(SYS_gettid), strerrorKey);
}


char *strerror(int err)
{
  int s;
  char *buf;

  printf("own_strerror(%ld)\n", err);

  /* Make first caller allocate key for thread-specific data */
  s = pthread_once(&once, createKey);
  if (s != 0)
    printf("error: pthread_once");

  buf = pthread_getspecific(strerrorKey);
  if (buf == NULL) { /* If first call from this thread, allocate
                        buffer for thread, and save its location */

    buf = malloc(MAX_ERROR_LEN);
    if (buf == NULL)
      printf("error: malloc");

    printf("own_strerror: key: %ld, buff:%p\n", strerrorKey, buf);

    s = pthread_setspecific(strerrorKey, buf);
    if (s != 0)
      printf("error: pthread_setspecific");
  }

  if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
    snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
  } else {
    strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
    buf[MAX_ERROR_LEN - 1] = '\0'; /* Ensure null termination */
  }

  return buf;
}

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2018.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 31-2 */

/* strerror_test.c

   A program to test whether the implementation of strerror() thread-safe.
*/

#include <stdio.h>
#include <string.h>                 /* Get declaration of strerror() */
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
// #include "tlpi_hdr.h"

#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

/*
// ret = posix_spawn(&pid,args[0], NULL, NULL, args, environ);

PROLOG
       This  manual page is part of the POSIX Programmer's Manual.  The Linux implementation of this interface may differ (consult the corresponding Linux manual page for details of Linux behavior), or
       the interface may not be implemented on Linux.

NAME
       posix_spawn, posix_spawnp - spawn a process (ADVANCED REALTIME)

SYNOPSIS
       #include <spawn.h>

       int posix_spawn(pid_t *restrict pid, const char *restrict path,
              const posix_spawn_file_actions_t *file_actions,
              const posix_spawnattr_t *restrict attrp,
              char *const argv[restrict], char *const envp[restrict]);
       int posix_spawnp(pid_t *restrict pid, const char *restrict file,
              const posix_spawn_file_actions_t *file_actions,
              const posix_spawnattr_t *restrict attrp,
              char *const argv[restrict], char * const envp[restrict]);

static void *
threadFunc(void *arg)
{
  (void) arg;

  char *str;

  printf("Other thread (pid:%ld, tid:%ld) about to call strerror()\n"
      , getpid(), syscall(SYS_gettid));
  str = strerror(EPERM);
  printf("Other thread: str (%p) = %s\n", str, str);

  return NULL;
}

int
main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

    pthread_t t;
    int s;
    char *str;

    str = strerror(EINVAL);
    printf("Main thread  (pid:%ld, tid:%ld) has called strerror()\n"
        , getpid(), syscall(SYS_gettid));

    s = pthread_create(&t, NULL, threadFunc, NULL);
    if (s != 0)
        printf("error: pthread_create");

    s = pthread_join(t, NULL);
    if (s != 0)
        printf("error: pthread_join");

    /* If strerror() is not thread-safe, then the output of this printf() be
       the same as that produced by the analogous printf() in threadFunc() */

    printf("Main thread:  str (%p) = %s\n", str, str);

    exit(EXIT_SUCCESS);
}

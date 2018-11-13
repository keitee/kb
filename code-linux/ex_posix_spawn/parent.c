/* 
   A program to test posix_spawn.
*/

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>       // for exit
#include <spawn.h>        // must be before wait.h
#include <sys/wait.h>

/*
// ret = posix_spawn(&pid,args[0], NULL, NULL, args, environ);

PROLOG
       This  manual page is part of the POSIX Programmer's Manual.  The Linux
       implementation of this interface may differ (consult the corresponding
       Linux manual page for details of Linux behavior), or the interface may
       not be implemented on Linux.

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

DESCRIPTION
       The posix_spawn() and posix_spawnp() functions shall create a new process
       (child process) from the specified process image. The new process image
       shall be constructed from a  regular  executable file called the new
       process image file.

       When a C program is executed as the result of this call, it shall be
       entered as a C-language function call as follows:

              int main(int argc, char *argv[]);

       where argc is the argument count and argv is an array of character
       pointers to the arguments themselves. In addition, the following
       variable:

              extern char **environ;

       shall be initialized as a pointer to an array of character pointers to
       the environment strings.


RETURN VALUE
       Upon successful completion, posix_spawn() and posix_spawnp() shall return
       the process ID of the child process to the parent process, in the
       variable pointed to by a non-NULL  pid  argument,  and shall  return zero
       as the function return value. Otherwise, no child process shall be
       created, the value stored into the variable pointed to by a non-NULL pid
       is unspecified, and an error number shall be returned as the function
       return value to indicate the error. If the pid argument is a null
       pointer, the process ID of the child is not returned to the caller.

*/

extern char **environ;

static void *threadFunc(void *arg)
{
  (void) arg;
  int ret = 0;
  char *args[4];
  pid_t pid = -1;

  // https://sourceware.org/bugzilla/show_bug.cgi?id=18433
  // Bug 18433 - posix_spawn does not return correctly upon failure to execute
  // Version:	2.19
  //
  // kyoupark@kit-debian64:~/git/kb/code-linux/ex_posix_spawn$ /lib/x86_64-linux-gnu/libc.so.6
  // GNU C Library (Debian GLIBC 2.19-18+deb8u10) stable release version 2.19, by Roland McGrath et al.
  // Copyright (C) 2014 Free Software Foundation, Inc.
  // This is free software; see the source for copying conditions.
  // There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A
  // PARTICULAR PURPOSE.
  // Compiled by GNU CC version 4.8.4.
  // Compiled on a Linux 3.16.43 system on 2017-06-18.
  // Available extensions:
  //         crypt add-on version 2.1 by Michael Glad and others
  //         GNU Libidn by Simon Josefsson
  //         Native POSIX Threads Library by Ulrich Drepper et al
  //         BIND-8.2.3-T5B
  // libc ABIs: UNIQUE IFUNC
  // For bug reporting instructions, please see:
  // <http://www.debian.org/Bugs/>.

  args[0] = "/home/kyoupark/git/kb/code-linux/ex_posix_spawn/child_out";
  // args[0] = "/home/kyoupark/git/kb/code-linux/xxxxxxx";
  args[1] = "use posix_spawn from parent";
  args[2] = NULL;

  ret = posix_spawn(&pid, args[0], NULL, NULL, args, environ);
  if (ret != 0)
  {
    printf("posix_spawn is failed, errno = %d\n", errno);
  }
  else
  {
    // posix_spawn() return value is not reliable since the bug as above.
    printf("posix_spawn returns okay, pid = %d, errno = %d, %s\n", 
        pid, errno, strerror(errno));
  }

  // if(-1 != waitpid(pid, &status, 0))
  // {
  //   printf("%s child exited with status %d\n", args[0], status);
  // }

  return NULL;
}


int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  pthread_t t;
  int s;

  s = pthread_create(&t, NULL, threadFunc, NULL);
  if (s != 0)
    printf("error: pthread_create\n");
  
  printf("parent: pthread created\n");

  s = pthread_join(t, NULL);
  if (s != 0)
    printf("error: pthread_join\n");

  printf("parent: ends\n");

  exit(EXIT_SUCCESS);
}

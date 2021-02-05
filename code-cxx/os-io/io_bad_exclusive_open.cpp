#include "lpi_error.h"
#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <thread>
#include <unistd.h>

/*
Listing 5-1: Incorrect code to exclusively open a file

O_EXCL

This flag is used in conjunction with O_CREAT to indicate that if the file
already exists, it should not be opened; instead, open() should fail, with errno
set to *or-errno-eexist* EEXIST. In other words, this flag allows the caller to
ensure that it is the process creating the file. 

ENOENT *os-errno-enoent*

The specified file doesn’t exist, and O_CREAT was not specified, or O_CREAT
was specified, and one of the directories in pathname doesn’t exist or is a
symbolic link pointing to a nonexistent pathname (a dangling link).


The check on the prior existence of the file and the creation of the file are
performed atomically. "To see why this is important", consider the code shown in
Listing 5-1,


./io_bad_exclusive_open x
[PID 11843] Created file "x" exclusively

./io_bad_exclusive_open make
[PID 11929] File "make" already exists

Figure 5-1: Failing to exclusively create a file

Suppose that when our process first called open(), the file did not exist, but
by the time of the second open(), some other process had created the file. This
could happen if the kernel scheduler decided that the process’s time slice had
expired and gave control to another process, as shown in Figure 5-1, or if the
two processes were running at the same time on a multiprocessor system. 

Figure 5-1 portrays the case where two processes are both executing the code
shown in Listing 5-1. In this scenario, process A would "wrongly conclude" that
it had created the file, since the second open() succeeds whether or not the
file exists.


./io_bad_exclusive_open tfile sleep &
[1] 15415
[PID 15415] File "tfile" doesn't exist yet

./io_bad_exclusive_open tfile
[PID 15428] File "tfile" doesn't exist yet
[PID 15428] Created file "tfile" exclusively
$ [PID 15415] Done sleeping
[PID 15415] Created file "tfile" exclusively      // this NOT true

[1]+  Done                    ./io_bad_exclusive_open tfile sleep

$ ls
tfile


Both processes claim to have created the file because the code of the first
process was interrupted between the existence check and the creation of the
file. Using a single open() call that specifies the O_CREAT and O_EXCL flags
prevents this possibility by guaranteeing that the check and creation steps are
carried out as a single atomic (i.e., uninterruptible) operation.

now not able to simulate this race situation with the single open call.

*/

int main(int argc, char **argv)
{
  if (argc < 2)
    usageErr("%s file\n", argv[0]);

  int fd = open(argv[1], O_WRONLY);

  // Open 1: check if file exists
  if (fd != -1)
  {
    // Open succeeded
    printf("[PID %ld] File \"%s\" already exists\n", (long)getpid(), argv[1]);
    close(fd);
  }
  else
  {
    if (errno != ENOENT)
    {
      /* Failed for unexpected reason */
      errExit("open");
    }
    else
    {
      /* { WINDOW FOR FAILURE */
      // To demonstrate that there is indeed a problem with this code, we could
      // replace the commented line WINDOW FOR FAILURE in Listing 5-1 with a
      // piece of code that creates an artificially long delay between the check
      // for file existence and the creation of the file

      printf("[PID %ld] File \"%s\" doesn't exist yet\n",
             (long)getpid(),
             argv[1]);
      if (argc > 2)
      {           /* Delay between check and create */
        sleep(5); /* Suspend execution for 5 seconds */
        printf("[PID %ld] Done sleeping\n", (long)getpid());
      }
      /* } WINDOW FOR FAILURE */

      fd = open(argv[1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
      if (fd == -1)
        errExit("open");

      printf("[PID %ld] Created file \"%s\" exclusively\n",
             (long)getpid(),
             argv[1]);

      /* MAY NOT BE TRUE! */
    }
  }

  exit(EXIT_SUCCESS);
}

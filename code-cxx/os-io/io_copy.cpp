#include "lpi_error.h"
#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <thread>
#include <unistd.h>

/*
Listing 4-1: Using I/O system calls

We can use the program in Listing 4-1 as follows:
$ ./copy oldfile newfile

*/

const int BUF_SIZE{1024};

int main(int argc, char **argv)
{
  if (argc < 3)
    usageErr("%s oldfile newfile\n", argv[0]);

  int ifd, ofd, oflags;
  mode_t file_perms;
  ssize_t num_read;
  char buf[BUF_SIZE];

  // open input and output files
  ifd = open(argv[1], O_RDONLY);
  if (-1 == ifd)
    errExit("open file %s", argv[1]);

  oflags = O_CREAT | O_WRONLY | O_TRUNC;

  // rw-rw-rw-
  // However, gets:
  // -rw-r--r-- 1 keitee keitee 4.8K Jan 24 13:40 make

  file_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  ofd = open(argv[2], oflags, file_perms);
  if (-1 == ofd)
    errExit("open file %s", argv[2]);

  // transfer data until we see EOF of input or an error
  while ((num_read = read(ifd, buf, BUF_SIZE)) > 0)
    if (write(ofd, buf, num_read) != num_read)
      errExit("couldn't write whole buffer");

  if (-1 == num_read)
    errExit("read");

  if (-1 == close(ifd))
    errExit("close input");

  if (-1 == close(ofd))
    errExit("close output");

  exit(EXIT_SUCCESS);
}

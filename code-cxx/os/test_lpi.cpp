#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <thread>
#include <vector>

#include <fcntl.h>
#include <sys/prctl.h>
#include <sys/uio.h>      // readv()

#include "gmock/gmock.h"

// use lpi log
#include "lpi_error.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

// (gdb) b
// PatternObserver_sendNotificationWithCallerDispatcherButRaiseException_Test::TestBody()
//

/*
={=============================================================================
lpi-file-io

*/

TEST(LpiIO, open)
{
  int fd;

  fd = open("startup", O_RDONLY);
  if (fd == -1)
  {
    errMsg("failed to open file");
  }
}

/*
={=============================================================================
lpi-io

5.7 Scatter-Gather I/O: readv() and writev()

Instead of accepting a single buffer of data to be read or written, these
functions transfer multiple buffers of data in a single system call. The set of
buffers to be transferred is defined by the array iov.

Each element of iov is a structure of the following form:

struct iovec {
  void *iov_base; // Start address of buffer
  size_t iov_len; // Number of bytes to transfer to/from buffer 
};

–––––––––––––––––––––––––––––––––––––––––––––––––––––––––– fileio/t_readv.c

TODO:
How can test it??

Gather output

The writev() system call performs gather output. It concatenates (“gathers”)
data from all of the buffers specified by iov and writes them as a sequence of
contiguous bytes to the file referred to by the file descriptor fd.

The primary advantages of readv() and writev() are convenience and speed. For
example, we could replace a call to writev() by either:

o code that allocates a single large buffer, copies the data to be written from
  other locations in the process’s address space into that buffer, and then
  calls write() to output the buffer; or

o a series of write() calls that output the buffers individually.

The first of these options, while semantically equivalent to using writev(),
leaves us with the inconvenience (and inefficiency) of allocating buffers and
copying data in user space.

The second option is not semantically equivalent to a single call to writev(),
since the write() calls are not performed atomically. Furthermore, performing a
single writev() system call is cheaper than performing multiple write() calls
(refer to the dis- cussion of system calls in Section 3.1).

*/

TEST(LpiIO, readv)
{
  int fd;

  struct iovec iov[3];

  // 3 buffers
  struct stat mystat;
  int x;
  // #define STR_SIZE 100
  char str[100];

  ssize_t numRead, totalRequired;

  // fd = open(argv[1], O_RDONLY);
  fd = open("some input", O_RDONLY);
  if (fd == -1)
    errExit("open failed");

  totalRequired = 0;

  iov[0].iov_base = &mystat;
  iov[0].iov_len = sizeof(struct stat);
  totalRequired += iov[0].iov_len;

  iov[1].iov_base = &x;
  iov[1].iov_len = sizeof(x);
  totalRequired += iov[1].iov_len;

  iov[2].iov_base = str;
  iov[2].iov_len = 100;
  totalRequired += iov[2].iov_len;

  numRead = readv(fd, iov, 3);
  if (numRead == -1)
    errExit("readv failed");

  if (numRead < totalRequired)
    errMsg("read fewer bytes than requested");

  // The gtest macros return a stream for outputting diagnostic messages when a
  // test fails.
  EXPECT_TRUE(false) << "total bytes requested: " << totalRequired
    << ", bytes read: " << numRead;
}


// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

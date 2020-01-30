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
#include <sys/uio.h> // readv()

// #define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from <stdio.h> */
//
// ibc.cpp: In member function ‘virtual void Syscall_GetPidThroughSyscall_Test::TestBody()’:
// libc.cpp:605:13: error: ‘SYS_getpid’ was not declared in this scope
//      syscall(SYS_getpid);
//              ^
// libc.cpp: In function ‘sanitizer_syscall::uptr sanitizer_syscall::internal_stat(const char*, void*)’:
// libc.cpp:699:23: error: ‘__NR_stat’ was not declared in this scope
//  #define SYSCALL(name) __NR_##name
//                        ^
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

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
lpi-io call-writev

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
(refer to the discussion of system calls in Section 3.1).

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
  iov[0].iov_len  = sizeof(struct stat);
  totalRequired += iov[0].iov_len;

  iov[1].iov_base = &x;
  iov[1].iov_len  = sizeof(x);
  totalRequired += iov[1].iov_len;

  iov[2].iov_base = str;
  iov[2].iov_len  = 100;
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
// *sys-memchr*
/*
NAME
       memchr, memrchr, rawmemchr - scan memory for a character

SYNOPSIS
       #include <string.h>

       void *memchr(const void *s, int c, size_t n);

       void *memrchr(const void *s, int c, size_t n);

       void *rawmemchr(const void *s, int c);

DESCRIPTION
       The  memchr()  function  scans  the  initial  n  bytes of the memory area
       pointed to by s for the first instance of c.  Both c and the bytes of the
       memory area pointed to by s are interpreted as unsigned char.

RETURN VALUE
       The memchr() and memrchr() functions return a pointer to the matching
       byte or NULL if the character does not occur in the given memory area.

       The rawmemchr() function returns a pointer to the matching byte, if one
       is found.  If no matching byte is found, the result is unspecified.


STRCHR(3)

NAME
       strchr, strrchr, strchrnul - locate character in string

SYNOPSIS
       #include <string.h>

       char *strchr(const char *s, int c);

       char *strrchr(const char *s, int c);

       #define _GNU_SOURCE         // See feature_test_macros(7)
       #include <string.h>

       char *strchrnul(const char *s, int c);

DESCRIPTION
       The strchr() function returns a pointer to the first occurrence of the
       character c in the string s.

       The strrchr() function returns a pointer to the last occurrence of the
       character c in the string s.

       The strchrnul() function is like strchr() except that if c is not found
       in s, then it returns a pointer to the null byte at the end of s, rather
       than NULL.

       Here "character" means "byte"; these functions do not work with wide or
       multibyte characters.

RETURN VALUE
       The strchr() and strrchr() functions return a pointer to the matched
       character or NULL if the character is not found.  The terminating null
       byte is considered part of the string, so that if c is specified as '\0',
       these functions return a pointer to the terminator.

       The strchrnul() function returns a pointer to the matched character, or a
       pointer to the null byte at the end of s (i.e., s+strlen(s)) if the
       character is not found.

*/

TEST(Glibc, memchar)
{
  char *start{};
  char text[] = "memchr, memrchr, rawmemchr - scan memory for a character";

  // found `s`
  start = (char *)memchr(text, 's', strlen(text));
  if (start)
  {
    EXPECT_STREQ(start, "scan memory for a character");
  }

  // if not found
  start = (char *)memchr(text, 'p', sizeof(text));
  EXPECT_EQ(start, nullptr);

  // note that `null` termination char is different from '\n'

  char text2[] = "memchr, memrchr, rawmemchr - scan memory for a character\n";

  start = (char *)memchr(text2, '\n', strlen(text2));
  EXPECT_TRUE(start);

  // just 1 less in length
  auto diff = start - text2;
  EXPECT_EQ(strlen(text2) - 1, diff);
}

TEST(Glibc, strchar)
{
  char *start{};
  char text[] = "memchr, memrchr, rawmemchr - scan memory for a character";

  // found `s`
  start = (char *)strchr(text, 's');
  if (start)
  {
    EXPECT_STREQ(start, "scan memory for a character");
  }

  // if not found
  start = (char *)strchr(text, 'p');
  EXPECT_EQ(start, nullptr);

  // note that `null` termination char is different from '\n'

  char text2[] = "memchr, memrchr, rawmemchr - scan memory for a character\n";

  // note that strchr() have two args

  start = (char *)strchr(text2, '\n');
  EXPECT_TRUE(start);

  // just 1 less in length
  auto diff = start - text2;
  EXPECT_EQ(strlen(text2) - 1, diff);
}

TEST(Glibc, strrchar)
{
  char *start{};
  char fname[] = "/home/keitee/git/kb/code-cxx/libc/libc.cpp";

  // extract filename only from full path
  start = (char *)strrchr(fname, '/');
  if (start)
  {
    EXPECT_STREQ(start + 1, "libc.cpp");
  }
}

namespace use_internal_strchr_01
{

  char *internal_strchr(const char *s, int c)
  {
    unsigned int i = 0;
    while (s[i] && s[i] != c)
      ++i;

    return strlen(s) == i ? nullptr : (char *)(s + i);
  }

} // namespace use_internal_strchr_01

TEST(Glibc, strrcharOwn)
{
  using namespace use_internal_strchr_01;

  char *start{};
  char text[] = "memchr, memrchr, rawmemchr - scan memory for a character";

  start = (char *)internal_strchr(text, 's');
  if (start)
  {
    // cout works as well as printf
    // cout << start << endl;
    EXPECT_STREQ(start, "scan memory for a character");
  }

  // if not found
  start = (char *)internal_strchr(text, 'p');
  EXPECT_EQ(start, nullptr);

  // note that `null` termination char is different from '\n'

  char text2[] = "memchr, memrchr, rawmemchr - scan memory for a character\n";

  start = (char *)internal_strchr(text2, '\n');
  EXPECT_TRUE(start);

  auto diff = start - text2;
  EXPECT_EQ(strlen(text2) - 1, diff);
}

namespace use_internal_strchr_02
{

  // sanitizer_common/sanitizer_libc.cc
  char *internal_strchr(const char *s, int c)
  {
    while (true)
    {
      if (*s == (char)c)
        return const_cast<char *>(s);
      if (*s == 0)
        return nullptr;
      s++;
    }
  }

} // namespace use_internal_strchr_02

TEST(Glibc, strrcharAsanVersion)
{
  using namespace use_internal_strchr_02;

  char *start{};
  char text[] = "memchr, memrchr, rawmemchr - scan memory for a character";

  start = (char *)internal_strchr(text, 's');
  if (start)
  {
    // cout works as well as printf
    // cout << start << endl;
    EXPECT_STREQ(start, "scan memory for a character");
  }

  // if not found
  start = (char *)internal_strchr(text, 'p');
  EXPECT_EQ(start, nullptr);

  // note that `null` termination char is different from '\n'

  char text2[] = "memchr, memrchr, rawmemchr - scan memory for a character\n";

  start = (char *)internal_strchr(text2, '\n');
  EXPECT_TRUE(start);

  auto diff = start - text2;
  EXPECT_EQ(strlen(text2) - 1, diff);
}

// ={=========================================================================
// *sys-readlink*

/*
READLINK(2)

NAME
       readlink, readlinkat - read value of a symbolic link

SYNOPSIS
       #include <unistd.h>

       ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);

       #include <fcntl.h>           // Definition of AT_* constants
       #include <unistd.h>

       ssize_t readlinkat(int dirfd, const char *pathname,
                          char *buf, size_t bufsiz);

   Feature Test Macro Requirements for glibc (see feature_test_macros(7)):

       readlink():
           _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE &&
_XOPEN_SOURCE_EXTENDED || _POSIX_C_SOURCE >= 200112L

       readlinkat():
           Since glibc 2.10:
               _XOPEN_SOURCE >= 700 || _POSIX_C_SOURCE >= 200809L
           Before glibc 2.10:
               _ATFILE_SOURCE

DESCRIPTION
       readlink() places the contents of the symbolic link pathname in the
       buffer buf, which has size bufsiz.  readlink() does not append a null
       byte to buf.  It will truncate the contents (to a length of bufsiz
       characters), in case the buffer is too small to hold all of the contents.

RETURN VALUE
       On success, these calls return the number of bytes placed in buf.  On
       error, -1 is returned and errno is set to indicate the error.

*/

TEST(Glibc, readlink)
{
  char buffer[100];

  ssize_t len = readlink("/proc/self/exe", buffer, 100);
  if (len != -1)
  {
    buffer[len] = '\0';

    // cout works as well as printf
    // cout << "exe: " << buffer << endl;

    EXPECT_STREQ(buffer, "/home/kyoupark/git/kb/code-linux/ex_libc/libc_out");
  }

  len = readlink("/proc/self/cmdline", buffer, 100);
  if (len != -1)
  {
    buffer[len] = '\0';

    // cout works as well as printf
    cout << "cmdline: " << buffer << endl;

    EXPECT_STREQ(buffer, "/home/kyoupark/git/kb/code-linux/ex_libc/libc_out");
  }
}

/* TODO

s64 internal_atoll(const char *nptr) {
  return internal_simple_strtoll(nptr, nullptr, 10);
}

void *internal_memchr(const void *s, int c, uptr n) {
  const char *t = (const char *)s;
  for (uptr i = 0; i < n; ++i, ++t)
    if (*t == c)
      return reinterpret_cast<void *>(const_cast<char *>(t));
  return nullptr;
}

void *internal_memrchr(const void *s, int c, uptr n) {
  const char *t = (const char *)s;
  void *res = nullptr;
  for (uptr i = 0; i < n; ++i, ++t) {
    if (*t == c) res = reinterpret_cast<void *>(const_cast<char *>(t));
  }
  return res;
}

int internal_memcmp(const void* s1, const void* s2, uptr n) {
  const char *t1 = (const char *)s1;
  const char *t2 = (const char *)s2;
  for (uptr i = 0; i < n; ++i, ++t1, ++t2)
    if (*t1 != *t2)
      return *t1 < *t2 ? -1 : 1;
  return 0;
}

void *internal_memcpy(void *dest, const void *src, uptr n) {
  char *d = (char*)dest;
  const char *s = (const char *)src;
  for (uptr i = 0; i < n; ++i)
    d[i] = s[i];
  return dest;
}

void *internal_memmove(void *dest, const void *src, uptr n) {
  char *d = (char*)dest;
  const char *s = (const char *)src;
  sptr i, signed_n = (sptr)n;
  CHECK_GE(signed_n, 0);
  if (d < s) {
    for (i = 0; i < signed_n; ++i)
      d[i] = s[i];
  } else {
    if (d > s && signed_n > 0)
      for (i = signed_n - 1; i >= 0 ; --i) {
        d[i] = s[i];
      }
  }
  return dest;
}

// Semi-fast bzero for 16-aligned data. Still far from peak performance.
void internal_bzero_aligned16(void *s, uptr n) {
  struct S16 { u64 a, b; } ALIGNED(16);
  CHECK_EQ((reinterpret_cast<uptr>(s) | n) & 15, 0);
  for (S16 *p = reinterpret_cast<S16*>(s), *end = p + n / 16; p < end; p++) {
    p->a = p->b = 0;
    // Make sure this does not become memset.
    SanitizerBreakOptimization(nullptr);
  }
}

void *internal_memset(void* s, int c, uptr n) {
  // The next line prevents Clang from making a call to memset() instead of the
  // loop below.
  // FIXME: building the runtime with -ffreestanding is a better idea. However
  // there currently are linktime problems due to PR12396.
  char volatile *t = (char*)s;
  for (uptr i = 0; i < n; ++i, ++t) {
    *t = c;
  }
  return s;
}

uptr internal_strcspn(const char *s, const char *reject) {
  uptr i;
  for (i = 0; s[i]; i++) {
    if (internal_strchr(reject, s[i]))
      return i;
  }
  return i;
}

char* internal_strdup(const char *s) {
  uptr len = internal_strlen(s);
  char *s2 = (char*)InternalAlloc(len + 1);
  internal_memcpy(s2, s, len);
  s2[len] = 0;
  return s2;
}

char* internal_strndup(const char *s, uptr n) {
  uptr len = internal_strnlen(s, n);
  char *s2 = (char*)InternalAlloc(len + 1);
  internal_memcpy(s2, s, len);
  s2[len] = 0;
  return s2;
}

int internal_strcmp(const char *s1, const char *s2) {
  while (true) {
    unsigned c1 = *s1;
    unsigned c2 = *s2;
    if (c1 != c2) return (c1 < c2) ? -1 : 1;
    if (c1 == 0) break;
    s1++;
    s2++;
  }
  return 0;
}

int internal_strncmp(const char *s1, const char *s2, uptr n) {
  for (uptr i = 0; i < n; i++) {
    unsigned c1 = *s1;
    unsigned c2 = *s2;
    if (c1 != c2) return (c1 < c2) ? -1 : 1;
    if (c1 == 0) break;
    s1++;
    s2++;
  }
  return 0;
}

char* internal_strchr(const char *s, int c) {
  while (true) {
    if (*s == (char)c)
      return const_cast<char *>(s);
    if (*s == 0)
      return nullptr;
    s++;
  }
}

char *internal_strchrnul(const char *s, int c) {
  char *res = internal_strchr(s, c);
  if (!res)
    res = const_cast<char *>(s) + internal_strlen(s);
  return res;
}

char *internal_strrchr(const char *s, int c) {
  const char *res = nullptr;
  for (uptr i = 0; s[i]; i++) {
    if (s[i] == c) res = s + i;
  }
  return const_cast<char *>(res);
}

uptr internal_strlen(const char *s) {
  uptr i = 0;
  while (s[i]) i++;
  return i;
}

char *internal_strncat(char *dst, const char *src, uptr n) {
  uptr len = internal_strlen(dst);
  uptr i;
  for (i = 0; i < n && src[i]; i++)
    dst[len + i] = src[i];
  dst[len + i] = 0;
  return dst;
}

char *internal_strncpy(char *dst, const char *src, uptr n) {
  uptr i;
  for (i = 0; i < n && src[i]; i++)
    dst[i] = src[i];
  internal_memset(dst + i, '\0', n - i);
  return dst;
}

uptr internal_strnlen(const char *s, uptr maxlen) {
  uptr i = 0;
  while (i < maxlen && s[i]) i++;
  return i;
}

char *internal_strstr(const char *haystack, const char *needle) {
  // This is O(N^2), but we are not using it in hot places.
  uptr len1 = internal_strlen(haystack);
  uptr len2 = internal_strlen(needle);
  if (len1 < len2) return nullptr;
  for (uptr pos = 0; pos <= len1 - len2; pos++) {
    if (internal_memcmp(haystack + pos, needle, len2) == 0)
      return const_cast<char *>(haystack) + pos;
  }
  return nullptr;
}

s64 internal_simple_strtoll(const char *nptr, char **endptr, int base) {
  CHECK_EQ(base, 10);
  while (IsSpace(*nptr)) nptr++;
  int sgn = 1;
  u64 res = 0;
  bool have_digits = false;
  char *old_nptr = const_cast<char *>(nptr);
  if (*nptr == '+') {
    sgn = 1;
    nptr++;
  } else if (*nptr == '-') {
    sgn = -1;
    nptr++;
  }
  while (IsDigit(*nptr)) {
    res = (res <= UINT64_MAX / 10) ? res * 10 : UINT64_MAX;
    int digit = ((*nptr) - '0');
    res = (res <= UINT64_MAX - digit) ? res + digit : UINT64_MAX;
    have_digits = true;
    nptr++;
  }
  if (endptr) {
    *endptr = (have_digits) ? const_cast<char *>(nptr) : old_nptr;
  }
  if (sgn > 0) {
    return (s64)(Min((u64)INT64_MAX, res));
  } else {
    return (res > INT64_MAX) ? INT64_MIN : ((s64)res * -1);
  }
}

bool mem_is_zero(const char *beg, uptr size) {
  CHECK_LE(size, 1ULL << FIRST_32_SECOND_64(30, 40));  // Sanity check.
  const char *end = beg + size;
  uptr *aligned_beg = (uptr *)RoundUpTo((uptr)beg, sizeof(uptr));
  uptr *aligned_end = (uptr *)RoundDownTo((uptr)end, sizeof(uptr));
  uptr all = 0;
  // Prologue.
  for (const char *mem = beg; mem < (char*)aligned_beg && mem < end; mem++)
    all |= *mem;
  // Aligned loop.
  for (; aligned_beg < aligned_end; aligned_beg++)
    all |= *aligned_beg;
  // Epilogue.
  if ((char*)aligned_end >= beg)
    for (const char *mem = (char*)aligned_end; mem < end; mem++)
      all |= *mem;
  return all == 0;
}

*/

// ={=========================================================================
// *sys-syscall*

/*

syscall

SYSCALL(2)                                           Linux Programmer's Manual

NAME
       syscall - indirect system call

SYNOPSIS
       #define _GNU_SOURCE          // See feature_test_macros(7)
       #include <sys/syscall.h>     // For SYS_xxx definitions
       #include <unistd.h>

       long syscall(long number, ...);

DESCRIPTION
       syscall()  is  a small library function that invokes the system call
       whose assembly language interface has the specified number with the
       specified arguments.  Employing syscall() is useful, for example, when
       invoking a system call that has no wrapper function in the C library.

       syscall() saves CPU registers before making the system call, restores the
       registers upon return from the system call, and stores any error code
       returned by the system call in errno(3) if an error occurs.

       Symbolic constants for system call numbers can be found in the header
       file <sys/syscall.h>.

RETURN VALUE
       The return value is defined by the system call being invoked.  In
       general, a 0 return value indicates success.  A -1 return value indicates
       an error, and an error code is stored in errno.

NOTES
       syscall() first appeared in 4BSD.

   Architecture-specific requirements
       Each architecture ABI has its own requirements on how system call
       arguments are passed to the kernel.  For system calls that have a Glibc
       wrapper (e.g., most system calls), Glibc handles the details of  copying
       arguments  to the  right  registers  in a manner suitable for the
       architecture.  However, when using syscall() to make a system call, the
       caller might need to handle architecture-dependent details; this
       requirement is most commonly encountered on certain 32-bit architectures.

       For example, on the ARM architecture Embedded ABI (EABI), a 64-bit value
       (e.g., long long) must be aligned to an even register pair.  Thus, using
       syscall() instead of the wrapper provided by glibc, the readahead()
       system call would be invoked as follows on the ARM architecture with the
       EABI:

           syscall(SYS_readahead, fd, 0,
                   (unsigned int) (offset >> 32),
                   (unsigned int) (offset & 0xFFFFFFFF),
                   count);

       Since the offset argument is 64 bits, and the first argument (fd) is
       passed in r0, the caller must manually split and align the 64-bit value
       so that it is passed in the r2/r3 register pair.  That means inserting a
       dummy value into r1 (the second argument of 0).

       Similar issues can occur on MIPS with the O32 ABI, on PowerPC with the
       32-bit ABI, and on Xtensa.

       The affected system calls are fadvise64_64(2), ftruncate64(2),
       posix_fadvise(2), pread64(2), pwrite64(2), readahead(2),
       sync_file_range(2), and truncate64(2).

*/

TEST(Glibc, gettid)
{
  // cout << "tid : " << syscall(SYS_gettid) << endl;
  syscall(SYS_gettid);
}

TEST(Glibc, getpid)
{
  {
    // int pid{};

    for (int i = 0; i < (1 << 24); ++i)
    {
      getpid();
    }
    // cout << "pid : " << pid << endl;
  }
  {
    // int pid{};

    for (int i = 0; i < (1 << 24); ++i)
    {
      syscall(SYS_getpid);
    }
    // cout << "pid : " << pid << endl;
  }
}

namespace sanitizer_syscall
{

  // NAME
  //        stat, fstat, lstat, fstatat - get file status
  //
  // SYNOPSIS
  //        #include <sys/types.h>
  //        #include <sys/stat.h>
  //        #include <unistd.h>
  //
  //        int stat(const char *pathname, struct stat *buf);
  //        int fstat(int fd, struct stat *buf);
  //        int lstat(const char *pathname, struct stat *buf);
  //
  //
  // DESCRIPTION
  //        These  functions  return  information  about  a file, in the buffer
  //        pointed to by stat.  No permissions are required on the file itself,
  //        butin the case of stat(), fstatat(), and lstat()execute (search)
  //        permission is required on all of the directories in pathname that
  //        lead to the file.
  //
  //        stat() and fstatat() retrieve information about the file pointed to
  //        by pathname; the differences for fstatat() are described below.
  //
  //        lstat() is identical to stat(), except that if pathname is a
  //        symbolic link, then it returns information about the link itself,
  //        not the file that it refers to.
  //
  //        fstat() is identical to stat(), except that the file about which
  //        information is to be retrieved is specified by the file descriptor
  //        fd.
  //
  //        All of these system calls return a stat structure, which contains
  //        the following fields:
  //
  //            struct stat {
  //                dev_t     st_dev;         /* ID of device containing file */
  //                ino_t     st_ino;         /* inode number */
  //                mode_t    st_mode;        /* protection */
  //                nlink_t   st_nlink;       /* number of hard links */
  //                uid_t     st_uid;         /* user ID of owner */
  //                gid_t     st_gid;         /* group ID of owner */
  //                dev_t     st_rdev;        /* device ID (if special file) */
  //                off_t     st_size;        /* total size, in bytes */
  //                blksize_t st_blksize;     /* blocksize for filesystem I/O */
  //                blkcnt_t  st_blocks;      /* number of 512B blocks allocated
  //                */
  //
  //               /* Since Linux 2.6, the kernel supports nanosecond
  //                  precision for the following timestamp fields.
  //                  For the details before Linux 2.6, see NOTES. */
  //
  //               struct timespec st_atim;  /* time of last access */
  //               struct timespec st_mtim;  /* time of last modification */
  //               struct timespec st_ctim;  /* time of last status change */
  //
  //           #define st_atime st_atim.tv_sec      /* Backward compatibility */
  //           #define st_mtime st_mtim.tv_sec
  //           #define st_ctime st_ctim.tv_sec
  //           };
  //
  //       Because tests of the above form are common, additional macros are
  //       defined by POSIX to allow the test of the file type in st_mode to be
  //       written more concisely:
  //
  //           S_ISREG(m)  is it a regular file?
  //
  // <skipped>
  //
  // RETURN VALUE
  //       On success, zero is returned.  On error, -1 is returned, and errno is
  //       set appropriately.
  //

  // this means that uses either "sanitizer_syscall_linux_x86_64.inc" or
  // "sanitizer_syscall_generic.inc"
  //
  // #if SANITIZER_LINUX && defined(__x86_64__)
  // #include "sanitizer_syscall_linux_x86_64.inc"
  // #elif SANITIZER_LINUX && defined(__aarch64__)
  // #include "sanitizer_syscall_linux_aarch64.inc"
  // #else
  // #include "sanitizer_syscall_generic.inc"
  // #endif

  typedef unsigned long uptr;
  typedef unsigned long long u64;

// #if !((SANITIZER_FREEBSD || SANITIZER_MAC) && defined(__x86_64__))
#define SYSCALL(name) __NR_##name

  // when not use template version
  // #define internal_syscall syscall

  // kyoupark@kit-debian64:~/git/kb/code-linux/ex_syscall$ nm -C syscall.o |
  // grep internal_syscall 000000000000045e t unsigned long
  // sanitizer_syscall::internal_syscall<unsigned long>(unsigned long long,
  // unsigned long) 0000000000000433 t unsigned long
  // sanitizer_syscall::internal_syscall<unsigned long, unsigned long>(unsigned
  // long long, unsigned long, unsigned long)

  template <typename T1>
  static uptr internal_syscall(u64 nr, T1 arg1)
  {
    u64 retval;
    asm volatile("syscall"
                 : "=a"(retval)
                 : "a"(nr), "D"((u64)arg1)
                 : "rcx", "r11", "memory", "cc");
    return retval;
  }

  template <typename T1, typename T2>
  static uptr internal_syscall(u64 nr, T1 arg1, T2 arg2)
  {
    u64 retval;
    asm volatile("syscall"
                 : "=a"(retval)
                 : "a"(nr), "D"((u64)arg1), "S"((u64)arg2)
                 : "rcx", "r11", "memory", "cc");
    return retval;
  }

  uptr internal_stat(const char *path, void *buf)
  {

    // #if SANITIZER_LINUX && (defined(__x86_64__) || SANITIZER_WORDSIZE == 64)
    // # define SANITIZER_LINUX_USES_64BIT_SYSCALLS 1

    return internal_syscall(SYSCALL(stat), (uptr)path, (uptr)buf);
  }

  bool FileExistsInternal(const char *filename)
  {
    struct stat st;

    if (internal_stat(filename, &st))
      return false;

    // Sanity check: filename is a regular file.
    return S_ISREG(st.st_mode);
  }

  bool FileExistsExternal(const char *filename)
  {
    struct stat st;

    if (stat(filename, &st))
      return false;

    // Sanity check: filename is a regular file.
    return S_ISREG(st.st_mode);
  }

} // namespace sanitizer_syscall

TEST(Glibc, SanitizerSyscallInternal)
{
  using namespace sanitizer_syscall;

  for (int i = 0; i < (1 << 24); ++i)
  {
    EXPECT_TRUE(FileExistsInternal("syscall.cpp"));
  }
}

TEST(Glibc, SanitizerSyscallExternal)
{
  using namespace sanitizer_syscall;

  for (int i = 0; i < (1 << 24); ++i)
  {
    EXPECT_TRUE(FileExistsExternal("syscall.cpp"));
  }
}

// to see how template are instanciated
TEST(Glibc, SingleArgSanitizerSyscall)
{
  using namespace sanitizer_syscall;

  internal_syscall(SYSCALL(unlink), (uptr) "syscall.o");
}

/*
={=============================================================================
glibc-env

GETENV(3)  

RETURN VALUE
       The getenv() function returns a pointer to the value in the environment,
       or NULL if there is no match.

SETENV(3) 

  int setenv(const char *name, const char *value, int overwrite);

DESCRIPTION
       The  setenv()  function  adds  the  variable name to the environment with
       the value `value`, if name does not already exist.  If name does exist in
       the environment, then its value is changed to value if overwrite is
       nonzero; if overwrite is zero, then the value of name is not changed (and
       setenv() returns a success status).  This function makes copies of the
       strings pointed to by name and value (by contrast with putenv(3)).

       The unsetenv() function deletes the variable name from the environment.
       If name does not exist in the environment, then the function succeeds,
       and the environment is unchanged.

*/

TEST(Glibc, env)
{
  // when there is no such env var
  {
    auto ret = getenv("TEST_LPI");
    EXPECT_THAT(ret, nullptr);
  }

  {
    // that is 'overwrite' on, 1, set value regardless
    auto rc = setenv("TEST_LPI", "1", 1);
    if (rc < 0)
      EXPECT_THAT(false, true);

    auto ret = getenv("TEST_LPI");
    EXPECT_THAT(std::string(ret), std::string("1"));

    // env var is already exist and overwrite is 0, so will not change it
    rc = setenv("TEST_LPI", "0", 0);
    if (rc < 0)
      EXPECT_THAT(false, true);

    ret = getenv("TEST_LPI");
    EXPECT_THAT(std::string(ret), std::string("1"));
  }
}

/*
={=============================================================================
os-ansi-color

The scanf() function reads input from the standard input stream stdin, fscanf()
reads input from the stream pointer stream, and sscanf() reads its input from
the character string pointed to by str.

*/

#define ESC '\033'
#define EOE 'm'
#define DEFAULT_COLOR "01;31"

TEST(OsAnsi, color)
{
  // green text
  // all attributes off, greeen + message + back to the default.
  std::cout << "\033[0;32m" "this is ansi color coded text" << "\033[m" << std::endl;

  std::cout << ESC << "[0;32" << EOE << "this is ansi color coded text" << ESC << EOE << std::endl;

  // underscored red on white
  std::cout << "\033[4;31;47m" "this is ansi color coded text" << "\033[m" << std::endl;

  // bold red
  std::cout << "\033[" DEFAULT_COLOR "mthis is ansi color coded text" << "\033[m" << std::endl;

  // underscored red on white
  std::cout << "\033[1;32m" "this is ansi color coded text" << "\033[m" << std::endl;

  // underscored red on white
  printf("\033[1;32m" "this is ansi color coded text\n" "\033[m");
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

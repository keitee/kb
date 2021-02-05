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
#include <fnmatch.h>
#include <pthread.h>
#include <semaphore.h>
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
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include "gmock/gmock.h"

// use lpi log
#include "lpi_error.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

/*

*call-perror* *call-strerror*

https://man7.org/linux/man-pages/man3/strerror.3.html

The strerror() function returns the error string corresponding to the error
number `given in its errnum argument`

#include <stdio.h>
void perror(const char *msg);

#include <string.h>
char *strerror(int errnum);


This is error message that GCC uses with system_error

config/os/generic/error_constants.h:146:      resource_deadlock_would_occur =           EDEADLK,

template<typename _Mutex>
class unique_lock
{
  bool
    try_lock()
    {
      if (!_M_device)
        __throw_system_error(int(errc::operation_not_permitted));
      else if (_M_owns)
        __throw_system_error(int(errc::resource_deadlock_would_occur));
      else
      {
        _M_owns = _M_device->try_lock();
        return _M_owns;
      }
    }
}

*/

TEST(OsGlibc, errno_message)
{
  const std::string message{"Resource deadlock avoided"};
  EXPECT_THAT(message, strerror(EDEADLK));
}

/*
// ={=========================================================================
NOTE: "format" is different from format in printf
call-scanf

       int sscanf(const char *str, const char *format, ...);

       The "format" string consists of a sequence of "directives" which describe
       how to process the sequence of input characters.  If processing of a
       directive fails, no further input is read, and scanf() returns.  A
       "failure" can be either of the following: input failure, meaning that
       input characters were unavailable, or matching failure, meaning that the
       input was inappropriate (see below).

       A "directive" is one of the following:

       ·      A sequence of white-space characters (space, tab, newline, etc.;
       see isspace(3)).  This directive matches any amount of white space,
       including none, in the input.

       ·      An ordinary character (i.e., one other than white space or '%').
       This character must exactly match the next character of input.

       ·      A  conversion specification, which commences with a '%' (percent)
       character.  A sequence of characters from the input is converted
       according to this specification, and the result is placed in the
       corresponding pointer argument.  If the next item of input does not match
       the conversion specification, the conversion fails—this is a matching
       failure.

       Each conversion specification in format begins with either the character
       '%' or the character sequence "%n$" (see below for the distinction)
       followed by:
 
       ·      An optional decimal integer which specifies the maximum field
       width.  Reading of characters stops "either" when this maximum is reached
       or when a nonmatching character is found, whichever  happens  first.
       Most conversions  discard  initial  white  space characters (the
       exceptions are noted below), and these discarded characters don't count
       toward the maximum field width.  String input conversions store a
       terminating null byte ('\0') to mark the end of the input; the maximum
       field width does not include this terminator.


   Conversions
       The following type modifier characters can appear in a conversion
        specification:


       [      

       Matches a nonempty sequence of characters from the specified "set" of
       accepted characters; the next pointer must be a pointer to char, and
       there must be enough room for all the characters in the string,  plus a
       terminating null byte. The usual skip of leading white space is
       suppressed. The string is to be made up of characters in (or not in) a
       particular set; the set is defined by the characters between the open
       bracket [ character and a close bracket ] character. The set excludes
       those characters if the first character after the open bracket is a
       circumflex (^). To include a close bracket in the set,  make  it  the
       first character after the open bracket or the circumflex; any other
       position will end the set. The hyphen character - is also special; when
       placed between two other characters, it adds all intervening charac‐ ters
       to the set. 

       To include a hyphen, make it the last character before the final close
       bracket. For instance, [^]0-9-] means the set "everything except close
       bracket, zero through nine,  and  hyphen".   The string ends with the
       appearance of a character not in the (or, with a circumflex, in) set or
       when the field width runs out.


RETURN VALUE
       On success, these functions return "the number of input items"
       successfully matched and assigned; this can be fewer than provided for,
       or even zero, in the event of an early matching failure.

       The value EOF is returned if the end of input is reached before either
       the first successful conversion or a matching failure occurs.  EOF is
       also returned if a read error occurs, in which case the error indicator
        for the stream (see ferror(3)) is set, and errno is set to indicate the
        error.

*/

TEST(OsGlibc, check_scanf)
{
  {
    const char address[]{"AA:BB:CC:DD:EE:FF"};

    {
      int bytes[6]{};
      int bytesConsumed{};

      auto bytesRead = sscanf(address,
                              "%02X:%02X:%02X:%02X:%02X:%02X%n",
                              &bytes[0],
                              &bytes[1],
                              &bytes[2],
                              &bytes[3],
                              &bytes[4],
                              &bytes[5],
                              &bytesConsumed);

      EXPECT_THAT(strlen(address), 17);
      EXPECT_THAT(bytesConsumed, 17);

      EXPECT_THAT(bytesRead, 6);
      EXPECT_THAT(bytes[0], 0xAA);
      EXPECT_THAT(bytes[1], 0xBB);
      EXPECT_THAT(bytes[2], 0xCC);
    }
  }

  {
    const char input[]{"0r"};
    int fd{};
    char buf[10]{};
    auto num_read = sscanf(input, "%d%2[rw]", &fd, buf);

    EXPECT_THAT(num_read, 2);
    EXPECT_THAT(fd, 0);
    EXPECT_THAT(strcmp(buf, "r"), 0);

    // cout << "fd: " << fd << ", buf: " << buf << endl;
  }

  // expect the second conversion fails
  {
    const char input[]{"0x"};
    int fd{};
    char buf[10]{};
    auto num_read = sscanf(input, "%d%2[rw]", &fd, buf);

    EXPECT_THAT(num_read, 1);
    EXPECT_THAT(fd, 0);
    // EXPECT_THAT(strcmp(buf, "r"), 0);
    // cout << "fd: " << fd << ", buf: " << buf << endl;
  }
}

/*
={=============================================================================
call-memset call-bzero

man bzero

CONFORMING TO
       The bzero() function is "deprecated" (marked as LEGACY in POSIX.1-2001);
       use memset(3) in new programs.  POSIX.1-2008 removes the specification of
       bzero().  The bzero() function first appeared in 4.3BSD.

       The explicit_bzero() function is a nonstandard extension that is also
       present on some of the BSDs.  Some other implementations have a similar
       function, such as memset_explicit() or memset_s().


SYNOPSIS
       #include <string.h>
       void *memset(void *s, int c, size_t n);

DESCRIPTION
       The memset() function fills the first n bytes of the memory area pointed
       to by s with the constant byte c.

*/

// ={=========================================================================
TEST(OsGlibc, check_memset)
{
  char coll1[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  char coll2[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  char coll3[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  char coll4[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  EXPECT_THAT(coll1, ElementsAreArray({1, 1, 1, 1, 1, 1, 1, 1, 1, 1}));

  // same
  EXPECT_THAT(0, memcmp(coll1, coll2, 10));

  // reset and same
  bzero(coll2, 10);
  EXPECT_THAT(coll2, ElementsAreArray({0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
  EXPECT_THAT(0, memcmp(coll2, coll4, 10));

  // reset and same
  memset(coll3, 0, 10);
  EXPECT_THAT(coll3, ElementsAreArray({0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
  EXPECT_THAT(0, memcmp(coll3, coll4, 10));
}

// ={=========================================================================
TEST(OsGlibc, check_memcmp)
{
  char coll1[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  char coll2[10] = {1, 1, 1, 1, 1, 2, 2, 2, 2, 2};

  EXPECT_THAT(coll1, ElementsAreArray({1, 1, 1, 1, 1, 1, 1, 1, 1, 1}));

  // same
  EXPECT_THAT(0, memcmp(coll1, coll2, 2));
  EXPECT_THAT(0, memcmp(coll1, coll2, 3));
  EXPECT_THAT(0, memcmp(coll1, coll2, 4));
  EXPECT_THAT(0, memcmp(coll1, coll2, 5));

  // not same and coll1 is less so -1.
  EXPECT_THAT(memcmp(coll1, coll2, 6), -1);
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
       character or "NULL if the character is not found." The terminating null
       byte is considered part of the string, so that if c is specified as '\0',
       these functions return a pointer to the terminator.

       The strchrnul() function returns a pointer to the matched character, or a
       pointer to the null byte at the end of s (i.e., s+strlen(s)) if the
       character is not found.

*/

// ={=========================================================================
TEST(OsGlibc, check_memchar)
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

// ={=========================================================================
TEST(OsGlibc, check_strchar)
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

// ={=========================================================================
TEST(OsGlibc, check_strrchar)
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

// ={=========================================================================
TEST(OsGlibc, check_strrchar_mine)
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

TEST(OsGlibc, glibc_strrcharAsanVersion)
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

TEST(OsGlibc, glibc_readlink)
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

TEST(OsGlibc, glic_gettid)
{
  // cout << "tid : " << syscall(SYS_gettid) << endl;
  syscall(SYS_gettid);
}

TEST(OsGlibc, glibc_getpid)
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

TEST(OsGlibc, glibc_SanitizerSyscallInternal)
{
  using namespace sanitizer_syscall;

  for (int i = 0; i < (1 << 24); ++i)
  {
    EXPECT_TRUE(FileExistsInternal("syscall.cpp"));
  }
}

TEST(OsGlibc, glibc_SanitizerSyscallExternal)
{
  using namespace sanitizer_syscall;

  for (int i = 0; i < (1 << 2); ++i)
  {
    EXPECT_TRUE(FileExistsExternal("test_os"));
  }
}

// to see how template are instanciated
TEST(OsGlibc, glibc_SingleArgSanitizerSyscall)
{
  using namespace sanitizer_syscall;

  internal_syscall(SYSCALL(unlink), (uptr) "syscall.o");
}

/*
={=============================================================================
glibc-env

#include <stdlib.h>

GETENV(3)  

RETURN VALUE
       The getenv() function returns a pointer to the value in the environment,
       or NULL if there is no match.

char *getenv(const char *name);

note:
"no match" means when there is no env variable set so when "SAMPLE_ENV=" and it
has null but is set, getenv returns "\0" null string but not null pointer. So
need to use strcmp to check if env variable is set or unset since check on NULL
means that env not defined but not set. 

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

TEST(OsGlibc, glibc_env)
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
glibc-fnmatch

FNMATCH(3)

NAME
       fnmatch - match filename or pathname

SYNOPSIS
       #include <fnmatch.h>

       int fnmatch(const char *pattern, const char *string, int flags);

DESCRIPTION
       The fnmatch() function checks whether the string argument matches the
       pattern argument, which is a shell wildcard pattern.


       FNM_PERIOD
              If  this  flag  is  set, a leading period in string has to be
              matched exactly by a period in pattern.  A period is considered to
              be leading if it is the first character in string, or if both
              FNM_PATHNAME is set and the period immediately follows a slash.

RETURN VALUE
       Zero if string matches pattern, FNM_NOMATCH if there is no match or
       another nonzero value if there is an error.

*/

TEST(OsGlibc, glibc_fnmatch)
{
  std::string filter("com.sky.as.*");

  EXPECT_THAT(fnmatch(filter.c_str(), "com.sky.as.player", FNM_PERIOD), 0);

  // FNM_PERIOD has no effect
  EXPECT_THAT(fnmatch(filter.c_str(), "com.sky.as.player", 0), 0);

  // still match
  EXPECT_THAT(fnmatch(filter.c_str(), "com.sky.as.pplayer", 0), 0);

  // but not match
  EXPECT_THAT(fnmatch(filter.c_str(), "com.sky.ax.player", 0), Not(0));
}

/*
={=============================================================================
glibc-fgets

// clang-format off

echo "&vgcassetid=F67307ADA1614C35A0BA4DE39F22272&vgctoken=00000040000001A4000000600000001A00000061000000020002000000630000000800B980D500B98675000000700000017A0006B40A223124823D8BB018C766555E7CDC360F8ADE3F1AF7F91B310D0E2F7624828DBFDE2D3D4E59&streamtype=1" | base64 | tr -d '\n'
JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAwMDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMwMDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNEOEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUyRDNENEU1OSZzdHJlYW10eXBlPTEK

encoded:JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAw
encoded:MDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMw
encoded:MDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNE
encoded:OEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUy
encoded:RDNENEU1OSZzdHJlYW10eXBlPTEK

        JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAw
        MDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMw
        MDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNE
        OEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUy
        RDNENEU1OSZzdHJlYW10eXBlPTEK

keitee@kit-ubuntu:~/git/kb/code-cxx/cxx/build$ echo "&vgcassetid=F67307ADA1614C35A0BA4DE39F22272&vgctoken=00000040000001A4000000600000001A00000061000000020002000000630000000800B980D500B98675000000700000017A0006B40A223124823D8BB018C766555E7CDC360F8ADE3F1AF7F91B310D0E2F7624828DBFDE2D3D4E59&streamtype=1" | base64
JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAw
MDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMw
MDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNE
OEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUy
RDNENEU1OSZzdHJlYW10eXBlPTEK

encoded drmData {
JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAw
MDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMw
MDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNE
OEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUy
RDNENEU1OSZzdHJlYW10eXBlPTEK}

// clang-format on

see how fgets works which removes nees of using "tr" command and this is the
issue. note that we we run below, then there is no need to use tr command.

echo "...." | base64 | base64 -d

However, when use fgets, this is an issue since fgets stops when see first new
line so we get only first line of encoding result.

also, busybox base64 do not support "-w" option to output single full line.


from man:

fgets stores "\n" when read stops

fgets()  reads  in  at most one less than size characters from stream and stores
them into the buffer pointed to by s.

Reading stops after an EOF or a newline. If a newline is read, it is stored into
the buffer.

terminating null byte ('\0') is stored after the last character in the buffer.

fgets() returns s on success, and NULL on error or when end of file occurs while
no characters have been read.
 
NOTE
not NULL when reading stops at new line.

NOTE:
see TEST(OsPipe, check_popen) which is the first version which works because
encoding result from input is within a single line. no multiple lines. so didn't
shows the problem.
 
*/

namespace osfgets
{
  constexpr auto BASE64_COMMAND{"/bin/base64"};
  constexpr auto BASE64_DECODE_COMMAND{"/usr/bin/base64 -d"};
  constexpr int BASE64_MAX_SIZE{100};

  std::string base64_encode(const std::string &input)
  {
    std::ostringstream cmd{};
    std::ostringstream encoded{};
    char read[BASE64_MAX_SIZE]{};

    cmd << "echo '" << input << "' | " << BASE64_COMMAND;

    auto pfd = popen(cmd.str().c_str(), "r");
    if (pfd)
    {
      while (NULL != fgets(read, BASE64_MAX_SIZE, pfd))
      {
        read[strlen(read) - 1] = '\0';
        encoded << read;
      }

      pclose(pfd);
    }
    else
    {
      // LOG_ERROR( "popen failed {%s}", cmd.str().c_str());
      std::cout << "popen failed : " << cmd.str().c_str() << std::endl;
    }

    return encoded.str();
  }

  // NOTE:
  // if reduce read size to 100 for decode, it fails since input for reads can
  // be big but input from base64 tool is wrapped at 76 by default. that's why
  // we need to do while and remove `\n` to make a single line.
  //
  // constexpr int BASE64_MAX_SIZE{100};

  std::string base64_decode(const std::string &input)
  {
    std::ostringstream cmd{};
    std::ostringstream decoded{};
    char read[BASE64_MAX_SIZE]{};

    cmd << "echo '" << input << "' | " << BASE64_DECODE_COMMAND;

    // std::cout << "cmd : " << cmd.str() << std::endl;

    auto pfd = popen(cmd.str().c_str(), "r");
    if (pfd)
    {
      while (NULL != fgets(read, BASE64_MAX_SIZE, pfd))
      {
        read[strlen(read) - 1] = '\0';
        decoded << read;
      }

      pclose(pfd);
    }
    else
    {
      // LOG_ERROR( "popen failed {%s}", cmd.str().c_str());
      std::cout << "popen failed : " << cmd.str().c_str() << std::endl;
    }

    return std::string{decoded.str()};
  }
} // namespace osfgets

TEST(OsGlibc, check_fgets)
{
  using namespace osfgets;

  const char input_1[]{
    "&vgcassetid=F67307ADA1614C35A0BA4DE39F22272&vgctoken="
    "00000040000001A4000000600000001A00000061000000020002000000630000000800B980"
    "D500B98675000000700000017A0006B40A223124823D8BB018C766555E7CDC360F8ADE3F1A"
    "F7F91B310D0E2F7624828DBFDE2D3D4E59&streamtype=1"};

  std::string expected_1{
    "JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMD"
    "AwMDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAw"
    "NjMwMDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMj"
    "Q4MjNEOEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4"
    "REJGREUyRDNENEU1OSZzdHJlYW10eXBlPTEK"};

  // clang-format off
  const char input_2[]{"&vgcassetid=F67307ADA1614C35A0BA4DE39F22272&vgctoken=00000040000001A4000000600000001A00000061000000020002000000630000000800B980D500B98675000000700000017A0006B40A223124823D8BB018C766555E7CDC360F8ADE3F1AF7F91B310D0E2F7624828DBFDE2D3D4E59&streamtype=1"};
  std::string expected_2{"JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAwMDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMwMDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNEOEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUyRDNENEU1OSZzdHJlYW10eXBlPTEK"};
  // clang-format on

  // to double confirm input and expected result of encoding.
  EXPECT_THAT(strcmp(input_1, input_2), 0);
  EXPECT_THAT(expected_1, expected_2);

  std::ostringstream drmData{};

  drmData << input_1;

  // encode drmData using base64
  // printf("input drmData {%s}\n", drmData.str().c_str());
  // std::cout << "input drmData : " << drmData.str().c_str() << std::endl;

  std::string encodedData = base64_encode(drmData.str());

  // printf("encoded drmData {%s}\n", encodedData.c_str());
  // std::cout << "encoded drmData : " << encodedData << std::endl;

  EXPECT_THAT(encodedData, expected_1);

  std::string decodedData = base64_decode(encodedData);

  EXPECT_THAT(decodedData, std::string(input_1));
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
  std::cout << "\033[0;32m"
               "this is ansi color coded text"
            << "\033[m" << std::endl;

  std::cout << ESC << "[0;32" << EOE << "this is ansi color coded text" << ESC
            << EOE << std::endl;

  // underscored red on white
  std::cout << "\033[4;31;47m"
               "this is ansi color coded text"
            << "\033[m" << std::endl;

  // bold red
  std::cout << "\033[" DEFAULT_COLOR "mthis is ansi color coded text"
            << "\033[m" << std::endl;

  // underscored red on white
  std::cout << "\033[1;32m"
               "this is ansi color coded text"
            << "\033[m" << std::endl;

  // underscored red on white
  printf("\033[1;32m"
         "this is ansi color coded text\n"
         "\033[m");
}

/*
={=============================================================================
os-sem-unnamed

Listing 53-6

thread_incr_psem.c

Use a POSIX unnamed semaphore to synchronize access by two threads to a global
variable.
See also thread_incr.c and thread_incr_mutex.c.
 
*/

namespace os_sem
{
  static int glob{0};
  static sem_t sem;

  static void *threadFunc(void *arg)
  {
    int loops = *((int *)arg);
    int count{0}, j{0};

    for (j = 0; j < loops; j++)
    {
      // Returns 0 on success, or -1 on error
      if (-1 == sem_wait(&sem))
        errExit("sem_wait failed");

      count = glob;
      count++;
      glob = count;

      if (-1 == sem_post(&sem))
        errExit("sem_post failed");
    }

    return nullptr;
  }
} // namespace os_sem

// see `sem` is global

TEST(OsSem, sem_unnamed1)
{
  using namespace os_sem;

  pthread_t t1, t2;
  int loops, s;

  // to be sure
  glob = 0;

  // loops = 10,000,000; 10M
  loops = 10000000;

  // init sem to be shared between threads
  if (-1 == sem_init(&sem, 0, 1))
    errExit("sem_init failed");

  // create two threads
  //
  // int pthread_create
  //  (pthread_t *tid, const pthread_attr_t *attr, void *(*func)(void*), void *arg);

  s = pthread_create(&t1, nullptr, threadFunc, &loops);
  if (0 != s)
    errExitEN(s, "pthread_create failed");

  s = pthread_create(&t2, nullptr, threadFunc, &loops);
  if (0 != s)
    errExitEN(s, "pthread_create failed");

  // wait for threads to finish
  //
  // int pthread_join( pthread_t thread, void **retval );

  s = pthread_join(t1, nullptr);
  if (0 != s)
    errExitEN(s, "pthread_join failed");

  s = pthread_join(t2, nullptr);
  if (0 != s)
    errExitEN(s, "pthread_join failed");

  // since two threads runs
  EXPECT_THAT(glob, (2 * loops));
}

// NOTE: see that `sem_local` is local to this function but not global. the
// point is that as long as sem_t is accessable, it is okay to be defined and
// inited in local scope.

TEST(OsSem, sem_unnamed2)
{
  using namespace os_sem;

  int loops, s;

  sem_t sem_local;

  // to be sure
  glob = 0;

  // loops = 10,000,000; 10M
  loops = 10000000;

  // init sem to be shared between threads
  if (-1 == sem_init(&sem_local, 0, 1))
    errExit("sem_init failed");

  // create two threads and sue std::thread since it's more easy to pass args.

  auto f = [&]() {
    int count{0}, j{0};

    for (j = 0; j < loops; j++)
    {
      // Returns 0 on success, or -1 on error
      if (-1 == sem_wait(&sem_local))
        errExit("sem_wait failed");

      count = glob;
      count++;
      glob = count;

      if (-1 == sem_post(&sem_local))
        errExit("sem_post failed");
    }
  };

  std::thread t1(f);
  std::thread t2(f);

  // wait for threads to finish

  t1.join();
  t2.join();

  // since two threads runs
  EXPECT_THAT(glob, (2 * loops));
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

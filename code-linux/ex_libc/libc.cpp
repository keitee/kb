#include "gmock/gmock.h"

#include <iostream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

#include <string.h>

// ={=========================================================================

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

*/

TEST(Memchar, Base)
{
  char *start{};
  char text[] = "memchr, memrchr, rawmemchr - scan memory for a character";

  start = (char*)memchr(text, 's', strlen(text));
  if (start)
  {
    // cout works as well as printf
    // cout << start << endl; 
    EXPECT_STREQ(start, "scan memory for a character");
  }

  // if not found
  start = (char*)memchr(text, 'p', sizeof(text));
  EXPECT_EQ(start, nullptr);

  // note that `null` termination char is different from '\n'

  char text2[] = "memchr, memrchr, rawmemchr - scan memory for a character\n";

  start = (char *)memchr(text2, '\n', strlen(text2));
  EXPECT_TRUE(start);

  auto diff = start-text2;
  EXPECT_EQ(strlen(text2)-1, diff);
}

TEST(Strchar, Base)
{
  char *start{};
  char text[] = "memchr, memrchr, rawmemchr - scan memory for a character";

  start = (char*)strchr(text, 's');
  if (start)
  {
    // cout works as well as printf
    // cout << start << endl; 
    EXPECT_STREQ(start, "scan memory for a character");
  }

  // if not found
  start = (char*)strchr(text, 'p');
  EXPECT_EQ(start, nullptr);

  // note that `null` termination char is different from '\n'

  char text2[] = "memchr, memrchr, rawmemchr - scan memory for a character\n";

  start = (char *)strchr(text2, '\n');
  EXPECT_TRUE(start);

  auto diff = start-text2;
  EXPECT_EQ(strlen(text2)-1, diff);
}

namespace use_internal_strchr_01 {

char *internal_strchr(const char *s, int c)
{
  unsigned int i = 0;
  while (s[i] && s[i] != c)
    ++i;

  return strlen(s) == i ? nullptr : (char *)(s + i);
}

} // namespace

TEST(Strchar, OwnVersion)
{
  using namespace use_internal_strchr_01;

  char *start{};
  char text[] = "memchr, memrchr, rawmemchr - scan memory for a character";

  start = (char*)internal_strchr(text, 's');
  if (start)
  {
    // cout works as well as printf
    // cout << start << endl; 
    EXPECT_STREQ(start, "scan memory for a character");
  }

  // if not found
  start = (char*)internal_strchr(text, 'p');
  EXPECT_EQ(start, nullptr);

  // note that `null` termination char is different from '\n'

  char text2[] = "memchr, memrchr, rawmemchr - scan memory for a character\n";

  start = (char *)internal_strchr(text2, '\n');
  EXPECT_TRUE(start);

  auto diff = start-text2;
  EXPECT_EQ(strlen(text2)-1, diff);
}

namespace use_internal_strchr_02 {

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

} // namespace

TEST(Strchar, AsanVersion)
{
  using namespace use_internal_strchr_02;

  char *start{};
  char text[] = "memchr, memrchr, rawmemchr - scan memory for a character";

  start = (char*)internal_strchr(text, 's');
  if (start)
  {
    // cout works as well as printf
    // cout << start << endl; 
    EXPECT_STREQ(start, "scan memory for a character");
  }

  // if not found
  start = (char*)internal_strchr(text, 'p');
  EXPECT_EQ(start, nullptr);

  // note that `null` termination char is different from '\n'

  char text2[] = "memchr, memrchr, rawmemchr - scan memory for a character\n";

  start = (char *)internal_strchr(text2, '\n');
  EXPECT_TRUE(start);

  auto diff = start-text2;
  EXPECT_EQ(strlen(text2)-1, diff);
}

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
           _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED || _POSIX_C_SOURCE >= 200112L

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

TEST(Readlink, Base)
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
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

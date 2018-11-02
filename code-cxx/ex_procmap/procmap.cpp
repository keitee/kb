#include "gmock/gmock.h"

#include <iostream>
#include <syscall.h>    // system call symbolic names
#include <unistd.h>
#include <asm/param.h>  // EXEC_PAGESIZE
#include <fcntl.h>      // O_RDONLY, etc
#include <sys/mman.h>   // mmap defines

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

*/

// types

// x86_64/VM
// size of (int) is                 : 4
// size of (unsigned int) is        : 4
// size of (long) is                : 8
// size of (unsigned long) is       : 8
// size of (long int) is            : 8
// size of (unsigned int) is        : 8
// size of (long long) is           : 8
// size of (unsigned long long) is  : 8
// size of (* int) is               : 8
// size of (* unsigned int) is      : 8

typedef unsigned  long      uptr;
typedef signed    long      sptr;
typedef unsigned long long  u64;
typedef u64                 OFF_T;

struct ProcSelfMapsBuff 
{
  char *data;
  uptr mmapped_size;
  uptr len;
};

enum FileAccessMode
{
  RdOnly,
  WrOnly,
  RdWr
};


//=============================================================================
// locks TODO

// typedef GenericScopedLock<StaticSpinMutex> SpinMutexLock;

template<typename MutexType>
class GenericScopedLock 
{
  public:
    explicit GenericScopedLock(MutexType *mu) : mu_(mu)
      { mu_->Lock(); }

    ~GenericScopedLock() 
      { mu_->UnLock(); }

  private:
      MutexType *mu_;

      GenericScopedLock(const GenericScopedLock &);

      // GenericScopedLock &operator=(const GenericScopedLock &);
      void operator=(const GenericScopedLock &);
};


//=============================================================================
// check

// #define FORMAT(f, a)  __attribute__((format(printf, f, a)))
// 
// static void SharedPrintfCode(bool append_pid, const char *format,
//                              va_list args) {
//   // TODO
// }
// 
// // sanitizer_common/sanitizer_printf.cc
// FORMAT(1,2)
// void Report(const char *format, ...)
// {
//   va_list args;
//   va_start(arg, format);
//   SharedPrintCode(true, format, args);
//   va_end(args);
// }
// 
// void CheckFailed(const char *file, int line, const char *cond,
//     u64 v1, u64 v2)
// {
//   Report("Sanitize CHECK failed: %s:%d %s (%lld, %lld)\n",
//       file, line, cond, v1, v2);
// 
//   // Die();
// }

void CheckFailed(const char *file, int line, const char *cond,
    u64 v1, u64 v2)
{
  printf("Sanitize CHECK failed: %s:%d %s (%lld, %lld)\n",
      file, line, cond, v1, v2);
}

#define CHECK_IMPL(c1, op, c2) \
  do { \
    unsigned long long v1 = (unsigned long long)(c1); \
    unsigned long long v2 = (unsigned long long)(c2); \
    if (!(v1 op v2)) \
      CheckFailed(__FILE__, __LINE__, \
          "(" #c1 ") " #op " (" #c2 ")", v1, v2); \
  } while (false)

#define CHECK(a)        CHECK_IMPL((a), !=, 0)
#define CHECK_EQ(a, b)  CHECK_IMPL((a), ==, (b))
#define CHECK_GT(a, b)  CHECK_IMPL((a), >, (b))


//=============================================================================
// syscall
// sanitizer_common/sanitizer_syscall_linux_x86_64.inc

// *call-syscall-return*
// On Linux, system call service routines follow a convention of returning a
// nonnegative value to indicate success. In case of an error, the routine
// returns a negative number, which is the negated value of one of the errno
// constants. 
//
// When a negative value is returned, the C library wrapper function negates it
// (to make it positive), copies the result into errno, and returns -1 as the
// function result of the wrapper to indicate an error to the calling program.

// sanitizer_common/sanitizer_libc.h
// Error handling
// bool internal_iserror(uptr retval, int *rverrno = nullptr);

bool internal_iserror(uptr retval, int *rverrno = nullptr)
{
  if (retval >= (uptr)-4095)
  {
    if (rverrno)
      *rverrno = -retval;
    return true;
  }
  return false;
}

// EINTR "interrupted system call"
#define HANDLE_EINTR(res, f)  \
{                             \
  int rverrno;                \
  do {                        \
    res = (f);                \
  } while (internal_iserror(res, &rverrno) && rverrno == EINTR); \
}

#define SYSCALL(name) __NR_ ## name

// arg 1
template <typename T1>
static uptr internal_syscall(u64 nr, T1 arg1) {
  u64 retval;
  asm volatile("syscall" : "=a"(retval) : "a"(nr), "D"((u64)arg1) :
               "rcx", "r11", "memory", "cc");
  return retval;
}

// arg 2
template <typename T1, typename T2>
static uptr internal_syscall(u64 nr, T1 arg1, T2 arg2) {
  u64 retval;
  asm volatile("syscall" : "=a"(retval) : "a"(nr), "D"((u64)arg1),
               "S"((u64)arg2) : "rcx", "r11", "memory", "cc");
  return retval;
}

// arg 3
template <typename T1, typename T2, typename T3>
static uptr internal_syscall(u64 nr, T1 arg1, T2 arg2, T3 arg3) {
  u64 retval;
  asm volatile("syscall" : "=a"(retval) : "a"(nr), "D"((u64)arg1),
               "S"((u64)arg2), "d"((u64)arg3) : "rcx", "r11", "memory", "cc");
  return retval;
}

// arg 4
template <typename T1, typename T2, typename T3, typename T4>
static uptr internal_syscall(u64 nr, T1 arg1, T2 arg2, T3 arg3, T4 arg4) {
  u64 retval;
  asm volatile("mov %5, %%r10;"
               "syscall" : "=a"(retval) : "a"(nr), "D"((u64)arg1),
               "S"((u64)arg2), "d"((u64)arg3), "r"((u64)arg4) :
               "rcx", "r11", "r10", "memory", "cc");
  return retval;
}

// arg 6
template <typename T1, typename T2, typename T3, typename T4, typename T5,
          typename T6>
static uptr internal_syscall(u64 nr, T1 arg1, T2 arg2, T3 arg3, T4 arg4,
                             T5 arg5, T6 arg6) {
  u64 retval;
  asm volatile("mov %5, %%r10;"
               "mov %6, %%r8;"
               "mov %7, %%r9;"
               "syscall" : "=a"(retval) : "a"(nr), "D"((u64)arg1),
               "S"((u64)arg2), "d"((u64)arg3), "r"((u64)arg4), "r"((u64)arg5),
               "r"((u64)arg6) : "rcx", "r11", "r10", "r8", "r9",
               "memory", "cc");
  return retval;
}

// sanitizer_common/sanitizer_libc.cc
void *internal_memchr(const void *s, int c, uptr n) {
  const char *t = (const char *)s;
  for (uptr i = 0; i < n; ++i, ++t)
    if (*t == c)
      return reinterpret_cast<void *>(const_cast<char *>(t));
  return nullptr;
}

// sanitizer_common/sanitizer_linux.cc
uptr internal_mmap(void *addr, uptr length, int prot, int flags, int fd,
                   OFF_T offset) {
  return internal_syscall(SYSCALL(mmap), (uptr)addr, length, prot, flags, fd,
                          offset);
}

uptr internal_munmap(void *addr, uptr length) {
  return internal_syscall(SYSCALL(munmap), (uptr)addr, length);
}

uptr internal_open(const char *filename, int flags, unsigned int mode) {
  return internal_syscall(SYSCALL(open), (uptr)filename, flags, mode);
}

uptr internal_close(int fd) {
  return internal_syscall(SYSCALL(close), fd);
}

uptr internal_read(int fd, void *buf, uptr count) {
  sptr res;
  HANDLE_EINTR(res, (sptr)internal_syscall(SYSCALL(read), fd, (uptr)buf,
               count));
  return res;
}


//=============================================================================
// syscall

uptr GetPageSize()
{
  // from linux
  return EXEC_PAGESIZE;
}

uptr GetPageSizeCached()
{
  static uptr page_size;
  if (!page_size)
    page_size = GetPageSize();
  return page_size;
}

int OpenFile(const char *file_name, FileAccessMode mode, int *errno_p)
{
  int flags;

  switch (mode) 
  {
    case RdOnly: flags = O_RDONLY; break;
    case WrOnly: flags = O_WRONLY; break;
    case RdWr: flags = O_RDWR | O_CREAT; break;
  }

  int res = internal_open(file_name, flags, 0660);
  if (internal_iserror(res, errno_p))
    return (int) -1; // invalid fd
  
  return res;
}

void CloseFile(int fd)
{
  internal_close(fd);
}

void UnmapOrDie(void *addr, uptr size)
{
  // check on args
  if (!addr || !size)
    return;

  uptr res = internal_munmap(addr, size);
  if (internal_iserror(res))
  {
    printf("ERROR: %s failed to deallocate 0x%zx (%zd) bytes at address %p\n",
      "asan", size, size, addr);

    // check and die
    // CHECK("unable to unmap" && 0);
  }

  // TODO
  // DecreaseTotalMmap(size);
}

bool IsPowerOfTwo(uptr x) {
  return (x & (x - 1)) == 0;
}

uptr RoundUpTo(uptr size, uptr boundary) {
  CHECK(IsPowerOfTwo(boundary));
  return (size + boundary - 1) & ~(boundary - 1);
}

void *MmapOrDie(uptr size, const char *mem_type) 
{
  size = RoundUpTo(size, GetPageSizeCached());

  // use anonymous mapping
  uptr res = internal_mmap(nullptr, size,
      PROT_READ|PROT_WRITE,
      MAP_PRIVATE|MAP_ANON, -1, 0);

  int reserrno;
  if (internal_iserror(res, &reserrno))
  {

    printf("ERROR: %s failed to "
         "%s 0x%zx (%zd) bytes of %s (error code: %d)\n",
         "asan", "allocate", size, size, mem_type, reserrno);
  }

  // TODO
  // IncreaseTotalMmap(size);
  return (void *)res;
}

bool ReadFromFile(int fd, void *buff, uptr buff_size, uptr *bytes_read,
    int *error_p)
{
  uptr res = internal_read(fd, buff, buff_size);
  if (internal_iserror(res, error_p))
    return false;

  if (bytes_read)
    *bytes_read = res;

  return true;
}


// 1 << 26 is 64M
// Opens the file 'file_name" and reads up to 'max_len' bytes.
// The resulting buffer is mmaped and stored in '*buff'.
// The size of the mmaped region is stored in '*buff_size'.
// The total number of read bytes is stored in '*read_len'.
// Returns true if file was successfully opened and read.

bool ReadFileToBuffer(const char *file_name, char **buff, uptr *buff_size,
    uptr *read_len, uptr max_len = 1 << 26, int *errno_p = nullptr)
{
  uptr page_size = GetPageSizeCached();
  uptr min_file_len = page_size;

  *buff = nullptr;
  *buff_size = 0;
  *read_len = 0;

  // Q: why for loop? can mapped_size and len be different? Yes, can be
  // different.
  //
  // ReadFileToBuffer("/proc/self/maps", &proc_maps->data,
  //  &proc_maps->mmapped_size, &proc_maps->len);
  //
  // starts from one page(4k), read it, get returned read byte which is not
  // null. ends while
  //
  // starts from two page(8k), read it, get returned read which is 0 for second
  // read. ends while and for.
  
  for (uptr size = min_file_len; size < max_len; size *= 2)
  {
    // 0, read-only
    int fd = OpenFile(file_name, RdOnly, errno_p);
    if ((int)-1 == fd) return false;

    UnmapOrDie(*buff, *buff_size);
    *buff = (char*)MmapOrDie(size, __func__);
    *buff_size = size;
    *read_len = 0;

    // read up one page at a time
    bool reached_eof = false; 
    while (*read_len + page_size <= size)
    {
      uptr just_read;
      if (!ReadFromFile(fd, *buff + *read_len, page_size, &just_read, errno_p))
      {
        UnmapOrDie(*buff, *buff_size);
        return false;
      }

      if (just_read == 0)
      {
        reached_eof = true;
        break;
      }

      *read_len += just_read;
    } // while end

    CloseFile(fd);

    if (reached_eof)
      break;

  } // for end

  return true;
}

void ReadProcMaps(ProcSelfMapsBuff *proc_maps)
{
  CHECK(ReadFileToBuffer("/proc/self/maps", &proc_maps->data,
        &proc_maps->mmapped_size, &proc_maps->len));
}


//=============================================================================
// MemoryMappingLayout

static bool IsOneOf(char c, char c1, char c2)
{
  return c == c1 || c == c2;
}

static int TranslateDigit(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  if (c >= 'A' && c <= 'F')
    return c = 'A' + 10;

  return -1;
}

bool IsDecimal(char c)
{
  int d = TranslateDigit(c);
  return d >= 0 && d <= 9;
}

// like atoi and move up the given arg
//
// note:
// although p is defined as const, this function modifies variable via pointer.
//
// const char *current = "08048000-...";
// ParseHex(&current);
// ParseNumber(&current)
// {
//  (*p)++; means that changes current vaiable. 
// }
//
// That's okay from compiler pov since &current is address and it not changed,
// that is, maintained constness.
//
// Even this do not cause error:
//
// current++;
// ++current;
//

static uptr ParseNumber(const char **p, int base)
{
  uptr n = 0;
  int d;
  CHECK(base >= 2 && base <= 16);
  while ((d = TranslateDigit(**p)) >= 0 && d < base)
  {
    n = n * base + d;
    (*p)++;
  }

  return n;
}

uptr ParseHex(const char **p)
{
  return ParseNumber(p, 16);
}

class MemoryMappingLayout
{
  public:
    explicit MemoryMappingLayout(bool cache_enabled);
    ~MemoryMappingLayout();

    bool Next(uptr *start, uptr *end, uptr *offset,
        char *filename, uptr filename_size, uptr *protection);

    void Print();

  private:
    void LoadFromCache();
    void Reset();

    // asan reads:
    // In some cases, e.g. when running under a sandbox on Linux, ASan is unable
    // to obtain the memory mappings. It should fall back to pre-cached data
    // instead of aborting.
    static void CacheMemoryMappings();

    ProcSelfMapsBuff proc_self_maps_;
    static ProcSelfMapsBuff cached_proc_self_maps_;
    const char *current_;
};

// asan says it "linker initialized"
ProcSelfMapsBuff MemoryMappingLayout::cached_proc_self_maps_;


MemoryMappingLayout::MemoryMappingLayout(bool cache_enabled)
{
  ReadProcMaps(&proc_self_maps_);

  if (cache_enabled)
  {
    if (proc_self_maps_.mmapped_size == 0)
    {
      LoadFromCache();
      CHECK_GT(proc_self_maps_.len, 0);
    }
  }
  else
  {
    CHECK_GT(proc_self_maps_.len, 0);
  }

  Reset();

  if (cache_enabled)
    CacheMemoryMappings();
}

MemoryMappingLayout::~MemoryMappingLayout()
{
  //  Q: who's unmap cached_proc_self_maps_?
  
  // asan reads:
  // Only unmap the buffer if it is different from the cached one. Otherwise
  // it will be unmapped when the cache is refreshed.
  if (proc_self_maps_.data != cached_proc_self_maps_.data)
  {
    UnmapOrDie(proc_self_maps_.data, proc_self_maps_.mmapped_size);
  }
}

void MemoryMappingLayout::CacheMemoryMappings()
{
  // TODO SpinMutexLock l(&cache_lock_);
  ProcSelfMapsBuff old_proc_self_maps;
  old_proc_self_maps = cached_proc_self_maps_;
  ReadProcMaps(&cached_proc_self_maps_);

  if (cached_proc_self_maps_.mmapped_size == 0)
  {
    cached_proc_self_maps_ = old_proc_self_maps;
  }
  else
  {
    if (old_proc_self_maps.mmapped_size)
    {
      UnmapOrDie(old_proc_self_maps.data, old_proc_self_maps.mmapped_size);
    }
  }
}

void MemoryMappingLayout::LoadFromCache()
{
  // TODO SpinMutexLock l(&cache_lock_);
  if (cached_proc_self_maps_.data)
  {
    proc_self_maps_ = cached_proc_self_maps_;
  }
}

void MemoryMappingLayout::Reset()
{
  current_ = proc_self_maps_.data;
}

void MemoryMappingLayout::Print()
{
  cout << "mapping print: " << endl;
  cout << proc_self_maps_.data << endl;
}

// sanitizer_common/sanitizer_procmaps_linux.cc
bool MemoryMappingLayout::Next(uptr *start, uptr *end, uptr *offset,
        char *filename, uptr filename_size, uptr *protection)
{
  char *last = proc_self_maps_.data + proc_self_maps_.len;

  if (current_ >= last) return false;

  uptr dummy;

  // when not interested to have back these

  if (!start) start = &dummy;
  if (!end) end = &dummy;
  if (!offset) offset = &dummy;
  if (!protection) protection = &dummy;

  char *next_line = (char*)internal_memchr(current_, '\n', last-current_);
  if (next_line == nullptr)
    next_line = last;

  // address           perms offset  dev   inode       pathname
  // 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon

  *start = ParseHex(&current_);
  CHECK_EQ(*current_++, '-');

  *end = ParseHex(&current_);
  CHECK_EQ(*current_++, ' ');

  // check, read and incease
  CHECK(IsOneOf(*current_, '-', 'r'));
  *protection = 0;
  if (*current_++ == 'r')
    *protection |= 1; // kProtectionRead

  // check, read and incease
  CHECK(IsOneOf(*current_, '-', 'w'));
  if (*current_++ == 'r')
    *protection |= 2; // kProtectionWrite

  // check, read and incease
  CHECK(IsOneOf(*current_, '-', 'x'));
  if (*current_++ == 'r')
    *protection |= 4; // kProtectionExecute

  // check, read and incease
  CHECK(IsOneOf(*current_, 's', 'p'));
  if (*current_++ == 'r')
    *protection |= 8; // kProtectionShared

  CHECK_EQ(*current_++, ' ');
  *offset = ParseHex(&current_);

  // dev part
  CHECK_EQ(*current_++, ' ');
  ParseHex(&current_);
  CHECK_EQ(*current_++, ':');
  ParseHex(&current_);
  CHECK_EQ(*current_++, ' ');

  // discard inode
  while (IsDecimal(*current_))
    ++current_;

  // skip spaces
  while (current_ < next_line && *current_ == ' ')
    ++current_;

  // pathname
  uptr i = 0;
  while (current_ < next_line)
  {
    if (filename && i < filename_size-1)
      filename[i++] = *current_;
    
    ++current_;
  }

  if (filename && i < filename_size)
    filename[i] = 0;

  current_ = next_line + 1;

  return true;
}


//=============================================================================
//

TEST(ProcMap, PageSize)
{
  EXPECT_THAT(GetPageSizeCached(), 4096);
}

TEST(ProcMap, Check)
{
  CHECK(1);
  CHECK(0);
}

TEST(ProcMap, ReadProcMaps)
{
  ProcSelfMapsBuff proc_self_maps;
  ReadProcMaps(&proc_self_maps);
  EXPECT_NE(proc_self_maps.len, proc_self_maps.mmapped_size);
}


// mapping print:
// 00400000-0047a000 r-xp 00000000 08:01 85197425                           /home/kyoupark/git/kb/code-cxx/t_ex_procmap/procmap_out
// 00679000-0067a000 rw-p 00079000 08:01 85197425                           /home/kyoupark/git/kb/code-cxx/t_ex_procmap/procmap_out
// 0067a000-0067b000 rw-p 00000000 00:00 0
// 01b13000-01b34000 rw-p 00000000 00:00 0                                  [heap]
// 7f96319a8000-7f9631b49000 r-xp 00000000 08:01 55184367                   /lib/x86_64-linux-gnu/libc-2.19.so
// 7f9631b49000-7f9631d49000 ---p 001a1000 08:01 55184367                   /lib/x86_64-linux-gnu/libc-2.19.so
// 7f9631d49000-7f9631d4d000 r--p 001a1000 08:01 55184367                   /lib/x86_64-linux-gnu/libc-2.19.so
// 7f9631d4d000-7f9631d4f000 rw-p 001a5000 08:01 55184367                   /lib/x86_64-linux-gnu/libc-2.19.so
// 7f9631d4f000-7f9631d53000 rw-p 00000000 00:00 0
// 7f9631d53000-7f9631d69000 r-xp 00000000 08:01 55181316                   /lib/x86_64-linux-gnu/libgcc_s.so.1
// 7f9631d69000-7f9631f68000 ---p 00016000 08:01 55181316                   /lib/x86_64-linux-gnu/libgcc_s.so.1
// 7f9631f68000-7f9631f69000 rw-p 00015000 08:01 55181316                   /lib/x86_64-linux-gnu/libgcc_s.so.1
// 7f9631f69000-7f9632069000 r-xp 00000000 08:01 55184371                   /lib/x86_64-linux-gnu/libm-2.19.so
// 7f9632069000-7f9632268000 ---p 00100000 08:01 55184371                   /lib/x86_64-linux-gnu/libm-2.19.so
// 7f9632268000-7f9632269000 r--p 000ff000 08:01 55184371                   /lib/x86_64-linux-gnu/libm-2.19.so
// 7f9632269000-7f963226a000 rw-p 00100000 08:01 55184371                   /lib/x86_64-linux-gnu/libm-2.19.so
// 7f963226a000-7f9632356000 r-xp 00000000 08:01 104206913                  /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.20
// 7f9632356000-7f9632556000 ---p 000ec000 08:01 104206913                  /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.20
// 7f9632556000-7f963255e000 r--p 000ec000 08:01 104206913                  /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.20
// 7f963255e000-7f9632560000 rw-p 000f4000 08:01 104206913                  /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.20
// 7f9632560000-7f9632575000 rw-p 00000000 00:00 0
// 7f9632575000-7f963258d000 r-xp 00000000 08:01 55184363                   /lib/x86_64-linux-gnu/libpthread-2.19.so
// 7f963258d000-7f963278c000 ---p 00018000 08:01 55184363                   /lib/x86_64-linux-gnu/libpthread-2.19.so
// 7f963278c000-7f963278d000 r--p 00017000 08:01 55184363                   /lib/x86_64-linux-gnu/libpthread-2.19.so
// 7f963278d000-7f963278e000 rw-p 00018000 08:01 55184363                   /lib/x86_64-linux-gnu/libpthread-2.19.so
// 7f963278e000-7f9632792000 rw-p 00000000 00:00 0
// 7f9632792000-7f96327b3000 r-xp 00000000 08:01 55184364                   /lib/x86_64-linux-gnu/ld-2.19.so
// 7f963298c000-7f9632992000 rw-p 00000000 00:00 0
// 7f96329ab000-7f96329b2000 rw-p 00000000 00:00 0
// 7f96329b2000-7f96329b3000 r--p 00020000 08:01 55184364                   /lib/x86_64-linux-gnu/ld-2.19.so
// 7f96329b3000-7f96329b4000 rw-p 00021000 08:01 55184364                   /lib/x86_64-linux-gnu/ld-2.19.so
// 7f96329b4000-7f96329b5000 rw-p 00000000 00:00 0
// 7ffea6f7a000-7ffea6f9b000 rw-p 00000000 00:00 0                          [stack]
// 7ffea6fb4000-7ffea6fb6000 r-xp 00000000 00:00 0                          [vdso]
// 7ffea6fb6000-7ffea6fb8000 r--p 00000000 00:00 0                          [vvar]
// ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]

TEST(DISABLED_ProcMap, MappingPrint)
{
  MemoryMappingLayout proc_maps(/*cache_enabled*/true);
  proc_maps.Print();
}

// break ProcMap_TranslateDigit_Test::ProcMap_TranslateDigit_Test
TEST(ProcMap, TranslateDigit)
{
  uptr *start{};
  uptr *end{};
  uptr *offset{};
  uptr *protection{};
  char filename[8*10];
  uptr filename_size = 8*10;
  uptr dummy;

  // when not interested to have back these

  if (!start) start = &dummy;
  if (!end) end = &dummy;
  if (!offset) offset = &dummy;
  if (!protection) protection = &dummy;

  // address           perms offset  dev   inode       pathname
  // 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon

  const char *current = "08048000-08056000 r-xp 00000000 03:0c 64593   /foo/bar\n";

  char *next_line = (char *)memchr(current, '\n', strlen(current));
  EXPECT_TRUE(next_line);

  *start = ParseHex(&current);
  EXPECT_EQ(*start, 0x08048000);
  EXPECT_EQ(*current++, '-');

  *end = ParseHex(&current);
  EXPECT_EQ(*end, 0x08056000);
  EXPECT_EQ(*current++, ' ');

  // check read permission, do not increase current
  EXPECT_TRUE(IsOneOf(*current, '-', 'r'));
  *protection = 0;
  
  if (*current++ == 'r')
    *protection |= 1; // read

  // check write permission, do not increase current
  EXPECT_TRUE(IsOneOf(*current, '-', 'w'));
  *protection = 0;
  
  if (*current++ == 'w')
    *protection |= 2; // read

  // check exec permission, do not increase current
  EXPECT_TRUE(IsOneOf(*current, '-', 'x'));
  *protection = 0;
  
  if (*current++ == 'x')
    *protection |= 4; // read

  // check shared permission, do not increase current
  EXPECT_TRUE(IsOneOf(*current, 's', 'p'));
  *protection = 0;
  
  if (*current++ == 's')
    *protection |= 8; // shared

  EXPECT_EQ(*current++, ' ');
  *offset = ParseHex(&current);

  // discard `dev` part
  EXPECT_EQ(*current++, ' ');
  ParseHex(&current);
  EXPECT_EQ(*current++, ':');
  ParseHex(&current);
  EXPECT_EQ(*current++, ' ');

  // discard `inode` part
  while (IsDecimal(*current))
    ++current;

  // skip spaces
  while (current < next_line && *current == ' ')
    ++current;

  // fill in the filename only when filename and filename_size(array size) are
  // not null
  // commented out filename to avoid warnings.

  unsigned int i = 0;
  while (current < next_line)
  {
    if (/* filename && */ i < filename_size-1)
      filename[i++] = *current;

    ++current;
  }

  if (/* filename && */ i < filename_size)
    filename[i] = 0;

  EXPECT_STREQ(filename, "/foo/bar");

  // since it deals with many lines 
  // current = next_line + 1;
}


TEST(ProcMap, MappingNext)
{
  MemoryMappingLayout proc_maps(/*cache_enabled*/true);

  uptr start, end, offset, protection;

  char filename[128];

  cout << setw(20) << left << "| start " 
    << setw(20) << "| end" << setw(20) << "| perms" << setw(20) << "| offset" 
    << setw(40) << "| pathname" << endl;
  cout << "=================================================================" 
    << endl;

  while (proc_maps.Next(&start, &end, &offset, filename,
        128, &protection)) 
  {
    cout << setw(20) << left << hex << start
      << setw(20) << hex << end << setw(20) << hex << protection 
      << setw(20) << hex << offset << setw(40) << hex << filename << endl;
  }
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

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
// locks

typedef GenericScopedLock<StaticSpinMutex> SpinMutexLock;

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


// 2018.10.27

class MemoryMappingLayout
{
  public:
    explicit MemoryMappingLayout(bool cache_enabled);
    ~MemoryMappingLayout();

  private:
    void LoadFromCache();

    ProcSelfMapsBuff proc_self_maps_;
};


MemoryMappingLayout::MemoryMappingLayout(bool cache_enabled)
{
  ReadProcMaps(&proc_self_maps_);

  if (cache_enabled)
  {
    if (proc_self_maps_.mmapped_size == 0)
    {
      LoadFromCache();
    }
  }
}

MemoryMappingLayout::LoadFromCache()
{
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


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

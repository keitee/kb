#include "gmock/gmock.h"

#include <iostream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

// #define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from <stdio.h> */
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

// ={=========================================================================

/*

syscall

SYSCALL(2)                                           Linux Programmer's Manual

NAME
       syscall - indirect system call

SYNOPSIS
       #define _GNU_SOURCE          // See feature_test_macros(7)
       #include <unistd.h>
       #include <sys/syscall.h>     // For SYS_xxx definitions

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
       arguments are passed to the kernel.  For system calls that have a glibc
       wrapper (e.g., most system calls), glibc handles the details of  copying
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

TEST(Syscall, GetTid)
{
  // cout << "tid : " << syscall(SYS_gettid) << endl;
  syscall(SYS_gettid);
}

TEST(Syscall, GetPidThroughGlibc)
{
  // int pid{};

  for (int i = 0; i < (1 << 24); ++i)
  {
    getpid();
  }
  // cout << "pid : " << pid << endl;
}

TEST(Syscall, GetPidThroughSyscall)
{
  // int pid{};

  for (int i = 0; i < (1 << 24); ++i)
  {
    syscall(SYS_getpid);
  }
  // cout << "pid : " << pid << endl;
}

namespace sanitizer_syscall {

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
//        permission is required on all of the directories in pathname that lead to
//        the file.
// 
//        stat() and fstatat() retrieve information about the file pointed to by
//        pathname; the differences for fstatat() are described below.
// 
//        lstat() is identical to stat(), except that if pathname is a symbolic
//        link, then it returns information about the link itself, not the file
//        that it refers to.
// 
//        fstat() is identical to stat(), except that the file about which
//        information is to be retrieved is specified by the file descriptor fd.
// 
//        All of these system calls return a stat structure, which contains the
//        following fields:
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
//                blkcnt_t  st_blocks;      /* number of 512B blocks allocated */
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
//       On success, zero is returned.  On error, -1 is returned, and errno is set
//       appropriately.
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


// kyoupark@kit-debian64:~/git/kb/code-linux/ex_syscall$ nm -C syscall.o | grep internal_syscall
// 000000000000045e t unsigned long sanitizer_syscall::internal_syscall<unsigned long>(unsigned long long, unsigned long)
// 0000000000000433 t unsigned long sanitizer_syscall::internal_syscall<unsigned long, unsigned long>(unsigned long long, unsigned long, unsigned long)

template <typename T1>
  static uptr internal_syscall(u64 nr, T1 arg1)
  {
    u64 retval;
    asm volatile("syscall" : "=a"(retval) : "a"(nr), "D"((u64)arg1) :
        "rcx", "r11", "memory", "cc");
    return retval;
  }

template <typename T1, typename T2>
  static uptr internal_syscall(u64 nr, T1 arg1, T2 arg2)
  {
    u64 retval;
    asm volatile("syscall" : "=a"(retval) : "a"(nr), "D"((u64)arg1),
        "S"((u64)arg2) : "rcx", "r11", "memory", "cc");
    return retval;
  }

uptr internal_stat(const char *path, void *buf) {

  // #if SANITIZER_LINUX && (defined(__x86_64__) || SANITIZER_WORDSIZE == 64)
  // # define SANITIZER_LINUX_USES_64BIT_SYSCALLS 1

  return internal_syscall(SYSCALL(stat), (uptr)path, (uptr)buf);
}

bool FileExistsInternal(const char *filename) {
  struct stat st;

  if (internal_stat(filename, &st))
    return false;

  // Sanity check: filename is a regular file.
  return S_ISREG(st.st_mode);
}

bool FileExistsExternal(const char *filename) {
  struct stat st;

  if (stat(filename, &st))
    return false;

  // Sanity check: filename is a regular file.
  return S_ISREG(st.st_mode);
}

} // namespace

TEST(Syscall, SanitizerSyscallInternal)
{
  using namespace sanitizer_syscall;

  for (int i = 0; i < (1 << 24); ++i)
  {
    EXPECT_TRUE(FileExistsInternal("syscall.cpp"));
  }
}

TEST(Syscall, SanitizerSyscallExternal)
{
  using namespace sanitizer_syscall;

  for (int i = 0; i < (1 << 24); ++i)
  {
    EXPECT_TRUE(FileExistsExternal("syscall.cpp"));
  }
}

// to see how template are instanciated
TEST(Syscall, SingleArgSanitizerSyscall)
{
  using namespace sanitizer_syscall;

  internal_syscall(SYSCALL(unlink), (uptr)"syscall.o");
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

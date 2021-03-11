#if 0

$ strace ./test_trace

execve("./test_trace", ["./test_trace"], 0x7ffca6c77540 /* 86 vars */) = 0
brk(NULL)                               = 0x5592e2510000
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=131291, ...}) = 0
mmap(NULL, 131291, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f4f52014000
close(3)                                = 0
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\260\34\2\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0755, st_size=2030544, ...}) = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f4f52012000
mmap(NULL, 4131552, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f4f51a1d000
mprotect(0x7f4f51c04000, 2097152, PROT_NONE) = 0
mmap(0x7f4f51e04000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1e7000) = 0x7f4f51e04000
mmap(0x7f4f51e0a000, 15072, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7f4f51e0a000
close(3)                                = 0
arch_prctl(ARCH_SET_FS, 0x7f4f520134c0) = 0
mprotect(0x7f4f51e04000, 16384, PROT_READ) = 0
mprotect(0x5592e1f35000, 4096, PROT_READ) = 0
mprotect(0x7f4f52035000, 4096, PROT_READ) = 0
munmap(0x7f4f52014000, 131291)          = 0
fstat(1, {st_mode=S_IFREG|0644, st_size=1565, ...}) = 0
brk(NULL)                               = 0x5592e2510000
brk(0x5592e2531000)                     = 0x5592e2531000
write(1, "hello world.\n", 13hello world.
)          = 13
exit_group(0)                           = ?
+++ exited with 0 +++


$ ltrace ./test_trace

puts("hello world.")                             = 13
hello world.
+++ exited (status 0) +++


$ uftrace ./test_trace
hello world.
#DURATION TID FUNCTION
            [  3901] | main() {
   4.507 us [  3901] |   puts();
   6.538 us [  3901] | } /* main */

#endif

#include <stdio.h>

int main()
{
  printf("hello world.\n");
}

#define _GNU_SOURCE
#include <link.h>
#include <stdio.h>
#include <string.h>

/*

global-buffer-overflow
gcc -fsanitize=address testmain.c -o out_asn

32 bits

kyoupark@kt-office-debian:~/git/kb/asan$ ./gbo_host_asan
main: begin.
main: this global buffer overflow..
main:
=================================================================
==28181==ERROR: AddressSanitizer: global-buffer-overflow on address 0x00447060 at pc 0x0044593a bp 0xbf7ffb18 sp 0xbf7ffb0c
READ of size 8 at 0x00447060 thread T0
    #0 0x445939 in call_gbo /home/kyoupark/git/kb/asan/gbo.c:62
    #1 0x4459f2 in main /home/kyoupark/git/kb/asan/gbo.c:72
    #2 0xb7042285 in __libc_start_main (/lib/i386-linux-gnu/libc.so.6+0x18285)
    #3 0x445760  (/home/kyoupark/git/kb/asan/gbo_host_asan+0x760)

0x00447066 is located 0 bytes to the right of global variable 'gclientname' defined in 'gbo.c:57:13' (0x447060) of size 6
  'gclientname' is ascii string '_DLRH'
SUMMARY: AddressSanitizer: global-buffer-overflow /home/kyoupark/git/kb/asan/gbo.c:62 in call_gbo
Shadow bytes around the buggy address:
  0x20088db0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x20088dc0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x20088dd0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x20088de0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x20088df0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x20088e00: 00 00 00 00 00 00 00 00 00 00 00 00[06]f9 f9 f9
  0x20088e10: f9 f9 f9 f9 00 00 00 00 00 00 00 00 00 00 00 00
  0x20088e20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x20088e30: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x20088e40: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x20088e50: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Heap right redzone:      fb
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack partial redzone:   f4
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==28181==ABORTING

64 bits

kyoupark@kit-debian64:~/git/kb/asan$ ./gbo_host_asan
main: begin.
main: this global buffer overflow..
main:
=================================================================
==24198==ERROR: AddressSanitizer: global-buffer-overflow on address 0x000000601300 at pc 0x400a0c bp 0x7fffb2829f10 sp 0x7fffb2829f08
READ of size 8 at 0x000000601300 thread T0
    #0 0x400a0b in call_gbo /home/kyoupark/git/kb/asan/gbo.c:12
    #1 0x400b17 in main /home/kyoupark/git/kb/asan/gbo.c:22
    #2 0x7fcca65b9b44 in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x21b44)
    #3 0x400858 (/home/kyoupark/git/kb/asan/gbo_host_asan+0x400858)

0x000000601306 is located 0 bytes to the right of global variable 'gclientname' from 'gbo.c' (0x601300) of size 6
  'gclientname' is ascii string '_DLRH'
SUMMARY: AddressSanitizer: global-buffer-overflow /home/kyoupark/git/kb/asan/gbo.c:12 call_gbo
Shadow bytes around the buggy address:
  0x0000800b8210: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8220: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8230: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8240: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8250: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x0000800b8260:[06]f9 f9 f9 f9 f9 f9 f9 00 00 00 00 00 00 00 00
  0x0000800b8270: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8280: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8290: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b82a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b82b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Heap right redzone:      fb
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack partial redzone:   f4
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Contiguous container OOB:fc
  ASan internal:           fe
==24198==ABORTING

*/

static int header_handler(struct dl_phdr_info* info, size_t size, void* data)
{
  int j = 0;
  printf("name=%s (%d segments) address=%p\n",
      info->dlpi_name, info->dlpi_phnum, (void*)info->dlpi_addr);
  for (j = 0; j < info->dlpi_phnum; j++) {
    printf("\t\t header %2d: address=%10p\n", j,
        (void*) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr));
    printf("\t\t\t type=%u, flags=0x%X\n",
        info->dlpi_phdr[j].p_type, info->dlpi_phdr[j].p_flags);
  }
  printf("\n");
  return 0;
}

static char gclientname[] = "_DLRH";

int call_normal()
{
  printf("call_normal: begin.\n" );
  int result = 0, i = 0;

  for(i; i < 10; ++i)
    result += i;

  printf("call_normal: end.\n" );
  return result;
}

void call_gbo()
{
  char buffer[10];
  memcpy(buffer, gclientname, 8);
  return;
}

int main(int argc, char **argv)
{
  dl_iterate_phdr(header_handler, NULL);

  printf("main: begin.\n" );
  printf("main: this global buffer overflow..\n" );
  printf("main: \n" );

  call_gbo();

  printf("main: continue running..\n" );
  call_normal();
  printf("main: ends.\n" );

  return 0;
}


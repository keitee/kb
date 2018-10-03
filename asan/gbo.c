#include <stdio.h>
#include <string.h>

/*

global-buffer-overflow
gcc -fsanitize=address testmain.c -o out_asn

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

static char gclientname[] = "_DLRH";

void call_gbo()
{
  char buffer[10];
  memcpy(buffer, gclientname, 8);
  return;
}

int main(int argc, char **argv)
{
    printf("main: begin.\n" );
    printf("main: this global buffer overflow..\n" );
    printf("main: \n" );

    call_gbo();

    printf("main: ends.\n" );

    return 0;
}


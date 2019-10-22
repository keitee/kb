/*

// builds only on 32 bits

gcc -g -c ml_main.c -o ml_mainreloc.o
gcc -shared -o libmlreloc.so ml_mainreloc.o


kyoupark@kt-office-debian:~/works$ objdump -d ml_reloc.o

ml_reloc.o:     file format elf32-i386


Disassembly of section .text:

00000000 <ml_func>:
   0:   55                      push   %ebp
   1:   89 e5                   mov    %esp,%ebp
   3:   e8 fc ff ff ff          call   4 <ml_func+0x4>
   8:   05 01 00 00 00          add    $0x1,%eax
   d:   8b 88 00 00 00 00       mov    0x0(%eax),%ecx
  13:   8b 55 08                mov    0x8(%ebp),%edx
  16:   01 ca                   add    %ecx,%edx
  18:   89 90 00 00 00 00       mov    %edx,0x0(%eax)
  1e:   8b 90 00 00 00 00       mov    0x0(%eax),%edx
  24:   8b 45 0c                mov    0xc(%ebp),%eax
  27:   01 d0                   add    %edx,%eax
  29:   5d                      pop    %ebp
  2a:   c3                      ret

Disassembly of section .text.__x86.get_pc_thunk.ax:

00000000 <__x86.get_pc_thunk.ax>:
   0:   8b 04 24                mov    (%esp),%eax
   3:   c3                      ret
kyoupark@kt-office-debian:~/works$ objdump -d -Mintel ml_reloc.o

ml_reloc.o:     file format elf32-i386


Disassembly of section .text:

00000000 <ml_func>:
   0:   55                      push   ebp
   1:   89 e5                   mov    ebp,esp
   3:   e8 fc ff ff ff          call   4 <ml_func+0x4>
   8:   05 01 00 00 00          add    eax,0x1
   d:   8b 88 00 00 00 00       mov    ecx,DWORD PTR [eax+0x0]
  13:   8b 55 08                mov    edx,DWORD PTR [ebp+0x8]
  16:   01 ca                   add    edx,ecx
  18:   89 90 00 00 00 00       mov    DWORD PTR [eax+0x0],edx
  1e:   8b 90 00 00 00 00       mov    edx,DWORD PTR [eax+0x0]
  24:   8b 45 0c                mov    eax,DWORD PTR [ebp+0xc]
  27:   01 d0                   add    eax,edx
  29:   5d                      pop    ebp
  2a:   c3                      ret

Disassembly of section .text.__x86.get_pc_thunk.ax:

00000000 <__x86.get_pc_thunk.ax>:
   0:   8b 04 24                mov    eax,DWORD PTR [esp]
   3:   c3                      ret
kyoupark@kt-office-debian:~/works$
*/

int myglob = 42;

int ml_func(int a, int b)
{
    myglob += a;
    return b + myglob;
}

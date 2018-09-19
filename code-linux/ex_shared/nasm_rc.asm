
/*
$ nasm -f elf64 nasm_rc.asm -o nasm_rc.o
$ ld -o nasm_rc64 nasm_rc.o
*/

section    .text
    ; The _start symbol must be declared for the linker (ld)
    global _start

_start:
    ; Execute sys_exit call. Argument: status -> ebx
    mov     eax, 1
    mov     ebx, 42
    int     0x80

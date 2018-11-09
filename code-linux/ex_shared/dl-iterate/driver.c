#define _GNU_SOURCE
#include <link.h>
#include <stdlib.h>
#include <stdio.h>

// kyoupark@kit-debian64:~/git/kb/code-linux/ex_shared/dl-iterate$ LD_LIBRARY_PATH=. ./driver_out
//
// name= (8 segments) address=(nil)
//                  header  0: address=  0x400040 type=6, flags=0x5
//                  header  1: address=  0x400200 type=3, flags=0x4
//                  header  2: address=  0x400000 type=1, flags=0x5
//                  header  3: address=  0x600ac8 type=1, flags=0x6
//                  header  4: address=  0x600ae0 type=2, flags=0x6
//                  header  5: address=  0x40021c type=4, flags=0x4
//                  header  6: address=  0x400974 type=1685382480, flags=0x4
//                  header  7: address=     (nil) type=1685382481, flags=0x6
// 
// name=linux-vdso.so.1 (4 segments) address=0x7ffc273f4000
//                  header  0: address=0x7ffc273f4000 type=1, flags=0x5
//                  header  1: address=0x7ffc273f4318 type=2, flags=0x4
//                  header  2: address=0x7ffc273f4818 type=4, flags=0x4
//                  header  3: address=0x7ffc273f4854 type=1685382480, flags=0x4
// 
// name=./libshared.so (6 segments) address=0x7f72375d7000
//                  header  0: address=0x7f72375d7000 type=1, flags=0x5
//                  header  1: address=0x7f72377d7758 type=1, flags=0x6
//                  header  2: address=0x7f72377d7770 type=2, flags=0x6
//                  header  3: address=0x7f72375d7190 type=4, flags=0x4
//                  header  4: address=0x7f72375d76d0 type=1685382480, flags=0x4
//                  header  5: address=0x7f72375d7000 type=1685382481, flags=0x6
// 
// name=/lib/x86_64-linux-gnu/libc.so.6 (10 segments) address=0x7f723722c000
//                  header  0: address=0x7f723722c040 type=6, flags=0x5
//                  header  1: address=0x7f723739a650 type=3, flags=0x4
//                  header  2: address=0x7f723722c000 type=1, flags=0x5
//                  header  3: address=0x7f72375cd748 type=1, flags=0x6
//                  header  4: address=0x7f72375d0ba0 type=2, flags=0x6
//                  header  5: address=0x7f723722c270 type=4, flags=0x4
//                  header  6: address=0x7f72375cd748 type=7, flags=0x4
//                  header  7: address=0x7f723739a66c type=1685382480, flags=0x4
//                  header  8: address=0x7f723722c000 type=1685382481, flags=0x6
//                  header  9: address=0x7f72375cd748 type=1685382482, flags=0x4
// 
// name=/lib64/ld-linux-x86-64.so.2 (7 segments) address=0x7f72377d8000
//                  header  0: address=0x7f72377d8000 type=1, flags=0x5
//                  header  1: address=0x7f72379f8c00 type=1, flags=0x6
//                  header  2: address=0x7f72379f8e70 type=2, flags=0x6
//                  header  3: address=0x7f72377d81c8 type=4, flags=0x4
//                  header  4: address=0x7f72377f5680 type=1685382480, flags=0x4
//                  header  5: address=0x7f72377d8000 type=1685382481, flags=0x6
//                  header  6: address=0x7f72379f8c00 type=1685382482, flags=0x4

// /usr/include/link.h
//
// struct dl_phdr_info
//   {
//     ElfW(Addr) dlpi_addr;
//     const char *dlpi_name;
//     const ElfW(Phdr) *dlpi_phdr;
//     ElfW(Half) dlpi_phnum;
//   };
// 
// /usr/include/elf.h
// 
// /* Program segment header.  */
// 
// typedef struct
// {
//   Elf32_Word	p_type;			/* Segment type */
//   Elf32_Off	p_offset;		/* Segment file offset */
//   Elf32_Addr	p_vaddr;		/* Segment virtual address */
//   Elf32_Addr	p_paddr;		/* Segment physical address */
//   Elf32_Word	p_filesz;		/* Segment size in file */
//   Elf32_Word	p_memsz;		/* Segment size in memory */
//   Elf32_Word	p_flags;		/* Segment flags */
//   Elf32_Word	p_align;		/* Segment alignment */
// } Elf32_Phdr;
// 
// Program Headers:
//   Type           Offset             VirtAddr           PhysAddr
//                  FileSiz            MemSiz              Flags  Align
//   PHDR           0x0000000000000040 0x0000000000400040 0x0000000000400040
//                  0x00000000000001c0 0x00000000000001c0  R E    8
//   INTERP         0x0000000000000200 0x0000000000400200 0x0000000000400200
//                  0x000000000000001c 0x000000000000001c  R      1
//       [Requesting program interpreter: /lib64/ld-linux-x86-64.so.2]

static int header_handler(struct dl_phdr_info* info, size_t size, void* data)
{
    printf("name=%s (%d segments) address=%p\n",
            info->dlpi_name, info->dlpi_phnum, (void*)info->dlpi_addr);

    for (int j = 0; j < info->dlpi_phnum; j++) {

         printf("\t\t header %2d: address=%10p type=%u, flags=0x%X\n", j,
             (void*) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr),
                 info->dlpi_phdr[j].p_type, info->dlpi_phdr[j].p_flags);

         // printf("\t\t\t type=%u, flags=0x%X\n",
         //        info->dlpi_phdr[j].p_type, info->dlpi_phdr[j].p_flags);
    }

    printf("\n");
    return 0;
}


extern int ml_func(int, int);

int main(int argc, const char* argv[])
{
    dl_iterate_phdr(header_handler, NULL);

    int t = ml_func(argc, argc);
    return t;
}

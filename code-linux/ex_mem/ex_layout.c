#include <cstdio>
#include <cstdlib>
#include <unistd.h>

int g[100];
int bssvar;
int datavar = 99;

int main()
{
  int stackvar;
  char *mallocptr = (char *)malloc(100);

  printf("=== start of pid %ld\n", (long)getpid());
  printf("address of main func      = 0x%16lx\n", (unsigned long)main);
  printf("address of malloc func    = 0x%16lx\n", (unsigned long)malloc);
  printf("address of global var     = 0x%16lx\n", (unsigned long)g);
  printf("address of bss section    = 0x%16lx\n", (unsigned long)&bssvar);
  printf("address of data section   = 0x%16lx\n", (unsigned long)&datavar);
  printf("address of stack section  = 0x%16lx\n", (unsigned long)&stackvar);
  printf("address of malloc section = 0x%16lx\n", (unsigned long)&mallocptr);

  while(1)
    sleep(10);

  printf("=== end of pid %ld\n", (long)getpid());
}

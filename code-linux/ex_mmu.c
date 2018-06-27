#include <stdio.h>
#include <stdlib.h>

int bssvar;
int datavar = 99;

int main()
{
  int stackvar;
  char *mallocptr = malloc(100);

  printf("\n");
  printf("address of bss section    = 0x%16lx\n", (unsigned long)&bssvar);
  printf("address of data section   = 0x%16lx\n", (unsigned long)&datavar);
  printf("address of stack section  = 0x%16lx\n", (unsigned long)&stackvar);
  printf("address of malloc section = 0x%16lx\n", (unsigned long)&mallocptr);

  while(1)
    sleep(10);
}

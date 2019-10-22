/*

// builds only on 32 bits

gcc -g -c ml_main.c -o ml_mainreloc.o
gcc -shared -o libmlreloc.so ml_mainreloc.o

*/

#include <stdio.h>

int myglob_1 = 10;

// extern void order_so_1(int size);

int ml_util_func(int a)
{
  printf("ml_util_func in 1:\n");
  return a+1;
}

void order_so_1(int size)
{
  printf("order_so_1(): {\n");
  myglob_1 += ml_util_func(size);

  order_so_2(myglob_1);
  printf("order_so_1(): myglob_1: %d }\n", myglob_1);
}


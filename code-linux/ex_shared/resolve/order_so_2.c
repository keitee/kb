/*

// builds only on 32 bits

gcc -g -c ml_main.c -o ml_mainreloc.o
gcc -shared -o libmlreloc.so ml_mainreloc.o

*/

#include <stdio.h>

int myglob_2 = 20;

int ml_util_func(int a)
{
  printf("ml_util_func in 2:\n");
  return a+2;
}

void order_so_2(int size)
{
  printf("order_so_2(): {\n");
  myglob_2 += ml_util_func(size);

  order_so_3(myglob_2);

  printf("order_so_2(): myglob_2: %d }\n", myglob_2);
}


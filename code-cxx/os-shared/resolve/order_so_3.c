/*

// builds only on 32 bits

gcc -g -c ml_main.c -o ml_mainreloc.o
gcc -shared -o libmlreloc.so ml_mainreloc.o

*/

#include <stdio.h>

int myglob_3 = 30;

int ml_util_func(int a)
{
  printf("ml_util_func in 3:\n");
  return a+3;
}

void order_so_3(int size)
{
  printf("order_so_3(): {\n");
  myglob_3 += ml_util_func(size);

  printf("order_so_3(): myglob_3: %d }\n", myglob_3);
}


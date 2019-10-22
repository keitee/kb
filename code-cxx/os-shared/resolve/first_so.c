/*

// builds only on 32 bits

gcc -g -c ml_main.c -o ml_mainreloc.o
gcc -shared -o libmlreloc.so ml_mainreloc.o

*/

#include <stdio.h>

int myglob = 42;

int ml_util_func(int a)
{
  return a+1;
}

void *malloc(size_t size)
{
  printf("my malloc: {\n");
  int c = 10 + ml_util_func(20);
  myglob += 30;
  ml_util_func(40);
  printf("my malloc: }\n");

  return (void *)0;
}

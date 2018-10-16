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

void foo()
{
  printf("so foo(): {\n");
  myglob += 30;
  printf("so foo(): myglob: %d }\n", myglob);
}


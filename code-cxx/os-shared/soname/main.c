// $ cat main.c 
#include <stdio.h>

extern void foo(void);

int main(void)
{
  printf("main: calls foo()...\n");
  foo();
  return 0;
}

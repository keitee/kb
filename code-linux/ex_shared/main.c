#include <stdio.h>
#include <unistd.h>

int main()
{
  printf("main: call.\n");

  foo();

  printf("main: ends.\n");
}


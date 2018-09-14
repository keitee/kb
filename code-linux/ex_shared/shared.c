#include <stdio.h>
#include <unistd.h>

void foo()
{
  printf("shared: foo.\n");

  while(1)
    sleep(10);

  printf("shared: end.\n");
}


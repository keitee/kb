/* 
   A program to test posix_spawn.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // sleep

int main(int argc, char *argv[])
{
  printf("child: starts\n");

  for (int i = 0; i < argc; ++i)
  {
    printf("child: argv[%d] = %s\n", i, argv[i]);
  }

  printf("child: sleep\n");
  for (int i = 0; i < 60; ++i)
  {
    printf("child: sleep %d\n", 1);
    sleep(1);
  }

  printf("child: exit\n");
  exit(EXIT_SUCCESS);
}

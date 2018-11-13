/* 
   A program to test posix_spawn and tool-asan
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // sleep
#include <string.h>   // sleep

static char gclientname[] = "_DLRH";

void call_gbo()
{
  char buffer[10];
  memcpy(buffer, gclientname, 10);
  return;
}

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
    if (i == 30)
      call_gbo();
  }

  printf("child: exit\n");
  exit(EXIT_SUCCESS);
}

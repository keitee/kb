#include "lpi_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*

Listing 27-2: Display argument list and environment
––––––––––––––––––––––––––––––––––––––––––––––––––––––– procexec/envargs.c
*/

extern char **environ;

int main(int argc, char **argv)
{
  int j;
  char **ep;

  for (j = 0; j < argc; j++)
    printf("argv[%d] = %s \n", j, argv[j]);

  for (ep = environ; *ep != NULL; ep++)
    printf("env: %s \n", *ep);

  printf("exit from \"test_process_envargs\"\n");

  exit(EXIT_SUCCESS);
}

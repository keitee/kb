#include "lpi_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*

This program creates an argument list and an environment for a new program, and
then calls execve(), using its command-line argument (argv[1]) as the pathname
to be executed.

Listing 27-2 shows a program that is designed to be executed by the program in
Listing 27-1. This program simply displays its command-line arguments and
environment list

The following shell session demonstrates the use of the programs in Listing
27-1 and Listing 27-2 (in this example, a relative pathname is used to specify
the program to be execed):

$ ./t_execve ./envargs 
argv[0] = envargs 
argv[1] = hello world 
argv[2] = goodbye 
env: GREET=salut 
env: BYE=adieu 


$ ./test_process_execve ./test_process_envargs
found / from argv[1] {./test_process_envargs}
argv[0] = test_process_envargs
argv[1] = hello world
argv[2] = goodbye
env: GREET=salut
env: BYE=adieu
exit from "test_process_envargs"


Listing 27-1: Using execve() to execute a new program
–––––––––––––––––––––––––––––––––––––––––––––––––––––– procexec/t_execve.c
*/

int main(int argc, char **argv)
{
  char *argVec[10];
  char *envVec[] = {"GREET=salut", "BYE=adieu", NULL};

  if (argc != 2 || strcmp(argv[1], "--help") == 0)
  {
    printf("%s pathname\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // get basename from argv[1]
  // when found, if not do "argVec[0]++", then:
  // argv[0] = /test_process_envargs

  argVec[0] = strrchr(argv[1], '/');
  if (argVec[0] != nullptr)
  {
    printf("found / from argv[1] {%s}\n", argv[1]);
    argVec[0]++;
  }
  else
  {
    printf("not found / from argv[1] {%s}\n", argv[1]);
    argVec[0] = argv[1];
  }

  // note: ternimate with NULL and argVec[] is passed as a whole to execed
  // application.

  argVec[1] = "hello world";
  argVec[2] = "goodbye";
  argVec[3] = nullptr;

  // Since it replaces the program that called it, a successful execve() never
  // returns. that is, return from the "argv[1]", test_process_envargs.

  execve(argv[1], argVec, envVec);

  printf("if we get here, someting wrong.\n");

  exit(EXIT_FAILURE);
}

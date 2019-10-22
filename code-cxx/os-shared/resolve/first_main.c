#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int myglob;

// when use:
// void *malloc(char size)
//
// gcc -g -std=c99 first_main.c libfirst.so -o first_out
// first_main.c:8:7: error: conflicting types for ‘malloc’
//  void *malloc(char size)
//        ^
// In file included from first_main.c:2:0:
// /usr/include/stdlib.h:466:14: note: previous declaration of ‘malloc’ was here
//  extern void *malloc (size_t __size) __THROW __attribute_malloc__ __wur;
//               ^
// makefile:31: recipe for target 'first_out' failed

void *malloc(size_t size)
{
  printf("my main malloc: {\n");
  myglob += 30;
  printf("my main malloc: myglob: %d }\n", myglob);
 
  return (void *)0;
}

// my main malloc: {
// my main malloc: myglob: 72 }

int main(int argc, char **argv)
{
    printf("====> \n" );
    printf("====> which malloc do you see?..\n" );
    printf("====> \n" );

    int *x = malloc(10*sizeof(int));

    // free(x);
    // x[5] = 10;

    sleep(20);

    return 0;
}


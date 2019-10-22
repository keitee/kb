#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int myglob;

#if 0
void foo(int size)
{
  printf("my main foo(int): {\n");
  myglob += 30;
  printf("my main foo(int): size: %d }\n", size);
}
#endif

// void foo()
// {
//   printf("my main foo(): {\n");
//   myglob += 30;
//   printf("my main foo(): myglob: %d }\n", myglob);
// }

int main(int argc, char **argv)
{
    printf("====> \n" );
    printf("====> which foo do you see?..\n" );
    printf("====> \n" );

    foo(10);

    sleep(20);

    return 0;
}


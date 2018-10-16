#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// $ LD_LIBRARY_PATH=. LD_PRELOAD=libpreload.so ./preload_out 

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


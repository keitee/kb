#include <stdio.h>
#include <stdlib.h>

//	gcc -fsanitize=address testmain.c -o out_asn
int main(int argc, char **argv)
{
    printf("====> \n" );
    printf("====> this is uaf(use after free)..\n" );
    printf("====> \n" );
    int *x = malloc(10*sizeof(int));
    free(x);
    x[5] = 10;
    return;
}


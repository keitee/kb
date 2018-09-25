#include <stdio.h>
#include <stdlib.h>

//	gcc -fsanitize=address testmain.c -o out_asn

int call_add(int a, int b)
{
    int c = a*10 + b;
    return c;
}

void call_uaf()
{
    int *x = malloc(10*sizeof(int));
    free(x);
    x[5] = 10;

    return;
}

int main(int argc, char **argv)
{
    printf("main: begin.\n" );
    printf("main: this is uaf(use after free)..\n" );
    printf("main: \n" );

    call_uaf();

    printf("main: returns from call_add() is %d\n", call_add(10, 20));
    printf("main: ends.\n" );

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int count = 0;
    char filename[256] = {0};
    FILE *fp;

    if (argc < 2)
    {
        fprintf(stderr, "specify num of files to run...\n");
        exit(EXIT_FAILURE);
    }

    // count  = atoi(argv[1]);
    count  = 200; 

    fprintf(stderr, "create, open but not close for %d...\n", count);

    for (unsigned int i = 0; i < 200; ++i)
    {
        sprintf(filename, "./kit_%d", i );

        fprintf(stderr, "opening %d, %s...\n", i, filename);
        if ((fp = fopen( filename, "w+")) == NULL)
        {
            fprintf(stderr, "fail to open %d, %s...\n", i, filename);
            exit(EXIT_FAILURE);
        }
    }

    sleep(100);

    exit(EXIT_SUCCESS);
}

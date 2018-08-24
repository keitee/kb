#include <stdio.h>
#include <stdlib.h>

// gcc -fsanitize=address testmain.c -o out_asn
// home/kyoupark/STB_SW/FUSIONOS_9/BLD_NDS_INTEL_X86_LNUX_MRFUSION_01/platform_cfg/linux/compiler/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc -fsanitize=address uaf.c

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


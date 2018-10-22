#include <stdio.h>
#include <stdlib.h>

int HDR_M=2;
int HDR_m=6;

int main(int argc __attribute__((unused)),
         char** argv __attribute__((unused)))
{
    if((LINUX_VERSION_CODE & ~0xFF)
        != KERNEL_VERSION(HDR_M,HDR_m,0))
    {
        printf("Incorrect selection of kernel headers: ");
        printf("expected %d.%d.x, got %d.%d.x\n", HDR_M, HDR_m,
               ((LINUX_VERSION_CODE>>16) & 0xFF),
               ((LINUX_VERSION_CODE>>8) & 0xFF));
        printf("return 1.\n");
        return 1;
    }
    printf("return 0.\n");
    return 0;
}

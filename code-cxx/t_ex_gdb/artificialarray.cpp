#include <cstdlib>

int main()
{
  int *x = (int *)malloc(25*sizeof(int));
  x[3] = 12;
  x[4] = 14;
}

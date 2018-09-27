/*

https://eli.thegreenplace.net/2011/02/07/how-debuggers-work-part-3-debugging-information
How debuggers work: Part 3 - Debugging information

*/

#include <stdio.h>

#define BASE_OFFSET   (1000)
#define VALUE_FIXED   (BASE_OFFSET+10)
#define VALUE_DEFINE(a)  (BASE_OFFSET+(10*a))

void do_stuff(int my_arg)
{
  int my_local = my_arg + 2;
  int i;

  for (i = 0; i < my_local; ++i)
  {
    printf("%d, i = %d\n", VALUE_FIXED, i);
    printf("%d, i = %d\n", VALUE_DEFINE(3), i);
  }
}


int main()
{
  do_stuff(2);
  return 0;
}

#include <cstdio>

/*
This program will not cause a bus error under Linux running on
the x86 architecture, because on these processors nonaligned memory
accesses are legal; they just execute more slowly than aligned accesses do.

In any event, a bus error is a processor-level exception that causes a
SIGBUS signal to be raised on a Unix system. By default, SIGBUS will cause a
process to dump core and terminate.
*/

int main(void)
{
  char *char_ptr;
  int *int_ptr;
  int int_array[2];

  // char_ptr points to first array element
  char_ptr = (char *) int_array;

  // Causes int_ptr to point one byte past the start of an existing int.
  // Since an int can't be only one byte, int_ptr is no longer aligned.
  int_ptr = (int *) (char_ptr+1);
  *int_ptr = 1; // And this might cause a bus error.

  return 0;
}

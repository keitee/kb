/*

// builds only on 32 bits

gcc -g -c ml_main.c -o ml_mainreloc.o
gcc -shared -o libmlreloc.so ml_mainreloc.o

*/

int myglob = 42;

int ml_util_func(int a)
{
  return a+1;
}

int ml_func(int a, int b)
{
  int c = b + ml_util_func(a);
  myglob += a;
  ml_util_func(b);
  return b + myglob;
}

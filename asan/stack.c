#include <stdio.h>
#include <stdlib.h>

//	gcc -fsanitize=address testmain.c -o out_asn

int call_two(int a1, int a2, int a3)
{
  printf("call_two: begin.\n" );

  int a1_ = a3;
  int a2_ = a3;
  int result = 0;

  int i = 0;
  for(i = 0; i < 10; ++i)
    a1_ += a1;

  for(i = 0; i < 20; ++i)
    a2_ += a2;

  result = a1_ + a2_;
  printf("call_two: return %ld\n", result);
  return result;
}

int call_one(int a1, int a2)
{
  printf("call_one: begin.\n" );

  int a1_ = 0;
  int a2_ = 0;
  int result = 0;

  int i = 0;
  for(i = 0; i < 10; ++i)
    a1_ += a1;

  for(i = 0; i < 20; ++i)
    a2_ += a2;

  result = call_two(a1_, a2_, 100);
  printf("call_one: return %ld\n", result);
  return result;
}

int main(int argc, char **argv)
{
    printf("main: begin.\n" );
    printf("main: this is uaf(use after free)..\n" );
    printf("main: \n" );

    printf("main: returns from call_one() is %d\n", call_one(10, 20));
    printf("main: ends.\n" );

    return 0;
}


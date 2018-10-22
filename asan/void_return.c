#include <stdio.h>
#include <stdlib.h>

//	gcc -fsanitize=address testmain.c -o out_asn

int call_two(int a1, int a2, int a3)
{
  printf("call_two: begin.\n" );

  int a1_ = a3;
  int a2_ = a3;

  int i = 0;
  for(i = 0; i < 5; ++i)
    a1_ += a1;

  for(i = 0; i < 10; ++i)
    a2_ += a2;

  printf("call_one: result %ld, %ld\n", a1_, a2);
  return (a1_+a2_);
}

void call_one(int a1, int a2)
{
  printf("call_one: begin.\n" );

  int a1_ = 0;
  int a2_ = 0;

  int i = 0;
  for(i = 0; i < 10; ++i)
    a1_ += a1;

  for(i = 0; i < 20; ++i)
    a2_ += a2;

  printf("call_one: result %ld, %ld\n", a1_, a2);
}

int main(int argc, char **argv)
{
  int result = 0;

  printf("main: begin.\n" );

  printf("main: calls one...\n" );
  call_one(10, 20);

  printf("main: calls two...\n" );
  result = call_two(10, 20, 30);
  printf("main: calls two result %ld\n", result );

  printf("main: ends.\n" );
  return 0;
}


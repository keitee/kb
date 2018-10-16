#include <stdio.h>

int count;

void foo() {
  printf("ent: this is foo function(%d)\n", count);
  count++;
  printf("lev: this is foo function(%d)\n", count);
}

// the both equally works
// void bar() __attribute__ ((__weak__, alias("foo")));
void bar() __attribute__ ((weak, alias("foo")));

int main(int argc, char **argv) {

  (void)argc;
  (void)argv;

  printf("call foo...");
  foo();

  printf("call bar...");
  bar();

  return 0;
}

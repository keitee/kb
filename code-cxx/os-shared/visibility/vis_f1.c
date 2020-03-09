// #include <iostream>
#include <stdio.h>

extern void vis_comm();

void vis_f1()
{
  // std::cout << "this is vis_f1" << std::endl;
  printf("this is vis_f1\n");
  vis_comm();
}

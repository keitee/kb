// #include <iostream>
#include <stdio.h>

extern void vis_comm();

void vis_f2()
{
  // std::cout << "this is vis_f2" << std::endl;
  printf("this is vis_f2\n");
  vis_comm();
}

#include "gmock/gmock.h"

#include <iostream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

*/

// types

// x86_64/VN
// size of (* int) is         : 8
// size of (* unsigned int) is: 8

typedef unsigned long uptr;

struct ProcSelfMapsBuff 
{
  char *data;
  uptr mmapped_size;
  uptr len;
};


// common global functions

void ReadProcMaps(ProcSelfMapsBuff *proc_maps)
{
  CHECK(ReadFileToBuffer("/proc/self/maps", &proc_maps->data,
        &proc_maps->mmapped_size, &proc_maps->len));
}


// 2018.10.27

class MemoryMappingLayout
{
  public:
    explicit MemoryMappingLayout(bool cache_enable);
    ~MemoryMappingLayout();
};


MemoryMappingLayout::MemoryMappingLayout(bool cache_enable)
{
}

TEST(IPV4, 20181015)
{
  ipv4 address(168, 192, 0, 1);   
  std::cout << address << std::endl;   

  ipv4 ip;   
  std::cout << ip << std::endl;   

  stringstream ss{"10.10.10.10"};
  ss >> ip;   

  EXPECT_THAT(ss.fail(), false);
  std::cout << ip << std::endl;
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

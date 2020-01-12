#include <incbin.h>
#include <iostream>

using namespace std;

INCBIN(DrmConfig, "alice.json");

int main()
{
  for (int i = 0; i < gDrmConfigSize; i++)
  {
    std::cout << gDrmConfigData[i];
  }
  std::cout << std::endl;
}

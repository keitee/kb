#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

typedef bool (*UPDATEFUNC)(int);

bool UpdateProgress(int percent)
{
  cout << flush << "\r" << percent << " % complete...";
  // cout << "\r" << percent << "% complete...";
  return true;
}

int main()
{
  UPDATEFUNC f = UpdateProgress;

  for (long l = 0; l < 100000000; ++l)
  {
    if (l % 1000000 == 0)
      f(l / 1000000);

    for (long x = 0; x < 100; ++x)
      x = x; 

    // this_thread::sleep_for(std::chrono::milliseconds{1});
  }

  cout << endl;

  return EXIT_SUCCESS;
}

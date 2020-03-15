#include "time.h"
#include <iostream>

using namespace std;
using namespace std::chrono;

TestTimer::TestTimer(const std::string &text)
    : text_(text)
{
  start_ = system_clock::now();
}

TestTimer::~TestTimer()
{
  auto diff     = system_clock::now() - start_;
  auto elapsed_ = duration_cast<microseconds>(diff);
  cout << "time taken for " << text_ << " is " << elapsed_.count() << endl;
  cout << "time taken for " << text_ << " is " << elapsed_.count() / 1000
       << endl;
  cout << "time taken for " << text_ << " is " << elapsed_.count() * 0.001
       << endl;

  auto elapsedms_ = duration_cast<milliseconds>(diff);
  cout << "time taken for " << text_ << " is " << elapsedms_.count() << endl;
}

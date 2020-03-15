//
// Modern C++ Programming with Test-Driven Development
// c9/1/GeoServerTest.cpp
//

#include "location.h"

std::ostream &operator<<(std::ostream &os, const Location &location)
{
  os << "(" << location.latitude() << "," << location.longitude() << ")";
  return os;
}


#ifndef User_h
#define User_h

#include "location.h"

class User
{
public:
  User(const std::string &name, Location location)
      : name_(name)
      , location_(location)
  {}

  std::string name() { return name_; }
  Location location() { return location_; }

private:
  std::string name_;
  Location location_;
};

#endif

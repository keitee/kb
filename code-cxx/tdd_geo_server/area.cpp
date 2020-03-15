#include "area.h"

Area::Area(const Location &location, double width, double height)
{
  left_   = location.go(width / 2, West).longitude();
  right_  = location.go(width / 2, East).longitude();
  top_    = location.go(height / 2, North).latitude();
  bottom_ = location.go(height / 2, South).latitude();
}

Location Area::upperLeft() const
{
  return Location{top_, left_};
}

Location Area::upperRight() const
{
  return Location{top_, right_};
}

Location Area::lowerRight() const
{
  return Location{bottom_, right_};
}

Location Area::lowerLeft() const
{
  return Location{bottom_, left_};
}

bool Area::inBounds(const Location &location) const
{
  return location.latitude() <= top_ && location.latitude() >= bottom_ &&
         location.longitude() <= right_ && location.longitude() >= left_;
}

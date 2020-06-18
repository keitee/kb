#ifndef BRIDGE_PATTERN_CON_H
#define BRIDGE_PATTERN_CON_H

#include "bridge_p.h"
#include <string>

class XWindowImpl : public WindowImpl
{
private:
  std::string name_{};

public:
  XWindowImpl(const std::string &name)
      : WindowImpl(name)
      , name_{name}
  {}
  virtual ~XWindowImpl() = default;

  virtual void DrawContentsImpl();
};

#endif //  BRIDGE_PATTERN_CON_H

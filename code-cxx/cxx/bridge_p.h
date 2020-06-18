#ifndef BRIDGE_PATTERN_P_H
#define BRIDGE_PATTERN_P_H

#include <string>

class WindowImpl
{
private:
  std::string name_{};

public:
  WindowImpl(const std::string &name)
      : name_{name}
  {}
  virtual ~WindowImpl() = default;

  virtual void DrawContentsImpl();
};

#endif // BRIDGE_PATTERN_P_H

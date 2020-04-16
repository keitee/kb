#ifndef BRIDGE_PATTERN_H
#define BRIDGE_PATTERN_H

#include <memory>

class WindowImpl;

class Window
{
  private:
    std::shared_ptr<WindowImpl> impl;

  public:
    Window();
    virtual ~Window() = default;

    virtual void DrawContents();
};

#endif // BRIDGE_PATTERN_Hgt

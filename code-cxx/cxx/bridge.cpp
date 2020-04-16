#include "bridge.h"
#include <iostream>
#include <string>

// this is "bridge_p.h"

class WindowImpl
{
  private:
    std::string name_{};

  public:
    WindowImpl(const std::string &name)
        : name_{name}
    {}

    virtual void DrawContentsImpl();
};

void WindowImpl::DrawContentsImpl()
{
  std::cout << "WindowImpl::DrawContentsImpl(): name is " << name_ << " called"
            << std::endl;
}

// interface for clients

Window::Window()
    : impl{std::make_shared<WindowImpl>("impl")}
{}

void Window::DrawContents()
{
  std::cout << "Window::DrawContents() called" << std::endl;
  impl->DrawContentsImpl();
}

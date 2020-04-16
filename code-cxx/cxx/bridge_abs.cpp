#include "bridge.h"
#include "bridge_con.h"
#include "bridge_p.h"
#include <iostream>

// Impl base class

void WindowImpl::DrawContentsImpl()
{
  std::cout << "WindowImpl::DrawContentsImpl(): name is " << name_ << " called"
            << std::endl;
}

// interface for clients

Window::Window()
    : impl{std::make_shared<XWindowImpl>("ximpl")}
{}

void Window::DrawContents()
{
  std::cout << "Window::DrawContents() called" << std::endl;
  impl->DrawContentsImpl();
}

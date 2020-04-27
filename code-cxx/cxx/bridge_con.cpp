#include "bridge_con.h"
#include <iostream>
#include <string>

// Impl sub class

void XWindowImpl::DrawContentsImpl()
{
  std::cout << "XWindowImpl::DrawContentsImpl(): name is " << name_ << " called"
            << std::endl;
}

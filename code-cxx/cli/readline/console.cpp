#include "console.h"
#include <iostream>

Console::Console()
{
  registerCommands_();
}

void Console::registerCommands_()
{
  m_readLine.addCommand("test2",
                        "command for member function",
                        std::bind(&Console::testCommand2,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        nullptr);
}

void Console::runCommand(const std::string &command)
{
  m_readLine.runCommand(command);
}

void Console::addCommand(const char *name,
                         const char *description,
                         handler f,
                         void *data)
{
  m_readLine.addCommand(name, description, f, data);
}

bool Console::testCommand2(const std::string &command, void *)
{
  std::cout << "command " << command << " runs\n";
}

#pragma once

#include "readline.h"

class Console
{
public:
  Console();
  ~Console() = default;

  void runCommand(const std::string &command);
  void
  addCommand(const char *name, const char *description, handler f, void *data);

  bool testCommand2(const std::string &command, void *);

private:
  void registerCommands_();
  ReadLine m_readLine;
};

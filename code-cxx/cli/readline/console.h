#pragma once

#include "readline.h"

class Console
{
public:
  Console();
  ~Console() = default;

  void runCommand(const std::string &command);
  void addCommand(const std::string &name,
                  const std::string &description,
                  handler f,
                  const std::string &args);

  bool testCommand2(const std::string &command,
                    const std::vector<std::string> &);

private:
  void registerCommands_();
  ReadLine m_readLine;
};

#include "scli.h"
#include <iostream>
#include <readline/readline.h>

void CommandHandler::addCommand(const char *name,
                                const char *description,
                                handler f,
                                void *data)
{
  m_commands.emplace_back(HandlerContext(name, description, f, data));
}

void CommandHandler::showCommands()
{
  std::cout << "commands : " << std::endl;

  for (const auto &e : m_commands)
  {
    std::cout << " " << e.name() << " : " << e.description() << std::endl;
  }
}

// return false if input command is empty or cannot find input command in
// commands vector.

bool CommandHandler::handle(const std::string &command)
{
  if (!command.empty())
  {
    for (const auto &e : m_commands)
    {
      if (e.isMatch(command))
        return e.fire(command);
    }
  }

  return false;
}

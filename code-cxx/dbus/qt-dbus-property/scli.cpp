/*=============================================================================
 
*/

#include "scli.h"

void CommandHandler::addCommand(char const *name, char const *description, handler f, void *data)
{
  commands_.push_back(HandlerContext(name, description, f, data));
}

void CommandHandler::showCommands()
{
  std::cout << "Commands : " << std::endl;

  for (auto const &e : commands_)
  {
    std::cout << " " << e.getName() << " : " << e.getDescription() << std::endl;
  }
}

/* 
 * return false to mean "not handled" when cannot find the input command or
 * input is empty
*/

bool CommandHandler::handle(std::string const &command)
{
  if (!command.empty())
  {
    for (auto const &e : commands_)
    {
      if (e.isMatch(command))
          return e.fire(command);
    }
  }

  return false;
}


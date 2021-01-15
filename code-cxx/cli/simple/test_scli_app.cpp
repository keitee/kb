#include "scli.h"
#include <iostream>

static bool helpCommand(std::string const &command, void *data)
{
  CommandHandler *handler = static_cast<CommandHandler *>(data);
  handler->showCommands();
  return true;
}

static bool quitCommand(std::string const &command, void *data)
{
  bool *running = static_cast<bool *>(data);
  *running      = false;
  std::cout << "quit command : " << command << std::endl;
  return true;
}

static bool returnFalse(std::string const &command, void *data)
{
  return false;
}

static bool echoCommand(std::string const &command, void *data)
{
  std::cout << "the entered command : " << command << std::endl;
  return true;
}

int main(int argc, char **argv)
{
  CommandHandler handler;
  bool running{true};

  handler.addCommand("q", "quit", quitCommand, &running);
  handler.addCommand("h", "help", helpCommand, &handler);
  handler.addCommand("return", "check return value", returnFalse, nullptr);
  handler.addCommand("echo", "echo command entered", echoCommand, nullptr);

  while (running)
  {
    // prompt
    std::cout << ":> " << std::flush;

    std::string line;
    if (std::getline(std::cin, line))
    {
      auto ret = handler.handle(line);
      if (!ret)
        std::cout << line << " is not handled" << std::endl;
    }
    else if (std::cin.bad())
    {
      std::cout << "std io error" << std::endl;
      break;
    }
    else if (std::cin.eof())
    {
      std::cout << "std eof" << std::endl;
      break;
    }
  } // while

  return EXIT_SUCCESS;
}

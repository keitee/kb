#include "console.h"
#include <iostream>
#include <iterator>
#include <sstream>

// standard readline include files
#include <readline/history.h>
#include <readline/readline.h>

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
                        "-one -two -three");
}

// parse the command line and
void Console::runCommand(const std::string &line)
{
  if (false == line.empty())
  {
    std::stringstream ss{line};

    // use stream iterator to parse it up
    std::istream_iterator<std::string> beg{ss};
    std::istream_iterator<std::string> end{};
    std::vector<std::string> commands{beg, end};
    std::string command{};

    if (commands.size())
    {
      // for (const auto &c: commands)
      // {
      //   std::cout << "c: " << c << "\n";
      // }
      // std::cout << "\n";

      // command
      command = commands[0];

      // erase the first which is the command and the rest is args
      commands.erase(commands.begin());

      m_readLine.runCommand(command, commands);
    }

    add_history(line.c_str());
  }
}

void Console::addCommand(const std::string &name,
                         const std::string &description,
                         handler f,
                         const std::string &args)
{
  m_readLine.addCommand(name, description, f, args);
}

bool Console::testCommand2(const std::string &command,
                           const std::vector<std::string> &args)
{
  std::cout << __func__ << ": cmds : " << command << " runs\n";

  for (const auto &e : args)
  {
    std::cout << __func__ << ": args : " << e << " runs\n";
  }
}

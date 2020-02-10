#ifndef SCLI_H
#define SCLI_H

#include <functional>
#include <string>
#include <vector>

class CommandHandler
{
public:
  using handler = std::function<bool(const std::string &, void *)>;

private:
  class HandlerContext
  {
  private:
    std::string m_name;
    std::string m_description;
    void *m_data;
    handler m_handler;

  public:
    HandlerContext(const std::string &name,
                   const std::string &description,
                   handler handler,
                   void *data)
        : m_name(name)
        , m_description(description)
        , m_handler(handler)
        , m_data(data)
    {}

    // the input `command` is what user has typed and m_name is the added entry.
    bool isMatch(const std::string &command) const
    {
      // if the command is single word and has no space in it, pos will be
      // npos.
      auto pos = command.find_first_of(" ");

      // command has a space in it and found a word. is it going to be a match?
      if (pos != std::string::npos)
      {
        return (command.substr(0, pos) == m_name);
      }
      else
      {
        // whether command is longer than m_name or m_name is longer, only
        // supports the exect match.

        return (0 == command.compare(0,
                                     (m_name.size() > command.size()
                                        ? m_name.size()
                                        : command.size()),
                                     m_name));
      }
    }

    bool fire(const std::string &command) const
    {
      return m_handler(command, m_data);
    }

    const std::string &name() const { return m_name; }
    const std::string &description() const { return m_description; }
  };

private:
  std::vector<HandlerContext> m_commands;

public:
  CommandHandler() {}

  void
  addCommand(const char *name, const char *description, handler f, void *data);
  void showCommands();
  bool handle(const std::string &command);
};

#endif // SCLI_H

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

using handler =
  std::function<bool(const std::string &, const std::vector<std::string> &)>;

// ReadLinePrivate

class ReadLinePrivate
{
private:
  class Command
  {
  private:
    const std::string m_name{};
    const std::string m_description{};
    handler m_handler{nullptr};
    const std::string m_args{};

  public:
    Command(const std::string &name,
            const std::string &description,
            handler handler,
            const std::string &args)
        : m_name(name)
        , m_description(description)
        , m_handler(handler)
        , m_args(args)
    {}

    /*
    comment out since the input command is trimmed when use stringstream
    parse in console. so supports only exact match.
    
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
    */

    bool fire(const std::string &command,
              const std::vector<std::string> &args) const
    {
      return m_handler(command, args);
    }

    const std::string &name() const { return m_name; }
    const std::string &description() const { return m_description; }
    const std::string &args() const { return m_args; }
  };

private:
  std::vector<Command> m_commands;

private:
  ReadLinePrivate();

  bool helpCommand_(const std::string &command,
                    const std::vector<std::string> &args);

public:
  void addCommand(const std::string &name,
                  const std::string &description,
                  handler f,
                  const std::string &args);

  bool runCommand(const std::string &command,
                  const std::vector<std::string> &args);

  // static ReadLinePrivate *instance();
  static std::shared_ptr<ReadLinePrivate> instance();

  ~ReadLinePrivate();

  // member function
  char *completionGenerator_(const char *text, int state);

  // static functions
  static char *completion_generator_(const char *text, int state);
  static char **completer_(const char *text, int start, int end);
};

// ReadLine

class ReadLine
{
private:
  std::shared_ptr<ReadLinePrivate> m_private;

public:
  ReadLine();
  ~ReadLine();

public:
  void addCommand(const std::string &name,
                  const std::string &description,
                  handler f,
                  const std::string &args);

  void runCommand(const std::string &command,
                  const std::vector<std::string> &args);
};

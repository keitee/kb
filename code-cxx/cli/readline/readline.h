#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

using handler = std::function<bool(const std::string &, void *)>;

// ReadLinePrivate

class ReadLinePrivate
{
private:
  class Command
  {
  private:
    std::string m_name{};
    std::string m_description{};
    void *m_data{nullptr};
    handler m_handler{nullptr};

  public:
    Command(const std::string &name,
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
  std::vector<Command> m_commands;

private:
  ReadLinePrivate();
  bool helpCommand_(const std::string &command, void *);

public:
  void
  addCommand(const char *name, const char *description, handler f, void *data);
  bool runCommand(const std::string &command);

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
  void
  addCommand(const char *name, const char *description, handler f, void *data);
  void runCommand(const std::string &command);
};

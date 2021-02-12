#pragma once

#include "ipoll.h"
#include "pollloop.h"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

// reference code
//
// class IReadLine
// {
// public:
//   virtual ~IReadLine() = default;
//
// public:
//   virtual void quit() const = 0;
//   virtual void printLine(const char *fmt, ...) const
//     __attribute__((format(printf, 2, 3))) = 0;
// };
//
// using handler = std::function<void(const std::shared_ptr<IReadLine> &source,

class ReadLine;

// command handler
using handler = std::function<void(const std::shared_ptr<ReadLine> &source,
                                   const std::vector<std::string> &args)>;

class ReadLine : public IPollSource,
                 public std::enable_shared_from_this<ReadLine>
{
private:
  // NOTE: really necessary? the reference code uses two locks; one for
  // protecting m_commands vector and the other for protecting instance_ between
  // callbacks from readline and signal handler from kernel(?)
  //
  // instance_lock seems necessary since ctrl-c can be pressed any time. lock
  // may be not since adding commands is not done dynamically and the rest is
  // reading operations. remains to be seen
  //
  // std::mutex m_lock{};
  // std::mutex m_instance_lock{}; should be static since used in instance_()

  mutable bool m_quit{};
  mutable std::mutex m_quit_lock{};
  mutable std::condition_variable m_cond{};

  std::shared_ptr<PollLoop> m_pollloop{};

private:
  // note that it is a wrapper and do not have member functions
  struct Command
  {
    const std::string m_name{};
    const std::string m_description{};
    const std::string m_help{};
    handler m_handler{nullptr};
    const std::string m_args{};

    Command(const std::string &name,
            const std::string &description,
            const std::string &help,
            handler handler,
            const std::string &args)
        : m_name(name)
        , m_description(description)
        , m_help(help)
        , m_handler(handler)
        , m_args(args)
    {}
  };

  std::vector<Command> m_commands;

private:
  void initialise_();
  void runCommand_(const std::string &command,
                   const std::vector<std::string> &args);

  // member function
  char *completionGenerator_(const char *text, int state);
  void commandLineHandler_(const char *text);

  // static functions
  static char *completion_generator_(const char *text, int state);
  static char **completer_(const char *text, int start, int end);
  static void commandline_handler_(char *text);

  static void signalHandler(int);

  void quitCommand_(const std::shared_ptr<ReadLine> &readline,
                    const std::vector<std::string> &args);

  void helpCommand_(const std::shared_ptr<ReadLine> &readline,
                    const std::vector<std::string> &args);

public:
  static ReadLine &instance();
  explicit ReadLine();
  ~ReadLine();

  ReadLine(const ReadLine &) = delete;
  ReadLine &operator=(const ReadLine &) = delete;

  // bool isValid() const;
  void addCommand(const std::string &name,
                  const std::string &description,
                  const std::string &help,
                  handler f,
                  const std::string &args);

  void runCommand(int argc, const char **argv);
  void run();
  void quit() const;
  void printLine(const char *fmt, ...) const;

  virtual void process(const std::shared_ptr<IPollLoop> &poll,
                       uint32_t events) override;
};

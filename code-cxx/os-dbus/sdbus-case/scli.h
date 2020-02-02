/*=============================================================================

*/

#ifndef __SCLI_H__
#define __SCLI_H__

#include <functional>
#include <iostream>
#include <string>
#include <vector>

class CommandHandler
{
public:
  CommandHandler() {}

  using handler = std::function<bool(std::string const &, void *)>;

  void
  addCommand(char const *name, char const *description, handler f, void *data);
  void showCommands();
  bool handle(std::string const &command);

private:
  class HandlerContext
  {
  public:
    HandlerContext(std::string const &name,
                   std::string const &description,
                   handler f,
                   void *data)
        : name_(name)
        , description_(description)
        , f_(f)
        , data_(data)
    {}

    bool isMatch(std::string const &command) const
    {
      auto index = command.find_first_of(" ");

      // if command is single word which has no space in it, index is npos

      if (std::string::npos != index)
      {
        return command.substr(0, index) == name_;
      }
      else
      {
        // each HandlerContext has its name in `name_` and compare it to
        // input command in range of [0, size].
        //
        // where size is less<>(name_.size(), command.size()) and this
        // supports that "h" matches to "help".

        return (
          0 == command.compare(0,
                               (name_.size() > command.size() ? name_.size()
                                                              : command.size()),
                               name_));
      }
    }

    bool fire(std::string const &command) const { return f_(command, data_); }

    std::string const &getName() const { return name_; }
    std::string const &getDescription() const { return description_; }

  private:
    std::string name_;
    std::string description_;
    handler f_;
    void *data_;
  };

  std::vector<HandlerContext> commands_;
};

#endif /* __SCLI_H__ */

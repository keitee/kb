#ifndef WORK_H
#define WORK_H

#include <functional>

// work.h, 14/03/2020
// that represents a work item to execute

class Work
{
private:
  std::function<void()> f_;
  int id_;

public:
  static const int DEFAULT_ID{0};

  explicit Work(int id = DEFAULT_ID)
      : id_{id}
      , f_{[] {}}
  {}

  explicit Work(std::function<void()> f, int id = DEFAULT_ID)
      : id_{id}
      , f_{f}
  {}

  // no check on f_ since f_ cannot be null
  void execute() { f_(); }

  int id() const { return id_; }
};

#if 0
// old
// Work, which represents a work item to be queued and executed

class Work
{
    public:
        static const int DefaultId{0};

        // TN: see that cxx-lambda is used
        Work(int id=DefaultId)
            : id_{id}, func_{[]{}} {}

        Work(std::function<void()> func, int id=DefaultId)
            : id_{id}, func_{func} {}

        void execute()
        {
            func_();
        }

        int id() const
        {
            return id_;
        }

    private:
        int id_;
        std::function<void()> func_;
};
#endif

#endif // WORK_H

#ifndef WORK_H
#define WORK_H

#include <functional>

// 
// Work, which represents a work item to be queued and executed
//

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

#endif // WORK_H

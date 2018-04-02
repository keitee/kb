#include <iostream>
#include <set>
#include <vector>
#include <memory>

#include "gtest/gtest.h"

using namespace std;

template <typename T>
void PRINT_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << elem << " ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}


// ={=========================================================================
// cxx-sp-hasptr
// pointerlike and use reference counting.

class HasPtr
{
    public:
        HasPtr(const std::string &s = std::string())
            : ps_(new std::string(s)), use_(new std::size_t(1))
        {
            std::cout << "::HasPtr(): " << *ps_ << " used " << *use_ << std::endl;
        }

        // copy ctor
        HasPtr(const HasPtr &rhs)
            : ps_(rhs.ps_), use_(rhs.use_)
        {
            // increase a counter
            ++*use_;
            std::cout << "::HasPtr(const HasPtr &): " << *ps_ << " used " << *use_ << std::endl;
        }

        // copy assign
        // decrease lhs and increase rhs
        HasPtr& operator=(const HasPtr &rhs)
        {
            // increase first to support cxx-self-assign. ???
            ++*rhs.use_;

            // do dtor's work. when there is no users
            if (--*use_ == 0)
            {
                delete ps_;
                delete use_;
            }

            // do ctor's work
            ps_ = rhs.ps_;
            use_ = rhs.use_;
            std::cout << "::HasPtr &operator=(const HasPtr &): " << *ps_ << " used " << *use_ << std::endl;

            return *this;
        }

        // move ctor
        HasPtr(HasPtr &&rhs)
            : ps_(rhs.ps_), use_(rhs.use_)
        {
            // do not increase a counter
            // ++*use_;
            
            // this why ~HasPtr() has checks
            rhs.ps_ = nullptr;
            rhs.use_ = nullptr;
            std::cout << "::HasPtr(HasPtr &&): " << *ps_ << " used " << *use_ << std::endl;
        }

        // move assign
        HasPtr& operator=(HasPtr &&rhs)
        {
            // cxx-self-assign direct check
            if (this != &rhs)
            {
                if (--*use_ == 0)
                {
                    delete ps_; delete use_;
                }

                ps_ = rhs.ps_; use_ = rhs.use_;

                std::cout << "::HasPtr &operator=(HasPtr &&): " << *ps_ << " used " << *use_ << std::endl;

                rhs.ps_ = nullptr;
                rhs.use_ = nullptr;
            }

            return *this;
        }

        ~HasPtr()
        {
            // when there is no users
            // have to check if use_ is not null since use_ can be null due to
            // move support. Otherwise, cxx-seg-fault. 
            // For copy, works okay without it.
            if (use_ && --*use_ == 0)
            {
                std::cout << "::~HasPtr(): " << *ps_ << " used " << *use_ << std::endl;
                delete ps_; delete use_;

                // cxx-nullptr
                // error: cannot convert ‘size_t* {aka long unsigned int*}’ to ‘std::string* {aka std::basic_string<char>*}’ in assignment
                // rhs.ps_ = rhs.use_ = nullptr;
                ps_ = nullptr;
                use_ = nullptr;
            }
        }

    private:
        // value type
        std::string *ps_;

        //  other member
        // int other_;

        // reference count to track how many shares this
        std::size_t *use_;
};

HasPtr use_ptr(HasPtr hp)
{
    HasPtr ret;
    std::cout << "use_ptr: use copy controls" << std::endl;
    ret = hp;
    return ret;
}

#include <iostream>             // for cout
#include <vector>
#include <string>
#include <memory>
#include <initializer_list>

// g++ -std=c++11 -g t_strblob.cpp

//////////////////////////////////////////////////////////////////////////////
// StrBlob without StrBlobPtr
//
// CPR 455
//
// * show how to implement a new collection type using library container.
// `is-implemented-in-terms-of-model`
//
// * This shows when shared_ptr is useful than own raii since this case don't
// need `copy-controls` and use syn versions but not a problem as it has only
// one member which is shared_ptr.

namespace SB1
{

using namespace std;

class StrBlob
{
    public:
        typedef vector<string>::size_type size_type;

        // interface
        StrBlob() : data_(make_shared<vector<string>>()) {}
        StrBlob(initializer_list<string> il) :
            data_(make_shared<vector<string>>(il)) {}

        size_type size() const { return data_->size(); }
        bool empty() const { return data_->empty(); }

        void push_back(const string &e);
        void pop_back();

        std::string &front();
        std::string &back();

    private:
        shared_ptr<vector<string>> data_;

        // *cpp-except*
        // see that return type has nothing to do with exception.
        void check(size_type index, const string &message) const
        {
            if (index >= data_->size())
                throw out_of_range(message);
        }
};

void StrBlob::push_back(const string &e)
{
    data_->push_back(e);
}

void StrBlob::pop_back()
{
    // check if we calls it on empty vector since calls on empty
    // vector is `undefined` behavior.
    check(0, "pop_back() on empty container");

    // std::vector.pop_back() removes the last but do not return.
    data_->pop_back();
}

std::string &StrBlob::front()
{
    check(0, "front() on empty container");
    return data_->front();
}

std::string &StrBlob::back()
{
    check(0, "back() on empty container");
    return data_->back();
}

} // namespace SB1


// = 01 ======
// size : 4
// front: one
// back : four
// size : 5
// front: one
// back : five
// size : 0
// terminate called after throwing an instance of 'std::out_of_range'
//   what():  front() on empty container
// Aborted

void t_strblob_01()
{
    using namespace std;

    SB1::StrBlob sb{"one", "two", "three", "four"};

    cout << "size : " << sb.size() << endl;
    cout << "front: " << sb.front() << endl;
    cout << "back : " << sb.back() << endl;

    sb.push_back("five");

    cout << "size : " << sb.size() << endl;
    cout << "front: " << sb.front() << endl;
    cout << "back : " << sb.back() << endl;

    SB1::StrBlob s2;

    cout << "size : " << s2.size() << endl;
    cout << "front: " << s2.front() << endl;
    cout << "back : " << s2.back() << endl;
}


// = 02 ======
// b1 size: 1
// b1 size: 4
// b2 size: 4
// b1 size: 4

void t_strblob_02()
{
    SB1::StrBlob b1{"b1"};
    {
        std::cout << "b1 size: " << b1.size() << std::endl;

        SB1::StrBlob b2{ "this", "is", "second blob" };
        b1 = b2;
        b2.push_back(". really?");

        std::cout << "b1 size: " << b1.size() << std::endl;
        std::cout << "b2 size: " << b2.size() << std::endl;
    }

    std::cout << "b1 size: " << b1.size() << std::endl;
}

//////////////////////////////////////////////////////////////////////////////
// StrBlob with StrBlobPtr

// namespace SB2
// {

using namespace std;

// forward declaration
// *cpp-incomplete-type* issue decide which class comes first.
class StrBlobPtr;

class StrBlob
{
    // friend declaration
    friend class StrBlobPtr;

    public:
        typedef vector<string>::size_type size_type;

        // interface
        StrBlob() : data_(make_shared<vector<string>>()) {}
        StrBlob(initializer_list<string> il) :
            data_(make_shared<vector<string>>(il)) {}

        size_type size() const { return data_->size(); }
        bool empty() const { return data_->empty(); }

        void push_back(const string &e);
        void pop_back();

        std::string &front();
        std::string &back();

        // iterator support
        // cause *cpp-incomplete-type* issue and can't be defined until
        // StrBlobPtr is defined.
        //
        // StrBlobPtr begin() { return StrBlobPtr(*this); }
        // StrBlobPtr end() { return StrBlobPtr(*this, data_->size()); }

        StrBlobPtr begin();
        StrBlobPtr end();

    private:
        shared_ptr<vector<string>> data_;

        void check(size_type index, const string &message) const
        {
            if (index >= data_->size())
                throw out_of_range(message);
        }
};

void StrBlob::push_back(const string &e)
{
    data_->push_back(e);
}

void StrBlob::pop_back()
{
    // check if we calls it on empty vector since calls on empty
    // vector is `undefined` behavior.
    check(0, "pop_back() on empty container");

    // std::vector.pop_back() removes the last but do not return.
    data_->pop_back();
}

std::string &StrBlob::front()
{
    check(0, "front() on empty container");
    return data_->front();
}

std::string &StrBlob::back()
{
    check(0, "back() on empty container");
    return data_->back();
}

// CPR 473, *cpp-iter*
//
// <- new container class ->  <- companion class like iterator ->
//    StrBlob                    StrBlobPtr
//    - shared_ptr               - weak_ptr
//
//                      vector[]
//                      - underlying container
class StrBlobPtr
{
    friend bool eq(const StrBlobPtr&, const StrBlobPtr&);

    public:
        StrBlobPtr() {}
        // cannot use on const StrBlob since ctor uses non-const.
        StrBlobPtr(StrBlob &blob, size_t size = 0) : blob_(blob.data_), curr_(size) {}

        StrBlobPtr &incr();     // prefix version, ++iter
        StrBlobPtr &decr();     // prefix version, --iter
        string &deref() const;  // *iter

        // CPR 566, *cpp-overload-operator-increment-decrement*
        //
        // * No language requirement but prefer to be a member since change the
        // state of the object.
        //
        // * As for built-in type, there are both prefix and postfix versions.
        
        // prefix versions
        //
        // note: 
        // To be consistent with the built-in operators, the prefix should
        // return a reference

        StrBlobPtr &operator++();
        StrBlobPtr &operator--();

        // postfix versions
        //
        // The postfix version takes an extra parameter which is unused and is
        // to distinguish it from the prefix.
        //
        // note:
        //
        // To be consistent with the built-in operators, the postfix should
        // return the old `value`, not a reference.

        StrBlobPtr operator++(int);
        StrBlobPtr operator--(int);

        // may use const to prevent like, a++++.
        // const StrBlobPtr operator++(int);
        // const StrBlobPtr operator--(int);
        

        // CPR 569, *cpp-overload-operator-member-access*
        //
        // * Used in iterator and smart pointer class
        //
        // * operator arrow must be a member and dereference usually be a member
        // as well.
        
        string &operator*() const;
        string *operator->() const;

    private:
        shared_ptr<vector<string>> check(size_t index, const string &message) const
        {
            // check member differs from the one of StrBlob.
            auto pblob = blob_.lock();
            if (!pblob)
                throw runtime_error("unbound StrBlobPtr");

            if (index >= pblob->size())
                throw out_of_range(message);

            return pblob;
        }

        // current index
        size_t curr_{0};
        weak_ptr<vector<string>> blob_;
};

inline string &StrBlobPtr::deref() const
{
    auto blob = check(curr_, "dereference nonexistent element");
    // [] is higher than *
    return (*blob)[curr_];
}

inline StrBlobPtr &StrBlobPtr::incr()
{
    // if curr_ already points past the end
    check(curr_, "increment past the end");
    ++curr_;
    return *this;
}

inline StrBlobPtr &StrBlobPtr::decr()
{
    // if curr_ is zero, decrementing it will yield an invalid index
    --curr_;
    // check(-1, "decrement before the begin");
    check(curr_, "decrement before the begin");
    return *this;
}

// same as incr()
inline StrBlobPtr &StrBlobPtr::operator++()
{
    // if curr_ already points past the end
    check(curr_, "increment past the end");
    ++curr_;
    return *this;
}

inline StrBlobPtr StrBlobPtr::operator++(int)
{
    StrBlobPtr ret = *this;     // saves the current value
    ++*this;                    // no checks since prefix ++ do.
    return ret;
}

// same as decr()
inline StrBlobPtr &StrBlobPtr::operator--()
{
    // if curr_ is zero, decrementing it will yield an invalid index
    --curr_;
    // check(-1, "decrement before the begin");
    check(curr_, "decrement before the begin");
    return *this;
}

inline StrBlobPtr StrBlobPtr::operator--(int)
{
    StrBlobPtr ret = *this;     // saves the current value
    --*this;                    // no checks since prefix -- do. R associative
    return ret;
}

// same as deref()
// note:
// Although it's const member function since do not change its state, returns a
// reference and client can change through it.

inline string &StrBlobPtr::operator*() const
{
    auto blob = check(curr_, "dereference nonexistent element");
    // [] is higher than *
    return (*blob)[curr_];
}

// note:
//
// StrBlob a1{"hi", "bye", "now"};
// StrBlobPtr p{a1};
//
// *p = "okay";     // can change. assigns ot the fist element in a1
//
// cout << p->size() << endl;       // prints 4, the size of the first element.
// cout << (*p).size() << endl;     // same
//
// The arrow operator never lose its meaning of member access. Depending on the
// type of point:
//
// (*point).mem;                // point is a built-in pointer type
// point.operator()->mem;       // point is an object of class type. iterator
// 
//
// <ex> when item is shared pointer itself
//
// multiset<std::shared_ptr<Quote>, comp> items;
//
// for (auto item = items.cbegin(); item != items.cend();
//  item = items.upper_bound(*item))
// {
//  os << (*item)->isbn() << " occurs, " << endl;
// }
//
// <ex> shared pointer is not built-in array type
//
// For array:
// 
// *(parray+1) is synomym for parray[1]
// 
// the same hold for shared_ptr? No. array is special and not vector.
// 
// shared_ptr<vector<string>> sp(new vector<string>{"one", "two", "three"});
// 
// // OK
// cout << "0: " << (*sp)[0] << endl;
//     
// // OK
// cout << "0: " << *(sp->begin()+0) << endl;
// 
// // ERROR
// cout << "0: " << *(*sp+0) << endl;


inline string *StrBlobPtr::operator->() const
{
    // take address of a string reference
    return & this->operator*();
}


// "casue *cpp-incomplete-type* issue and can't be defined until
// StrBlobPtr is defined." so defined here.
// return a temporary which points either begin or end.
StrBlobPtr StrBlob::begin()
{
    return StrBlobPtr(*this);
}

StrBlobPtr StrBlob::end()
{
    return StrBlobPtr(*this, data_->size());
}

// } // namespace SB2


// named equality operators for StrBlobPtr
inline bool eq(const StrBlobPtr &lhs, const StrBlobPtr &rhs)
{
    auto l = lhs.blob_.lock(), r = rhs.blob_.lock();

    // if the underlying vector is the same
    if (l == r)
        // then they're equal if they're both null, !r
        // or if they point to the same element.
        return (!r || lhs.curr_ == rhs.curr_);
    else
        return false;
}

inline bool neq(const StrBlobPtr &lhs, const StrBlobPtr &rhs)
{
    return !eq(lhs, rhs);
}


// = 03 ======
// b1 size: 1
// b2 size: 4
// b1 size: 4
// a
// an
// the
// about

void t_strblob_03()
{
    StrBlob b1{"b1"};
    {
        std::cout << "b1 size: " << b1.size() << std::endl;

        StrBlob b2{"a", "an", "the"};
        b1 = b2;
        b2.push_back("about");

        std::cout << "b2 size: " << b2.size() << std::endl;
    }

    std::cout << "b1 size: " << b1.size() << std::endl;

    for (auto it = b1.begin(); neq(it, b1.end()); it.incr())
        cout << it.deref() << endl;
}


// = 04 ======
// 10
// 10
// b1 size: 5
// super okay
// bye
// for
// now
// about

void t_strblob_04()
{
    StrBlob a1{"hi", "bye", "for", "now"};
    StrBlobPtr p{a1};
    
    *p = "super okay";     // can change. assigns ot the fist element in a1
    
    cout << p->size() << endl;       // prints 4, the size of the first element.
    cout << (*p).size() << endl;     // same

    a1.push_back("about");

    cout << "b1 size: " << a1.size() << endl;

    for (auto it = a1.begin(); neq(it, a1.end()); ++it)
        cout << *it << endl;
}

int main()
{
    using namespace std;

    cout << "= 04 ======" << endl;
    t_strblob_04();

    // cout << "= 03 ======" << endl;
    // t_strblob_03();

    // cout << "= 02 ======" << endl;
    // t_strblob_02();

    // cout << "= 01 ======" << endl;
    // t_strblob_01();
}

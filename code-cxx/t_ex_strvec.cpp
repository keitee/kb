#include <iostream>
#include <string>
#include <memory>
#include <fstream>
// #include <chrono>
// #include <thread>

// * valuelike copy context
//
class StrVec
{
    public:
        StrVec() {}
        StrVec(std::initializer_list<std::string> il)
        {
            auto vec = alloc_and_copy(il.begin(), il.end());
            element_ = vec.first;
            free_ = cap_ = vec.second;
        }

        // copy controls
        StrVec(const StrVec &rhs)
        {
            auto vec = alloc_and_copy(rhs.begin(), rhs.end());
            element_ = vec.first;
            free_ = cap_ = vec.second;
        }

        StrVec &operator=(const StrVec &rhs)
        {
            // *cxx-except-self-assign*
            auto vec = alloc_and_copy(rhs.begin(), rhs.end());
            free();
            element_ = vec.first;
            free_ = cap_ = vec.second;
            return *this;
        }

        // move controls *cxx-move*
        StrVec(StrVec &&rhs) noexcept
            : element_(rhs.element_), free_(rhs.free_), cap_(rhs.cap_)
        {
            rhs.element_ = rhs.free_ = rhs.cap_ = nullptr;
        }

        StrVec &operator=(StrVec &&rhs) noexcept
        {
            // *cxx-except-self-assign* direct test
            if (this != &rhs)
            {
                // free existing elements
                free();
                element_ = rhs.element_;
                free_ = rhs.free_;
                cap_ = rhs.cap_;
                rhs.element_ = rhs.free_ = rhs.cap_ = nullptr;
            }

            return *this;
        }

        ~StrVec() { free(); }

        void push_back(const std::string &s)
        {
            // need reallocate? Ensure that there is room for another element.
            check_and_alloc();

            // construct a copy of s
            //
            // The first argument to construct must be a pointer to
            // 'unconstructed' space allocated by `allocate()`. 
            //
            // The second argument determine 'which' constructor to use to
            // construct the object in that space. This is string's copy ctor.

            // *cxx-side-effect*
            alloc.construct(free_++, s);
        }

	    // emplace member covered in chapter 16
	    template <class... Args> void emplace_back(Args&&... args)
        {
            check_and_alloc(); // reallocates the StrVec if necessary
            alloc.construct(free_++, std::forward<Args>(args)...);
        }

        size_t size() const { return free_ - element_; }
        size_t capacity() const { return cap_ - element_; }

        std::string *begin() const { return element_; }
        // *cxx-off-the-end*
        std::string *end() const { return free_; }

        // since element_ is pointer
        std::string &operator[](std::size_t n) { return element_[n]; }
        const std::string &operator[](std::size_t n) const { return element_[n]; }

    private:

        // *cxx-static-class-variable*
        static std::allocator<std::string> alloc;

        std::pair<std::string *, std::string *>
            alloc_and_copy(const std::string *begin, const std::string *end)
            {
                // allocate `unconstructed` space to hold elements
                // *cxx-iter-arithmetic*
                auto data = alloc.allocate(end - begin);

                // uninitialized_copy() 
                // * construct copies of given elements in uninitialized space
                // * returns the position after the last initialized element.
                return {data, uninitialized_copy(begin, end, data)};
            }

        // `string is valuelike` and has own memory for those characters. After
        // copying a string, there are two strings.  However, when reallocates
        // copies, there will be only `one user` after the copy and as soon as
        // copy is done, no need to keep the old.

        // So copying the data in these std::strings in the old is unnecessary
        // in this case so StrVec's performance will be much better if can avoid
        // the overhead of allocating and deallocating the char memories for
        // strings themselves each time we reallocate.

        void reallocate()
        {
            auto newcap = size() ? size()*2 : 1;

            auto newspace = alloc.allocate(newcap);

            std::cout << "reallocate: size() " << size() << std::endl;

            auto dest = newspace;
            auto source = element_;

            // move the data from the old memory to the new
            // std::move() returns rvalue, which cause construct() to use string
            // move ctor.
            //
            // seg-fault when have a typo and use element in the loop:
            //
            // for (size_t i = 0; i != size(); ++i)
            //     alloc.construct(dest++, std::move(*element_++));
            //
            // *gdb-debug* bt when use -g
            // Program terminated with signal SIGSEGV, Segmentation fault.
            // #0  0x00007f94aa120113 in std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(std::string&&) () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
            // (gdb) bt
            // #0  0x00007f94aa120113 in std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(std::string&&) () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
            // #1  0x0000000000401bf7 in __gnu_cxx::new_allocator<std::string>::construct<std::string<std::string> > (this=0x602cd1 <StrVec::alloc>, __p=0xa53000) at /usr/include/c++/4.9/ext/new_allocator.h:120
            // #2  0x0000000000401932 in StrVec::reallocate (this=0x7ffdc9026bc0) at t_ex_strvec.cpp:117
            // #3  0x0000000000401aa5 in StrVec::check_and_alloc (this=0x7ffdc9026bc0) at t_ex_strvec.cpp:147
            // #4  0x000000000040165e in StrVec::push_back (this=0x7ffdc9026bc0, s="two") at t_ex_strvec.cpp:44
            // #5  0x000000000040133e in main () at t_ex_strvec.cpp:198
            //
            // when not use -g
            // (gdb) bt
            // #0  0x00007f3348f5c113 in std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(std::string&&) () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
            // #1  0x0000000000401bf7 in void __gnu_cxx::new_allocator<std::string>::construct<std::string, std::string>(std::string*, std::string&&) ()
            // #2  0x0000000000401932 in StrVec::reallocate() ()
            // #3  0x0000000000401aa5 in StrVec::check_and_alloc() ()
            // #4  0x000000000040165e in StrVec::push_back(std::string const&) ()
            // #5  0x000000000040133e in main ()
            //
            // How to debug? See that uses 'construct' and gdb is useful to see
            // what's going on when stepping through. Found out that the loop
            // continues and saw that when add to print i and size().
            //
            // ...
            // i: 16856, size: 18446744073709534761
            // Segmentation fault (core dumped)
            //
            // (gdb) f 2
            // #2  0x0000000000401932 in StrVec::reallocate (this=0x7ffdc9026bc0) at t_ex_strvec.cpp:117
            // 117                     alloc.construct(dest++, std::move(*element_++));
            // (gdb) p i
            // $1 = 16856
            // (gdb) p/u free_-element_
            // $6 = 18446744073709534760
            //
            // Why? Since element_ is member data and keep increasing it, then
            // size() member function would produce negative which turns into
            // big number sicne size() returns size_t, unsigned int.

            for (size_t i = 0; i != size(); ++i)
            {
                // std::cout << "i: " << i << ", size: " << size() << std::endl;
                alloc.construct(dest++, std::move(*source++));
            }

            // std::cout.flush();
            // std::this_thread::sleep_for(std::chrono::seconds{5});

            // to point the new space
            element_ = newspace;
            free_ = dest;
            cap_ = element_ + newcap;
        }

        void free()
        {
            // may not pass deallocate() a nullptr.
            if (element_)
            {
                // move backward and use "--p" to delete [element_, free)
                // call type dtor, which is string dtor.
                for (auto p = free_; p != element_; /* empty */ )
                    alloc.destroy(--p);

                alloc.deallocate(element_, cap_ - element_);
            }
        }

        void check_and_alloc()
		{ 
            std::cout << "size: " << size() << ", cap: " << capacity() << std::endl;
            if (size() == capacity()) reallocate(); 
        }

        std::string *element_{nullptr};
        std::string *free_{nullptr};
        std::string *cap_{nullptr};
};


// *cxx-static-class-variable*
std::allocator<std::string> StrVec::alloc;


void print(const StrVec &svec)
{
    // since element type, string, supports stream output
    for (auto e : svec)
        std::cout << e << " ";
    std::cout << std::endl;
}

StrVec getVec(std::istream &is)
{
    StrVec svec;
    std::string s;

    while (is >> s)
        svec.push_back(s);

    // this is where move is necessary
    return svec;
}


// None two three
// size: 0, cap: 0
// reallocate: size() 0
// size: 1, cap: 1
// reallocate: size() 1
// one two
// sv size: 3
// svec size: 2
// sv size: 2
// svec size: 0

int main()
{
	StrVec sv = {"one", "two", "three"};

	// run `the string empty funciton` on the first element in sv
	if (!sv[0].empty()) 
		sv[0] = "None"; // assign a new value to the first string 

    print(sv);

    StrVec svec;
    svec.push_back({"one"});
    svec.push_back({"two"});
    print(svec);

    std::cout << "sv size: " << sv.size() << std::endl;
    std::cout << "svec size: " << svec.size() << std::endl;

    sv = std::move(svec);

    std::cout << "sv size: " << sv.size() << std::endl;
    std::cout << "svec size: " << svec.size() << std::endl;

// GCC_4_7_0/13$ more data/storyDataFile
// Alice Emma has long flowing red hair.
// Her Daddy says when the wind blows
// through her hair, it looks almost alive,
// like a fiery bird in flight.
// A beautiful fiery bird, he tells her,
// magical but untamed.
// "Daddy, shush, there is no such thing,"
// she tells him, at the same time wanting
// him to tell her more.
// Shyly, she asks, "I mean, Daddy, is there?"
//
//	// we'll call getVec a couple of times
//	// and will read the same file each time
//	ifstream in("data/storyDataFile");
//	StrVec svec = getVec(in);
//	print(svec);
//	in.close();
//
//	cout << "copy " << svec.size() << endl;
//	auto svec2 = svec;
//	print(svec2);
//
//	cout << "assign" << endl;
//	StrVec svec3;
//	svec3 = svec2;
//	print(svec3);
//
//	StrVec v1, v2;
//	v1 = v2;                   // v2 is an lvalue; copy assignment
//
//	in.open("data/storyDataFile");
//	v2 = getVec(in);          // getVec(in) is an rvalue; move assignment
//	in.close();
//
//	StrVec vec;  // empty StrVec
//	string s = "some string or another";
//	vec.push_back(s);      // calls push_back(const string&)
//	vec.push_back("done"); // calls push_back(string&&)
//
//	// emplace member covered in chpater 16
//	s = "the end";
//	vec.emplace_back(10, 'c'); // adds cccccccccc as a new last element
//	vec.emplace_back(s);  // uses the string copy constructor
//	string s1 = "the beginning", s2 = s;
//	vec.emplace_back(s1 + s2); // uses the move constructor

	return 0;
}

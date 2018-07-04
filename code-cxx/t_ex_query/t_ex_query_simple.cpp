#include <iostream>
#include <memory>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

// *cxx-design*
// When design a class, can be helpful to write programs using the class before
// implementing the members. That way, can see whether the class has the
// `operation` we need. A good way to start the design of a program is to list
// the program's operations. Again, what operation we need helps us see what
// `data structure` we will need.
//
// {
//     std::ifstream ifs("README", std::ifstream::in);
// 
//     TextQuery tq(ifs);
// 
//     while (true)
//     {
//         cout << "pls enter the word to search or q to quit: ";
// 
//         string word;
// 
//         // if see condition to quit
//         if (!(cin >> word) || s == "q") break;
// 
//         // run query and print the result
//         print(cout, tq.query(word)) << endl;
//     }
// 
//     ifs.close();
// }
//
// *TN* This ex is more than simple grep since simple grep is one-pass but this
// do mutiple-pass. So needs more structures.
//
// *TN* Search a given word whenever a user runs a query? If built a structure
// which has a word and where that word was shown then no need to search a whole
// file every time. So use a map<word, set<lines>>
//
// *TN* What if have to return the result from query()? Since the result is not
// simple due to more structures used, need to pack them to return. Like
// iterator, pack them as a class. So needs to make it sharable, not a member
// and use shared_ptr on file and set.
//
// The hard part in designing query function is deciding what the query function
// return. The easiest way to return "all" those data is to define a second
// class.

class QueryResult
{
    using lineno = std::size_t;

    friend std::ostream &print(std::ostream &os, const QueryResult &result);

    public:
        QueryResult(const std::string &word,
                std::shared_ptr<std::vector<std::string>> input_file,
                std::shared_ptr<std::set<lineno>> lines)
            : word_(word), input_file_(input_file), lines_(lines)
        {}

    private:
        std::string word_;

        // vector<line> file;
        std::shared_ptr<std::vector<std::string>> input_file_;

        // shared_ptr<set<lineno>> lines_;
        std::shared_ptr<std::set<lineno>> lines_;
};


// utility
std::string make_plural(size_t count, const std::string &message, const std::string &ending)
{
    return (count > 1) ? message + ending : message;
}


class TextQuery
{
    using lineno = std::size_t;

    public:
        // *TN* have to make empty vector here and otherwise access nullptr in
        // ctor.
        TextQuery(std::istream &input)
            : input_file_(std::make_shared<std::vector<std::string>>())
        {
            std::string line{};

            while (getline(input, line))
            {
                std::stringstream ssline{line};
                std::string word;
                    
                input_file_->push_back(line);

                // can use a varialbe and increase it in the loop
                lineno current_line = input_file_->size()-1;

                // *cxx-stringstream* parse up a line
                while (ssline >> word)
                {
                    auto &lines = word_map_[word];

                    // if a word is not in the map, that is to be seen in the
                    // first time, the second is default inicialized shared_ptt,
                    // which is null. Have to create a empty set.

                    if (!lines)
                        lines = std::make_shared<std::set<lineno>>();

                    lines->insert(current_line);
                }
            }
        }

        QueryResult query(const std::string &word) const
        {
            // *TN* How to handle the case when not found? Use empty set rather
            // then nullptr since print() do not have to handle a special case.
            static auto notfound = std::make_shared<std::set<lineno>>();

            // *TN* should not use [] since it can add a new one
            auto pos = word_map_.find(word);

            if (pos != word_map_.end())
                return QueryResult(word, input_file_, pos->second);
            else
                return QueryResult(word, input_file_, notfound);
        }

        // when not use QueryResult and do the same as print().
        void query_and_print(const std::string &word) const
        {
            auto pos = word_map_.find(word);

            if (pos != word_map_.end())
            {
                // *cxx-map-find*
                std::cout << word << " occurs " << pos->second->size() << " "
                    << make_plural(pos->second->size(), "time", "s") << std::endl;

                for (auto e : *pos->second)
                    std::cout << "(line " << e+1 << ") " << (*input_file_)[e] << std::endl;
            }
            else
                std::cout << word << " not found" << std::endl;
        }

    private:
        // vector<line> file;
        std::shared_ptr<std::vector<std::string>> input_file_;

        // *TN* map is itself not a shared_ptr
        // map<word, shared_ptr<set<lineno>>> word_map;
        std::map<std::string, std::shared_ptr<std::set<lineno>>> word_map_;
};

// *cxx-interface* part of interface
// *TN* return ostream since to give a user to control output
std::ostream &print(std::ostream &os, const QueryResult &result)
{
    os << result.word_ << " occurs " << result.lines_->size() << " "
        << make_plural(result.lines_->size(), "time", "s") << std::endl;

    // since it is a vector, can use *cxx-iter*arithmetic*:
    // 
    // for (auto e : *(pos->second))
    //     cout << "(line " << e + 1 << ") " << *(result_file_->begin()+e) << endl;

    for (auto e : *result.lines_)
        os << "(line " << e+1 << ") " << (*result.input_file_)[e] << std::endl;

    // *TN* if miss out this line, gets *seg-fault* and the trace is:
    //
    // ostream &print(ostream &os, const QueryResult &result)
    // {
    //      ...
    //      return os;
    // }
    //
    // Program terminated with signal SIGSEGV, Segmentation fault.
    // #0  0x00007f84dd7af05c in std::basic_ostream<char, std::char_traits<char> >& std::endl<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&) ()
    //    from /usr/lib/x86_64-linux-gnu/libstdc++.so.6

    return os;
}


// *cxx-simple-grep*
// = 01 ======
// pls enter the word to search or q to quit: GNU
// GNU occurs 10 times
// (line 1) This directory contains the sources of the GNU C Library.
// (line 4) The GNU C Library is the standard system C library for all GNU systems,
// (line 5) and is an important part of what makes up a GNU system.  It provides the
// (line 14) The GNU C Library implements much of the POSIX.1 functionality in the
// (line 17) incorporated into an official GNU C Library release.
// (line 19) When working with Linux kernels, this version of the GNU C Library
// (line 25) The GNU C Library supports these configurations for using Linux kernels:
// (line 53) the GNU C Library.  You might also consider reading the WWW pages for
// (line 56) The GNU C Library is (almost) completely documented by the Texinfo manual
// (line 69) The GNU C Library is free software.  See the file COPYING.LIB for copying

void t_query_01()
{
    using namespace std;

    std::ifstream ifs("README", std::ifstream::in);

    TextQuery tq(ifs);

    cout << "pls enter the word to search or q to quit: ";

    string word;

    // if see condition to quit
    if (!(cin >> word)) return;

    // run query and print the result
    tq.query_and_print(word);
}


// = 02 ======
// pls enter the word to search or q to quit: GNU
// GNU occurs 10 times
// (line 1) This directory contains the sources of the GNU C Library.
// (line 4) The GNU C Library is the standard system C library for all GNU systems,
// (line 5) and is an important part of what makes up a GNU system.  It provides the
// (line 14) The GNU C Library implements much of the POSIX.1 functionality in the
// (line 17) incorporated into an official GNU C Library release.
// (line 19) When working with Linux kernels, this version of the GNU C Library
// (line 25) The GNU C Library supports these configurations for using Linux kernels:
// (line 53) the GNU C Library.  You might also consider reading the WWW pages for
// (line 56) The GNU C Library is (almost) completely documented by the Texinfo manual
// (line 69) The GNU C Library is free software.  See the file COPYING.LIB for copying
// 
// pls enter the word to search or q to quit: q

void t_query_02()
{
    using namespace std;

    std::ifstream ifs("README", std::ifstream::in);

    TextQuery tq(ifs);

    while (true)
    {
        cout << "pls enter the word to search or q to quit: ";

        string word;

        // if see condition to quit
        if (!(cin >> word) || word == "q") break;

        // run query and print the result
        print(cout, tq.query(word)) << endl;
    }

    ifs.close();
}
 

// *cxx-simple-grep*
// = 03 ======
// pls enter the word to search or q to quit: GNU
// GNU occurs 10 times
// (line 1) This directory contains the sources of the GNU C Library.
// (line 4) The GNU C Library is the standard system C library for all GNU systems,
// (line 5) and is an important part of what makes up a GNU system.  It provides the
// (line 14) The GNU C Library implements much of the POSIX.1 functionality in the
// (line 17) incorporated into an official GNU C Library release.
// (line 19) When working with Linux kernels, this version of the GNU C Library
// (line 25) The GNU C Library supports these configurations for using Linux kernels:
// (line 53) the GNU C Library.  You might also consider reading the WWW pages for
// (line 56) The GNU C Library is (almost) completely documented by the Texinfo manual
// (line 69) The GNU C Library is free software.  See the file COPYING.LIB for copying

// no iteraction version
void t_query_03()
{
    using namespace std;

    std::ifstream ifs("README", std::ifstream::in);

    TextQuery tq(ifs);

    // run query and print the result
    tq.query_and_print("GNU");
}

int main()
{
    using namespace std;

    cout << "= 03 ======" << endl;
    t_query_03();

    // cout << "= 02 ======" << endl;
    // t_query_02();

    // cout << "= 01 ======" << endl;
    // t_query_01();
}

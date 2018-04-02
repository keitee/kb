#include <iostream>
#include <memory>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <iterator>     // for inserter
#include <algorithm>

// g++ -std=c++11 t_ex_xxx.cpp

// Have to use cleanup_str() to see matches on "hair." or "hair,", which uses
// *cxx-ispunct* ispunct that in any case they are isgraph but not isalnum.
//
// (1) Alice Emma has long flowing red hair. 
// (2) Her Daddy says when the wind blows 
// (3) through her hair, it looks almost alive, 
// (4) like a fiery bird in flight. 
// (5) A beautiful fiery bird, he tells her, 
// (6) magical but untamed. 
// (7) "Daddy, shush, there is no such thing," 
// (8) she tells him, at the same time wanting 
// (9) him to tell her more.
// (10) Shyly, she asks, "I mean, Daddy, is there?"


//////////////////////////////////////////////////////////////////////////////
// simple example
class QueryResult
{
    using lineno = std::size_t;

    // to support extended example
    using line_iter = std::set<lineno>::const_iterator;

    friend std::ostream &print(std::ostream &os, const QueryResult &result);

    public:
        QueryResult(const std::string &word,
                std::shared_ptr<std::vector<std::string>> input_file,
                std::shared_ptr<std::set<lineno>> lines)
            : word_(word), input_file_(input_file), lines_(lines)
        {}

        std::shared_ptr<std::vector<std::string>> get_file() 
        { return input_file_; }

        line_iter begin() const { return lines_->cbegin(); }
        line_iter end() const   { return lines_->cend(); }

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
                    // not covered in the book -- cleanup_str removes
                    // punctuation and converts all text to lowercase so that
                    // the queries operate in a case insensitive manner
                    word = cleanup_str(word);

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
        std::string cleanup_str(const std::string &word)
        {
            std::string ret;
            for (auto it = word.begin(); it != word.end(); ++it)
            {
                // if (!ispunct(*it))
                //     ret += tolower(*it);

                // to keep the case
                if (!ispunct(*it))
                    ret += *it;
            }
            return ret;
        }

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
}


//////////////////////////////////////////////////////////////////////////////
// extended example

// *TN*
// Query q = Query("hair");
//
// builds a chain and each operand(Query) has sp which is a cheap and leak safe.
//
// Q q -> NQ -> (Q -> WQ)     () has pimpl relationship
//
// and pss TextQuery
//
// q.eval(tq);
//
//
// Query q = Query("fiery") & Query("bird") | Query("wind");
//
// q -> OQ  -> AQ   -> Q-> WQ
//                  -> Q-> WQ
//          -> Q -> WQ
//
// *TN*
// QueryBase tree are not intended to be used by user so no public interface;
// all private and protected dtor. Instead, use friend between Query and
// QueryBase tree. Also private ctor as well.

class QueryBase
{
    using lineno = std::size_t;

    // friend
    friend class Query;

    protected:
        virtual ~QueryBase() = default;

    private:
        virtual QueryResult eval(const TextQuery &) const = 0;
        virtual std::string rep() const = 0;
};

// *cxx-pattern-interface* *cxx-pattern-pimpl*
class Query
{
    // friend
    friend Query operator~(const Query &operand);
    friend Query operator&(const Query &lhs, const Query &rhs);
    friend Query operator|(const Query &lhs, const Query &rhs);
    
    public:
        Query(const std::string &word);

        // *cxx-interface-class*
        // * these are non-virtual calls which makes virtual calls via pimpl.
        // *cxx-override-access-control*

        // runs a query, which is heart of this system.
        // need QueryBase due to *cxx-incomplete-type* to call function.
        QueryResult eval(const TextQuery &t) const
        { return qimpl_->eval(t); }

        // return string representation of a query
        std::string rep() const
        { return qimpl_->rep(); }

    private:
        Query(std::shared_ptr<QueryBase> query) : qimpl_(query) {}

        std::shared_ptr<QueryBase> qimpl_;
};


class WordQuery : public QueryBase
{
    // friend
    friend class Query;

    WordQuery(const std::string &word) : query_(word)
    {
        std::cout << "WQ::WQ(" << word << ")" << std::endl;
    }

    QueryResult eval(const TextQuery &tq) const
    { return tq.query(query_); }

    std::string rep() const
    { return query_; }

    std::string query_;
};

class NotQuery : public QueryBase
{
    friend Query operator~(const Query &operand);

    NotQuery(const Query &q) : rhs_(q)
    {
        std::cout << "NQ::NQ(" << rep() << ")" << std::endl;
    }

    QueryResult eval(const TextQuery &tq) const
    { 
        auto result_lines = std::make_shared<std::set<size_t>>();

        // return set which has a query word and begin/end represents lines
        // which has this word.
        auto query_result = rhs_.eval(tq);
        auto it = query_result.begin(), eos = query_result.end();
        
        if (it == eos)
            std::cout << "QueryResult::eval: it == eos" << std::endl;

        // build a new set which do not have this word
        auto file_size = query_result.get_file()->size();
        for (size_t i = 0; i < file_size; ++i)
        {
            // if not found a line in the line set
            if (it == eos || *it != i)
                result_lines->insert(i);
            // if found
            else if (it != eos)
                ++it;

            // *cxx-undefined*
            // Can we do this? No since it blocks when do ++it and call trace
            // is below. This seems that increasing iterator which is past 
            // off-the-end is a cause.
            //
            // // if not found a line in the line set
            // if (*it != i)
            //     result_lines->insert(i);
            // else
            //     ++it;

            // (gdb) bt
            // #0  0x00007ffff7b466e0 in std::_Rb_tree_increment(std::_Rb_tree_node_base const*) () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
            // #1  0x0000000000403c65 in std::_Rb_tree_const_iterator<unsigned long>::operator++ (this=0x7fffffffdd60) at /usr/include/c++/4.9/bits/stl_tree.h:283
            // #2  0x00000000004032ea in NotQuery::eval (this=0x614f40, tq=...) at t_ex_query_extend.cpp:343
            // #3  0x0000000000402d69 in Query::eval (this=0x7fffffffe090, t=...) at t_ex_query_extend.cpp:277
            // #4  0x0000000000402145 in t_query_03 () at t_ex_query_extend.cpp:396
            // #5  0x000000000040225c in main () at t_ex_query_extend.cpp:406
        }

        return QueryResult(rep(), query_result.get_file(), result_lines);
    }

    std::string rep() const
    { return "~(" + rhs_.rep() + ")"; }

    // *TN*
    // the operand(s) can be an object of any of the concrete classes derived
    // from QueryBase: A NotQuery can be applied to any, or another NotQuery, or
    // compound query. To allow this flexibility, the operands must be stored as
    // pointers to QueryBase. That way we can bind the pointer to whichever
    // concrete class we need.
    //
    // However, rather than using simple pointer, will use Query object. Just as
    // user code is simplified by using the interface class, we can simplify our
    // own class by using the same class.
    //
    // Q: really simplified?

    Query rhs_;
};


// *cxx-abc* since do not override eval().
class BinaryQuery : public QueryBase
{
    protected:
        using lineno = std::size_t;

        BinaryQuery(const Query &lhs, const Query &rhs, std::string op):
            lhs_(lhs), rhs_(rhs), op_(op) {}

        std::string rep() const
        { return "(" + lhs_.rep() + " " + op_ + " " + rhs_.rep() + ")"; }

        Query lhs_, rhs_;
        std::string op_;
};

class AndQuery : public BinaryQuery
{
    friend Query operator&(const Query &lhs, const Query &rhs);

    AndQuery(const Query &lhs, const Query &rhs):
        BinaryQuery(lhs, rhs, "&") 
    {
        std::cout << "AQ::AQ(" << rep() << ")" << std::endl;
    }

    QueryResult eval(const TextQuery &tq) const
    {
        auto result_lines = std::make_shared<std::set<lineno>>();

        auto lhs_result = lhs_.eval(tq), rhs_result = rhs_.eval(tq);

        // *algo-set-intersection* *cxx-set-and*
        // *TN"
        // std::inserter(result_lines, ...) cause compile errors.

        std::set_intersection(
                lhs_result.begin(), lhs_result.end(),
                rhs_result.begin(), rhs_result.end(),
                std::inserter(*result_lines, result_lines->begin()));

        return QueryResult(rep(), lhs_result.get_file(), result_lines);
    }
};

class OrQuery : public BinaryQuery
{
    friend Query operator|(const Query &lhs, const Query &rhs);

    OrQuery(const Query &lhs, const Query &rhs):
        BinaryQuery(lhs, rhs, "|")
    {
        std::cout << "OQ::OQ(" << rep() << ")" << std::endl;
    }

    QueryResult eval(const TextQuery &tq) const
    {
        auto lhs_result = lhs_.eval(tq), rhs_result = rhs_.eval(tq);

        // *cxx-set-insert* *cxx-set-or*
        auto result_lines = std::make_shared<std::set<lineno>>(
                lhs_result.begin(), lhs_result.end());
        result_lines->insert(rhs_result.begin(), rhs_result.end());

        return QueryResult(rep(), lhs_result.get_file(), result_lines);
    }
};


// implementations
inline Query::Query(const std::string &word) :  qimpl_(new WordQuery(word)) 
{
    std::cout << "Q::Q(" << word << ")" << std::endl;
}

inline Query operator~(const Query &operand)
{
    // *TN* use Query ctor so have to have friend relationship with Query.
    return std::shared_ptr<QueryBase>(new NotQuery(operand));
}

inline Query operator&(const Query &lhs, const Query &rhs)
{
    return std::shared_ptr<QueryBase>(new AndQuery(lhs, rhs));
}

inline Query operator|(const Query &lhs, const Query &rhs)
{
    return std::shared_ptr<QueryBase>(new OrQuery(lhs, rhs));
}

std::ostream &operator<<(std::ostream &os, const Query &query)
{
    return os << query.rep();
}


//////////////////////////////////////////////////////////////////////////////
// example

// = 03 ======
// ~Query("hair")
// WQ::WQ(hair)
// Q::Q(hair)
// NQ::NQ(~(hair))
// ~(hair) occurs 8 times
// (line 2) Her Daddy says when the wind blows
// (line 4) like a fiery bird in flight.
// (line 5) A beautiful fiery bird, he tells her,
// (line 6) magical but untamed.
// (line 7) "Daddy, shush, there is no such thing,"
// (line 8) she tells him, at the same time wanting
// (line 9) him to tell her more.
// (line 10) Shyly, she asks, "I mean, Daddy, is there?"

void t_query_03()
{
    using namespace std;

    std::ifstream ifs("t_query.dat", std::ifstream::in);

    TextQuery tq(ifs);

    // cout << "~Query(\"hair\")" << endl;
    cout << R"(~Query("hair"))" << endl;
    Query q = ~Query("hair");

    const auto result = q.eval(tq);

    print(cout, result) << endl;
}

int t_query_04()
{
    using namespace std;

    std::ifstream ifs("t_query.dat", std::ifstream::in);

    TextQuery tq(ifs);


    // WQ::WQ(hair)
    // Q::Q(hair)
    //
    // Executing Query for: hair
    // hair occurs 2 times
    //         (line 1) Alice Emma has long flowing red hair.
    //         (line 3) through her hair, it looks almost alive,
    Query q0 = Query("hair");

    cout << "\nExecuting Query for: " << q0 << endl;

    const auto results0 = q0.eval(tq);

    print(cout, results0) << endl;


    // WQ::WQ(Alice)
    // Q::Q(Alice)
    //
    // Executing Query for: Alice
    // Alice occurs 1 time
    //         (line 1) Alice Emma has long flowing red hair.
    Query q1 = Query("Alice");
 
    cout << "\nExecuting Query for: " << q1 << endl;

    const auto results1 = q1.eval(tq);

    print(cout, results1) << endl;


    // WQ::WQ(Alice)
    // Q::Q(Alice)
    // NQ::NQ(~(Alice))
    //
    // Executing Query for: ~(Alice)
    // ~(Alice) occurs 9 times
    //         (line 2) Her Daddy says when the wind blows
    //         (line 3) through her hair, it looks almost alive,
    //         (line 4) like a fiery bird in flight.
    //         (line 5) A beautiful fiery bird, he tells her,
    //         (line 6) magical but untamed.
    //         (line 7) "Daddy, shush, there is no such thing,"
    //         (line 8) she tells him, at the same time wanting
    //         (line 9) him to tell her more.
    //         (line 10) Shyly, she asks, "I mean, Daddy, is there?"
    Query q2 = ~Query("Alice");
 
    cout << "\nExecuting Query for: " << q2 << endl;

    const auto results2 = q2.eval(tq);

    print(cout, results2) << endl;


    // WQ::WQ(Alice)
    // Q::Q(Alice)
    // WQ::WQ(hair)
    // Q::Q(hair)
    // OQ::OQ((hair | Alice))
    //
    // Executing Query for: (hair | Alice)
    // (hair | Alice) occurs 2 times
    //         (line 1) Alice Emma has long flowing red hair.
    //         (line 3) through her hair, it looks almost alive,
    Query q3 = Query("hair") | Query("Alice");
 
    cout << "\nExecuting Query for: " << q3 << endl;

    const auto results3 = q3.eval(tq);

    print(cout, results3) << endl;


    // WQ::WQ(Alice)
    // Q::Q(Alice)
    // WQ::WQ(hair)
    // Q::Q(hair)
    // AQ::AQ((hair & Alice))
    // 
    // Executing Query for: (hair & Alice)
    // (hair & Alice) occurs 1 time
    //         (line 1) Alice Emma has long flowing red hair.
    Query q4 = Query("hair") & Query("Alice");
 
    cout << "\nExecuting Query for: " << q4 << endl;

    const auto results4 = q4.eval(tq);

    print(cout, results4) << endl;


    // WQ::WQ(wind)
    // Q::Q(wind)
    // WQ::WQ(bird)
    // Q::Q(bird)
    // WQ::WQ(fiery)
    // Q::Q(fiery)
    // AQ::AQ((fiery & bird))
    // OQ::OQ(((fiery & bird) | wind))
    // 
    // Executing Query for: ((fiery & bird) | wind)
    // ((fiery & bird) | wind) occurs 3 times
    //         (line 2) Her Daddy says when the wind blows
    //         (line 4) like a fiery bird in flight.
    //         (line 5) A beautiful fiery bird, he tells her,
    Query q5 = Query("fiery") & Query("bird") | Query("wind");
 
    cout << "\nExecuting Query for: " << q5 << endl;

    const auto results5 = q5.eval(tq);

    print(cout, results5) << endl;
}


int main()
{
    using namespace std;

    // cout << "= 04 ======" << endl;
    // t_query_04();

    cout << "= 03 ======" << endl;
    t_query_03();

    // cout << "= 02 ======" << endl;
    // t_query_02();

    // cout << "= 01 ======" << endl;
    // t_query_01();
}

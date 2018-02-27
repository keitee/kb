#include <iostream>
#include <cstring>
#include <vector>
#include <boost/algorithm/string.hpp>

// g++ -g -std=c++0x t_override.cpp

using namespace std;

int t_string_01()
{
  string message = "this is message";

  cout << message << endl;
}

// cstring.
// return bool if source and target are the same from the end.
// bool strend(char *s, char *t);

bool strend_01(char *s, char *t)
{
    // assume s is bigger than t
    size_t diff = strlen(s) - strlen(t);

    char *psource = s+diff, *ptarget = t;
    while (*psource)
    {
        if (*psource != *ptarget)
            return false;
        
        ++psource, ++ptarget;
    }

    return true;
}

bool strend_02(char *s, char *t)
{
    // assume s is bigger than t. uses parameter
    s += strlen(s) - strlen(t);

    while (*s++ == *t++)
    {
        // if side-effect, +1, points NULL then all matches up.
        if (*s == '\0')
            return true;
    }

    return false;
}


// = 02 ======
// strend(this is first message, ssage) returns 1
// strend(this is first message, xsage) returns 0
// strend(this is first message, ssage) returns 1
// strend(this is first message, xsage) returns 0

int t_string_02()
{
    char s1[] = "this is first message";
    char t1[] = "ssage";
    char t2[] = "xsage";

    cout << "strend(" << s1 << ", " << t1 << ") returns " << strend_01(s1, t1) << endl;
    cout << "strend(" << s1 << ", " << t2 << ") returns " << strend_01(s1, t2) << endl;

    cout << "strend(" << s1 << ", " << t1 << ") returns " << strend_02(s1, t1) << endl;
    cout << "strend(" << s1 << ", " << t2 << ") returns " << strend_02(s1, t2) << endl;
}


// = 03 ======
// size of s1 cstring  : 8
// size of s1 cstring  : 8
// size of s1 cstring  : 1
// size of s2 cstring  : 22
// size of s2 cstring  : 22
// strlen of s2 cstring: 21
// size of string      : 21

int t_string_03()
{
    char *s1 = "this is first message";
    char s2[] = "this is first message";
    cout << "size of s1 cstring  : " << sizeof(s1) << endl;
    cout << "size of s1 cstring  : " << sizeof s1 << endl;
    cout << "size of s1 cstring  : " << sizeof *s1 << endl;
    cout << "size of s2 cstring  : " << sizeof(s2) << endl;
    cout << "size of s2 cstring  : " << sizeof(s2)/sizeof(s2[0]) << endl;
    cout << "strlen of s2 cstring: " << strlen(s2) << endl;

    string s{s1};
    cout << "size of string      : " << s.size() << endl;
}


// = 04 ======
// iv : 10
// id : 10
// cv : A
// cv : 65
// cv : 65

int t_string_04()
{
    string str{"STRING"};
    int iv{10};
    double id{10.0};
    char cv{65};

    cout << "iv : " << iv << endl;
    cout << "id : " << id << endl;
    cout << "cv : " << cv << endl;
    cout << "cv : " << +cv << endl;
    cout << "cv : " << to_string(cv) << endl;

    cout << "str[0] : " << str[0] << endl;
    char ch = str[0];
    cout << "str[0] : " << ch << endl;
}


// = 05 ======
// XXXXXXXXXXXXXXXXXfoo
// XXXXXXXXXXXXXXXXXfoo
// fooXXXXXXXXXXXXXXXXX

int t_string_05()
{
    string s1{"foo"};
    s1.insert(0, 20 - s1.size(), 'X');
    cout << s1 << endl;

    string s2{"foo"};
    s2.insert(s2.begin(), 20 - s2.size(), 'X');
    cout << s2 << endl;

    string s3{"foo"};
    s3.append(20 - s3.size(), 'X');
    cout << s3 << endl;
}

template<typename T>
void pad(T &s, typename T::size_type n, typename T::value_type c)
{
    if (n > s.size())
        s.append(n - s.size(), c);
}

int t_string_05_01()
{
    string s1{"foo"};
    pad(s1, 20, 'X');
    cout << s1 << endl;

    string s3{"foo"};
    s3.append(20 - s3.size(), 'X');
    cout << s3 << endl;

    wstring ws1{L"foo"};
    pad(ws1, 20, 'X');
}

// to trim `some` number of char from the end
void rtrim(std::string &s, char c)
{
    if (s.empty())
        return;

    auto it = s.end();
    for (it; it != s.begin() && *--it == c;)
        ;

    // move it one back since increased in for
    if (*it != c)
        ++it;

    s.erase(it, s.end());
}

// to trim `whitespace` from the end
void rtrimws(std::string &s)
{
    if (s.empty())
        return;

    auto it = s.end();
    for (it; it != s.begin() && isspace(*--it);)
        ;

    // move it one back since increased in for
    ++it;

    s.erase(it, s.end());
}

// = 06 ======
// z
// s: zoo                              , size : 33
// s: zoo, size : 3

int t_string_06()
{
    string s1{"zoo"};
    rtrim(s1, 'o');
    cout << s1 << endl;

    string s2{"zoo                              "};
    cout << "s: " << s2 << ", size : " << s2.size() << endl; 
    rtrimws(s2);
    cout << "s: " << s2 << ", size : " << s2.size() << endl; 
}


// Q: to split the string "Name|Address|Phone" into three separate strings,
// "Name", "Address", and "Phone", with the delimiter removed.

int t_string_07()
{
    //              4       12
    string str{"Name|Address|Phone"};

    auto it = str.find('|');
    cout << "it: " << it << ", " << str.substr(0, it) << endl;

    auto pit = it;

    it = str.find('|', pit+1);
    cout << "it: " << it << ", " << str.substr(pit+1, it-pit-1) << endl;

    pit = it;
    it = str.find('|', pit+1);
    cout << "it: " << it << ", " << str.substr(pit+1, it-pit-1) << endl;
}

void split(const string &s, char c, vector<string> &v)
{
    string::size_type current = 0, next = s.find(c);

    while (next != string::npos)
    {
        v.push_back(s.substr(current, next-current));

        current = ++next;
        next = s.find(c, next);

        if (next == string::npos)
            v.push_back(s.substr(current, next));
    }
}

int t_string_08()
{
    //              4       12
    string str{"Name|Address|Phone"};
    vector<string> sv;

    split(str, '|', sv);

    for (auto &e : sv)
        cout << e << endl;
}

int t_string_09()
{
    //              4       12
    string str{"Name|Address|Phone"};
    vector<string> sv;

    boost::split(sv, str, boost::is_any_of("|"));

    for (auto &e : sv)
        cout << e << endl;
}

//={==========================================================================
class StringTokenizer
{
    public:
        StringTokenizer(const string &s, const char *delim = nullptr )
            : str_(s)
        {
            // set default delimiters
            if (!delim)
                delim_ = " \n\r\t\v\f";
            else
                delim_ = delim;

            begin_ = str_.find_first_not_of(delim_);
            end_ = str_.find_first_of(delim_);
        }

        // return # of tokens
        size_t countTokens()
        {
            // if we already have count calculated
            if (count_)
                return count_;

            size_t token_count{}, pos{};

            for (;;)
            {
                // if there are only delimiters then there are no tokens
                if ((pos = str_.find_first_not_of(delim_, pos)) == string::npos)
                    break;

                pos = str_.find_first_of(delim_, pos);
                ++token_count;

                // okay, see no more delimiter so stop.
                if (pos == string::npos)
                    break;
            }

            return count_ = token_count;
        }

        bool hasMoreTokens()
        { return begin_ != end_; }

        // get the next out of input string
        void nextToken(string &token)
        {
            // char and delimiter. so continue
            if ((begin_ != string::npos) && (end_ != string::npos))
            {
                token = str_.substr(begin_, end_- begin_);

                begin_ = str_.find_first_not_of(delim_, end_);
                end_ = str_.find_first_of(delim_, begin_);
            }
            // chars but no more delimiter
            else if(begin_ != string::npos && end_ == string::npos)
            {
                token = str_.substr(begin_, end_);
                begin_ = string::npos;
            }
            // no char but delimiter
            // else if(begin_ == string::npos && end_ != string::npos)
            // no char and no delimiter
            // else if(begin_ == string::npos && end_ == string::npos)
            else
            {
                // *TN* 
                // no need to set token since begin_ and end_ are state
                // variable. hasMoreTokens() use this to signal user to stop
                // loop.

                token = "";
                begin_ = end_ = string::npos;
            }
        }

    private:
        StringTokenizer() {}    // *cxx-ctor-private*
        size_t begin_{};        // char pos
        size_t end_{};          // delim pos
        size_t count_{};
        string str_{};
        string delim_{};
};


// class StringTokenizer
// {
// public:
//     StringTokenizer(const string &s, const char *delim = nullptr):
//         str_(s), count_(-1), begin_(0), end_(0)
//     {
//         // set default whitespace
//         if (!delim)
//             delim_ = " \f\n\r\t\v";
//         else
//             delim_ = delim;
// 
//         // set to the first token
//         begin_ = str_.find_first_not_of(delim_);
//         end_ = str_.find_first_of(delim_, begin_);
//     }
// 
//     size_t countTokens()
//     {
//         // if we've already counted
//         if (count_ >= 0)
//             return count_;
// 
//         size_t token_count = 0, pos = 0;
// 
//         for (;;)
//         {
//             // if see only delimeters then no need to do more
//             // non-delimeter pos
//             if ((pos = str_.find_first_not_of(delim_, pos)) == string::npos)
//                 break;
// 
//             // okay, saw non-delimeters and search for delimeter
//             // org
//             // delim_pos = str_.find_first_of(delim_, delim_pos+1);
//             pos = str_.find_first_of(delim_, pos);
//             token_count++;
// 
//             // delimeter pos
//             if (pos == string::npos)
//                 break;
//         }
// 
//         return count_ = token_count;
//     }
// 
//     bool hasMoreTokens() { return (begin_ != end_); }
// 
//     // return the current token and update begin/end of the next
//     void nextToken(string &s)
//     {
//         // so there is one token at least
//         if (begin_ != string::npos && end_ != string::npos)
//         {
//             s = str_.substr(begin_, end_ - begin_);
//             begin_ = str_.find_first_not_of(delim_, end_);
//             end_ = str_.find_first_of(delim_, begin_);
//         }
//         // when there is only one token left
//         else if(begin_ != string::npos && end_ == string::npos)
//         {
//             // org
//             // s = str_.substr(begin_, str_.size()-begin_);
//             // begin_ = str_.find_first_not_of(delim_, end_);
//             s = str_.substr(begin_, end_);
//             begin_ = string::npos;
//         }
//     }
// 
// private:
//     StringTokenizer() {};
//     string delim_;
//     string str_;
//     int count_;
//     size_t begin_;
//     size_t end_;
// };

int t_string_10()
{
    //              4       12
    // string str{"Name|Address|Phone"};
    // string str{"Name Address Phone"};
    string str{"Name "};

    StringTokenizer st(str);

    cout << "there are " << st.countTokens() << " tokens." << endl;

    while (st.hasMoreTokens())
    {
        string token{};
        st.nextToken(token);
        cout << "token = " << token << endl;
    }
}

int t_string_11()
{
    //              4       12
    string str{"Name|Address|Phone"};
    vector<string> sv;

    size_t char_pos = 0, delim_pos = 0;

    while (delim_pos != string::npos)
    {
        // delim_pos = str.find(char_pos, '|');
        delim_pos = str.find('|', char_pos);

        sv.push_back(str.substr(char_pos, delim_pos-char_pos));
        
        if (delim_pos != string::npos)
            char_pos = ++delim_pos;
    }

    for (const auto &e : sv)
        cout << "token: " << e << endl;
}


//={==========================================================================
template<typename T>
int countUnique(const T &s)
{
    T chars;

    // for (auto it = s.cbegin(); it != s.cend(); ++it)
    // for (const auto it = s.begin(); it != s.end(); ++it)
    // {
    //     if (chars.find(*it) == T::npos)
    //         chars += *it;
    // }
    for (const auto e : chars)
    {
        if (chars.find(e) == T::npos)
            chars += e;
    }

    return chars.size();
}

int t_string_12()
{
    string s = "Abracadabra";

    cout << countUnique(s) << endl;
}


//={==========================================================================
int main()
{
    cout << "= 12 ======" << endl;
    t_string_12();

    cout << "= 11 ======" << endl;
    t_string_11();

    cout << "= 10 ======" << endl;
    t_string_10();

    cout << "= 09 ======" << endl;
    t_string_09();

    cout << "= 08 ======" << endl;
    t_string_08();

    cout << "= 07 ======" << endl;
    t_string_07();

    // cout << "= 06 ======" << endl;
    // t_string_06();

    // cout << "= 05_01 ======" << endl;
    // t_string_05_01();

    // cout << "= 05 ======" << endl;
    // t_string_05();

    // cout << "= 04 ======" << endl;
    // t_string_04();

    // cout << "= 03 ======" << endl;
    // t_string_03();

    // cout << "= 02 ======" << endl;
    // t_string_02();
}

#include <iostream>
#include <fstream>
#include <vector>

// * Can use argv for MAXSIZE but uses the fixed value for simplicity.
// * See how C++ is simpler than C.

// output:
// (4) Please see http://www.gnu.org/software/libc/bugs.html for bug reporting
// (5) information.  We are now using the Bugzilla system to track all bug reports.
// (6) This web page gives detailed information on how to report bugs properly.
// (7)
// (8) The GNU C Library is free software.  See the file COPYING.LIB for copying
// (9) conditions, and LICENSES for notices about a few contributions that require
// (0) these additional notices to be distributed.  License copyright years may be
// (1) listed using range notation, e.g., 2000-2013, indicating that every year in
// (2) the range, inclusive, is a copyrightable year that would otherwise be listed
// (3) individually.

// no reverse() and push_back() in while
void tail_01()
{
    using namespace std;

    vector<string> svec;
    ifstream ifs{"README", ifstream::in};
    string line;

    // assume that this tail supports 10 lines
    const size_t MAXLINE = 10;

    size_t current_line = 0;

    while (getline(ifs, line))
    {
        if (current_line < MAXLINE)
            svec.push_back(line);
        else
            svec[current_line % MAXLINE] = line;

        ++current_line;
    }

    // print out MAXLINE after the last written
    for (size_t i = 0; i < MAXLINE; ++i)
    {
        current_line %= MAXLINE;

        cout << "(" << current_line << ") " << svec[current_line] << endl;

        ++current_line;
    }
}

// reserve()
void tail_02()
{
    using namespace std;

    vector<string> svec;
    ifstream ifs{"README", ifstream::in};
    string line;

    // assume that this tail supports 10 lines
    const size_t MAXLINE = 10;

    // reads input
    // * makes a vector available by adding empty strings and which simplify code
    //   to read a file.
    svec.reserve(MAXLINE);

    for (size_t i = 0; i < MAXLINE; ++i)
        svec.push_back(string());

    size_t current_line = 0;

    while (getline(ifs, line))
    {
        svec[current_line % MAXLINE] = line;
        ++current_line;
    }

    // print out MAXLINE after the last written
    for (size_t i = 0; i < MAXLINE; ++i)
    {
        current_line %= MAXLINE;

        cout << "(" << current_line << ") " << svec[current_line] << endl;

        ++current_line;
    }
}

// move version
void tail_03()
{
    using namespace std;

    vector<string> svec;
    ifstream ifs{"README", ifstream::in};
    string line;

    // assume that this tail supports 10 lines
    const size_t MAXLINE = 10;

    // reads input
    // * makes a vector available by adding empty strings and which simplify code
    //   to read a file.
    svec.reserve(MAXLINE);

    for (size_t i = 0; i < MAXLINE; ++i)
        svec.push_back(string());

    size_t current_line = 0;

    while (getline(ifs, line))
    {
        svec[current_line % MAXLINE] = std::move(line);
        ++current_line;
    }

    // print out MAXLINE after the last written
    for (size_t i = 0; i < MAXLINE; ++i)
    {
        current_line %= MAXLINE;

        cout << "(" << current_line << ") " << svec[current_line] << endl;

        ++current_line;
    }
}

int main()
{
    using namespace std;

    cout << "========" << endl;
    tail_03();

    cout << "========" << endl;
    tail_02();

    cout << "========" << endl;
    tail_01();
}

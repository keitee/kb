#include "gmock/gmock.h"

#include <iostream>
#include <list>
#include <deque>
#include <fstream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

32. Pascal's triangle

Write a function that prints up to 10 rows of Pascal's triangle to the console.

Pascal's triangle is a construction representing binomial coefficients. The
triangle starts with a row that has a single value of 1. Elements of each row
are constructed by summing the numbers above, to the left and right, and
treating blank entries as 0. Here is an example of the triangle with five rows:

        1
      1   1
    1   2   1
  1   3   3   1
1   4   6   4   1

To print the triangle, we must:

Shift the output position to the right with an appropriate number of spaces, so
that the top is projected on the middle of the triangle's base. Compute each
value by summing the above left and right values. A simpler formula is that for
a row i and column j, each new value x is equal to the previous value of x
multiplied by (i - j) / (j + 1), where x starts at 1.

*/


namespace U32_2018_12_03 {

  // since the formular is given, looks like that tricky thing is to cal spaces
  // to space out the first element
  // - it is about stream
  // - space between column can be fixed as 1.
  //
  // so no more than that. 

} // namespace


// n(row) is 4:
//
// temp(0) = 1 * (0 - 0) / (0 + 1);
// y = 1, maxlen = 0
// 
// temp(1) = 1 * (1 - 0) / (0 + 1);
// y = 1, maxlen = 0
// temp(0) = 1 * (1 - 1) / (1 + 1);
// y = 1, maxlen = 0
// 
// temp(2) = 1 * (2 - 0) / (0 + 1);
// y = 1, maxlen = 0
// temp(1) = 2 * (2 - 1) / (1 + 1);
// y = 2, maxlen = 0
// temp(0) = 1 * (2 - 2) / (2 + 1);
// y = 1, maxlen = 0
// 
// temp(3) = 1 * (3 - 0) / (0 + 1);
// y = 1, maxlen = 0
// temp(3) = 3 * (3 - 1) / (1 + 1);
// y = 3, maxlen = 0
// temp(1) = 3 * (3 - 2) / (2 + 1);
// y = 3, maxlen = 0
// temp(0) = 1 * (3 - 3) / (3 + 1);
// y = 1, maxlen = 0

namespace U32_Text {

  // necessary to cal space between columns 
  unsigned int number_of_digits(unsigned int i)
  {
    return i > 0 ? (int)log10((double)i) + 1 : 1;
  }

  void print_pascal_triangle(const int n)
  {
    for (int i = 0; i < n; ++i)
    {
      auto x = 1;

      // *cxx-string-ctor*
      std::cout << std::string((n - i - 1)*(n / 2), '.');

      for (int j = 0; j <= i; ++j)
      {
        auto y = x;

        // get new x which becomes y so there is no summing and it calculate
        // values of each row using this formular
        x = x * (i - j) / (j + 1);

        // auto temp = x * (i - j) / (j + 1);
        // std::cout << "temp(" << temp << ") = " << x << " * (" << i << " - " << j << ") / (" 
        //   << j << " + 1);" << endl;
        // x = temp;

        auto maxlen = number_of_digits(x) - 1;

        // std::cout << "y = " << y << ", maxlen = " << maxlen << endl;

        std::cout << y << std::string(n - 1 - maxlen - n%2, '.');
      }
      
      std::cout << std::endl;
    }
  }

} // namespace


TEST(U32, Text)
{
  using namespace U32_Text;

  print_pascal_triangle(10);
}


// ={=========================================================================
/*

33. Tabular printing of a list of processes

Suppose you have a snapshot of the list of all processes in a system. The
information for each process includes name, identifier, status (which can be
either running or suspended), account name (under which the process runs),
memory size in bytes, and platform (which can be either 32-bit or 64-bit). Your
task is to write a function that takes such a list of processes and prints them
to the console alphabetically, in tabular format. All columns must be
left-aligned, except for the memory column which must be right-aligned. The
value of the memory size must be displayed in KB. The following is an example of
the output of this function:

chrome.exe      1044   Running    marius.bancila    25180  32-bit
chrome.exe      10100  Running    marius.bancila   227756  32-bit
cmd.exe         512    Running    SYSTEM               48  64-bit
explorer.exe    7108   Running    marius.bancila    29529  64-bit
skype.exe       22456  Suspended  marius.bancila      656  64-bit

{
   using namespace std::string_literals;

   std::vector<procinfo> processes
   {
      {512, "cmd.exe"s, procstatus::running, "SYSTEM"s, 
            148293, platforms::p64bit },
      {1044, "chrome.exe"s, procstatus::running, "marius.bancila"s, 
            25180454, platforms::p32bit},
      {7108, "explorer.exe"s, procstatus::running, "marius.bancila"s,  
            2952943, platforms::p64bit },
      {10100, "chrome.exe"s, procstatus::running, "marius.bancila"s, 
            227756123, platforms::p32bit},
      {22456, "skype.exe"s, procstatus::suspended, "marius.bancila"s, 
            16870123, platforms::p64bit }, 
   };

   print_processes(processes);
}

*/

namespace U33_2018_12_03 {

  enum class procstatus {suspended, running};
  enum class platforms {p32bit, p64bit};

  struct procinfo
  {
    int         id;
    std::string name;
    procstatus  status;
    std::string account;
    size_t      memory;
    platforms   platform;
  };

  void print_processes(const std::vector<procinfo> &proc)
  {
    for (const auto &e : proc)
    {
      cout << setw(20) << left << e.name
        << setw(20) << left << e.id
        << setw(20) << left << (e.status == procstatus::running ? "Running" : "Suspended")
        << setw(20) << left << e.account
        << setw(20) << right << e.memory
        << setw(10) << right << (e.platform == platforms::p64bit ? "64-bit" : "32-bit") << endl;
    }
  }

} // namespace

TEST(U33, 2018_12_03)
{
  using namespace U33_2018_12_03;

   std::vector<procinfo> processes
   {
      {512, "cmd.exe", procstatus::running, "SYSTEM", 
            148293, platforms::p64bit },
      {1044, "chrome.exe", procstatus::running, "marius.bancila", 
            25180454, platforms::p32bit},
      {7108, "explorer.exe", procstatus::running, "marius.bancila",  
            2952943, platforms::p64bit },
      {10100, "chrome.exe", procstatus::running, "marius.bancila", 
            227756123, platforms::p32bit},
      {22456, "skype.exe", procstatus::suspended, "marius.bancila", 
            16870123, platforms::p64bit }, 
   };

   print_processes(processes);
}


// ={=========================================================================
/*

34. Removing empty lines from a text file

Write a program that, given the path to a text file, modifies the file by
removing all empty lines. Lines containing only whitespaces are considered
empty.

A possible approach to solving this task is to do the following:

Create a temporary file to contain only the text you want to retain from the
original file

Read line by line from the input file and copy to the temporary file all lines
that are not empty

Delete the original file after finishing processing it

Move the temporary file to the path of the original file

An alternative is to move the temporary file and overwrite the original one. The
following implementation follows the steps listed. The temporary file is created
in the temporary directory returned by filesystem::temp_directory_path():

*/

namespace U34_2018_12_03 
{
  void remove_empty_lines(const string &filename)
  {
    ifstream ifs{filename};
    string ofilename = filename + "_removed_empty_lines";
    ofstream ofs{ofilename};
    string line{};

    while (std::getline(ifs, line))
    {
      // when see a ling which has char other than WS
      
      // if (line.length() > 0 && line.find_first_not_of(" \t") != string::npos)

      if (line.length() > 0 && line.find_first_not_of(" ") != string::npos)
        ofs << line << endl;
    }
  }

} // namespace

TEST(U34, 2018_12_03)
{
  using namespace U34_2018_12_03;

  remove_empty_lines("readme");
}


// ={=========================================================================
/*

35. Computing the size of a directory

Write a function that computes the size of a directory, in bytes, recursively.
It should be possible to indicate whether symbolic links should be followed or
not.

requires *cxx-cxx17-filesystem*

*/


/*

36. Deleting files older than a given date

Write a function that, given the path to a directory and a duration, deletes all
the entries (files or subdirectories) older than the specified duration, in a
recursive manner. The duration can represent anything, such as days, hours,
minutes, seconds, and so on, or a combination of that, such as one hour and
twenty minutes. If the specified directory is itself older than the given
duration, it should be deleted entirely.

requires *cxx-cxx17-filesystem*

*/


/*

37. Finding files in a directory that match a regular expression

Write a function that, given the path to a directory and a regular expression,
returns a list of all the directory entries whose names match the regular
expression.

requires *cxx-cxx17-filesystem*

*/


// ={=========================================================================
/*

38. Temporary log files

Create a logging class that writes text messages to a discardable text file. The
text file should have a unique name and must be located in a temporary
directory. Unless specified otherwise, this log file should be deleted when the
instance of the class is destroyed. However, it should be possible to retain the
log file by moving it to a permanent location.

{
   logger log;
   try 
   {
      log << "this is a line" << "and this is another one";
      throw std::runtime_error("error");
   }
   catch (...) 
   {
      log.persist(R"(lastlog.txt)");
   }
}

requires *cxx-cxx17-filesystem*

*/


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

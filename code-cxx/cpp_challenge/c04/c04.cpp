#include "gmock/gmock.h"

#include <iostream>
#include <list>
#include <deque>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

32. Pascal's triangle

Write a function that prints up to 10 rows of Pascal's triangle to the console.

*/

namespace U32_2018_12_03 {

  // as itoa, can use modulus and reverse the result. How does the text? 
  // use *cxx-stringstream*

  template<typename T>
    string bytes_to_hexstr(T coll)
    {
      ostringstream os;

      for(const auto e : coll)
        os << hex << (int)e;

      return os.str();
    }

} // namespace


TEST(U32, 2018_12_03)
{
  using namespace U32_2018_12_03;

  {
    std::vector<unsigned char> v{ 0xBA, 0xAD, 0xF0, 0x0D };

    // EXPECT_THAT(bytes_to_hexstr(v), "BAADF00D");
    EXPECT_THAT(bytes_to_hexstr(v), "baadf0d");
  }
}


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

*/


/*

34. Removing empty lines from a text file

Write a program that, given the path to a text file, modifies the file by
removing all empty lines. Lines containing only whitespaces are considered
empty.

*/


/*

35. Computing the size of a directory

Write a function that computes the size of a directory, in bytes, recursively.
It should be possible to indicate whether symbolic links should be followed or
not.

*/


/*

36. Deleting files older than a given date

Write a function that, given the path to a directory and a duration, deletes all
the entries (files or subdirectories) older than the specified duration, in a
recursive manner. The duration can represent anything, such as days, hours,
minutes, seconds, and so on, or a combination of that, such as one hour and
twenty minutes. If the specified directory is itself older than the given
duration, it should be deleted entirely.

*/


/*

37. Finding files in a directory that match a regular expression

Write a function that, given the path to a directory and a regular expression,
returns a list of all the directory entries whose names match the regular
expression.

*/


/*

38. Temporary log files

Create a logging class that writes text messages to a discardable text file. The
text file should have a unique name and must be located in a temporary
directory. Unless specified otherwise, this log file should be deleted when the
instance of the class is destroyed. However, it should be possible to retain the
log file by moving it to a permanent location.

*/


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

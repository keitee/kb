#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <deque>
#include <list>
#include <forward_list>
#include <unordered_set>
#include <queue>
// #include <algorithm>
#include <numeric>        // std::accumulate for gcc 6
#include <limits>         // std::numeric_limits
#include <random>

#include "gmock/gmock.h"

using namespace std;
using namespace testing;

template <typename T>
void INSERT_ELEMENTS( T& coll, int first, int last )
{
    for (auto i = first; i <= last; i++)
        coll.insert( coll.end(), i );
}

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

template <typename T>
void PRINT_M_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << " {" << elem.first << ", " << elem.second << "}";
        ++count;
    }

    cout << " (" << count << ")" << endl;
}

// template <typename T>
// void PRINT_Q_ELEMENTS(const T& coll, const string &optstr = "")
// {
//   size_t count = coll.size();
//   cout << optstr;
// 
//   for(size_t i = 0; i < count; ++i)
//     cout << coll.top() << " ";
// 
//   cout << "(" << count << ")" << endl;
// }


// ={=========================================================================

void print_permutations_recursive_01(std::string in, std::string out)
{
  if (in.empty())
    cout << out << endl;
  else
  {
    for (size_t i = 0; i < in.size(); ++i)
    {
      print_permutations_recursive_01(in.substr(1), out + in[0]);
      std::rotate(in.begin(), in.begin()+1, in.end());
    }
  }
}

// this version do not work since:
// 1. when input has one char, in becomes empty and has runtime error when calls
// rotate() since it has invalid iterator. so has empty check.
//
// 2. even empty check is in place, the reuslt do not show full permutations
// since in gets changed in the loop which is maintained in the verison 01.

void print_permutations_recursive_02(std::string in, std::string out)
{
  if (in.empty())
    cout << out << endl;
  else
  {
    for (size_t i = 0; i < in.size(); ++i)
    {
      out.push_back(in[in.size()-1]);
      in.pop_back();
      print_permutations_recursive_02(in, out);
      if(!in.empty())
        std::rotate(in.begin(), in.begin()+1, in.end());
    }
  }
}

TEST(StlEx, Ex)
{
  cout << "----" << endl;
  print_permutations_recursive_01("123", "");

  cout << "----" << endl;
  print_permutations_recursive_02("123", "");
}

// ={=========================================================================
int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


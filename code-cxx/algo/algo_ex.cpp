#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <bitset>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

// ={=========================================================================
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
        cout << "(" << elem.first << ", " << elem.second << ") ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}


// ={=========================================================================


#define SORT_QUICK_DEBUG

int build_partition(vector<int> &coll, int first, int last)
{
  int pivot_pos = (first + last) / 2;
  int pivot_value = coll[pivot_pos];

#ifdef SORT_QUICK_DEBUG
    cout << "pivot_pos(" << pivot_pos << ") build before(" << first << ", " << last << "): ";

    for (int i = first; i <= last; ++i)
      cout << coll[i] << ", ";

    cout << endl;
#endif // SORT_QUICK_DEBUG

  // move the pivot to the first pos
  swap(coll[first], coll[pivot_pos]);

  int last_small = first;

  for (int inspect = first + 1; inspect <= last; ++inspect)
  {
    if (coll[inspect] < pivot_value)
    {
      ++last_small;

      // last_small == inspect case does happens and it is enhancement from the
      // previous code

      if (last_small != inspect)
        swap(coll[last_small], coll[inspect]);
    }
  }

  // move the pivot back
  swap(coll[first], coll[last_small]);

#ifdef SORT_QUICK_DEBUG
    cout << "pivot_pos(" << pivot_pos << ") build after (" << first << ", " << last << "): ";

    for (int i = first; i <= last; ++i)
      cout << coll[i] << ", ";

    cout << endl;
#endif // SORT_QUICK_DEBUG

  return last_small;
}

void sort_quick_01(vector<int> &coll, int first, int last)
{
  int last_small{};

  if (first < last)
  {
    last_small = build_partition(coll, first, last); 
    sort_quick_01(coll, first, last_small-1);
    sort_quick_01(coll, last_small+1, last);
  }
}

// from ansic, p87. exactly same way.
void sort_quick_02(vector<int> &v, int left, int right)
{
  int i, last;

  // do nothing if array contains fewer than two elements
  if( left >= right )
    return;

  // move partition elem
  // swap( v, left, (left+right)/2 );
  swap( v[left], v[(left+right)/2]);

  last = left;  // to v[0]

  // partition
  for(i = left+1; i <= right; i++)
    if( v[i] < v[left] )
    {
      // swap( v, ++last, i );   // shall ++last
      swap(v[++last], v[i]);   // shall ++last
    }

  // restore partition elem
  // swap(v, left, last);
  swap(v[left], v[last]);

  sort_quick_02( v, left, last-1 );
  sort_quick_02( v, last+1, right );
}

TEST(Algo, Ex)
{
  {
    vector<int> coll{29, 33, 35, 26, 19, 12, 22};

    sort_quick_01(coll, 0, coll.size()-1);

    EXPECT_THAT(coll, 
        ElementsAre(12, 19, 22, 26, 29, 33, 35));
  }

  {
    vector<int> coll{29, 33, 35, 26, 19, 12, 22};

    sort_quick_02(coll, 0, coll.size()-1);

    EXPECT_THAT(coll, 
        ElementsAre(12, 19, 22, 26, 29, 33, 35));
  }
}


int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

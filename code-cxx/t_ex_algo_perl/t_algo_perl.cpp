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
// algo-swap: swap without a temporary

int bigrand()
{
  return RAND_MAX*rand() + rand();
}
 
int randint(int l, int u)
{
  // return l + (bigrand() % (u-l+1));
  return l + (rand() % (u-l+1));
}
 
TEST(C12, RandWithDuplicates)
{
  multiset<int> coll{};
 
  for (int i = 0; i < 100; ++i)
  {
    auto value = randint(10, 100);
    coll.push_back(value);
    cout << value << ", ";
  }
  cout << endl;
 
  sort(coll.begin(), coll.end());
  PRINT_ELEMENTS(coll);
}

// const int N=1000;
// const int M=100;
// int arr[N];
// 
// int randint(int from, int to) {
// 	return from + (rand()%(to-from+1));
// }
// 
// void swap(int i, int j) {
// 	int t = arr[i];
// 	arr[i] = arr[j];
// 	arr[j] = t;
// }
// 
// TEST(C12, Rand)
// {
//   vector<int> coll{};
// 	int i;
// 	srand(time(NULL));
// 	for (i = 0; i < N; i++)
// 		arr[i] = i;
// 	for (i = 0; i < M; i++)
// 		swap(i, randint(i, N-1));
// 	for (i = 0; i < M; i++)
//   {
//     cout << arr[i] << ", ";
// 		// printf("%d\n", arr[i]);
//   }
//   cout << endl;
// }



int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

#include <iostream>
#include <vector>

using namespace std;

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

using ITER = vector<int>::iterator;

ITER partition_01(ITER begin, ITER end, int value)
{
  // find the end of the first group which not conforms to the value
  ITER list_end = begin;
  for (; list_end != end; ++list_end)
  {
    if (*list_end > value)
      break;
  }

  ITER run = list_end; 
  for(++run; run != end; ++run)
  {
    if (*run < value)
    {
      // cout << "swap(" << *run << ", " << *list_end << ")" << endl;
      swap(*run, *list_end);
      ++list_end;
    }
  }

  return list_end;
}

ITER partition_02(ITER begin, ITER end, int value)
{
  vector<int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};
  ITER one, two;
  one = coll.begin();
  two = one+1;
  cout << "one: " << *one << ", two: " << *two << endl;
  swap(*one, *two);
  // iter_swap(one, two);

  // begin = coll.begin();
  // end = coll.end();

  // // find the end of the first group which not conforms to the value
  // ITER list_end = begin;
  // for (; list_end != end; ++list_end)
  // {
  //   if (*list_end > value)
  //     break;
  // }

  // ITER run = list_end; 
  // for(++run; run != end; ++run)
  // {
  //   if (*run < value)
  //   {
  //     // cout << "swap(" << *run << ", " << *list_end << ")" << endl;
  //     swap(*run, *list_end);
  //     ++list_end;
  //   }
  // }

  PRINT_ELEMENTS(coll);

  // return list_end;
}

void partition_03()
{
  vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};
  // EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

  vector<int>::iterator one, two;
  one = coll.begin();
  two = ++one;
  swap(*one, *two);
  PRINT_ELEMENTS(coll);
  // EXPECT_THAT(coll, ElementsAre(2, 1, 3, 4, 5, 6, 7, 8, 9));

  // one = coll.begin();
  // two = --coll.end();
  // swap(*one, *two);
  // EXPECT_THAT(coll, ElementsAre(9, 1, 3, 4, 5, 6, 7, 8, 2));
}


int main()
{
  vector<int> coll{43,6,11,42,29,23,21,19,34,37,48,24,15,20,13,26,41,30,6,23};

  // partition_01(coll.begin(), coll.end(), 25);
  // PRINT_ELEMENTS(coll);

  partition_02(coll.begin(), coll.end(), 25);
  // partition_03();


  // EXPECT_THAT(coll, ElementsAreArray({6,11,23,21,19,24,15,20,13,6,23,43,42,29,34,26,41,30,37,48}));

  // this now fails since `current` is iterator of internal coll but not input
  // call. Have to work out one.
  // EXPECT_THAT(distance(coll.begin(), iter), 11);

  return 0;
}

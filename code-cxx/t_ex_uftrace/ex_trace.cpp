#include <iostream>
#include <vector>

using namespace std;

int main()
{
  vector<int> coll{1, 2, 3, 4, 5, 6};

  cout << "{";

  for (auto e : coll)
    cout << e << ",";

  cout << "}" << endl;

  return 0;
}

/*

core dump

*cxx-issue-case* *cxx-vaarg-issue*
This not only cause compile warnings. It causes core dump on embedded which
is very difficult to get call traces since it's crashes in libc.

But works okay in PC build

#endif

TEST(CxxCoredump, check_vaarg_issue)
{
  typedef long int int64;

  int64 value{-1};

  EXPECT_THAT(sizeof(int64), 8);

  // cause core
  printf("value (%ld)\n", value);
}

No it do not crash itself and the real problem is about using diag macro. TODO:

*/

#include <functional>
#include <iostream>
#include <string>
#include <thread>

namespace
{
  void cause_crash()
  {
    std::string s = 0;
    return;
  }
}

int main()
{
  cause_crash();
}

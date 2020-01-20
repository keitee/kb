#include <cstdlib>
#include <iostream>

#include <gmock/gmock.h>

using namespace testing;

// To show how tests from other files are added

TEST(Second, check)
{
  EXPECT_THAT(true, true);
}

// int main(int argc, char **argv)
// {
//   int item = 0;
//
//   testing::InitGoogleTest(&argc, argv);
//
// //  List list;
// //  CreatList(&list);
// //
// //  std::cout << "type in 5 numbers." << std::endl;
// //
// //  for(int i=0; i < 5; i++)
// //  {
// //    std::cin >> item;
// //    AddList(&list, item );
// //  }
// //
// //  TraverseList(&list, EntryPrint);
// //
//   return RUN_ALL_TESTS();
// }

#include <exception>
#include <iostream>
#include <memory>
#include <set>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std;
using namespace testing;

// ={=========================================================================
// test_abort

/*
HOWTO repeatedly run a test in GDB, until it fails

Those of you grappling with intermittent failures may find this useful. I hit a
test that was intermittently aborting in x86 debug build, due to use of a
"singular iterator". It was particularly awkward to reproduce in gdb, so I
wanted to script it such that gdb `would only prompt in the event of failure`, and
otherwise quit and allow the next iteration.

Here's what I ended-up with:
while gdb --eval-command=start --eval-command="b abort" \
--eval-command=continue --eval-command=quit \
--args ./dbusoutputmanagertest; do echo "OK"; done


$ gdb --eval-command=start --eval-command="b abort" --eval-command=continue --eval-command=quit --args ./test_abort
GNU gdb (Ubuntu 8.1-0ubuntu3.2) 8.1.0.20180409-git
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./test_abort...done.
Temporary breakpoint 1 at 0x8c8f6: file /home/keitee/git/kb/code-cxx/gdb/test_abort.cpp, line 30.
Starting program: /home/keitee/git/kb/code-cxx/gdb/build/test_abort
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Temporary breakpoint 1, main (argc=1, argv=0x7fffffffdbf8) at /home/keitee/git/kb/code-cxx/gdb/test_abort.cpp:30
30        testing::InitGoogleMock(&argc, argv);
Breakpoint 2 at 0x7ffff72646c0: file abort.c, line 49.
Continuing.
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from Gdb
[ RUN      ] Gdb.test_abort
[       OK ] Gdb.test_abort (0 ms)
[----------] 1 test from Gdb (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
[Inferior 1 (process 24206) exited normally]

exit status is 0

keitee@kit-ubuntu:~/git/kb/code-cxx/gdb/build$ echo $?
0


When call abort()

TEST(Gdb, test_abort)
{
  EXPECT_THAT(true, true);
  abort();
}

$ gdb --eval-command=start --eval-command="b abort" --eval-command=continue --eval-command=quit --args ./test_abort
...
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from Gdb
[ RUN      ] Gdb.test_abort

Breakpoint 2, __GI_abort () at abort.c:49
49      abort.c: No such file or directory.
A debugging session is active.

        Inferior 1 [process 25484] will be killed.

Quit anyway? (y or n) n
Not confirmed.
(gdb) bt
#0  __GI_abort () at abort.c:49
#1  0x00005555555e0892 in Gdb_test_abort_Test::TestBody (this=0x555555884fd0) at /home/keitee/git/kb/code-cxx/gdb/test_abort.cpp:75
#2  0x000055555562be6c in void testing::internal::HandleSehExceptionsInMethodIfSupported<testing::Test, void>(testing::Test*, void (testing::Test::*)(), char const*) ()
#3  0x00005555556255a5 in void testing::internal::HandleExceptionsInMethodIfSupported<testing::Test, void>(testing::Test*, void (testing::Test::*)(), char const*) ()
#4  0x0000555555601e68 in testing::Test::Run() ()
#5  0x000055555560280f in testing::TestInfo::Run() ()
#6  0x0000555555602f11 in testing::TestSuite::Run() ()
#7  0x000055555560ea59 in testing::internal::UnitTestImpl::RunAllTests() ()
#8  0x000055555562d09f in bool testing::internal::HandleSehExceptionsInMethodIfSupported<testing::internal::UnitTestImpl, bool>(testing::internal::UnitTestImpl*, bool (testing::internal::UnitTestImpl::*)(), char const*) ()
#9  0x000055555562654d in bool testing::internal::HandleExceptionsInMethodIfSupported<testing::internal::UnitTestImpl, bool>(testing::internal::UnitTestImpl*, bool (testing::internal::UnitTestImpl::*)(), char const*) ()
#10 0x000055555560d2f2 in testing::UnitTest::Run() ()
#11 0x00005555555e1156 in RUN_ALL_TESTS () at /home/keitee/works/googletest/googletest/include/gtest/gtest.h:2470
#12 0x00005555555e08f5 in main (argc=1, argv=0x7fffffffdbf8) at /home/keitee/git/kb/code-cxx/gdb/test_abort.cpp:84
(gdb)

So

while gdb --eval-command=start --eval-command="b abort" --eval-command=continue --eval-command=quit \
--args ./test_abort; do echo "OK"; done

will run while there is no abort() call, that is no hit to break point. when
abort() is called that is when break point it hit, program stops and gdb runs as
shown above.

however, even if quit gdb, it returns 0 and cotinue running. have to use 
(gdb) exit 1

*/

TEST(Gdb, test_abort)
{
  EXPECT_THAT(true, true);
}

// TEST(Gdb, test_abort)
// {
//   EXPECT_THAT(true, true);
//   abort();
// }

int main(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test.  Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

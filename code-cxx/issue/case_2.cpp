/*

core dump

[ RUN      ] StringCtor.check_crash
unknown file: Failure
C++ exception with description "basic_string::_M_construct null not valid" thrown in the test body.
[  FAILED  ] StringCtor.check_crash (0 ms)

may wonder who will do this. However, it happens when making changes and caused
half a day to find out why.

was:

  struct DrmState
  {
    std::string state{};
    int euid{};
  };

DrmController::DrmController(PlaybackService *service)
    : m_state{AS_STATE_UNAVAILABLE, 0}
{
  ...
}

now, due to requirment change, have to make euid as string. so did that in code
but when runs it on the box, keep crashing while cannot see why and when all
changes looks okay. still see crash even when remove all codes changing euid.
Sadly, no backtrace or useful from core since it crashes very early.

the fix:

  struct DrmState
  {
    std::string state{};
    std::string euid{};
  };

DrmController::DrmController(PlaybackService *service)
    : m_state{AS_STATE_UNAVAILABLE, ""}
{
}

The problem is when init std::string with 0 which is left while makeing changes.
WOW.


core is made but not useful from core file.

./issue_case_2
terminate called after throwing an instance of 'std::logic_error'
  what():  basic_string::_M_construct null not valid
Aborted (core dumped)

(gdb) bt
#0  0x00007fe9ab060f47 in ?? ()
#1  0x0000000000000000 in ?? ()

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

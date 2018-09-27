#include <iostream>
#include <asm/param.h>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;


// ={=========================================================================
// sys-pagesize

TEST(LinuxSys, GetPageSize)
{
  // GETPAGESIZE(2)
  // get memory page size

  auto psize = getpagesize();

  EXPECT_THAT(psize, 4096);

  // kyoupark@kit-debian64:~/asan/gcc/linux-3.14.43$ gl EXEC_PAGESIZE
  // EXEC_PAGESIZE       6 arch/alpha/include/uapi/asm/param.h #define EXEC_PAGESIZE 8192
  // EXEC_PAGESIZE      19 arch/arm64/include/uapi/asm/param.h #define EXEC_PAGESIZE 65536
  // EXEC_PAGESIZE       6 arch/cris/include/uapi/asm/param.h #define EXEC_PAGESIZE  8192
  // EXEC_PAGESIZE       4 arch/frv/include/uapi/asm/param.h #define EXEC_PAGESIZE   16384
  // EXEC_PAGESIZE      22 arch/hexagon/include/uapi/asm/param.h #define EXEC_PAGESIZE       16384
  // EXEC_PAGESIZE      13 arch/ia64/include/uapi/asm/param.h #define EXEC_PAGESIZE  65536
  // EXEC_PAGESIZE       5 arch/m68k/include/uapi/asm/param.h #define EXEC_PAGESIZE  4096
  // EXEC_PAGESIZE       7 arch/m68k/include/uapi/asm/param.h #define EXEC_PAGESIZE  8192
  // EXEC_PAGESIZE      12 arch/mips/include/uapi/asm/param.h #define EXEC_PAGESIZE  65536
  // EXEC_PAGESIZE      22 arch/openrisc/include/uapi/asm/param.h #define EXEC_PAGESIZE      8192
  // EXEC_PAGESIZE       4 arch/sparc/include/uapi/asm/param.h #define EXEC_PAGESIZE 8192    /* Thanks for sun4's we carry baggage... */
  // EXEC_PAGESIZE      18 arch/xtensa/include/uapi/asm/param.h #define EXEC_PAGESIZE        4096
  // EXEC_PAGESIZE       9 include/uapi/asm-generic/param.h #define EXEC_PAGESIZE    4096

  EXPECT_THAT(EXEC_PAGESIZE, 4096);
}


// ={=========================================================================
// sys-memory


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


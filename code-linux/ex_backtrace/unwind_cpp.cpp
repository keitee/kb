#define UNW_LOCAL_ONLY
#include <cxxabi.h>       // for abi::
#include <libunwind.h>
#include <stdio.h>
#include <cstdlib>

// Call this function to get a backtrace.
void backtrace() {
  unw_cursor_t cursor;
  unw_context_t context;

  // Initialize cursor to current frame for local unwinding.
  unw_getcontext(&context);
  unw_init_local(&cursor, &context);

  // Unwind frames one by one, going up the frame stack.
  while (unw_step(&cursor) > 0) {
    unw_word_t offset, pc;
    unw_get_reg(&cursor, UNW_REG_IP, &pc);
    if (pc == 0) {
      break;
    }
    printf("0x%lx:", pc);

    char sym[256];
    if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {

      // old
      // printf(" (%s+0x%lx)\n", sym, offset);

      char* nameptr = sym;
      int status;
      char* demangled = abi::__cxa_demangle(sym, nullptr, nullptr, &status);
      if (status == 0) {
        nameptr = demangled;
      }
      printf(" (%s+0x%lx)\n", nameptr, offset);
      free(demangled);

    } else {
      printf(" -- error: unable to obtain symbol name for this frame\n");
    }
  }
}

void foo() {
  backtrace(); // <-------- backtrace here!
}

void bar() {
  foo();
}

// LD_LIBRARY_PATH=/usr/local/lib ./unwind_out

int main(int argc, char **argv) {

  bar();

  return 0;
}


// ={=========================================================================
// 
// int main(int argc, char** argv)
// {
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }


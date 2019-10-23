// #include <boost/lexical_cast.hpp>
#include "gmock/gmock.h"
#include <dlfcn.h>
#include <slog.h>

using namespace std;
using namespace std::placeholders;
using namespace testing;

/*
={=============================================================================

LPI-42

To build programs that use the dlopen API on Linux, we must specify the `-ldl`
option, in order to link against the libdl library.

#include <dlfcn.h>
void *dlopen(const char * libfilename , int flags );
Returns library handle on success, or NULL on error

       One of the following two values must be included in flags:

       RTLD_LAZY
              Perform  lazy binding.  Resolve symbols only as the code that
              references them is executed.  If the symbol is never referenced,
              then it is never resolved.  (Lazy binding is performed only for
              function references; references to variables are always
              immediately bound when the shared object is loaded.)
              Since glibc 2.1.1, this flag is overridden by the effect of the
              LD_BIND_NOW environment variable.

       RTLD_NOW
              If this value is specified, or the environment variable
              LD_BIND_NOW is set to a nonempty string, all undefined symbols in
              the shared object are resolved before dlopen() returns.  If this
              cannot be  done,  an  error  is returned.


<os-shared-dlerror>
The dlerror() function returns NULL `if no error has occurred` since the last
call to dlerror(). We’ll see how this is useful in the next section.

<os-shared-dlsym>

#include <dlfcn.h>
void *dlsym(void * handle , char * symbol );
Returns address of symbol, or NULL if symbol is not found

The value of a symbol returned by dlsym() may be NULL, which is
indistinguishable from the “symbol not found” return. In order to differentiate
the two possibilities, we must call dlerror() beforehand (to make sure that any
previously held error string is `cleared`) and then if, after the call to
dlsym(), dlerror() returns a non-NULL value, we know that an error occurred.


<os-readline>
https://tiswww.case.edu/php/chet/readline/readline.html

Function: int rl_on_new_line (void)
Tell the update functions that we have moved onto a new (empty) line, usually
after outputting a newline.

*/

typedef int (*rl_on_new_line_t)(void);

/*
assume that for `rl_on_new_line` symbol, there is symbol_t typedef and m_symbol
member variable to set.

*/

#define GET_RL_FUNC(f)                                                         \
  do                                                                           \
  {                                                                            \
    m_##f = reinterpret_cast<f##_t>(dlsym(handle, "" #f ""));                  \
    if (!m_##f)                                                                \
    {                                                                          \
      LOG_MSG("failed to get symbol '" #f "' (%s)", dlerror());                \
      dlclose(handle);                                                         \
      handle = nullptr;                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

TEST(OsShared, Dlapi)
{
  {
    void *handle = dlopen("libreadline.so", RTLD_NOW);
    EXPECT_THAT(handle, Ne(nullptr));

    // error: invalid conversion from ‘void*’ to ‘rl_on_new_line_t {aka int
    // (*)()}’ [-fpermissive]
    //
    // rl_on_new_line_t f_on_new_line = dlsym(handle, "rl_on_new_line");

    dlerror();

    rl_on_new_line_t f_on_new_line =
      (rl_on_new_line_t)dlsym(handle, "rl_on_new_line");

    if (!f_on_new_line)
      LOG_MSG("failed to find symbol rl_on_new_line_t (%s)", dlerror());

    EXPECT_THAT(f_on_new_line, Ne(nullptr));

    auto ret = f_on_new_line();
    LOG_MSG("return from rl_on_new_line_t (%d)", ret);

    dlclose(handle);
  }

  {
    void *handle = dlopen("libreadline.so", RTLD_NOW);
    EXPECT_THAT(handle, Ne(nullptr));

    dlerror();

    rl_on_new_line_t m_rl_on_new_line;

    GET_RL_FUNC(rl_on_new_line);

    if (!m_rl_on_new_line)
      LOG_MSG("failed to find symbol rl_on_new_line_t (%s)", dlerror());

    EXPECT_THAT(m_rl_on_new_line, Ne(nullptr));

    auto ret = m_rl_on_new_line();
    LOG_MSG("return from rl_on_new_line_t (%d)", ret);

    dlclose(handle);
  }
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

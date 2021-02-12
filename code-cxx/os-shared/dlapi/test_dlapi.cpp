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


<os-dlerror>
The dlerror() function returns NULL `if no error has occurred` since the last
call to dlerror(). We’ll see how this is useful in the next section.

<os-dlsym>

#include <dlfcn.h>
void *dlsym(void * handle , char * symbol);
Returns address of symbol, or NULL if symbol is not found


the reason to use dlerror() before calling any dlapi will clear error message

`The value of a symbol returned by dlsym() may be NULL`, which is
`indistinguishable from NULL for the "symbol not found" return.` 

In order to differentiate the two possibilities, we must call dlerror()
beforehand (to make sure that any previously held error string is cleared) and
then if, after the call to dlsym(), dlerror() returns a non-NULL value, we
know that an error occurred.

that is, how distinguishs when symbol value is NULL and when symbol is not
found. When symbol value itself is null, dlerror() should be null.

dlsym() return null and dlerror() returns not-null : symbol itself is null
dlsym() return null and dlerror() returns null     : symbol not found


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

/*
this macro can be revised to:

    #define GET_RL_FUNC(f) \
        do { \
            _ ## f = reinterpret_cast<f ## _t>(dlsym(mLibHandle, "" #f "")); \
            if (! _ ## f) { \
                AI_LOG_ERROR("failed to get symbol '" #f "' (%s)", dlerror()); \
                goto failed; \
            } \
        } while(0)

where uses "goto" to handle clean up than just return.

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

TEST(OsShared, dlapi)
{
  {
    void *handle = dlopen("libreadline.so", RTLD_NOW);
    EXPECT_THAT(handle, Ne(nullptr));

    // However, we can’t simply assign the result of dlsym() to such a pointer,
    // as in the fol- lowing example:
    //
    // funcp = dlsym(handle, symbol);
    //
    // The reason is that the C99 standard forbids assignment between a function
    // pointer and void *.
    //
    // error: invalid conversion from ‘void*’ to ‘rl_on_new_line_t {aka int
    // (*)()}’ [-fpermissive]
    //
    // rl_on_new_line_t f_on_new_line = dlsym(handle, "rl_on_new_line");

    // clear error message
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

  // use macro instead which set variable automatically
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

/*
$ strace -eopenat ./test_dlapi --gtest_filter=*dlopen*
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/tls/haswell/x86_64/libdl.so.2", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/tls/haswell/libdl.so.2", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/tls/x86_64/libdl.so.2", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/tls/libdl.so.2", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/haswell/x86_64/libdl.so.2", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/haswell/libdl.so.2", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/x86_64/libdl.so.2", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/libdl.so.2", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libdl.so.2", O_RDONLY|O_CLOEXEC) = 3
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/libpthread.so.0", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libpthread.so.0", O_RDONLY|O_CLOEXEC) = 3
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/libstdc++.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/lib/x86_64-linux-gnu/libstdc++.so.6", O_RDONLY|O_CLOEXEC) = 3
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/libgcc_s.so.1", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libgcc_s.so.1", O_RDONLY|O_CLOEXEC) = 3
openat(AT_FDCWD, "/home/keitee/git/kb/code-cxx/os-shared/dlapi/../../libs/build/libc.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libm.so.6", O_RDONLY|O_CLOEXEC) = 3
Note: Google Test filter = *dlopen*
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from OsShared
[ RUN      ] OsShared.dlopen
openat(AT_FDCWD, "/usr/lib/libreadline.so", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
/home/keitee/git/kb/code-cxx/os-shared/dlapi/test_dlapi.cpp:161: Failure
Value of: handle
Expected: isn't equal to (nullptr)
  Actual: NULL (of type void*)
[  FAILED  ] OsShared.dlopen (1 ms)
[----------] 1 test from OsShared (1 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (2 ms total)
[  PASSED  ] 0 tests.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] OsShared.dlopen

 1 FAILED TEST
+++ exited with 1 +++
$

LOG| F:test_dlapi.cpp C:virtual void OsShared_dlopen_Test::TestBody() L:00208 
failed to open libreadline.so {/usr/lib/libreadline.so: cannot open shared object file: No such file or directory}

so have to use dlopen("libreadline.so") and can rely on PATH. This example
comes from embedded where we know the exact path.

*/

TEST(OsShared, dlopen)
{
  {
    void *handle = dlopen("/usr/lib/libreadline.so", RTLD_NOW);
    if (nullptr == handle)
    {
      LOG_MSG("failed to open libreadline.so {%s}", dlerror());
    }

    ASSERT_THAT(handle, Ne(nullptr));

    // clear error message
    dlerror();

    rl_on_new_line_t f_on_new_line =
      (rl_on_new_line_t)dlsym(handle, "rl_on_new_line");

    if (!f_on_new_line)
      LOG_MSG("failed to find symbol rl_on_new_line_t (%s)", dlerror());

    EXPECT_THAT(f_on_new_line, Ne(nullptr));

    dlclose(handle);
  }
}

/*
42.1.5 Obtaining Information About Loaded Symbols: dladdr()

Given an address in addr (typically, one obtained by an earlier call to
dlsym()), dladdr() returns a structure containing information about that
address.

#define _GNU_SOURCE
#include <dlfcn.h>
int dladdr(const void *addr, Dl_info *info);

Returns nonzero value if addr was found in a shared library, otherwise 0

The info argument is a pointer to a caller-allocated structure that has the
following form:

typedef struct {

  // Pathname of shared library containing 'addr'
  const char *dli_fname;
  
  // Base address at which shared library is loaded
  void *dli_fbase;
  
  // Name of nearest run-time symbol with an address <= 'addr'
  const char *dli_sname;
  
  // Actual value of the symbol returned in 'dli_sname'
  void *dli_saddr;

} Dl_info;


The first two fields of the Dl_info structure specify the pathname and run-time
base address of the shared library containing the address specified in addr. The
last two fields return information about that address. Assuming that addr points
to the exact address of a symbol in the shared library, then dli_saddr returns
the same value as was passed in addr.

SUSv3 doesn’t specify dladdr(), and this function is not available on all UNIX
implementations.

*/

TEST(OsShared, dlapi_dladdr)
{
  {
    void *handle = dlopen("libreadline.so", RTLD_NOW);
    EXPECT_THAT(handle, Ne(nullptr));

    // clear error message
    dlerror();

    auto add = dlsym(handle, "rl_on_new_line");

    if (!add)
      LOG_MSG("failed to find symbol rl_on_new_line_t (%s)", dlerror());

    EXPECT_THAT(add, Ne(nullptr));

    Dl_info info;
    auto ret = dladdr(add, &info); 

    if (ret)
    {
      LOG_MSG("info.dli_fname (%s)", info.dli_fname);
    }

    dlclose(handle);
  }
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>

#include "slog.h"

namespace Slog
{

// lib/ename.c.inc

char const *ename[] = {
/* 0 */ "",
/* 1 */ "EPERM", "ENOENT", "ESRCH", "EINTR", "EIO", "ENXIO", "E2BIG",
/* 8 */ "ENOEXEC", "EBADF", "ECHILD", "EAGAIN/EWOULDBLOCK", "ENOMEM",
/* 13 */ "EACCES", "EFAULT", "ENOTBLK", "EBUSY", "EEXIST", "EXDEV",
/* 19 */ "ENODEV", "ENOTDIR", "EISDIR", "EINVAL", "ENFILE", "EMFILE",
/* 25 */ "ENOTTY", "ETXTBSY", "EFBIG", "ENOSPC", "ESPIPE", "EROFS",
/* 31 */ "EMLINK", "EPIPE", "EDOM", "ERANGE", "EDEADLK/EDEADLOCK",
/* 36 */ "ENAMETOOLONG", "ENOLCK", "ENOSYS", "ENOTEMPTY", "ELOOP", "",
/* 42 */ "ENOMSG", "EIDRM", "ECHRNG", "EL2NSYNC", "EL3HLT", "EL3RST",
/* 48 */ "ELNRNG", "EUNATCH", "ENOCSI", "EL2HLT", "EBADE", "EBADR",
/* 54 */ "EXFULL", "ENOANO", "EBADRQC", "EBADSLT", "", "EBFONT", "ENOSTR",
/* 61 */ "ENODATA", "ETIME", "ENOSR", "ENONET", "ENOPKG", "EREMOTE",
/* 67 */ "ENOLINK", "EADV", "ESRMNT", "ECOMM", "EPROTO", "EMULTIHOP",
/* 73 */ "EDOTDOT", "EBADMSG", "EOVERFLOW", "ENOTUNIQ", "EBADFD",
/* 78 */ "EREMCHG", "ELIBACC", "ELIBBAD", "ELIBSCN", "ELIBMAX",
/* 83 */ "ELIBEXEC", "EILSEQ", "ERESTART", "ESTRPIPE", "EUSERS",
/* 88 */ "ENOTSOCK", "EDESTADDRREQ", "EMSGSIZE", "EPROTOTYPE",
/* 92 */ "ENOPROTOOPT", "EPROTONOSUPPORT", "ESOCKTNOSUPPORT",
/* 95 */ "EOPNOTSUPP/ENOTSUP", "EPFNOSUPPORT", "EAFNOSUPPORT",
/* 98 */ "EADDRINUSE", "EADDRNOTAVAIL", "ENETDOWN", "ENETUNREACH",
/* 102 */ "ENETRESET", "ECONNABORTED", "ECONNRESET", "ENOBUFS", "EISCONN",
/* 107 */ "ENOTCONN", "ESHUTDOWN", "ETOOMANYREFS", "ETIMEDOUT",
/* 111 */ "ECONNREFUSED", "EHOSTDOWN", "EHOSTUNREACH", "EALREADY",
/* 115 */ "EINPROGRESS", "ESTALE", "EUCLEAN", "ENOTNAM", "ENAVAIL",
/* 120 */ "EISNAM", "EREMOTEIO", "EDQUOT", "ENOMEDIUM", "EMEDIUMTYPE",
/* 125 */ "ECANCELED", "ENOKEY", "EKEYEXPIRED", "EKEYREVOKED",
/* 129 */ "EKEYREJECTED", "EOWNERDEAD", "ENOTRECOVERABLE", "ERFKILL"
};

#define MAX_ENAME 132


void outputError(bool useErrorString, int err, bool useFlush,
    const char *format, va_list ap)
{
#define BUF_SIZE (1024)
  char buf[BUF_SIZE];
  char userMsg[BUF_SIZE];
  char errText[BUF_SIZE];

  // output formatted message from user
  vsnprintf(userMsg, BUF_SIZE, format, ap);

  // output error string from errno and only errno is not success
  if (useErrorString && err)
  {
    snprintf(errText, BUF_SIZE, " [%s %s]",
        (err > 0 && err <= MAX_ENAME) ? ename[err] : "?UNKNOWN?",
        strerror(err));
  }
  else
  {
    snprintf(errText, BUF_SIZE, ":");
  }

  // concat user msg and err string. note that added newline.
  snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);

  if (useFlush)
    fflush(stdout);

  fputs(buf, stderr);
  fflush(stderr);
}


/* 
Dump core if EF_DUMPCORE environment variable is defined and is a nonempty
string; otherwise call exit(3) or _exit(2), depending on the value of
'useExit3'. 
*/

void terminate(bool useExit3)
{
  char *s;

  s = getenv("EF_DUMPCORE");

  // have to set below to get core file
  // $ ulimit -c unlimited

  if (s != nullptr && *s != '\0')
    abort();
  else if(useExit3)
    exit(EXIT_FAILURE);
  else
    _exit(EXIT_FAILURE);
}



/* 

Display error message including 'errno' diagnostic, and return to caller 

Prints a message on standard error. Its argument list is the same as for
printf(), except that 

a terminating newline character is automatically appended to the output string. 

The errMsg() function prints the error text corresponding to the current value
of errno-this consists of the error name, such as EPERM, plus the error
description as returned by strerror()

followed by the formatted output specified in the argument list.

*/

void errMsg(const char *format, ...)
{
  va_list arg_list;
  int saved_errno;

  // in case we change it here
  saved_errno = errno;

  va_start(arg_list, format);
  outputError(true, errno, true, format, arg_list);
  va_end(arg_list);

  errno = saved_errno;
}


/* 

Display error message including 'errno' diagnostic, and terminate the process 

*/

void errExit(const char *format, ...)
{
  va_list arg_list;

  va_start(arg_list, format);
  outputError(true, errno, true, format, arg_list);
  va_end(arg_list);

  terminate(true);
}


/* 

Display error message including 'errno' diagnostic, and terminate the process by
calling _exit().

The relationship between this function and errExit() is analogous to that
between _exit(2) and exit(3): unlike errExit(), this function does not flush
stdout and calls _exit(2) to terminate the process (rather than exit(3), which
would cause exit handlers to be invoked).

These differences make this function especially useful in a library function
that creates a child process that must then terminate because of an error: the
child must terminate without flushing stdio buffers that were partially filled
by the caller and without invoking exit handlers that were established by the
caller.

void err_exit(const char *format, ...)

*/


/* 
The following function does the same as errExit(), but expects the error number
in 'errnum'

LPI-3.5.2 Common Functions and Header Files

<os-errno-thread>
The errno is a global integer variable. However, this doesn't suffice for
threaded programs. If a thread made a call that returned an error in a
`global-errno` variable, then this would confuse other threads that might also
be making calls and checking errno. In other words, race conditions would
result.

Each thread has its own errno value. On Linux, a `thread-specific-errno` is
achieved in a similar manner to most other UNIX: errno is defined as a macro
that `expands into a function call` returning a modifiable lvalue that is
distinct for each thread. Since the lvalue is modifiable, it is still possible
to write assignment statements of the form `errno = value` in threaded
programs.

To summarize, the errno mechanism has been adapted for threads in a manner
that leaves error reporting unchanged from the traditional UNIX API.

After all, errno macro returns a per-thread errno. A program is required to
declare errno by including <errno.h>, which enables the implementation of a
per-thread errno.


<ex>
The traditional method of returning status from system calls and some library
functions is to `return 0 on success and -1 on error`, with errno being set to
indicate the error. The functions in the `pthreads API do things differently`
All pthreads functions return 0 on success or a positive value on failure. The
failure value is one of the same values that can be placed in errno by
traditional UNIX system calls. 

We could diagnose errors from the POSIX threads functions using code such as
the following:

errno = pthread_create(&thread, NULL, func, &arg);
if (errno != 0)
   errExit("pthread_create");

This approach is `inefficient` because errno is defined in threaded programs
as a macro. Under the POSIX threads API, errno is redefined to be a function
that returns a `pointer to a thread-specific storage area` 

void errExitEN(int errnum, const char *format, ...);

errExitEN() function allows us to write a more efficient equivalent of the
above code because each reference to errno in a threaded program carries the
overhead of a function call, our example programs don’t directly assign the
return value of a Pthreads function to errno.

int s;
s = pthread_create(&thread, NULL, func, &arg);
if (s != 0)
   errExitEN(s, "pthread_create");

Mainly, we use errExitEN() in programs that employ the POSIX threads API.

*/

void errExitEN(int errnum, const char *format, ...)
{
  va_list arg_list;

  va_start(arg_list, format);

  // note that errnum but not errno
  outputError(true, errnum, true, format, arg_list);
  va_end(arg_list);

  terminate(true);
}

} // namespace

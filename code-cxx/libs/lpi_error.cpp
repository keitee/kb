/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2018.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/

/* Listing 3-3 */

/* error_functions.c

   Some standard error handling routines used by various programs.
*/

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>

#include "lpi_error.h"

namespace {

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

  // output error string from errno
  if (useErrorString)
  {
    // error nemonic and error string
    snprintf(errText, BUF_SIZE, " [%s %s]",
        (err > 0 && err <= MAX_ENAME) ? ename[err] : "",
        strerror(err));
  }
  else
  {
    snprintf(errText, BUF_SIZE, ":");
  }

  // concat user msg and err string. note that added newline.
  snprintf(buf, BUF_SIZE, "ERROR%s, %s\n", errText, userMsg);

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

} // namespace


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


void errExitEN(int errnum, const char *format, ...)
{
  va_list arg_list;

  va_start(arg_list, format);

  // note that errnum but not errno
  outputError(true, errnum, true, format, arg_list);
  va_end(arg_list);

  terminate(true);
}

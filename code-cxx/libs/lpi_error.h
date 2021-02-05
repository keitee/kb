/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/

/* Listing 3-2 */

/* error_functions.h

   Header file for error_functions.c.
*/
#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

/*
Display error message including `errno` diagnostic, and return to caller

Prints a message on standard error. Its argument list is the same as for
printf(), except that

a terminating newline character is automatically appended to the output string.

The errMsg() function prints the error text corresponding to the current value
of errno-this consists of the error name, such as EPERM, plus the error
description as returned by strerror()

followed by the formatted output specified in the argument list.

ERROR [ Success], this is error message from errMsg and value 10
ERROR [ENOENT No such file or directory], failed to open file
      ["errno name" and "strerr result"]
 
*/

void errMsg(const char *format, ...);

/*
Display error message including `errno` diagnostic, and terminate the process

*/

void errExit(const char *format, ...);

/*
The following function does the same as errExit(), but expects the error number
in 'errnum'

LPI-3.5.2 Common Functions and Header Files

*os-errno-thread*

The errno is a global integer variable. However, this doesn't suffice for
threaded programs. If a thread made a call that returned an error in a
`global-errno` variable, then this would confuse other threads that might also
be making calls and checking errno. In other words, race conditions would
result.

Each thread has its "own errno value." On Linux, a `thread-specific-errno` is
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
All pthreads functions return 0 on success or a positive value on failure. 

NOTE: The failure value is one of the same values that can be placed in errno by
traditional UNIX system calls.

We could diagnose errors from the POSIX threads functions using code such as
the following:

errno = pthread_create(&thread, NULL, func, &arg);
if (errno != 0)
   errExit("pthread_create");

(So here, uses errno once and one more in errExit())

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

(errExitEN() do not use errno at all)

So the point is that "errExitEN" do not use errno which is a function call in
threaded app and use args in a call. Hence, efficient.

*/
void errExitEN(int errnum, const char *format, ...);

// Print a command usage error message and terminate the process
void usageErr(const char *format, ...);
void cmdLineErr(const char *format, ...);

void prnMsg(const char *fmt, ...);

// org:
//
// #ifdef __GNUC__
//     /* This macro stops 'gcc -Wall' complaining that "control reaches
//        end of non-void function" if we use the following functions to
//        terminate main() or some other non-void function. */
//
// #define NORETURN __attribute__ ((__noreturn__))
// #else
// #define NORETURN
// #endif
//
// void errExit(const char *format, ...) NORETURN ;
// void errExitEN(int errnum, const char *format, ...) NORETURN ;
// void err_exit(const char *format, ...) NORETURN ;
// void fatal(const char *format, ...) NORETURN ;
// void usageErr(const char *format, ...) NORETURN ;
// void cmdLineErr(const char *format, ...) NORETURN ;

#endif

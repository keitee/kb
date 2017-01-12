#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([Httpsend], [0.0], [stanleyhuangyc@yahoo.com.cn])
AC_CONFIG_SRCDIR([src/httpsend.c])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_CHECK_PROGS([DOXYGEN], [doxygen])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for libraries.
# Checks for typedefs, structures, and compiler characteristics.
# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

echo \
"-------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION}

 Prefix: '${prefix}'.
 Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'

 Package features:
   Async Execution: ${async_exec}

 Now type 'make @<:@<target>@:>@'
   where the optional <target> is:
     all                - build all binaries
     install            - install everything

--------------------------------------------------"

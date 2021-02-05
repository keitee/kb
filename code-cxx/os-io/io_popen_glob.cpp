#include "lpi_lib.h"
#include "slog.h"
#include <cerrno>
#include <cstdlib> // exit
#include <fcntl.h> // Defines O_* constants

#include <iostream>
#include <sstream>
#include <string>

#include <limits.h> // PATH_MAX

#define LOG_EXIT LOG_MSG

/*
Listing 44-5: Globbing filename patterns with popen()

Listing 44-5 demonstrates the use of popen() and pclose(). This program
repeatedly reads a filename wildcard pattern w, and then uses popen() to obtain
the results from passing this pattern to the ls command t. (Techniques similar
to this were used on older UNIX implementations to perform filename generation,
also known as globbing, prior to the existence of the glob() library function.)


The following shell session demonstrates the use of the program in Listing 44-5.
In this example, we first provide a pattern that matches two filenames, and then
a pat- tern that matches no filename:

$ ./popen_glob

// Matches two filenames
pattern: popen_glob*
popen_glob
popen_glob.c
2 matching files
pclose() status = 0
child exited, status=0

// Matches no filename
pattern: x*
0 matching files
// ls(1) exits with status 1
pclose() status = 0x100
child exited, status=1

// Type Control-D to terminate
pattern: ^D$


$ ./io_popen_glob
pattern: io_*
io_bad_exclusive_open
io_bad_exclusive_open_2
io_copy
io_pipe_filter
io_pipe_popen
io_popen_glob
io_simple_pipe
    7 matching files
    pclose() status = 0
        child exited, status=0
pattern: x*
    0 matching files
    pclose() status = 0x200
        child exited, status=2
pattern: keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$

*/

const char *POPEN_FMT{"/bin/ls -d %s 2> /dev/null"};
const int PAT_SIZE{50};
const int PCMD_BUF_SIZE{(sizeof(POPEN_FMT) + PAT_SIZE)};

int main(int argc, char *argv[])
{
  char pat[PAT_SIZE]; /* Pattern for globbing */
  char popenCmd[PCMD_BUF_SIZE];
  FILE *fp;        /* File stream returned by popen() */
  bool badPattern; /* Invalid characters in 'pat'? */
  int len, status, fileCnt, j;
  char pathname[PATH_MAX];

  for (;;)
  { /* Read pattern, display results of globbing */
    printf("pattern: ");
    fflush(stdout);

    if (fgets(pat, PAT_SIZE, stdin) == NULL)
      break; /* EOF */

    len = strlen(pat);
    if (len <= 1) /* Empty line */
      continue;

    if (pat[len - 1] == '\n') /* Strip trailing newline */
      pat[len - 1] = '\0';

    // Ensure that the pattern contains only valid characters, i.e., letters,
    // digits, underscore, dot, and the shell globbing characters. (Our
    // definition of valid is more restrictive than the shell, which permits
    // other characters to be included in a filename if they are quoted.)

    // Note also the input checking performed in Listing 44-5 e. This is done to
    // prevent invalid input causing popen() to execute an unexpected shell
    // command. Suppose that these checks were omitted, and the user entered
    // the following input:
    //
    // pattern: ; rm *
    //
    // The program would then pass the following command to popen(), with
    // disastrous results: /bin/ls -d ; rm * 2> /dev/null Such checking of input
    // is always required in programs that use popen() (or system()) to execute
    // a shell command built from user input. (An alternative would be for the
    // application to quote any characters other than those being checked for,
    // so that those characters don’t undergo special processing by the shell.)

    for (j = 0, badPattern = false; j < len && !badPattern; j++)
      if (!isalnum((unsigned char)pat[j]) && strchr("_*?[^-].", pat[j]) == NULL)
        badPattern = true;

    if (badPattern)
    {
      printf("Bad pattern character: %c\n", pat[j - 1]);
      continue;
    }

    /* Build and execute command to glob 'pat' */

    snprintf(popenCmd, PCMD_BUF_SIZE, POPEN_FMT, pat);

    fp = popen(popenCmd, "r");
    if (fp == NULL)
    {
      printf("popen() failed\n");
      continue;
    }

    /* Read resulting list of pathnames until EOF */

    fileCnt = 0;
    while (fgets(pathname, PATH_MAX, fp) != NULL)
    {
      printf("%s", pathname);
      fileCnt++;
    }

    /* Close pipe, fetch and display termination status */

    status = pclose(fp);
    printf("    %d matching file%s\n", fileCnt, (fileCnt != 1) ? "s" : "");
    printf("    pclose() status = %#x\n", (unsigned int)status);

    if (status != -1)
      printWaitStatus("\t", status);
  }

  exit(EXIT_SUCCESS);
}

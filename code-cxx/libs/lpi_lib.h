#ifndef LPI_LIB_H
#define LPI_LIB_H

/* Return a string containing the current time formatted according to
   the specification in 'format' (see strftime(3) for specifiers).
   If 'format' is NULL, we use "%c" as a specifier (which gives the'
   date and time as for ctime(3), but without the trailing newline).
   Returns NULL on error. */

char *currTime(const char *format);
void printWaitStatus(const char *msg, int status);

#endif // LPI_LIB_H

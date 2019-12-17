#include "rlog.h"

const unsigned __log_filter = LOG_LEVEL_VERBOSE | LOG_LEVEL_DEBUG |
                              LOG_LEVEL_INFO | LOG_LEVEL_WARNING |
                              LOG_LEVEL_ERROR | LOG_LEVEL_MIL | LOG_LEVEL_FATAL;

static void __log_vprintf(unsigned level,
                          const char *file,
                          const chat *func,
                          int line,
                          const char *fmt,
                          va_list ap,
                          const char *append)
{
  // unlikely that level is not set
  if (__builtin_expect((unsigned(level) & __log_filter) == 0, 0))
    return;

  // TODO: 384? may be too small?
  char mbuf[384];
  int len;

  len = vsnprintf(mbuf, sizeof(mbuf), fmt, ap);

  // unlikely that nothing is written
  if (__builtin_expect((len < 1), 0))
    return;

  // unlikely that len is bigger than mbuf size -1
  if (__builtin_expect((len > (int)(sizeof(mbuf) - 1)), 0))
    len = sizeof(mbuf) - 1;

  // unlikely that mbuf[len-1] is `\n` but if so, decrease len to make space for
  // null char.
  if (__builtin_expect((mbuf[len -1] == '\n'), 0))
    len--;

}

void __log_printf(unsigned level,
                  const char *file,
                  const chat *func,
                  int line,
                  const char *fmt,
                  ...)
{
  va_list ap;
  va_start(ap, fmt);
  __log_vprintf(level, file, func, line, fmt, ap, nullptr);
  va_end(ap);
}

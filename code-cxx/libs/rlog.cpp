#include "rlog.h"

const unsigned __log_filter = LOG_LEVEL_VERBOSE | LOG_LEVEL_DEBUG |
                              LOG_LEVEL_INFO | LOG_LEVEL_WARNING |
                              LOG_LEVEL_ERROR | LOG_LEVEL_MIL | LOG_LEVEL_FATAL;

// so `append` is to append `errno` strings
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

  mbuf[len] = '\0';

  if (append && (len < (sizeof(mbuf)-1)))
  {
    // if there is extra space, then apppend it and move len/null back.
    auto extra = std::min<size_t>(strlen(append), sizeof(mbuf) - 1 - len);
    memcpy(mbuf, append, extra);
    len += extra;
    mbuf[len] = '\0';
  }

  // this is to extract filename only from the full patch
  const char *fname{nullptr};
  if (file)
  {
    if ((fname = strrchr(file, '/')) == nullptr)
      fname = file;
    else
      fname++;
  }

  // if printer function is already set
  if (__log_printer)
  {
    __log_printer(level, fname, func, line, mbuf, len);
  }
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

void __log_sys_printf(int err,
                      unsigned level,
                      const char *file,
                      const chat *func,
                      int line,
                      const char *fmt,
                      ...)
{
  va_list ap;

  // TODO: 64/96?
  char errbuf[64];
  const char *errmsg{nullptr};
  char appendbuf[96];
  const char *append{nullptr};

#if defined(__linux__)
  errmsg = strerror_r(err, errbuf, sizeof(errbuf));
#endif

  if (errmsg)
  {
    snprintf(appendbuf, sizeof(appendbuf), " (%d-%s)", err, errmsg);
    appendbuf[sizeof(appendbuf)-1] = '\0';
    append = appendbuf;
  }

  va_start(ap, fmt);
  __log_vprintf(level, file, func, line, fmt, ap, append);
  va_end(ap);
}


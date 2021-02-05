#include "rlog.h"
#include <cstdarg>
#include <sys/uio.h> // writev()

const unsigned __log_filter = LOG_LEVEL_VERBOSE | LOG_LEVEL_DEBUG |
                              LOG_LEVEL_INFO | LOG_LEVEL_WARNING |
                              LOG_LEVEL_ERROR | LOG_LEVEL_MIL | LOG_LEVEL_FATAL;

// default printer which output to consolel. it's expected that clients will
// override this with their own version if want to.

static void __default_log_printer(unsigned level,
                                  const char *file,
                                  const char *func,
                                  int line,
                                  const char *message,
                                  int length)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  struct iovec iov[5];
  char header[32];

  iov[0].iov_base = header;
  // micro seconds, 10-6
  iov[0].iov_len = snprintf(header,
                            sizeof(header),
                            "%.010lu.%.06lu ",
                            ts.tv_sec,
                            ts.tv_nsec / 1000);
  iov[0].iov_len = std::min(iov[0].iov_len, sizeof(header));

  switch (level)
  {
    case LOG_LEVEL_FATAL:
      iov[1].iov_base = (void *)"FTL: ";
      iov[1].iov_len  = 5;
      break;

    case LOG_LEVEL_MIL:
      iov[1].iov_base = (void *)"MIL: ";
      iov[1].iov_len  = 5;
      break;

    case LOG_LEVEL_ERROR:
      iov[1].iov_base = (void *)"ERR: ";
      iov[1].iov_len  = 5;
      break;

    case LOG_LEVEL_WARNING:
      iov[1].iov_base = (void *)"WRN: ";
      iov[1].iov_len  = 5;
      break;

    case LOG_LEVEL_INFO:
      iov[1].iov_base = (void *)"NFO: ";
      iov[1].iov_len  = 5;
      break;

    case LOG_LEVEL_DEBUG:
      iov[1].iov_base = (void *)"DBG: ";
      iov[1].iov_len  = 5;
      break;

    default:
      iov[1].iov_base = (void *)": ";
      iov[1].iov_len  = 5;
      break;
  }

  // extract filename
  const char *fname{nullptr};
  if (file)
  {
    if ((fname = strrchr(file, '/')) == nullptr)
      fname = file;
    else
      fname++;
  }

  char body[160];
  iov[2].iov_base = (void *)body;

  // if one of them is null.
  if (!fname || !func || (line <= 0))
  {
    iov[2].iov_len = snprintf(body, sizeof(body), "< M:? F:? L:? > ");
  }
  else
  {
    iov[2].iov_len = snprintf(body,
                              sizeof(body),
                              "< M:%.*s F:%.*s L:%d > ",
                              64,
                              fname,
                              64,
                              func,
                              line);
  }

  iov[2].iov_len = std::min(iov[2].iov_len, sizeof(body));

  iov[3].iov_base = (void *)message;
  iov[3].iov_len  = length;

  iov[4].iov_base = (void *)"\n";
  iov[4].iov_len  = 1;

  // man fileno
  // FERROR(3)
  // The function fileno() examines the argument stream and returns its integer
  // file descriptor.

  writev(fileno(stderr), iov, 5);
}

// set the default log printer to use the android log
static rlog::PrinterFunc __log_printer = __default_log_printer;

// so `append` is to append `errno` strings
static void __log_vprintf(unsigned level,
                          const char *file,
                          const char *func,
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
  if (__builtin_expect((mbuf[len - 1] == '\n'), 0))
    len--;

  mbuf[len] = '\0';

  if (append && (len < (int)(sizeof(mbuf) - 1)))
  {
    // if there is extra space, then apppend it and move len/null back.
    auto extra = std::min<size_t>(strlen(append), sizeof(mbuf) - 1 - len);

    // NOTE: `mbuf + len` since have to add `append` to message. if not, it
    // overwrites message.
    memcpy(mbuf + len, append, extra);

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
                  const char *func,
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
                      const char *func,
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
    appendbuf[sizeof(appendbuf) - 1] = '\0';
    append                           = appendbuf;
  }

  va_start(ap, fmt);
  __log_vprintf(level, file, func, line, fmt, ap, append);
  va_end(ap);
}

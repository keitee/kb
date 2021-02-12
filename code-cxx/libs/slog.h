#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H

#include <cstdio>
#include <cstring>

/*
TODO:
o add a way to set and control level of logging

*/

namespace Slog
{
  // *how-to-get-filename-without-full-path*
  // *cxx-error* on "FILE"
  // #define FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define FILEX (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

  void errMsg(int level,
              const char *file,
              int line,
              const char *function,
              const char *format,
              ...);

  void errExit(int level,
               const char *file,
               int line,
               const char *function,
               const char *format,
               ...);

  void prnMsg(int level,
              const char *file,
              int line,
              const char *function,
              const char *format,
              ...);
  enum
  {
    LOG_LOG,
    LOG_ENTER,
    LOG_EXIT,
    LOG_MIL
  };

} // namespace Slog

/*
note: do not use "\n" in client use:

LOG_MSG("this is func1()");

LOG| F:test_log.cxx C:void func1() L:00119 : this is func1()

*/

#define LOG_ENTER(fmt, ...)                                                    \
  Slog::prnMsg(Slog::LOG_ENTER, FILEX, __LINE__, __PRETTY_FUNCTION__, "")

#define LOG_EXIT(fmt, ...)                                                     \
  Slog::prnMsg(Slog::LOG_EXIT, FILEX, __LINE__, __PRETTY_FUNCTION__, "")

#define LOG_MSG(fmt, ...)                                                      \
  Slog::prnMsg(Slog::LOG_LOG,                                                  \
               FILEX,                                                          \
               __LINE__,                                                       \
               __PRETTY_FUNCTION__,                                            \
               fmt,                                                            \
               ##__VA_ARGS__)

#define LOG_ERR(fmt, ...)                                                      \
  Slog::errMsg(Slog::LOG_MIL,                                                  \
               FILEX,                                                          \
               __LINE__,                                                       \
               __PRETTY_FUNCTION__,                                            \
               fmt,                                                            \
               ##__VA_ARGS__)

#define LOG_ERR_EXIT(fmt, ...)                                                 \
  Slog::errExit(Slog::LOG_MIL,                                                 \
                FILEX,                                                         \
                __LINE__,                                                      \
                __PRETTY_FUNCTION__,                                           \
                fmt,                                                           \
                ##__VA_ARGS__)

#endif // SIMPLE_LOG_H

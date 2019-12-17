#ifndef RLOG_H
#define RLOG_H

#include <cstdio>
#include <cstring>

// from simple log
//
// namespace Slog
// {
//   // <how-to-get-filename-without-full-path>
//   //
//   // #include <string.h>
//   // char *strrchr(const char *s, int c);
//   // The strrchr() function returns a pointer to the last occurrence of the
//   // character c in the string s.
//
//   #define FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 :
//   __FILE__)
//
//   void errMsg(const char *file, int line, const char *function, const char
//   *format, ...); void errExit(const char *file, int line, const char
//   *function, const char *format, ...);
// } // namespace
//
//
// #define LOG_MSG(fmt, ...) \
//  Slog::errMsg(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__) 
//
// #define LOG_ERR(fmt, ...) \
// Slog::errExit(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)
//
// #define LOG_MSG(fmt, ...) \
//  Slog::errMsg(FILE, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__) 
//
// #define LOG_ERR(fmt, ...) \
//  Slog::errExit(FILE, __LINE__,__PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)

// 1. Unlike simple log, use macro
// 2. __builtin_expect(exp, 0) means `unlikely`
// 3. Unlike LPI log, use errno in the call argument

// log category
#define LOG_LEVEL_FATAL         0x01
#define LOG_LEVEL_ERROR         0x02
#define LOG_LEVEL_WARNING       0x04
#define LOG_LEVEL_MIL           0x08
#define LOG_LEVEL_INFO          0x10
#define LOG_LEVEL_DEBUG         0x20
#define LOG_LEVEL_VERBOSE       0x40

extern const unsigned __log_filter;

extern __log_printf(unsigned level,
                    const char *file,
                    const chat *func,
                    int line,
                    const char *fmt,
                    ...) __attribute__((format(printf, 5, 6)));

extern __log_sys_printf(int err,
                        unsigned level,
                        const char *file,
                        const chat *func,
                        int line,
                        const char *fmt,
                        ...) __attribute__((format(printf, 6, 7)));

// supports errno
#define __LOG_SYS_PRINTF(err, level, fmt, ...)                                 \
  do                                                                           \
  {                                                                            \
    if (__builtin_expect((unsigned(level) & __log_filter), 0))                 \
    {                                                                          \
      __log_sys_printf((err),                                                  \
                       (level),                                                \
                       __FILE__,                                               \
                       __FUNCTION__,                                           \
                       __LINE__,                                               \
                       fmt,                                                    \
                       ##__VA_ARGS__);                                         \
    }                                                                          \
  } while (0)

// no errno
#define __LOG_PRINTF(level, fmt, ...)                                          \
  do                                                                           \
  {                                                                            \
    if (__builtin_expect((unsigned(level) & __log_filter), 0))                 \
    {                                                                          \
      __log_printf((level),                                                    \
                   __FILE__,                                                   \
                   __FUNCTION__,                                               \
                   __LINE__,                                                   \
                   fmt,                                                        \
                   ##__VA_ARGS__);                                             \
    }                                                                          \
  } while (0)

#define logSysError(err, fmt, ...)                                             \
  __LOG_SYS_PRINTF(err, LOG_LEVEL_ERROR, fmt.##__VA_ARGS__)

#endif // RLOG_H

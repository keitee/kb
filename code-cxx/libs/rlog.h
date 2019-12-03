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
//   #define FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
// 
//   void errMsg(const char *file, int line, const char *function, const char *format, ...);
//   void errExit(const char *file, int line, const char *function, const char *format, ...);
// } // namespace
// 
//
// #define LOG_MSG(fmt, ...) Slog::errMsg(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)
// #define LOG_ERR(fmt, ...) Slog::errExit(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)
//
// #define LOG_MSG(fmt, ...) Slog::errMsg(FILE, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)
// #define LOG_ERR(fmt, ...) Slog::errExit(FILE, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)


// Unlike simple log, use macro

#define __LOG_SYS_PRINTF(err, level, fmt, ...) \
  do { \
    if (__builtin_expect((unsigned(level) & __log_filter), 0)) \
    { \
      __log_sys_printf((err), (level), __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__); \
    } \
  } while(0)


#define logSysError(err, fmt, ...) \
  __LOG_SYS_PRINTF(err, LOG_LEVEL_ERROR, fmt. ##__VA_ARGS__)

#endif // RLOG_H

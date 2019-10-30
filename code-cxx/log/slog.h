#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H

#include <cstdio>
#include <cstring>

namespace Slog
{
  // <how-to-get-filename-without-full-path>
  //
  // #include <string.h>
  // char *strrchr(const char *s, int c);
  // The strrchr() function returns a pointer to the last occurrence of the
  // character c in the string s.

  #define FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

  void errMsg(const char *file, int line, const char *function, const char *format, ...);
  void errExit(const char *file, int line, const char *function, const char *format, ...);
} // namespace

// #define LOG_MSG(fmt, ...) Slog::errMsg(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)
// #define LOG_ERR(fmt, ...) Slog::errExit(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_MSG(fmt, ...) Slog::errMsg(FILE, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) Slog::errExit(FILE, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)

#endif // SIMPLE_LOG_H

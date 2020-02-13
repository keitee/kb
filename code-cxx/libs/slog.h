#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H

#include <cstdio>
#include <cstring>

namespace Slog
{
  // *how-to-get-filename-without-full-path*
  // *cxx-error* on "FILE"
  // #define FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

  #define FILEX (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

  void errMsg(
    const char *file, int line, const char *function, const char *format, ...);
  void errExit(
    const char *file, int line, const char *function, const char *format, ...);
} // namespace Slog

// #define LOG_MSG(fmt, ...) Slog::errMsg(__FILE__, __LINE__,
// __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__) #define LOG_ERR(fmt, ...)
// Slog::errExit(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_MSG(fmt, ...)                                                      \
  Slog::errMsg(FILEX, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...)                                                      \
  Slog::errExit(FILEX, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)

#endif // SIMPLE_LOG_H

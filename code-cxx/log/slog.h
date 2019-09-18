#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H

namespace Slog
{
  void errMsg(const char *format, ...);
  void errExit(const char *format, ...);
} // namespace

#define LOG_MSG Slog::errMsg
#define LOG_ERR Slog::errExit

#endif // SIMPLE_LOG_H

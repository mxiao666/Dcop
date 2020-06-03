#ifndef _LOG_H_
#define _LOG_H_

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
enum
{
    LL_DEBUG,
    LL_INFO,
    LL_WARNING,
    LL_ERROR,
    LL_FATAL_ERROR,
    LL_TRACE,
    LL_LEVEL_NUM
};
int WriteLog(int v_level, int line,
             const char *func, const char *file, const char *format, ...);
int LogInit(int level = LL_INFO, const char *path = "message");
void LogDestory();
int GetLogLevel();
int SetLogLevel(int level);
int GetMethod();
int SetMethod(bool method);
int LVOS_Printf(int fd, const char *format, ...);
#define LVOS_Log(level, ...) \
    WriteLog(level, __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define LOG_DBG(format, ...)                   \
    WriteLog(LL_DEBUG, __LINE__, __FUNCTION__, \
             __FILE__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)                 \
    WriteLog(LL_INFO, __LINE__, __FUNCTION__, \
             __FILE__, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)                    \
    WriteLog(LL_WARNING, __LINE__, __FUNCTION__, \
             __FILE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)                 \
    WriteLog(LL_ERROR, __LINE__, __FUNCTION__, \
             __FILE__, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...)                       \
    WriteLog(LL_FATAL_ERROR, __LINE__, __FUNCTION__, \
             __FILE__, format, ##__VA_ARGS__)
#define LOG_TRACE(format, ...)                 \
    WriteLog(LL_TRACE, __LINE__, __FUNCTION__, \
             __FILE__, format, ##__VA_ARGS__)
#endif

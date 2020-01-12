#ifndef _LOG_H_
#define _LOG_H_

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#define LL_DEBUG 0x01
#define LL_INFO 0x02
#define LL_WARNING 0x03
#define LL_ERROR 0x04
#define LL_FATAL_ERROR 0x05
int WriteLog(int v_level, int line, const char *func, const char *file, const char *format, ...);
int LogInit(int level = LL_INFO, const char *path = "message");

#define LVOS_Log(level, ...) WriteLog(level, __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define LOG_DBG(format, ...) \
    WriteLog(LL_DEBUG, __LINE__, __FUNCTION__, __FILE__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) \
    WriteLog(LL_INFO, __LINE__, __FUNCTION__, __FILE__, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) \
    WriteLog(LL_WARNING, __LINE__, __FUNCTION__, __FILE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) \
    WriteLog(LL_ERROR, __LINE__, __FUNCTION__, __FILE__, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) \
    WriteLog(LL_FATAL_ERROR, __LINE__, __FUNCTION__, __FILE__, format, ##__VA_ARGS__)
#endif

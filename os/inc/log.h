#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int LVOS_Printf(int fd, const char *format, ...);

#ifndef __WIN32__
#include <elog.h>

#undef LOG_TAG
#define LOG_TAG "LiteOS"

#define LVOS_ASSERT ELOG_ASSERT
#define LOG_DBG log_d
#define LOG_INFO log_i
#define LOG_WARN log_w
#define LOG_ERROR log_e
#define LOG_FATAL LOG_ERROR
#define LOG_TRACE log_v
#define LOG_ASSERT log_a
#else
int Win_Printf(const char *format, ...);
#define LVOS_ASSERT Win_Printf
#define LOG_DBG Win_Printf
#define LOG_INFO Win_Printf
#define LOG_WARN Win_Printf
#define LOG_ERROR Win_Printf
#define LOG_FATAL Win_Printf
#define LOG_TRACE Win_Printf
#define LOG_ASSERT Win_Printf
#endif

#endif

#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <elog.h>

#undef LOG_TAG
#define LOG_TAG "LiteOS"

int LVOS_Printf(int fd, const char *format, ...);
#define LVOS_ASSERT ELOG_ASSERT
#define LOG_DBG     log_d
#define LOG_INFO    log_i
#define LOG_WARN    log_w
#define LOG_ERROR   log_e
#define LOG_FATAL   LOG_ERROR
#define LOG_TRACE   log_v
#define LOG_ASSERT  log_a
#endif

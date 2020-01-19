/***********************************************************************************
 * ??????  : os_log.c
 * ??????  : ¬��?? * ��������   : 2018??1??3?? * �ļ�����   : ��־��¼ʵ��?? * ��Ȩ˵��   : Copyright (c) 2008-2018   xx xx xx xx �������޹�?? * ??   ??  : 
 * �޸���־   : 
***********************************************************************************/

#include "log.h"
#include "stacktrace.h"
#include "macro_define.h"
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#ifndef __WIN32__
#include <unistd.h>
#include <functional>
#endif
const int LOG_MESSAGE_LEN = 512;
const int LOG_CONTENT_LEN = 64;

static char LogLevelStr[][8] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

static FILE *log_file;
static int log_level = LL_INFO;

int LogInit(int level, const char *path)
{
    log_level = level;
    log_file = fopen(path, "at+");
    if (NULL == log_file)
    {
        return RET_ERR;
    }
    setvbuf(log_file, NULL, _IOLBF,
#ifndef __WIN32__
            0
#else
            512
#endif
    ); /* �л�??*/
    debug_backtrace_init();
    return RET_OK;
}

int WriteLog(int v_level, int line, const char *func, const char *file, const char *format, ...)
{
    if (log_file == nullptr)
        return RET_ERR;
    if (v_level > LL_FATAL_ERROR || v_level < LL_DEBUG)
        return RET_ERR;
    if (log_level > v_level)
        return RET_ERR;

    /* ---ʱ��??-- */
    char log_time[LOG_CONTENT_LEN] = {0};
    time_t t = time(NULL);
    struct tm ptm;
#ifndef __WIN32__
    localtime_r(&t, &ptm);
#else
    localtime_s(&ptm, &t);
#endif
    snprintf(log_time, LOG_CONTENT_LEN - 1, "%4d-%02d-%02d %02d:%02d:%02d",
             ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour, ptm.tm_min, ptm.tm_sec);

    /* ---�ļ�??--�к�---����??--- */
    char log_pos[LOG_CONTENT_LEN] = {0};
    const char *linuxpos = strrchr(file, '/');
    const char *winwpso = strrchr(file, '\\');
    snprintf(log_pos, LOG_CONTENT_LEN - 1, " [%s] [%s:%d] [%s] ", LogLevelStr[--v_level],
             (NULL != linuxpos)
                 ? (linuxpos + 1)
                 : (NULL != winwpso)
                       ? (winwpso + 2)
                       : file,
             line, func);

    /* ---��־����--- */
    char log_msg[LOG_MESSAGE_LEN] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    int nWrittenBytes = vsnprintf(log_msg, sizeof(log_msg), format, arg_ptr);
    if (nWrittenBytes < 0)
    {
        //perror("vsnprintf");
        return RET_ERR;
    }
    va_end(arg_ptr);

    /* ---������־ƴ��--- */
    fprintf(log_file, "%s%s%s\n", log_time, log_pos, log_msg);
    return nWrittenBytes;
}

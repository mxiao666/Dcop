/***********************************************************************************
 * 文 件 名   : os_log.c
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月23日
 * 文件描述   : 日志记录实现类
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#include "os_log.h"
#include "os_stacktrace.h"
#include "os_macro_define.h"
#include <sys/file.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <functional>   
const int LOG_MESSAGE_LEN = 512;
const int LOG_CONTENT_LEN = 64;

static char LogLevelStr[][8] = {
    "DEBUG", "INFO", "WARN", "ERROR","FATAL" 
};
	 
static FILE *log_file;
static int log_level = LL_INFO;
#define GETFILENAME(pFileName) (NULL == strrchr(pFileName, '/') ? pFileName : strrchr(pFileName, '/') + 1)

int LogInit(int level, const char *path)
{
    log_level = level;
    log_file = fopen(path, "at+");
    if (NULL == log_file)
    {
        return RET_ERR;
    }
    setvbuf(log_file, NULL, _IOLBF, 0);  /* 行缓冲 */
	debug_backtrace_init();
    return RET_OK;
}
 
int WriteLog(int v_level,int line, const char *func, const char *file, const char * format, ...)
{
    if (v_level > LL_FATAL_ERROR || v_level < LL_DEBUG){return RET_ERR;}
    if (log_level > v_level){return RET_ERR;}
 
    /* ---时间戮--- */
    char log_time[LOG_CONTENT_LEN] = {0};
    time_t t = time(NULL);
    struct tm ptm;
    localtime_r(&t, &ptm);
    sprintf(log_time, "%4d-%02d-%02d %02d:%02d:%02d",
            ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour, ptm.tm_min, ptm.tm_sec);
	
     /* ---文件名---行号---函数名---- */
    char log_pos[LOG_CONTENT_LEN] = {0};
    sprintf(log_pos, " [%s] [%s:%d] [%s] ", LogLevelStr[--v_level], GETFILENAME(file), line, func);
	
    /* ---日志内容--- */
    char log_msg[LOG_MESSAGE_LEN] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    int nWrittenBytes = vsnprintf(log_msg,sizeof(log_msg), format, arg_ptr);
    if (nWrittenBytes < 0)
    {
        //perror("vsnprintf");
        return RET_ERR;
    }
    va_end(arg_ptr);
 
    /* ---完整日志拼接--- */
    fprintf(log_file, "%s%s%s\n", log_time, log_pos, log_msg);
    return nWrittenBytes;
}


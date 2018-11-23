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
#include "os_log_macro_define.h"
#include <sys/file.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

static char LogLevelStr[][8] = {
    "DEBUG", "TRACE", "NOTICE", "WARN","ERROR" 
};

Log_Writer INFO_W;
__thread char Log_Writer::m_buffer[_LOG_BUFFSIZE] = {0};

/*****************************************************************************
 * 函 数 名  : log_init
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : C方式调用初始化日志类
 * 输入参数  : LogLevel eLevel           日志等级
               const char* p_modulename  日志记录文件名
               const char* p_logdir      日志文件所在位置
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
bool log_init(LogLevel eLevel, const char* p_modulename, const char* p_logdir)
{
	//如果路径存在文件夹，则判断是否存在
	if (access (p_logdir, 0) == -1)
	{
#if _DEBUG_	
		if (mkdir (p_logdir, S_IREAD | S_IWRITE ) < 0)
			fprintf(stderr, "create folder failed\n");
#else
        return false;
#endif
	}
	char _location_str[_LOG_PATH_LEN];
	snprintf(_location_str, _LOG_PATH_LEN, "%s/%s.access", p_logdir, p_modulename);	
	INFO_W.loginit(eLevel, _location_str); 
    (void)debug_backtrace_init();
	return true;
}

/*****************************************************************************
 * 函 数 名  : Log_Writer.logLevelToString
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 日志等级到对应字符串
 * 输入参数  : LogLevel eLevel  日志等级
 * 输出参数  : 无
 * 返 回 值  : inline
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
inline const char* Log_Writer::logLevelToString(LogLevel eLevel) 
{
    if(LL_DEBUG > eLevel || LL_ERROR < eLevel)
    {
        return "UNKNOWN";
    }
    return (char*)LogLevelStr[eLevel];
}

/*****************************************************************************
 * 函 数 名  : Log_Writer.IsValidLevel
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 对应日志等级是否在配置等级内
 * 输入参数  : LogLevel eLevel  日志等级
 * 输出参数  : 无
 * 返 回 值  : inline
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
inline bool Log_Writer::IsValidLevel(LogLevel eLevel)
{
	return(eLevel >= m_system_level);
}

/*****************************************************************************
 * 函 数 名  : Log_Writer.Init
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 多带调用初始化日志类
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : int
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int Log_Writer::Init()
{
    return log_init(LL_TRACE, "messsage", "./");
}
/*****************************************************************************
 * 函 数 名  : Log_Writer.loginit
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 初始化日志类
 * 输入参数  : LogLevel eLevel            日志等级
               const  char *filelocation  记录日志文件名
               bool append                是否追加方式写入
               bool issync                是否同步写入
 * 输出参数  : 无
 * 返 回 值  : bool
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
bool Log_Writer::loginit(LogLevel eLevel, const  char *filelocation, bool append, bool issync)
{
	MACRO_RET(NULL != m_fp, false);
    m_system_level = eLevel;
    m_isappend = append; 
    m_issync = issync; 
	if(strlen(filelocation) >= (sizeof(m_filelocation) -1))
	{
#if _DEBUG_		
		fprintf(stderr, "the path of log file is too long:%lu limit:%lu\n", strlen(filelocation), strlen(m_filelocation) -1);
#endif
		exit(0);
	}
	//本地存储filelocation  以防止在栈上的非法调用调用
	strncpy(m_filelocation, filelocation, sizeof(m_filelocation));
	m_filelocation[sizeof(m_filelocation) -1] = '\0';
	
	if('\0' == m_filelocation[0])
	{
		m_fp = stdout;
#if _DEBUG_	        
		fprintf(stderr, "now all the running-information are going to put to stderr\n");
#endif
		return true;
	}
	
	m_fp = fopen(m_filelocation, append ? "a":"w");
	if(m_fp == NULL)
	{
#if _DEBUG_		
		fprintf(stderr, "cannot open log file,file location is %s\n", m_filelocation);
#endif
		exit(0);
	}
	//setvbuf (m_fp, io_cached_buf, _IOLBF, sizeof(io_cached_buf)); //buf set _IONBF  _IOLBF  _IOFBF
	setvbuf (m_fp,  (char *)NULL, _IOLBF, 0);
#if _DEBUG_	    
	fprintf(stderr, "now all the running-information are going to the file %s\n", m_filelocation);
#endif
	return true;
}

/*****************************************************************************
 * 函 数 名  : Log_Writer.premakestr
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 封装日期字符串
 * 输入参数  : char* m_buffer   封装日期字符串
               LogLevel eLevel  日志等级
 * 输出参数  : 无
 * 返 回 值  : int
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int Log_Writer::premakestr(char* m_buffer, LogLevel eLevel)
{
    const byte dateStrLen = 20;
    char timestr[dateStrLen] = {0};
    time_t timer = time(NULL); 
    struct tm now_time;    
    strftime(timestr, dateStrLen, "%Y-%m-%d %H:%M:%S", localtime_r(&timer, &now_time));    
    return snprintf(m_buffer, _LOG_BUFFSIZE, "%s [%s]", timestr, logLevelToString(eLevel));
}

/*****************************************************************************
 * 函 数 名  : Log_Writer.log
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 写日志入口
 * 输入参数  : LogLevel eLevel  日志等级
               char* logformat  日志记录格式
               ...              参数列表
 * 输出参数  : 无
 * 返 回 值  : bool
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
bool Log_Writer::log(LogLevel eLevel, char* logformat,...)
{
	MACRO_RET(!IsValidLevel(eLevel), false);
	int _size;
	int prestrlen = 0;
	
	char * star = m_buffer;
	prestrlen = premakestr(star, eLevel);
	star += prestrlen;
	
	va_list args;
	va_start(args, logformat);
	_size = vsnprintf(star, _LOG_BUFFSIZE - prestrlen, logformat, args);
	va_end(args);
	
	if(NULL == m_fp)
#if _DEBUG_	        
		fprintf(stderr, "%s", m_buffer);
#else
    (void*) m_buffer;
#endif
	else
		_write(m_buffer, prestrlen + _size);
	return true;
}

/*****************************************************************************
 * 函 数 名  : Log_Writer._write
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 写入格式化后的日志
 * 输入参数  : char *_pbuffer  写入封装后的流
               int len         参数长度
 * 输出参数  : 无
 * 返 回 值  : bool
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
bool Log_Writer::_write(char *_pbuffer, int len)
{
	if(0 != access(m_filelocation, W_OK))
	{	
		pthread_mutex_lock(&m_mutex);
		//锁内校验 access 看是否在等待锁过程中被其他线程loginit了  避免多线程多次close 和init
		if(0 != access(m_filelocation, W_OK))
		{
			logclose();
			loginit(m_system_level, m_filelocation, m_isappend, m_issync);
		}
		pthread_mutex_unlock(&m_mutex);
	}

	if(1 == fwrite(_pbuffer, len, 1, m_fp)) //only write 1 item
	{
		if(m_issync)
          	fflush(m_fp);
		*_pbuffer='\0';
    }
    else 
	{
#if _DEBUG_	
        int x = errno;
	    fprintf(stderr, "Failed to write to logfile. errno:%s    message:%s", strerror(x), _pbuffer);
#endif        
	    return false;
	}
	return true;
}

/*****************************************************************************
 * 函 数 名  : Log_Writer.getLevel
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 日志等级你获取
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : LogLevel
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
LogLevel inline Log_Writer::getLevel()
{
	return m_system_level; 
}

/*****************************************************************************
 * 函 数 名  : Log_Writer.logclose
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 关闭日志
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : bool
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
bool Log_Writer::logclose()
{
	if(m_fp == NULL)
		return false;
	fflush(m_fp);
	fclose(m_fp);
	m_fp = NULL;
	return true;
}
REG_FUNCTION(Log_Writer, "Log_Writer");


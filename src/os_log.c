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
#include "os_macro_define.h"
#include <sys/file.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <functional>   
const int LOG_MESSAGE_LEN = 512;
const int LOG_CONTENT_LEN = 256;
Logger* Logger::m_Logger = NULL;

static char LogLevelStr[][8] = {
    "DEBUG", "TRACE", "NOTICE", "WARN","ERROR" 
};
  
void Logger::SetFileName(const char* filename)  
{  
  m_FileName = filename;  
}

bool Logger::Start()  
{  
    if (m_FileName.empty())  
    {  
        time_t time_seconds = time(NULL);
		struct tm now_time = {0};
		const BYTE FileNameLength = 16;
        char timestr[FileNameLength] = { 0 };
		strftime(timestr, FileNameLength, "%Y%m%d%H%M%S", localtime_r(&time_seconds,&now_time));       
        m_FileName = timestr;  
    }  
  
    m_Fp = fopen(m_FileName.c_str(), "at+");  
    if (m_Fp == NULL)  
        return false;  
  
    m_SPthread.reset(new std::thread(std::bind(&Logger::threadfunc, this)));  
    debug_backtrace_init();
    return true;  
}  
  
void Logger::Stop()  
{  
    m_Exit = true;  
    m_Notify.notify_one();  
  
    //等待时间线程结束  
    m_SPthread->join();  
}  
 inline const char* Logger::logLevelToString(LogLevel eLevel) 
{
    if(LL_DEBUG > eLevel || LL_ERROR < eLevel)
    {
        return "UNKNOWN";
    }
    return (char*)LogLevelStr[eLevel];
} 
Logger* Logger::GetInstance()  
{  
   if(NULL == m_Logger)
   	{
   		m_Logger = new Logger();
   	}
    return m_Logger;  
} 
inline bool Logger::IsValidLevel(LogLevel eLevel)
{
	return(eLevel >= m_system_level);
}
bool Logger::isEmpty()
{
	return m_Queue.empty();
}

void Logger::AddToQueue(LogLevel pszLevel, const char* pszFile, int lineNo, const char* pszFuncSig, char* pszFmt, ...)  
{  
    char msg[LOG_CONTENT_LEN] = { 0 };  
  
    va_list vArgList;                              
    va_start(vArgList, pszFmt);  
    vsnprintf(msg, LOG_CONTENT_LEN, pszFmt, vArgList);  
    va_end(vArgList);  
  
    time_t now = time(NULL);  
    struct tm  now_time = {0};
	(void)localtime_r(&now,&now_time);  
    char content[LOG_MESSAGE_LEN] = { 0 };  
    sprintf(content, "[%04d-%02d-%02d %02d:%02d:%02d][%s][0x%04x][%s:%d %s]%s\n",  
                now_time.tm_year + 1900,  
                now_time.tm_mon + 1,  
                now_time.tm_mday,  
                now_time.tm_hour,  
                now_time.tm_min,  
                now_time.tm_sec,  
                logLevelToString(pszLevel),  
                std::this_thread::get_id(),  
                pszFile,  
                lineNo,  
                pszFuncSig,  
                msg);  
  
    {  
        std::lock_guard<std::mutex> guard(m_Mutex);  
        m_Queue.emplace_back(content);  
    }  
      
    m_Notify.notify_one();  
}  
  
void Logger::threadfunc()  
{  
    if (m_Fp == NULL)  
        return;  
  
    while (!m_Exit)  
    {  
        //写日志  
        std::unique_lock<std::mutex> guard(m_Mutex); 
        while (m_Queue.empty())  
        {  
            if (m_Exit)  
                return;  
  
            m_Notify.wait(guard);  
        }  
  
        //写日志  
        const std::string& str = m_Queue.front();  
  
        fwrite((void*)str.c_str(), str.length(), 1, m_Fp);  
        fflush(m_Fp);  
        m_Queue.pop_front();  
    }  
}


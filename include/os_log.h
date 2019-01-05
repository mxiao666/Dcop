/***********************************************************************************
 * 文 件 名   : os_log.h
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月23日
 * 文件描述   : 日志记录类
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/
#ifndef __LOGGER_H__  
#define __LOGGER_H__   
#include "os_stacktrace.h"
#include <string>  
#include <memory>  
#include <thread>  
#include <mutex>  
#include <condition_variable>  
#include <list>  
  
/*****************************************************************************
 * 函 数 名  : LogLevel
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 日志等级
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : typedef
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
typedef  enum LogLevel {  
	LL_DEBUG,
	LL_TRACE,
	LL_NOTICE, 
	LL_WARNING , 
	LL_ERROR
}LogLevel;
                                                                          
class Logger  
{  
public:  
    static Logger* GetInstance();   
    bool Start();  
    void Stop();  
    void SetFileName(const char* filename);  
    void AddToQueue(LogLevel pszLevel, const char* pszFile, int lineNo, const char* pszFuncSig, char* pszFmt, ...);  
    bool isEmpty();
private:
    inline const char*logLevelToString(LogLevel eLevel);
    inline bool IsValidLevel(LogLevel eLevel);
    Logger() = default;  
    Logger(const Logger& rhs) = delete;  
    Logger& operator =(Logger& rhs) = delete;  
  
    void threadfunc();  
  
  
private:  
    std::string                     m_FileName{"message"};  
    FILE*                           m_Fp{0};  
    std::shared_ptr<std::thread>    m_SPthread;  
    std::mutex                      m_Mutex;  
    std::condition_variable         m_Notify;            //有新的日志到来的标识  
    bool                            m_Exit{false};  
    std::list<std::string>          m_Queue;
    static Logger*                  m_Logger;
    LogLevel                        m_system_level;
};  
  
#endif //!__LOGGER_H__ 


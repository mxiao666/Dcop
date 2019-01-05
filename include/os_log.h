/***********************************************************************************
 * �� �� ��   : os_log.h
 * �� �� ��   : ¬����
 * ��������   : 2018��11��23��
 * �ļ�����   : ��־��¼��
 * ��Ȩ˵��   : Copyright (c) 2008-2018   xx xx xx xx �������޹�˾
 * ��    ��   : 
 * �޸���־   : 
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
 * �� �� ��  : LogLevel
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ��־�ȼ�
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : typedef
 * ���ù�ϵ  : 
 * ��    ��  : 

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
    std::condition_variable         m_Notify;            //���µ���־�����ı�ʶ  
    bool                            m_Exit{false};  
    std::list<std::string>          m_Queue;
    static Logger*                  m_Logger;
    LogLevel                        m_system_level;
};  
  
#endif //!__LOGGER_H__ 


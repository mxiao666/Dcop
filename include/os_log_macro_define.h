/***********************************************************************************
 * 文 件 名   : os_log_macro_define.h
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月23日
 * 文件描述   : 日志记录宏扩展
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef  __OS_LOG_MACRO_DEFINE__
#define  __OS_LOG_MACRO_DEFINE__
#include <string.h>
#include "os_log.h"
#define GETFILENAME(pFileName) (NULL == strrchr(pFileName, '/') ? pFileName : strrchr(pFileName, '/') + 1)
/*****************************************************************************
 * 函 数 名  : LVOS_Log
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 全局日志封装
 * 输入参数  : eLeve       日志等级
               log_fmt     日志格式
               log_arg...  参数
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
#define LVOS_Log(eLeve,...) \
    do{ \
        Logger::GetInstance()->AddToQueue(eLeve, GETFILENAME(__FILE__), __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__);\
    } while (0) 
        
    /*带有超频限制的日志打印函数*/
#define OS_PRINT_LIMIT_WITH_LEVEL(eLeve,interval,burst,log_fmt, log_arg...) \
        do{ \
            static u32 uiMissed = 0; \
            static u32 uiPrinted = 0; \
            static s64 uiLast = 0; \
            s64 uiNow = time(NULL);\
            /*如果两次打印时间大于设定的时间，打印被屏蔽的日志条数*/\
            if(uiNow >= (uiLast + (interval)))\
             { \
                if(uiPrinted > 0)\
                {\
                    LVOS_Log(LL_NOTICE,"%d repeat logs was suppressed.",uiPrinted); \
                }\
                uiPrinted = 0; \
                uiMissed = 0; \
            }\
            uiLast = uiNow; \
            if(uiPrinted < (burst)) \
            { \
                uiPrinted++; \
                LVOS_Log(eLeve,log_fmt, ##log_arg); \
            } \
            else \
            { \
                uiMissed++; \
                LVOS_Log(LL_DEBUG,log_fmt, ##log_arg); \
            } \
        }while(0)

            //============basic===================
            
#define LOG_ERROR(...) \
            do{ \
                Logger::GetInstance()->AddToQueue(LL_ERROR, GETFILENAME(__FILE__), __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__);\
            } while (0) 
            
#define LOG_WARN(...) \
            do{ \
                Logger::GetInstance()->AddToQueue(LL_WARNING, GETFILENAME(__FILE__), __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__);\
            } while (0) 
            
#define LOG_NOTICE(...) \
            do{ \
                Logger::GetInstance()->AddToQueue(LL_NOTICE, GETFILENAME(__FILE__), __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__);\
            } while (0) 
            
#define LOG_TRACE(...) \
            do{ \
                Logger::GetInstance()->AddToQueue(LL_TRACE, GETFILENAME(__FILE__), __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__);\
            } while (0) 
            
#define LOG_DEBUG(...) \
            do{ \
                Logger::GetInstance()->AddToQueue(LL_DEBUG, GETFILENAME(__FILE__), __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__);\
            } while (0) 
            
            //============extend===================
#define MACRO_RET(condition, return_val) {\
            if (condition) {\
                return return_val;\
            }\
        }
            
#define MACRO_WARN(condition, log_fmt, log_arg...) {\
            if (condition) {\
                LOG_WARN( log_fmt, ##log_arg);\
            }\
        }
            
#define MACRO_WARN_RET(condition, return_val, log_fmt, log_arg...) {\
            if ((condition)) {\
                LOG_WARN( log_fmt, ##log_arg);\
                return return_val;\
            }\
        }

#endif

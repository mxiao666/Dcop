#ifndef  __OS_LOG_MACRO_DEFINE__
#define  __OS_LOG_MACRO_DEFINE__
#include <string.h>
#include "os_log.h"

#define GETFILENAME(pFileName) (NULL == strrchr(pFileName, '/') ? pFileName : strrchr(pFileName, '/') + 1)
#define OS_PLUG_LOG(eLeve,log_fmt, log_arg...) \
    do{ \
           INFO_W.log(eLeve, "[%s:%d][%s] " log_fmt "\n", \
                     GETFILENAME(__FILE__), __LINE__, __FUNCTION__, ##log_arg); \
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
                    OS_PLUG_LOG(LL_NOTICE,"%d repeat logs was suppressed.",uiPrinted); \
                }\
                uiPrinted = 0; \
                uiMissed = 0; \
            }\
            uiLast = uiNow; \
            if(uiPrinted < (burst)) \
            { \
                uiPrinted++; \
                OS_PLUG_LOG(eLeve,log_fmt, ##log_arg); \
            } \
            else \
            { \
                uiMissed++; \
                OS_PLUG_LOG(LL_DEBUG,log_fmt, ##log_arg); \
            } \
        }while(0)

            //============basic===================
            
#define LOG_ERROR(log_fmt, log_arg...) \
            do{ \
                INFO_W.log(LL_ERROR,   "[%s:%d][%s] " log_fmt "\n", \
                             GETFILENAME(__FILE__), __LINE__, __FUNCTION__, ##log_arg); \
            } while (0) 
            
#define LOG_WARN(log_fmt, log_arg...) \
            do{ \
                INFO_W.log(LL_WARNING,   "[%s:%d][%s] " log_fmt "\n", \
                             GETFILENAME(__FILE__), __LINE__, __FUNCTION__, ##log_arg); \
            } while (0) 
            
#define LOG_NOTICE(log_fmt, log_arg...) \
            do{ \
                INFO_W.log(LL_NOTICE,   "[%s:%d][%s] " log_fmt "\n", \
                             GETFILENAME(__FILE__), __LINE__, __FUNCTION__, ##log_arg); \
            } while (0) 
            
#define LOG_TRACE(log_fmt, log_arg...) \
            do{ \
                INFO_W.log(LL_TRACE,   "[%s:%d][%s] " log_fmt "\n", \
                             GETFILENAME(__FILE__), __LINE__, __FUNCTION__, ##log_arg); \
            } while (0) 
            
#define LOG_DEBUG(log_fmt, log_arg...) \
            do{ \
                INFO_W.log(LL_DEBUG,   "[%s:%d][%s] " log_fmt "\n", \
                             GETFILENAME(__FILE__), __LINE__, __FUNCTION__, ##log_arg); \
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

#ifndef __OBJBASE_H__
#define __OBJBASE_H__
#include <atomic>
#include <stdio.h>
#include <condition_variable>
#include "cargclist.h"
#include "frameworkmgr.h"
#include <log.h>
typedef int (*Printfun)(int fd, const char *format, ...);

class objbase
{
private:
    std::atomic<int> m_debug;

public:
    std::mutex m_synclock;

public:
    objbase() : m_debug(0) {}
    virtual ~objbase() {}
    virtual int Init() { return 0; };
    virtual void dump(int fd = 0, Printfun callback = LVOS_Printf);
    int GetDebug() { return m_debug; }
    int SetDebug(int value) { return (m_debug = value); }
    virtual void PrintHead(int fd, Printfun callback,
                           const char *tableName, int tatol, char split = '-');
    virtual void PrintEnd(int fd, Printfun callback,
                          int tatol, char split = '-');
    virtual int Process(CAgrcList *message,
                        RspMsg *outmessage, int iModule, int iCmd) { return 0; };
};
REG_FRAMEWORK(objbase)
#define REG_FUNCTION_PLUS(Class) \
    FRAMEWORK_REG_FUNCTION(objbase, Class)

#define PROCESS_BEGIN                             \
    std::lock_guard<std::mutex> lock{m_synclock}; \
    int iRet = 0;                                 \
    switch (iCmd)                                 \
    {

#define PROCESS_CALL(cmd, func)                            \
    case cmd:                                              \
        iRet = (func)(message, outmessage, iModule, iCmd); \
        break;

#define PROCESS_END \
    default:        \
        iRet = -1;  \
        break;      \
        }           \
        return iRet;
#endif

#ifndef __OBJBASE_H__
#define __OBJBASE_H__
#include <atomic>
#include <stdio.h>
#include "cargclist.h"
#include "frameworkmgr.h"
typedef int (*Printfun)(const char *format, ...);

class objbase
{
private:
    std::atomic<int> m_debug;

public:
    objbase() : m_debug(0) {}
    virtual ~objbase() {}
    virtual int Init() { return 0; };
    virtual void dump(Printfun callback = printf);
    int GetDebug() { return m_debug; }
    int SetDebug(int value) { return (m_debug = value); }
    virtual void PrintHead(Printfun callback, const char *tableName, int tatol, char split = '-');
    virtual void PrintEnd(Printfun callback, int tatol, char split = '-');
    virtual int Process(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd) { return 0; };
};
REG_FRAMEWORK(objbase)
#define REG_FUNCTION_PLUS(Class) \
    FRAMEWORK_REG_FUNCTION(objbase, Class)
#endif

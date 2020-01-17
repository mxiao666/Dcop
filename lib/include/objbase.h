#ifndef __OBJBASE_H__
#define __OBJBASE_H__
#include <atomic>
#include <stdio.h>
#include "frameworkmgr.h"
typedef int (*Printfun)(const char *format, ...);
//Printfun g_printf = printf;
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
};
REG_FRAMEWORK(objbase, objbase)
#define REG_FUNCTION_PLUS(Class, Name) \
    FRAMEWORK_REG_FUNCTION(objbase, Class, Name)
#endif

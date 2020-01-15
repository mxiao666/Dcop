#ifndef __OBJBASE_H__
#define __OBJBASE_H__
#include <atomic>
#include <stdio.h>

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
    virtual void dump(Printfun callback = printf) { (void)callback("Query is NULL.\n"); }
    int GetDebug() { return m_debug; }
    int SetDebug(int value) { return (m_debug = value); }
    virtual void PrintHead(Printfun callback, const char *tableName, int tatol, char split = '-')
    {
        (void)callback("\n%*s\n", tatol / 2 + 1, tableName);
        for (int i = 0; i < tatol; i++)
            (void)callback("%c", split);
        (void)callback("\n");
    }
};

#endif

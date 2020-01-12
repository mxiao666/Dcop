#ifndef __OBJBASE_H__
#define __OBJBASE_H__
#include <atomic>

//using Printfun = (*)(const char *format, ...);
typedef int (*Printfun)(const char *format, ...);
class objbase
{
private:
    std::atomic<int> m_debug;

public:
    objbase() : m_debug(0) {}
    virtual ~objbase() {}
    virtual int Init() { return 0; };
    virtual void dump(Printfun callback) {}
    int GetDebug() { return m_debug; }
    int SetDebug(int value) { return (m_debug = value); }
    virtual void PrintHead(Printfun callback, const char *tableName, int tatol, char split)
    {
        (void)callback("%-*s\n", tatol / 2, tableName);
        for (int i = 0; i < tatol; i++)
            (void)callback("%c", split);
        (void)callback("\n");
    }
};

#endif

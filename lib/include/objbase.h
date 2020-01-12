#ifndef __OBJBASE_H__
#define __OBJBASE_H__
#include <atomic>

class objbase
{
private:
    std::atomic<int> m_debug;

public:
    objbase() : m_debug(0) {}
    virtual ~objbase() {}
    virtual int Init() { return 0; };
    void dump() {}
    int GetDebug() { return m_debug; }
    int SetDebug(int value) { return (m_debug = value); }
};

#endif

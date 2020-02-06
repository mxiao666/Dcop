#ifndef __OS_CARGCLIST__
#define __OS_CARGCLIST__
#include <iostream>
#include <map>
#include <string.h>
#include "template.h"
#include "cstream.h"
class CAgrcList
{
public:
    CAgrcList() : m_count(0) {}
    CAgrcList(CAgrcList* argc)
    {
        if (argc != nullptr)
        {
            m_count = argc->GetCount();
            std::map<const char *, CStream *> list = argc->GetList();
            for (auto &iter : list)
                m_arglist[iter.first] = (iter.second != nullptr) ? new CStream(iter.second->GetBuff()) : nullptr;
        }
        else
        {
            m_count = 0;
        }        
    }
    CAgrcList &addCount()
    {
        ++m_count;
        return *this;
    }
    CAgrcList &addAgrc(const char *pzName, const char *pzValue)
    {
        if (pzName != nullptr)
        {
            m_arglist[pzName] = new CStream((const BYTE *)pzValue);
        }
        return *this;
    }
    CStream *GetAgrc(const char *pzName)
    {
        auto iter = OS::find(pzName, m_arglist);
        if (iter != m_arglist.end())
        {
            return iter->second;
        }
        return nullptr;
    }
    int GetCount() { return m_count; }
    std::map<const char *, CStream *>& GetList() { return m_arglist; }
    size_t GetSize() { return m_arglist.size(); }
    ~CAgrcList()
    {
        for (auto &iter : m_arglist)
        {
            if (iter.second != nullptr)
                delete iter.second;
            iter.second = nullptr;
        }
    }

private:
    int m_count;
    std::map<const char *, CStream *> m_arglist;
};
typedef struct _RspMsg
{
    CAgrcList *msg;
    int count;
    _RspMsg()
    {
        msg = nullptr;
    }
    ~_RspMsg()
    {
        if (msg)
            delete[] msg;
    }
} RspMsg;
#endif

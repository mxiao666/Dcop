#ifndef __OS_CARGCLIST__
#define __OS_CARGCLIST__
#include <iostream>
#include <map>
#include "cstream.h"
#include <string.h>

class CAgrcList
{
public:
    CAgrcList():m_count(0){}
    CAgrcList& addCount()
    {
        ++m_count;
        return *this;
    }
    CAgrcList& addAgrc(const char* pzName, const char* pzValue)
    {
        if(pzName != nullptr)
        {
            CStream *arg = new CStream((const BYTE*)pzValue);
            m_arglist[pzName] = arg;
        }
        return *this;
    }
    CStream* GetAgrc(const char* pzName)
    {
        for (auto& iter : m_arglist)
        {
            if(0 == strncmp(iter.first, pzName, strlen(iter.first))) 
             return iter.second;
        }
        return nullptr;
    } 
    int GetCount(){ return m_count; }
    int GetSize(){ return m_arglist.size(); }
    ~CAgrcList()
    {
        for (auto& iter : m_arglist)
        {
            if(iter.second != nullptr) 
                delete iter.second;
            iter.second = nullptr;
        }
    }
private:
    int m_count;
    std::map<const char *,CStream*> m_arglist;   
};

#endif

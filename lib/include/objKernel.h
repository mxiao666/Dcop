#ifndef __OBJKERNEL_H__
#define __OBJKERNEL_H__
#include <iostream>
#include <map>
#include "objbase.h"

typedef struct _ObjModule
{
    objbase* obj;
    int refCount;
    _ObjModule(objbase* obj) : refCount(0)
    {
        this->obj = obj;
    }
    ~_ObjModule()
    {
        if(obj != nullptr)
        {
            delete obj;
            obj = nullptr;
        }
    }
} ObjModule;

class objKernel{
public:
    objKernel();
    ~objKernel();
    objbase* InterFace();
    objbase* Query(const char* pzName);
    void Release(const char* pzName);
    void Entry();
private:
    objKernel(objKernel&) = delete;
    const objKernel& operator=(const objKernel&) = delete;
    inline std::map<const char*, ObjModule*>::iterator find(const char* pzName);
    std::map<const char*, ObjModule*> m_objList;
};
#endif

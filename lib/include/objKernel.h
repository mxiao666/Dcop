#ifndef __OBJKERNEL_H__
#define __OBJKERNEL_H__
#include <iostream>
#include <map>
#include "objbase.h"

typedef struct _ObjModule
{
    objbase *obj;
    int refCount;
    _ObjModule(objbase *obj) : refCount(0)
    {
        this->obj = obj;
    }
    ~_ObjModule()
    {
        if (obj != nullptr)
        {
            delete obj;
            obj = nullptr;
        }
    }
} ObjModule;

class objKernel : public objbase
{
public:
    objKernel();
    ~objKernel();
    /*普通对象查询*/
    objbase *InterFace(const char *pzName);
    /*引用对象查询*/
    objbase *Query(const char *pzName);
    /*引用对象释放*/
    void Release(const char *pzName);
    void Entry();
    void dump(Printfun callback = printf);

private:
    objKernel(objKernel &) = delete;
    const objKernel &operator=(const objKernel &) = delete;
    std::map<const char *, ObjModule *> m_objList;
};
extern objKernel *g_objKernel;
#endif

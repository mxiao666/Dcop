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
    /*��ͨ�����ѯ*/
    objbase *InterFace(const char *pzName);
    /*���ö����ѯ*/
    objbase *Query(const char *pzName);
    /*���ö����ͷ�*/
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

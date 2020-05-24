#ifndef __OBJKERNEL_H__
#define __OBJKERNEL_H__
#include <iostream>
#include <map>
#include "objbase.h"

typedef struct _ObjModule
{
    objbase *obj;
    int refCount;
    int id;
    _ObjModule(objbase *obj, int key = 0) : refCount(0)
    {
        this->obj = obj;
        id = key;
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
    objbase *InterFace(int key);
    /*���ö����ѯ*/
    objbase *Query(const char *pzName);
    /*���ö����ͷ�*/
    void Release(const char *pzName);
    void Entry();
    void dump(int fd = 0, Printfun callback = LVOS_Printf);
    void Init(void (*EntryFunc)());
    void Reg(const char *pzName, void *obj, int id);

private:
    objKernel(objKernel &) = delete;
    const objKernel &operator=(const objKernel &) = delete;
    std::map<const char *, ObjModule *> m_objList;
    void (*m_EntryFunc)() = nullptr;
};
extern objKernel *g_objKernel;

#endif

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
    /*普通对象查询*/
    objbase *InterFace(const char *pzName);
    objbase *InterFace(int key);
    /*引用对象查询*/
    objbase *Query(int key);
    /*引用对象释放*/
    void Release(int key);
    void Entry();
    void dump(int fd = 0, Printfun callback = LVOS_Printf);
    void Init(void (*EntryFunc)());
    void Reg(const char *pzName, void *obj, int id);
    void UnReg(int id);
    void Welcome(int fd = 0, Printfun callback = LVOS_Printf);
    void ErroReg(ERR_CODE_INFO *err, int coount);
    ERR_CODE_INFO *ErrGet(int err);
    void ErrGet(std::map<int, ERR_CODE_INFO *> &list);
    /*
    ver[0] = Versione
    ver[1] = VerDate
    ver[2] = VerTimer
    */
    void VersionGet(char *ver[3]) const;

private:
    objKernel(objKernel &) = delete;
    const objKernel &operator=(const objKernel &) = delete;
    std::map<const char *, ObjModule *> m_objList;
    std::map<int, ERR_CODE_INFO *> m_errList;
    void (*m_EntryFunc)() = nullptr;
};
extern objKernel *g_objKernel;

#endif

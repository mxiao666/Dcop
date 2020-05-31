#include <string.h>
#include "objKernel.h"
#include "frameworkmgr.h"
#include "template.h"
#define VER "v1.0"
objKernel *g_objKernel;
objKernel::objKernel()
{
    Welcome();
    g_objKernel = this;
    m_objList["objKernel"] = new ObjModule((objbase *)(this), MODELU_KERNEL);
}
void objKernel::Welcome(int fd, Printfun callback)
{
    (void)callback(fd, "\r\n");
    (void)callback(fd, "  __      __    __\r\n");
    (void)callback(fd, " |  |    |  |  |__|   __________\r\n");
    (void)callback(fd, " |  |____|  |   __   |  __  __  |\r\n");
    (void)callback(fd, " |   ____   |  |  |  |  ||  ||  |\r\n");
    (void)callback(fd, " |  |    |  |  |  |  |  ||  ||  |\r\n");
    (void)callback(fd, " |__|    |__|  |__|  |__||__||__|\r\n");
    (void)callback(fd, "\r\n");
    (void)callback(fd, " Herobrine (Alpha) %-s\r\n", VER);
    (void)callback(fd, " Update Date: %-17.17s\r\n", __DATE__);
    (void)callback(fd, " Update Time: %-17.17s\r\n", __TIME__);
}
objKernel::~objKernel()
{
    for (auto &iter : m_objList)
    {
        ObjModule *pObj = iter.second;
        if (pObj != nullptr && pObj->obj != this)
            delete pObj;
        pObj = nullptr;
    }
    m_objList.clear();
}
objbase *objKernel::InterFace(const char *pzName)
{
    auto iter = OS::find(pzName, m_objList);
    if (iter != m_objList.end())
        return iter->second->obj;
    return nullptr;
}
objbase *objKernel::InterFace(int key)
{
    for (auto &iter : m_objList)
    {
        ObjModule *pObj = iter.second;
        if (pObj != nullptr && pObj->id == key)
            return pObj->obj;
    }
    return nullptr;
}
void objKernel::dump(int fd, Printfun callback)
{
    objbase::PrintHead(fd, callback, "objKernel", 50);
    (void)callback(fd,
                   "%-16s %-16s %-16s\r\n",
                   "objName",
                   "objPtr",
                   "refCount");
    for (auto &iter : m_objList)
    {
        (void)callback(fd,
                       "%-16s %#-16x %-16d\r\n",
                       iter.first,
                       iter.second->obj,
                       iter.second->refCount);
    }
    (void)callback(fd, "Tatol: %d\r\n", m_objList.size());
}
objbase *objKernel::Query(int key)
{
    for (auto &iter : m_objList)
    {
        ObjModule *pObj = iter.second;
        if (pObj != nullptr && pObj->id == key)
        {
            pObj->refCount++;
            return pObj->obj;
        }
    }
    return nullptr;
}
void objKernel::Release(int key)
{
    for (auto &iter : m_objList)
    {
        ObjModule *pObj = iter.second;
        if (pObj != nullptr && pObj->id == key && pObj->refCount > 0)
        {
            pObj->refCount--;
        }
    }
}
void objKernel::Entry()
{
    FRAMEWORK_INIT();
    for (auto &iter : m_objList)
    {
        ObjModule *pObj = iter.second;
        if (pObj != nullptr)
            pObj->obj->Init();
    }
    /*����ѭ������*/
    if (m_EntryFunc)
        m_EntryFunc();
}
void objKernel::Init(void (*EntryFunc)())
{
    if (EntryFunc)
        m_EntryFunc = EntryFunc;
}

void objKernel::Reg(const char *pzName, void *obj, int id)
{
    m_objList[pzName] = new ObjModule((objbase *)obj, id);
}
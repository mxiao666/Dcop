#include <string.h>
#include "objKernel.h"
#include "frameworkmgr.h"
#include "template.h"
objKernel *g_objKernel;
objKernel::objKernel()
{
    printf(
        "  __      __    __\n"
        " |  |    |  |  |__|   __________\n"
        " |  |____|  |   __   |  __  __  |\n"
        " |   ____   |  |  |  |  ||  ||  |\n"
        " |  |    |  |  |  |  |  ||  ||  |\n"
        " |__|    |__|  |__|  |__||__||__|\n"
        "\n"
        " [Herobrine (Alpha)             ]\n"
        " [Update Date: %-17.17s]\n"
        " [Update Time: %-17.17s]\n",
        __DATE__, __TIME__);
    g_objKernel = this;
}
objKernel::~objKernel()
{
    for (auto &iter : m_objList)
    {
        ObjModule *pObj = iter.second;
        if (pObj != nullptr)
            delete pObj;
        pObj = nullptr;
    }
}
objbase *objKernel::InterFace(const char *pzName)
{
    auto iter = OS::find(pzName, m_objList);
    if (iter != m_objList.end())
        return iter->second->obj;
    return nullptr;
}
void objKernel::dump(int fd, Printfun callback)
{
    objbase::PrintHead(fd, callback, "objKernel", 50);
    (void)callback(fd, "%-16s %-16s %-16s\n", "objName", "objPtr", "refCount");
    for (auto &iter : m_objList)
    {
        (void)callback(fd, "%-16s %#-16x %-16d\n", iter.first, iter.second->obj, iter.second->refCount);
    }
    (void)callback(fd, "Tatol: %d\n", m_objList.size());
}
objbase *objKernel::Query(const char *pzName)
{
    auto iter = OS::find(pzName, m_objList);
    if (iter != m_objList.end())
    {
        ++iter->second->refCount;
        return iter->second->obj;
    }
    return nullptr;
}
void objKernel::Release(const char *pzName)
{
    auto iter = OS::find(pzName, m_objList);
    if ((iter != m_objList.end()) && (--iter->second->refCount <= 0))
    {
        delete iter->second;
        iter->second = nullptr;
        m_objList.erase(iter);
    }
}
void objKernel::Entry()
{
    FRAMEWORK_BEGINE(objbase)
    m_objList[frmgr->ModuleName] = new ObjModule((objbase *)(frmgr->fun));
    FRAMEWORK_END(objbase)

    for (auto &iter : m_objList)
    {
        ObjModule *pObj = iter.second;
        if (pObj != nullptr)
            pObj->obj->Init();
    }
    /*启动循环处理*/
    if(m_EntryFunc)
        m_EntryFunc();
}
void objKernel::Init(void (*EntryFunc)())
{
    if (EntryFunc)
        m_EntryFunc = EntryFunc;
}


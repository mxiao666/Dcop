#include <string.h>
#include "objKernel.h"
#include "frameworkmgr.h"
#include "template.h"
#include "cnotify.h"

static const char *gVersione = "v1.0";
static const char *gVerDate = __DATE__;
static const char *gVerTimer = __TIME__;
objKernel *g_objKernel;

objKernel::objKernel()
{
    Welcome();
    g_objKernel = this;
    m_objList["objKernel"] = new ObjModule((objbase *)(this), MODELU_KERNEL);
}
void objKernel::VersionGet(char *ver[3]) const
{
    ver[0] = (char *)gVersione;
    ver[1] = (char *)gVerDate;
    ver[2] = (char *)gVerTimer;
}
void objKernel::Welcome(int fd, Printfun callback)
{
    (void)callback(fd, "\r\n");
    (void)callback(fd, "    ____  __________  ____\r\n");
    (void)callback(fd, "   / __ \\/ ____/ __ \\/ __ \\\r\n");
    (void)callback(fd, "  / / / / /   / / / / /_/ /\r\n");
    (void)callback(fd, " / /_/ / /___/ /_/ / ____/\r\n");
    (void)callback(fd, "/_____/\\____/\\____/_/\r\n");
    (void)callback(fd, "\r\n");
    (void)callback(fd, " Herobrine (Alpha) %-s\r\n", gVersione);
    (void)callback(fd, " Update Date: %-17.17s\r\n", gVerDate);
    (void)callback(fd, " Update Time: %-17.17s\r\n", gVerTimer);
    (void)callback(fd, "\r\n");
}
objKernel::~objKernel()
{
    Cnotify *pnotify =
        reinterpret_cast<Cnotify *>(g_objKernel->Query(MODELU_NOTIFY));
    pnotify->NotifyA(nullptr, nullptr, MODULE_ALL, CMD_SYS_SHUT_DOWN, true);
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
    objbase::PrintHead(fd, callback, "objKernel", 56);
    (void)callback(fd,
                   "%-8s %-16s %-16s %-16s\r\n",
                   "objId",
                   "objName",
                   "objPtr",
                   "refCount");
    objbase::PrintEnd(fd, callback, 56);
    for (auto &iter : m_objList)
    {
        (void)callback(fd,
                       "%#-8x %-16s %#-16x %-16d\r\n",
                       iter.second->id,
                       iter.first,
                       iter.second->obj,
                       iter.second->refCount);
    }
    objbase::PrintEnd(fd, callback, 56);
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
    ErroReg(gErrCode, gErrCodeCnt);
    FRAMEWORK_INIT();
    for (auto &iter : m_objList)
    {
        ObjModule *pObj = iter.second;
        if (pObj != nullptr)
            pObj->obj->Init();
    }
    /* 开始循环处理 */
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
void objKernel::UnReg(int id)
{
    for (auto iter = m_objList.begin(); iter != m_objList.end(); iter++)
    {
        ObjModule *pObj = iter->second;
        if (pObj != nullptr && pObj->id == id)
        {
            delete pObj;
            m_objList.erase(iter);
        }
    }
}
void objKernel::ErroReg(ERR_CODE_INFO *err, int count)
{
    for (int i = 0; i < count; i++)
    {
        auto iter = m_errList.find(err[i].id);
        if (iter != m_errList.end())
        {
            continue;
        }
        m_errList[err[i].id] = &err[i];
    }
}
ERR_CODE_INFO *objKernel::ErrGet(int err)
{

    auto iter = m_errList.find(err);
    if (iter != m_errList.end())
    {
        return iter->second;
    }
    return m_errList[ERR_UNKOWN];
}
void objKernel::ErrGet(std::map<int, ERR_CODE_INFO *> &list)
{
    list = m_errList;
}
#include <mutex>
#include <stdio.h>
#ifndef __WIN32__
#include <pthread.h>
#include <unistd.h>
#else
#include <thread>
#include <string>
#include <sstream>
#endif
#include "objTask.h"
#include "log.h"
#include "frameworkmgr.h"
#include "template.h"
static objTaskMgr *g_objTaskMgr;
objTaskMgr::objTaskMgr()
{
    g_objTaskMgr = this;
}
objTaskMgr::~objTaskMgr()
{
    g_objTaskMgr = nullptr;
}
objTaskMgr *objTaskMgr::GetInstance()
{
    return g_objTaskMgr;
}
void objTaskMgr::dump(int fd, Printfun callback)
{
    objbase::PrintHead(fd, callback, "objTaskMgr", 54);
    (void)callback(fd, "%-12s %-16s %-12s\r\n",
                   "taskName", "taskId", "taskPtr");
    for (auto &iter : m_objlist)
    {
        if (iter.second != nullptr)
            (void)callback(fd, "%-12s %#-16x %#-12x\r\n",
                           iter.second->Name(),
                           iter.second->GetId(),
                           iter.second);
    }
    (void)callback(fd, "Tatol: %d\r\n", m_objlist.size());
}
int objTaskMgr::addObj(objTask *obj)
{
    auto iter = m_objlist.find(obj->GetId());
    if ((iter != m_objlist.end()) && (m_objlist.size() != 0))
    {
        return -1;
    }
    std::unique_lock<std::mutex> lock{this->m_lock};
    m_objlist[obj->GetId()] = obj;
    return 0;
}
void objTaskMgr::delObj(u64 id)
{
    auto iter = m_objlist.find(id);
    if (iter != m_objlist.end())
    {
        std::unique_lock<std::mutex> lock{this->m_lock};
        if (iter->second != nullptr)
        {
            delete (iter->second);
            (iter->second) = nullptr;
        }
        m_objlist.erase(iter);
    }
}

objTask *objTaskEntry(const char *objTaskName,
                      FunEntry func,
                      objPara *pObjPara,
                      const char *file,
                      int len)
{
    objTask *obj = new objTask(objTaskName, func, pObjPara);
    if (obj == nullptr)
        return nullptr;
    auto callfunc = [](void *pObj) {
        objTask *obj = reinterpret_cast<objTask *>(pObj);
        if (obj != nullptr)
        {
#ifndef __WIN32__
            obj->SetId(pthread_self());
#else
            std::ostringstream oss;
            oss << std::this_thread::get_id();
            std::string stid = oss.str();
            obj->SetId(std::stoull(stid));
#endif
            objTaskMgr::GetInstance()->addObj(obj);
            obj->Run();
            objTaskMgr::GetInstance()->delObj(obj->GetId());
        }
        return (void *)0;
    };
    //objTaskMgr::GetInstance()->addObj(obj);
#ifndef __WIN32__
    pthread_t thread_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_id, &attr, callfunc, obj);
    pthread_attr_destroy(&attr);
#else
    std::thread task(callfunc, obj);
    task.detach();
#endif // !__WIN32__
    LVOS_Log(LL_DEBUG, "%s:%d objTask id(%#0x)", file, len, obj->GetId());
    return obj;
}

objTask::objTask(const char *pzName, FunEntry func, objPara *obj)
{
    if (pzName != nullptr)
        strncpy(name, pzName, TASK_NAME_LEN - 1);
    name[TASK_NAME_LEN - 1] = '\0';
    m_func = func;
    m_objPara = obj;
}
objTask::~objTask()
{
    if (m_objPara != nullptr)
        delete m_objPara;
    m_objPara = nullptr;
}

REG_TO_FRAMEWORK(TABLE_ONE, MODELU_KERNEL, objTaskMgr, MODELU_TASK)
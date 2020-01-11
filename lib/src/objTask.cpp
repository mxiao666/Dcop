#include <mutex>
#include <pthread.h>
#include <unistd.h>
#include "objTask.h"
#include "log.h"

objTaskMgr *objTaskMgr::GetInstance()
{
    static objTaskMgr m_objbase;
    return &m_objbase;
}
int objTaskMgr::addObj(objTask *&obj)
{
    auto iter = m_objlist.find(obj->GetId());
    if (iter != m_objlist.end())
    {
        return -1;
    }
    std::unique_lock<std::mutex> lock{this->m_lock};
    m_objlist[obj->GetId()] = &obj;
    return 0;
}
void objTaskMgr::delObj(int id)
{
    auto iter = m_objlist.find(id);
    if (iter != m_objlist.end())
    {
        std::unique_lock<std::mutex> lock{this->m_lock};
        if (iter->second != nullptr)
        {
            delete *(iter->second);
            *(iter->second) = nullptr;
        }
        m_objlist.erase(iter);
    }
}
objTask *objTaskEntry(const char *objTaskName,
                      FunEntry func,
                      objPara **pObjPara,
                      const char *file,
                      int len)
{
    objTask *obj = new objTask(objTaskName, func, pObjPara);

    if (obj == nullptr)
        return nullptr;

    pthread_t thread_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_id, &attr,
                   [](void *pObj) {
                       objTask *obj = reinterpret_cast<objTask *>(pObj);
                       if (obj != nullptr)
                       {
                           obj->SetId(pthread_self());
                           objTaskMgr::GetInstance()->addObj(obj);
                           obj->Run();
                           objTaskMgr::GetInstance()->delObj(pthread_self());
                       }
                       return (void *)0;
                   },
                   obj);
    pthread_attr_destroy(&attr);
    LVOS_Log(LL_DEBUG, "%s:%d objTask id(%#0x)", FILE_NAME(file), len, thread_id);
    return obj;
}

objTask::objTask(const char *pzName, FunEntry func, objPara **obj)
{
    if (pzName != nullptr)
        strncpy(name, pzName, TASK_NAME_LEN - 1);
    name[TASK_NAME_LEN - 1] = '\0';
    m_func = func;
    m_objPara = obj;
}
objTask::~objTask()
{
    if (*m_objPara != nullptr)
        delete *m_objPara;
    *m_objPara = nullptr;
}

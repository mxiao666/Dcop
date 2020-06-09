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
#include "cnotify.h"
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
    objbase::PrintHead(fd, callback, "objTaskMgr", 58);
    (void)callback(fd, "%-16s %-16s %-16s\r\n",
                   "taskName", "taskId", "taskPtr");
    objbase::PrintEnd(fd, callback, 58);
    for (auto &iter : m_objlist)
    {
        if (iter.second != nullptr)
            (void)callback(fd, "%-16s %#-16x %#-16x\r\n",
                           iter.second->Name(),
                           iter.second->GetId(),
                           iter.second);
    }
    objbase::PrintEnd(fd, callback, 58);
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
    LOG_TRACE("%s:%d objTask id(%#0x)", file, len, obj->GetId());
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

int workQ::Init()
{
    Cnotify *obj =
        reinterpret_cast<Cnotify *>(g_objKernel->InterFace(MODELU_NOTIFY));
    if (obj)
        obj->RegReceiver(MODELU_WORK_TASK, new REGNOTIFY(this, "workQ"));
    objPara *pObjPara = new objPara(this);
    CREATE_OBJTASK(
        "WorkQueue", [](objPara *pobjPara) {
            workQ *obj = reinterpret_cast<workQ *>(pobjPara->GetPara());
            if (obj)
                obj->Proc();
            return (void *)0;
        },
        pObjPara);
    return SUCCESS;
}
void workQ::Proc()
{
    while (!this->stoped)
    {
        Task task;
        {
            std::unique_lock<std::mutex> lock{this->m_lock};

            this->cv_task.wait(lock,
                               [this] {
                                   return this->stoped.load() ||
                                          !this->tasks.empty();
                               });
            if (this->stoped && this->tasks.empty())
            {
                return;
            }
            task = std::move(this->tasks.front());
            this->tasks.pop();
        }
        task();
    }
}
workQ::workQ()
{
}
workQ::~workQ()
{
    stoped.store(true);
    cv_task.notify_all();
}
void workQ::dump(int fd, Printfun callback)
{
    objbase::PrintHead(fd, callback, "work task queue", 44);
    callback(fd, "Queue size:%d\r\n", tasks.size());
    callback(fd, "Queue status:%s\r\n", stoped ? "Stop" : "Run");
    objbase::PrintEnd(fd, callback, 44);
}
int workQ::Process(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
{
    PROCESS_BEGIN(iCmd)
    PROCESS_CALL(CMD_SYS_SHUT_DOWN, sysShutdown)
    PROCESS_END()
}
int workQ::sysShutdown(CAgrcList *message, RspMsg *outmessage,
                       int iModule, int iCmd)
{
    stoped.store(true);
    cv_task.notify_all();
    return SUCCESS;
}
REG_TO_FRAMEWORK(TABLE_ONE, MODELU_KERNEL, workQ, MODELU_WORK_TASK)
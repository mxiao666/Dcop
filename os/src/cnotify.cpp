#include "cnotify.h"
#include "frameworkmgr.h"
#include "objTask.h"

void Cnotify::RegReceiver(int iModule, REGNOTIFY *pRecv)
{
    if (pRecv != nullptr)
    {
        std::unique_lock<std::mutex> lock{this->m_reglock};
        auto iter = observerList.find(iModule);
        if (iter == observerList.end())
        {
            observerList[iModule] = pRecv;
        }
    }
}
void Cnotify::UnRegReceiver(int iModule)
{
    std::unique_lock<std::mutex> lock{this->m_reglock};
    auto iter = observerList.find(iModule);
    if (iter != observerList.end())
    {
        delete iter->second;
        observerList.erase(iter);
    }
}
void Cnotify::NotifyA(CAgrcList *message,
                      RspMsg *outmessage,
                      int iModule,
                      int iCmd,
                      bool sync)
{
    for (auto &iter : observerList)
    {
        if (sync)
        {
            (void)iter.second->obj->Process(message, outmessage, iModule, iCmd);
        }
        else
        {
            (void)AsyncNotify(message, iter.first, iCmd, nullptr);
        }
    }
}

int Cnotify::Notify(CAgrcList *message,
                    RspMsg *outmessage,
                    int iModule,
                    int iCmd)
{
    auto iter = observerList.find(iModule);
    if (iter != observerList.end() && iModule == iter->first)
    {
        return iter->second->obj->Process(message, outmessage, iModule, iCmd);
    }
    return -1;
}
int Cnotify::AsyncNotify(CAgrcList *message, int iModule, int iCmd, void *parm)
{
    AsyncArgc *pool = new AsyncArgc(message, iModule, iCmd, parm);
    if (pool == nullptr)
        return -1;
    std::lock_guard<std::mutex> lock{m_lock};
    tasks.emplace(pool);
    cv_task.notify_one(); // 唤醒一个线程执行
    return 0;
}
Cnotify::~Cnotify()
{
    stoped.store(true);
    cv_task.notify_all();
    while (idlThrNum > 0)
        ;
    for (auto &iter : observerList)
    {
        delete iter.second;
    }
    observerList.clear();
}
Cnotify::Cnotify()
{
    stoped = false;
    idlThrNum = 10;
}
void Cnotify::AsyncProc()
{
    while (!this->stoped)
    {
        AsyncArgc *message = nullptr;
        {
            std::unique_lock<std::mutex> lock{this->m_lock};

            this->cv_task.wait(lock,
                               [this] {
                                   return this->stoped.load() ||
                                          !this->tasks.empty();
                               });
            if (this->stoped && this->tasks.empty())
            {
                idlThrNum--;
                return;
            }
            message = this->tasks.front(); // 取一个 task
            this->tasks.pop();
        }
        idlThrNum--;
        if (message != nullptr)
        {
            if (message->iModule == 0)
            {
                NotifyA(message->msg,
                        (RspMsg *)message->m_ptr,
                        message->iModule,
                        message->iCmd);
            }
            else
            {
                Notify(message->msg,
                       (RspMsg *)message->m_ptr,
                       message->iModule,
                       message->iCmd);
            }
            delete message;
        }
        idlThrNum++;
    }
    idlThrNum--;
}
int Cnotify::Init()
{
    for (int i = 0; i < idlThrNum; i++)
    {
        const int ibufmaxlen = 16;
        char buf[ibufmaxlen] = {0};
        (void)snprintf(buf, ibufmaxlen - 1, "NotifyTask%d", i);
        objPara *pObjPara = new objPara(this);
        CREATE_OBJTASK(
            buf, [](objPara *pobjPara) {
                Cnotify *obj = reinterpret_cast<Cnotify *>(pobjPara->GetPara());
                if (obj)
                    obj->AsyncProc();
                return (void *)0;
            },
            pObjPara);
    }

    return 0;
}
void Cnotify::dump(int fd, Printfun callback)
{
    objbase::PrintHead(fd, callback, "Cnotify", 44);
    (void)callback(fd, "%-12s %-16s %-16s\r\n", "objId", "objPtr", "objName");
    objbase::PrintEnd(fd, callback, 44);
    for (auto &iter : observerList)
        (void)callback(fd, "%#-12x %#-16x %-16s\r\n",
                       iter.first,
                       iter.second->obj,
                       iter.second->name);
    objbase::PrintEnd(fd, callback, 44);
    (void)callback(fd, "Tatol: %d\r\n", observerList.size());
}

REG_TO_FRAMEWORK(TABLE_ONE, MODELU_KERNEL, Cnotify, MODELU_NOTIFY)
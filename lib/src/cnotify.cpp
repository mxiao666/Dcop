#include "cnotify.h"
#include "frameworkmgr.h"
#include "objTask.h"

void Cnotify::RegReceiver(int iModule, objbase *pRecv)
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
        observerList.erase(iter);
    }
}
void Cnotify::SendToAll(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
{
    for (auto &iter : observerList)
    {
        iter.second->Process(message, outmessage, iModule, iCmd);
    }
}
int Cnotify::Notify(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
{
    auto iter = observerList.find(iModule);
    if (iter != observerList.end() && iModule == iter->first)
    {
        return iter->second->Process(message, outmessage, iModule, iCmd);
    }
    return -1;
}
int Cnotify::AsyncNotify(CAgrcList *message,int iModule, int iCmd)
{
    AsyncArgc *pool = new AsyncArgc(message, iModule, iCmd);
    if(pool == nullptr)
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
    while(idlThrNum > 0);
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
                               [this]
            {
                return this->stoped.load() || !this->tasks.empty();
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
                SendToAll(message->msg, nullptr, message->iModule, message->iCmd);
            }
            else
            {
                Notify(message->msg, nullptr, message->iModule, message->iCmd);
            }
        }
        idlThrNum++;
    }
}
int Cnotify::Init()
{
    for (int i = 0; i < idlThrNum; i++)
    {
        const int ibufmaxlen = 16;
        char buf[ibufmaxlen] = {0};
        (void)snprintf(buf, ibufmaxlen - 1, "NotifyTask%d", i);
        objPara *pObjPara = new objPara(this);
        CREATE_OBJTASK(buf, [](objPara * pobjPara)
        {
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
    objbase::PrintHead(fd, callback, "Cnotify", 30);
    (void)callback(fd, "%-12s %-16s\n", "cmdId", "objPtr");
    for (auto &iter : observerList)
        (void)callback(fd, "%-12d %#-16x\n", iter.first, iter.second);
    (void)callback(fd, "Tatol: %d\n", observerList.size());
}
REG_FUNCTION_PLUS(Cnotify)
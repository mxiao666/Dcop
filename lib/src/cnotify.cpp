#include "cnotify.h"
#include "frameworkmgr.h"

void Cnotify::RegReceiver(int iModule, objbase *pRecv)
{
    if (pRecv != nullptr)
    {
        auto iter = observerList.find(iModule);
        if (iter == observerList.end())
        {
            observerList[iModule] = pRecv;
        }
    }
}
void Cnotify::UnRegReceiver(int iModule)
{

    auto iter = observerList.find(iModule);
    if (iter != observerList.end())
    {
        observerList.erase(iter);
    }
}
void Cnotify::SendToAll(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd)
{
    for (auto &iter : observerList)
    {
        std::unique_lock<std::mutex> lock{m_excelock};
        iter.second->Process(message, outmessage, iModule, iCmd);
    }
}
int Cnotify::Notify(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd)
{
    auto iter = observerList.find(iModule);
    if (iter != observerList.end() && iModule == iter->first)
    {
        std::unique_lock<std::mutex> lock{m_excelock};
        return iter->second->Process(message, outmessage, iModule, iCmd);
    }
    return -1;
}
void Cnotify::dump(Printfun callback)
{
    objbase::PrintHead(callback, "Cnotify", 30);
    (void)callback("%-12s %-16s\n", "cmdId", "objPtr");
    for (auto &iter : observerList)
        (void)callback("%-12d %#-16x\n", iter.first, iter.second);
    (void)callback("Tatol: %d\n", observerList.size());
}
REG_FUNCTION_PLUS(Cnotify, "Cnotify")
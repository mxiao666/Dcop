#include "Cnotify.h"
#include "frameworkmgr.h"

void Cnotify::regReceiver(int iModule, receiver *pRecv)
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

void Cnotify::sendToAllRecv(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd)
{
    for (auto &iter : observerList)
    {
        iter.second->recvMessage(message, outmessage, iModule, iCmd);
    }
}
int Cnotify::notify(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd)
{
    auto iter = observerList.find(iModule);
    if (iter != observerList.end() && iModule == iter->first)
    {
        return iter->second->recvMessage(message, outmessage, iModule, iCmd);
    }
    return -1;
}
void Cnotify::dump(Printfun callback)
{
    objbase::PrintHead(callback, "Cnotify", 20);
    (void)callback("%-4s %-16s\n", "cmdId", "objPtr");
    for (auto &iter : observerList)
        (void)callback("%-4s %#-16x\n", iter.first, iter.second);
    (void)callback("Tatol: %d\n", observerList.size());
}
REG_FUNCTION_PLUS(Cnotify, "Cnotify")
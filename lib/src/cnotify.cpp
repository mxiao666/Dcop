#include "Cnotify.h"

Cnotify::Cnotify()
{
}
 
Cnotify::~Cnotify()
{
}
 
void Cnotify::regReceiver(int iModule, receiver* pRecv)
{
    if(pRecv != nullptr)
    {
        auto iter = observerList.find(iModule);
        if(iter == observerList.end())
        {
            observerList[iModule] = pRecv;
        }
    }
}
 
void Cnotify::sendToAllRecv(CAgrcList* message,CAgrcList* outmessage,int iModule, int iCmd)
{
    for(auto& iter : observerList)
    {
        iter.second->recvMessage(message, outmessage, iModule, iCmd);
    }
}
int Cnotify::notify(CAgrcList* message,CAgrcList* outmessage,int iModule, int iCmd)
{
    auto iter = observerList.find(iModule);
    if(iter != observerList.end() && iModule == iter->first)
    {
        return iter->second->recvMessage(message, outmessage, iModule, iCmd);
    }
    return -1;
}


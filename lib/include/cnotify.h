#ifndef NOTIFIER_H
#define NOTIFIER_H
 
#include <vector>
#include "receiver.h"
#include <string>
#include <map>

class Cnotify
{
public:
    Cnotify();
    virtual ~Cnotify();
 
    void sendToAllRecv(CAgrcList* message,CAgrcList* outmessage,int iModule, int iCmd);
    void regReceiver(int iCmd, receiver* pRecv);
    int  notify(CAgrcList* message,CAgrcList* outmessage,int iModule, int iCmd);
private:
    std::map<int,receiver*> observerList;
};
 
#endif // NOTIFIER_H



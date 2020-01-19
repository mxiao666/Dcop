#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <vector>
#include <string>
#include <map>
#include "objbase.h"
#include <condition_variable>

class Cnotify : public objbase
{
public:
    Cnotify(){};
    virtual ~Cnotify(){};
    void UnRegReceiver(int iModule);
    void SendToAll(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd);
    void RegReceiver(int iModule, objbase *pRecv);
    int Notify(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd);
    void dump(Printfun callback);

private:
    std::map<int, objbase *> observerList;
    std::mutex m_excelock;
};

#endif // NOTIFIER_H

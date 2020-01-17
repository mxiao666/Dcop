#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <vector>
#include "receiver.h"
#include <string>
#include <map>

class Cnotify : public objbase
{
public:
    Cnotify(){};
    virtual ~Cnotify(){};

    void sendToAllRecv(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd);
    void regReceiver(int iCmd, receiver *pRecv);
    int notify(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd);
    void dump(Printfun callback);

private:
    std::map<int, receiver *> observerList;
};

#endif // NOTIFIER_H

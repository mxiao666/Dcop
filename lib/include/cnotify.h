#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <vector>
#include <string>
#include <map>
#include "objbase.h"
class Cnotify : public objbase
{
public:
    Cnotify(){};
    virtual ~Cnotify(){};

    void sendToAllRecv(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd);
    void regReceiver(int iCmd, objbase *pRecv);
    int notify(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd);
    void dump(Printfun callback);

private:
    std::map<int, objbase *> observerList;
};

#endif // NOTIFIER_H

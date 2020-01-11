#ifndef GETMESSAGE_H
#define GETMESSAGE_H
#include "receiver.h"
#include "frameworkmgr.h"
#include "objbase.h"
class getMessage :public objbase, public receiver, virtual public IplugFrameWork
{
public:
    getMessage();
    ~getMessage();

    int recvMessage(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd);
};
REG_FUNCTION_PLUS(getMessage, "getMessage")
#endif // GETMESSAGE_H

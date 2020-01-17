#ifndef RECEIVER_H
#define RECEIVER_H

#include "cargclist.h"
#include "objbase.h"
class receiver :public objbase
{
public:
    receiver() {}
    virtual ~receiver() {}
    virtual int recvMessage(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd) = 0;
};

#endif // RECEIVER_H

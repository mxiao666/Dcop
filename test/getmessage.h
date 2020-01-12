#ifndef GETMESSAGE_H
#define GETMESSAGE_H
#include "receiver.h"
#include "cargclist.h"

class getMessage: public receiver
{
public:
    getMessage() {}
    ~getMessage() {}

    int recvMessage(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd);
};

#endif // GETMESSAGE_H

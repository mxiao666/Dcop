#ifndef GETMESSAGE_H
#define GETMESSAGE_H
#include "receiver.h"

class getMessage : public receiver
{
public:
    getMessage();
    ~getMessage();

    int recvMessage(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd);
};

#endif // GETMESSAGE_H

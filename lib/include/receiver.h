#ifndef RECEIVER_H
#define RECEIVER_H

#include <vector>
#include <string>
#include <iostream>
#include "cargclist.h"

class receiver
{
public:
    receiver() {}
    ~receiver() {}
    virtual int recvMessage(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd) = 0;
};

#endif // RECEIVER_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <iostream>
#include "objKernel.h"
#include "objTask.h"
#include "log.h"
#include "getmessage.h"
int main()
{
    LogInit(LL_DEBUG);

    objKernel *obj = new objKernel;
    obj->Entry();
    objbase *p = obj->Query("getMessage");
    CAgrcList msg;
    msg.addAgrc("A", "deda");
    std::cout << ((getMessage *)p)->recvMessage(&msg, nullptr, __LINE__, 100) << std::endl;
    obj->Release("objbase");
    // p = obj->Query("test2");
    //std::cout << (p)->a << std::endl;

    exit(0);
}

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <iostream>
#include "objKernel.h"
#include "objTask.h"
#include "log.h"
#include "getmessage.h"
#include "clibase.h"
void *test(objPara *)
{
    while (1)
        ;
    return (void *)0;
}
int main()
{
    LogInit(LL_DEBUG);

    objKernel *obj = new objKernel;
    obj->Entry();
    CREATE_OBJTASK("test_thread1", test, nullptr);
    objbase *cli = obj->Query("cliMgr");
    if (cli)
        ((cliMgr*)cli)->Proc();

    //objbase *p = obj->Query("objTaskMgr");
    //p->dump();
    //obj->dump();
    //obj->Release("getMessage");
    //std::cout << ((getMessage *)p)->recvMessage(&msg, nullptr, __LINE__, 100) << std::endl;

    return 0;
}

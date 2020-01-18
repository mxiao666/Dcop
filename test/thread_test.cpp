#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <iostream>
#include "objKernel.h"
#include "objTask.h"
#include "log.h"
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
    objbase *cli = obj->InterFace("cliMgr");
    if (cli)
        ((cliMgr *)cli)->Process();

    return 0;
}

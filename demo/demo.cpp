#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <iostream>
#include "objKernel.h"
#include "objTask.h"
#include "log.h"
#include "clibase.h"

int main()
{
    LogInit(LL_DEBUG);

    objKernel *obj = new objKernel;
    obj->Entry();
    objbase *cli = obj->InterFace("cliMgr");
    if (cli)
        ((cliMgr *)cli)->Process();
    delete obj;
    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include "objKernel.h"
#include "objTask.h"
#include "log.h"

int main()
{
    LogInit(LL_DEBUG);

    objKernel *obj = new objKernel;
    obj->Entry();
    objbase *p = obj->Query("test2");
    std::cout << (p)->a << std::endl;
    obj->Release("test2");
    // p = obj->Query("test2");
    std::cout << (p)->a << std::endl;

    exit(0);
}

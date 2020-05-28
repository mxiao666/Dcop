#ifndef __WIN32__
#include <unistd.h>
#endif
#include <iostream>
#include "objKernel.h"
#include "log.h"

int main()
{
#ifndef __WIN32__
    if (fork() == 0)
    {
        //创建会话组,只有不是进程组的组长则 setsid(); 才会成功
        setsid();
#else
    {
#endif
        objKernel *obj = new objKernel;
        obj->Entry();
        delete obj;
    }
    return 0;
}

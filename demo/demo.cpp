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
        //�����Ự��,ֻ�в��ǽ�������鳤�� setsid(); �Ż�ɹ�
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

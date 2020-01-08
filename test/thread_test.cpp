#include <stdlib.h>   
#include <stdio.h>   
#include <errno.h>   
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include "objbase.h"
#include "log.h"

using namespace std;


void* start_run(objPara* arg)
{
    //dosomework
    sleep(2);
    printf("%s,%d\n",__FILE__,__LINE__);
}
int main()
{
    LogInit(LL_DEBUG);

    objPara* parm = new objPara(nullptr);
    objbase::GetInstance();
    CREATE_OBJTASK("test", start_run, &parm);
    sleep(5);
    parm->GetPara();
    exit(0);
}


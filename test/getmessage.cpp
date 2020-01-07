#include "getmessage.h"
 
getMessage::getMessage()
{
 
}
getMessage::~getMessage()
{
 
}
 
int getMessage::recvMessage(CAgrcList* message,CAgrcList* outmessage,int iModule, int iCmd)
{
    std::cout<<"one receiver receive:"<<iModule<<"--"<<iCmd<<std::endl;
    printf("%s\n",(char*)message->GetAgrc("A")->GetBuff());
    return 0;
}



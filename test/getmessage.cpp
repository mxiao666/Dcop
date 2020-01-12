#include "getmessage.h"
#include "frameworkmgr.h"

int getMessage::recvMessage(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd)
{
    std::cout << "one receiver receive:" << iModule << "--" << iCmd << std::endl;
    printf("%s\n", (char *)message->GetAgrc("A")->GetBuff());
    return 0;
}
REG_FUNCTION_PLUS(getMessage, "getMessage")
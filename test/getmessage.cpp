#include "getmessage.h"
#include "frameworkmgr.h"
#include "clibase.h"
#include "objKernel.h"
int getMessage::recvMessage(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd)
{
    std::cout << "one receiver receive:" << iModule << "--" << iCmd << std::endl;
    printf("%s\n", (char *)message->GetAgrc("A")->GetBuff());
    return 0;
}
REG_FUNCTION_PLUS(getMessage, "getMessage")

class cmdTest : public CClibase
{
    int Set(CAgrcList *inPut, CAgrcList *outPut, bool *bOp)
    {
        return 0;
    }
    int Get(CAgrcList *inPut, CAgrcList *outPut, bool *bOp)
    {
        return 0;
    }
    int Add(CAgrcList *inPut, CAgrcList *outPut, bool *bOp)
    {
        return 0;
    }
    int Init()
    {
        cliMgr *cli = reinterpret_cast<cliMgr *>(g_objKernel->InterFace("cliMgr"));
        if (cli)
            cli->RegCmd("test-cmd", new cmdObj(9, this));
        return 0;
    }
};
CMD_REG_FUNCTION(cmdTest, "cmdTest")
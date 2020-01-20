#include "frameworkmgr.h"
#include "clibase.h"
#include "objKernel.h"
#include "cnotify.h"
#include "cmdid.h"
static ResTable reptbl[] =
    {
        {"ret", 8},
};
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
            cli->RegCmd("get-test", new cmdObj(this, MODELU_GET_TEST, CMD_GET_TEST, true));
        return 0;
    }
    int ResponseTable(ResTable **tbl)
    {
        *tbl = reptbl;
        return ARRAY_SIZE(reptbl);
    };
};
CMD_REG_FUNCTION(cmdTest)

class ifBoard : public objbase
{

public:
    int Process(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd)
    {
        outmessage->addAgrc("ret", "ok");
        outmessage->addCount();
        return 0;
    }
    void dump(Printfun callback = printf)
    {
        (void)callback("test module dump.\n");
    }
    int Init()
    {
        Cnotify *obj = reinterpret_cast<Cnotify *>(g_objKernel->InterFace("Cnotify"));
        if (obj)
            obj->RegReceiver(MODELU_GET_TEST, this);
        return 0;
    }
};
REG_FUNCTION_PLUS(ifBoard)
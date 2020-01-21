#include "frameworkmgr.h"
#include "clibase.h"
#include "objKernel.h"
#include "cnotify.h"
#include "cmdid.h"
static TblBody tblbody[] =
    {
        {"status", 8},
        {"error", 8},
};
ResTable reptbl = {"TEST", tblbody, ARRAY_SIZE(tblbody)};
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
        *tbl = &reptbl;
        return 1;
    };
};
CMD_REG_FUNCTION(cmdTest)

class ifBoard : public objbase
{

public:
    int Process(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
    {
        CAgrcList *list = new CAgrcList[1];
        list[0].addAgrc("status", "ok");
        list[0].addAgrc("error", "err");
        outmessage->count = 1;
        outmessage->msg = list;
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
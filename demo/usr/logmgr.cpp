#include "frameworkmgr.h"
#include "clibase.h"
#include "objKernel.h"
#include "cnotify.h"
#include "cmdid.h"
#include "log.h"
#include "template.h"
#define LEVEL "LEVEL"
static TblBody tblbody[] =
    {
        {LEVEL, 8},
};
static ResTable reptbl = {"LOG-INFO", tblbody, ARRAY_SIZE(tblbody)};
class LogCmd : public CClibase
{
    int Init()
    {
        cliMgr *cli = reinterpret_cast<cliMgr *>(g_objKernel->InterFace("cliMgr"));
        if (cli)
        {
            cli->RegCmd("set-log-level", new cmdObj(this, MODELU_LOG, CMD_SET_LOG_LEVEL, true));
            cli->RegCmd("get-log-level", new cmdObj(this, MODELU_LOG, CMD_GET_LOG_LEVEL, true));
        }
        return 0;
    }
    int ResponseTable(ResTable **tbl)
    {
        *tbl = &reptbl;
        return 1;
    };
};
CMD_REG_FUNCTION(LogCmd)
extern char LogLevelStr[][8];
class LogMgr : public objbase
{
public:
    int Process(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
    {
        int iRet = 0;
        switch (iCmd)
        {
        case CMD_GET_LOG_LEVEL:
        {
            if (outmessage == nullptr)
            {
                iRet = -1;
                break;
            }
            CAgrcList *list = new CAgrcList[1];
            list[0].addAgrc(LEVEL, LogLevelStr[GetLogLevel()]);
            outmessage->count = 1;
            outmessage->msg = list;
            iRet = 0;
            break;
        }
        case CMD_SET_LOG_LEVEL:
        {
            CStream *level = message->GetAgrc(LEVEL);
            if (level == nullptr)
            {
                iRet = -1;
                break;
            }
            int i = 0;
            for (; i < LL_LEVEL_NUM; i++)
            {
                if (OS::equal(level->c_str(), LogLevelStr[i]))
                {
                    SetLogLevel(i);
                    iRet = 0;
                    break;
                }
            }
            if (i == LL_LEVEL_NUM)
            {
                iRet = -1;
            }
            break;
        }
        default:
            iRet = -1;
            break;
        }
        return iRet;
    }
    void dump(Printfun callback = printf)
    {
        (void)callback("log-level:%s(%d)\n", LogLevelStr[GetLogLevel()], GetLogLevel());
    }
    int Init()
    {
        Cnotify *obj = reinterpret_cast<Cnotify *>(g_objKernel->InterFace("Cnotify"));
        if (obj)
            obj->RegReceiver(MODELU_LOG, this);
        return 0;
    }
};
REG_FUNCTION_PLUS(LogMgr)
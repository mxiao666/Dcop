#include "frameworkmgr.h"
#include "clibase.h"
#include "objKernel.h"
#include "cnotify.h"
#include "cmdid.h"
#include "log.h"
#include "template.h"
#define LEVEL "LEVEL"
#define TRACE "TRACE"
#define ON "ON"
#define OFF "OFF"
static TblBody tblbody[] = {
    {LEVEL, 8},
};
static RspTable reptbl = {"LOG-INFO", tblbody, ARRAY_SIZE(tblbody)};
static TblBody tracetblbody[] = {
    {TRACE, 8},
};
static RspTable tracereptbl = {"LOG-TRACE", tracetblbody, ARRAY_SIZE(tracetblbody)};
class LogCmd : public CClibase
{
    int Init()
    {
        cliMgr *cli = reinterpret_cast<cliMgr *>(g_objKernel->InterFace("cliMgr"));
        if (cli)
        {
            cli->RegCmd("set-log-level", new cmdObj(this, MODELU_LOG, CMD_SET_LOG_LEVEL));
            cli->RegCmd("get-log-level", new cmdObj(this, MODELU_LOG, CMD_GET_LOG_LEVEL, &reptbl, true));
            cli->RegCmd("set-log-trace", new cmdObj(this, MODELU_LOG, CMD_SET_LOG_TRACE));
            cli->RegCmd("get-log-trace", new cmdObj(this, MODELU_LOG, CMD_GET_LOG_TRACE, &tracereptbl, true));
        }
        return 0;
    }
};
CMD_REG_FUNCTION(LogCmd)
extern char LogLevelStr[][8];
class LogMgr : public objbase
{
private:
    int GetLevelByLog(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
    {
        if (outmessage == nullptr)
            return -1;
        CAgrcList *list = new CAgrcList[1];
        list[0].addAgrc(LEVEL, LogLevelStr[GetLogLevel()]);
        outmessage->count = 1;
        outmessage->msg = list;
        return 0;
    }
    int GetLogTreace(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
    {
        if (outmessage == nullptr)
            return -1;
        CAgrcList *list = new CAgrcList[1];
        list[0].addAgrc(TRACE, GetMethod() ? ON : OFF);
        outmessage->count = 1;
        outmessage->msg = list;
        return 0;
    }
    int SetLogByLevel(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
    {
        if (message == nullptr)
            return -1;
        CStream *level = message->GetAgrc(ARGC_DEFAULT);
        if (level == nullptr)
            return -1;
        for (int i = 0; i < LL_LEVEL_NUM; i++)
        {
            if (OS::equal(level->c_str(), LogLevelStr[i]))
            {
                SetLogLevel(i);
                return 0;
            }
        }
        return -1;
    }
    int SetLogTreace(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
    {
        if (message == nullptr)
            return -1;
        CStream *level = message->GetAgrc(ARGC_DEFAULT);
        if (level == nullptr)
            return -1;
        if (OS::equal(level->c_str(), ON))
        {
            SetMethod(true);
            return 0;
        }
        else if (OS::equal(level->c_str(), OFF))
        {
            SetMethod(false);
            return 0;
        }
        else
            return -1;
    }

public:
    int Process(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
    {
        PROCESS_BEGIN
        PROCESS_CALL(CMD_GET_LOG_LEVEL, GetLevelByLog)
        PROCESS_CALL(CMD_SET_LOG_LEVEL, SetLogByLevel)
        PROCESS_CALL(CMD_SET_LOG_TRACE, SetLogTreace)
        PROCESS_CALL(CMD_GET_LOG_TRACE, GetLogTreace)
        PROCESS_END
    }
    void dump(int fd, Printfun callback)
    {
        (void)callback(fd, "log-level:%s(%d)\n", LogLevelStr[GetLogLevel()], GetLogLevel());
        (void)callback(fd, "log-trace:%s(%d)\n", GetMethod() ? ON : OFF, GetMethod());
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
#ifndef __WIN32__
#include <unistd.h>
#endif
#include "log.h"
#include "stacktrace.h"
#include "type.h"
#include "frameworkmgr.h"
#include "clibase.h"
#include "objKernel.h"
#include "cnotify.h"
#include "cmdid.h"
#include "template.h"

#define LEVEL "LEVEL"
#define FORMAT "FORMAT"
#define TRACE "TRACE"
#define ON "ON"
#define OFF "OFF"
#define LOG_MESSAGE_LEN 1024

char LogLevelStr[][8] =
    {"ASSERT", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};

int LogInit()
{
    /* close printf buffer */
    setbuf(stdout, NULL);
    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
#ifdef ELOG_COLOR_ENABLE
    elog_set_text_color_enabled(false);
#endif
    debug_backtrace_init();
    /* start EasyLogger */
    elog_start();
    return RET_OK;
}

int LVOS_Printf(int fd, const char *format, ...)
{
    char buffer[LOG_MESSAGE_LEN] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    int nWrittenBytes = vsnprintf(buffer, sizeof(buffer), format, arg_ptr);
    va_end(arg_ptr);
    if (nWrittenBytes <= 0)
    {
        return nWrittenBytes;
    }
#ifndef __WIN32__
    if (fd != 0)
    {
        int tatol = 0;
        while (tatol != nWrittenBytes)
        {
            int len = write(fd, buffer + tatol, nWrittenBytes - tatol);
            if (len <= 0)
            {
                return len;
            }
            tatol += len;
        }
    }
    else
#endif
    {
        printf("%s", buffer);
    }
    return nWrittenBytes;
}

static TblBody tblbody[] = {
    {LEVEL, 8},
    {FORMAT, 8},
};
static RspTable reptbl = {"EVENT-LOG-INFO", tblbody, ARRAY_SIZE(tblbody)};
static TblBody tracetblbody[] = {
    {TRACE, 8},
};
static RspTable tracereptbl =
    {"LOG-TRACE", tracetblbody, ARRAY_SIZE(tracetblbody)};
class LogCmd : public CClibase
{
    virtual int Init()
    {
        cliMgr *cli =
            reinterpret_cast<cliMgr *>(g_objKernel->InterFace(MODELU_CLI));
        if (cli)
        {
            cli->RegCmd("set-log-level",
                        new cmdObj(this, MODELU_LOG, CMD_SET_LOG_LEVEL));
            cli->RegCmd("get-log-level",
                        new cmdObj(this, MODELU_LOG, CMD_GET_LOG_LEVEL,
                                   &reptbl, true));
            cli->RegCmd("set-log-trace",
                        new cmdObj(this, MODELU_LOG, CMD_SET_LOG_TRACE));
            cli->RegCmd("get-log-trace",
                        new cmdObj(this, MODELU_LOG, CMD_GET_LOG_TRACE,
                                   &tracereptbl, true));
        }
        return 0;
    }
};
REG_TO_FRAMEWORK(TABLE_TWO, MODELU_CLI, LogCmd, MODELU_CLI)
extern char LogLevelStr[][8];
class LogMgr : public objbase
{
private:
    int GetLevelByLog(CAgrcList *message,
                      RspMsg *outmessage,
                      int iModule,
                      int iCmd)
    {
        if (outmessage == nullptr)
            return -1;
        int ret = 0;
        CAgrcList *list = new CAgrcList[ELOG_LVL_TOTAL_NUM * 2];
        outmessage->msg = list;
        outmessage->count = ELOG_LVL_TOTAL_NUM;
        for (int i = 0; i < ELOG_LVL_TOTAL_NUM; i++)
        {
            char fmt[4] = {0};
            snprintf(fmt, 4, "%d", elog_get_fmt(i));
            list[i].addAgrc(LEVEL, LogLevelStr[i]);
            list[i].addAgrc(FORMAT, fmt);
        }
        return RET_OK;
    }
    int GetLogTreace(CAgrcList *message, RspMsg *outmessage,
                     int iModule, int iCmd)
    {
        if (outmessage == nullptr)
            return -1;
        CAgrcList *list = new CAgrcList[1];
        list[0].addAgrc(TRACE, elog_get_Terminal_enabled() ? ON : OFF);
        outmessage->count = 1;
        outmessage->msg = list;
        return 0;
    }
    int SetLogByLevel(CAgrcList *message, RspMsg *outmessage,
                      int iModule, int iCmd)
    {
        return -1;
        if (message == nullptr)
            return -1;
        CStream *level = message->GetAgrc(ARGC_DEFAULT);
        if (level == nullptr)
            return -1;
        for (int i = 0; i < 0; i++)
        {
            if (OS::equal(level->c_str(), LogLevelStr[i]))
            {
                // SetLogLevel(i);
                CAgrcList *list = new CAgrcList[1];
                // list[0].addAgrc(LEVEL, LogLevelStr[GetLogLevel()]);

                RspMsg *rspMessage = new RspMsg;
                rspMessage->count = 1;
                rspMessage->msg = list;
                rspMessage->cmd = CMD_GET_LOG_LEVEL;

                cliMgr::Report(rspMessage, MODELU_CLI, CMD_GET_LOG_LEVEL);

                return 0;
            }
        }
        return -1;
    }
    int SetLogTreace(CAgrcList *message, RspMsg *outmessage,
                     int iModule, int iCmd)
    {
        if (message == nullptr)
            return -1;
        CStream *level = message->GetAgrc(ARGC_DEFAULT);
        if (level == nullptr)
            return -1;
        if (OS::equal(level->c_str(), ON))
        {
            elog_set_Terminal_enabled(true);
            return 0;
        }
        else if (OS::equal(level->c_str(), OFF))
        {
            elog_set_Terminal_enabled(false);
            return 0;
        }
        else
            return -1;
    }
    int CompressLogFile(CAgrcList *message, RspMsg *outmessage,
                        int iModule, int iCmd)
    {
        log_a("Hello EasyLogger!");
        log_e("Hello EasyLogger!");
        log_w("Hello EasyLogger!");
        log_i("Hello EasyLogger!");
        log_d("Hello EasyLogger!");
        log_v("Hello EasyLogger!");
        return RET_OK;
    }

public:
    int Process(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
    {
        PROCESS_BEGIN(iCmd)
        PROCESS_CALL(CMD_GET_LOG_LEVEL, GetLevelByLog)
        PROCESS_CALL(CMD_SET_LOG_LEVEL, SetLogByLevel)
        PROCESS_CALL(CMD_SET_LOG_TRACE, SetLogTreace)
        PROCESS_CALL(CMD_GET_LOG_TRACE, GetLogTreace)
        PROCESS_CALL(CMD_MSG_TIMER, CompressLogFile)
        PROCESS_END()
    }
    void dump(int fd, Printfun callback)
    {
        objbase::PrintHead(fd, callback, "LOG_INFO", 32);
        //   (void)callback(fd, "log-level:%s(%d)\r\n",
        //                   LogLevelStr[GetLogLevel()],
        //                    GetLogLevel());
        (void)callback(fd,
                       "log-trace:%s(%d)\r\n",
                       elog_get_output_enabled() ? ON : OFF,
                       elog_get_output_enabled());
        objbase::PrintEnd(fd, callback, 32);
    }
    int Init()
    {
        LogInit();
        Cnotify *obj =
            reinterpret_cast<Cnotify *>(g_objKernel->InterFace(MODELU_NOTIFY));
        if (obj)
            obj->RegReceiver(MODELU_LOG, new REGNOTIFY(this, "LogMgr"));
        return 0;
    }
    LogMgr()
    {
    }
    ~LogMgr()
    {
    }
};

REG_TO_FRAMEWORK(TABLE_ONE, MODELU_KERNEL, LogMgr, MODELU_LOG)
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
#define VALUE "VALUE"
#define STATUS "STATUS"
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

class LogCmd : public CClibase
{
    virtual int Init()
    {
        LOCAL TblBody tblbody[] = {{LEVEL, 8}, {VALUE, 8}};
        LOCAL RspTable reptbl = {
            "EVENT-LOG-INFO", tblbody, ARRAY_SIZE(tblbody)};

        LOCAL TblBody tracetblbody[] = {{STATUS, 8}};
        LOCAL RspTable tracereptbl =
            {"LOG-TRACE", tracetblbody, ARRAY_SIZE(tracetblbody)};

        LOCAL CMD_OBJ syscmd[] =
            {
                {"set-log-level",
                 "set-log-level:level=debug,value=255",
                 MODELU_LOG, CMD_SET_LOG_LEVEL, this, &reptbl, true},
                {"get-log-level",
                 "get-log-level:debug",
                 MODELU_LOG, CMD_GET_LOG_LEVEL, this, &reptbl, true},
                {"set-log-trace",
                 "set-log-trace:on",
                 MODELU_LOG, CMD_SET_LOG_TRACE, this, &tracereptbl, true},
                {"get-log-trace",
                 "get-log-trace",
                 MODELU_LOG, CMD_GET_LOG_TRACE, this, &tracereptbl, true},
                {"set-log-text-color",
                 "set-log-text-color:off",
                 MODELU_LOG, CMD_SET_LOG_COLOR, this, &tracereptbl, true},
                {"get-log-text-color",
                 "get-log-text-color",
                 MODELU_LOG, CMD_GET_LOG_COLOR, this, &tracereptbl, true},
            };
        cliMgr *cli =
            reinterpret_cast<cliMgr *>(g_objKernel->InterFace(MODELU_CLI));
        if (cli)
        {
            cli->RegCmd(syscmd, ARRAY_SIZE(syscmd));
        }
        return 0;
    }
};
REG_TO_FRAMEWORK(TABLE_TWO, MODELU_CLI, LogCmd, MODELU_CLI)

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

        CAgrcList *list = new CAgrcList[ELOG_LVL_TOTAL_NUM * 2];
        outmessage->msg = list;
        outmessage->count = ELOG_LVL_TOTAL_NUM;
        for (int i = 0; i < ELOG_LVL_TOTAL_NUM; i++)
        {
            char fmt[4] = {0};
            snprintf(fmt, 4, "%d", elog_get_fmt(i));
            list[i].addAgrc(LEVEL, LogLevelStr[i]);
            list[i].addAgrc(VALUE, fmt);
        }
        return RET_OK;
    }
    int GetLogTreace(CAgrcList *message, RspMsg *outmessage,
                     int iModule, int iCmd)
    {
        if (outmessage == nullptr)
            return -1;
        CAgrcList *list = new CAgrcList[1];
        list[0].addAgrc(STATUS, elog_get_Terminal_enabled() ? ON : OFF);
        outmessage->count = 1;
        outmessage->msg = list;
        return 0;
    }
    int SetLogByLevel(CAgrcList *message, RspMsg *outmessage,
                      int iModule, int iCmd)
    {
        if (message == nullptr)
            return ERR_MSG_PARAM;
        CStream *level = message->GetAgrc(LEVEL);
        CStream *value = message->GetAgrc(VALUE);
        if (level == nullptr || value == nullptr)
            return ERR_MSG_PARAM;

        for (int i = 0; i < ELOG_LVL_TOTAL_NUM; i++)
        {
            if (OS::equal(level->c_str(), LogLevelStr[i]))
            {
                elog_set_fmt(i, ELOG_FMT_ALL & value->to_WORD());
                return SUCCESS;
            }
        }
        return ERR_MSG_FIELD;
    }
    int SetLogTreace(CAgrcList *message, RspMsg *outmessage,
                     int iModule, int iCmd)
    {
        if (message == nullptr)
            return ERR_MSG_PARAM;
        CStream *level = message->GetAgrc(ARGC_DEFAULT);
        if (level == nullptr)
            return ERR_MSG_FIELD;
        if (OS::equal(level->c_str(), ON))
        {
            elog_set_Terminal_enabled(true);
        }
        else
        {
            elog_set_Terminal_enabled(false);
        }
        return SUCCESS;
    }
    int SetLogTextColor(CAgrcList *message, RspMsg *outmessage,
                        int iModule, int iCmd)
    {
        if (message == nullptr)
            return ERR_MSG_PARAM;

        CStream *level = message->GetAgrc(ARGC_DEFAULT);
        if (level == nullptr)
            return ERR_MSG_FIELD;
        if (OS::equal(level->c_str(), ON))
        {
            elog_set_text_color_enabled(true);
        }
        else
        {
            elog_set_text_color_enabled(false);
        }
        return SUCCESS;
    }
    int GetLogTextColor(CAgrcList *message, RspMsg *outmessage,
                        int iModule, int iCmd)
    {
        if (outmessage == nullptr)
            return ERR_MSG_PARAM;
        CAgrcList *list = new CAgrcList[1];
        list[0].addAgrc(STATUS, elog_get_text_color_enabled() ? ON : OFF);
        outmessage->count = 1;
        outmessage->msg = list;
        return SUCCESS;
    }
    int CompressLogFile(CAgrcList *message, RspMsg *outmessage,
                        int iModule, int iCmd)
    {
        const char *pstr = __FILE__;
        log_a("Hello EasyLogger!");
        log_e("Hello EasyLogger!");
        log_w("Hello EasyLogger!");
        log_i("Hello EasyLogger!");
        log_d("Hello EasyLogger!");
        log_v("Hello EasyLogger!");
        elog_hexdump("test", 16, (BYTE *)pstr, strlen(pstr));
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
        PROCESS_CALL(CMD_SET_LOG_COLOR, SetLogTextColor)
        PROCESS_CALL(CMD_GET_LOG_COLOR, GetLogTextColor)
        PROCESS_END()
    }
    void dump(int fd, Printfun callback)
    {
        objbase::PrintHead(fd, callback, "LOG_INFO", 32);
        for (int i = 0; i < ELOG_LVL_TOTAL_NUM; i++)
        {
            (void)callback(fd,
                           "log-level:%s(%d)\r\n",
                           LogLevelStr[i],
                           elog_get_fmt(i));
        }
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
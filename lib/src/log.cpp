#include "log.h"
#include "stacktrace.h"
#include "macro_define.h"
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#ifndef __WIN32__
#include <unistd.h>
#include <functional>
#endif
#include "frameworkmgr.h"
#include "clibase.h"
#include "objKernel.h"
#include "cnotify.h"
#include "cmdid.h"
#include "template.h"
#define LEVEL "LEVEL"
#define TRACE "TRACE"
#define ON "ON"
#define OFF "OFF"

const int LOG_MESSAGE_LEN = 512;
const int LOG_CONTENT_LEN = 64;

static FILE *log_file;
static int log_level = LL_INFO;
static bool bConsolePrint = false;
char LogLevelStr[][8] =
    {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
int GetMethod() { return bConsolePrint; }
int SetMethod(bool method)
{
    bConsolePrint = method;
    return method;
}
int GetLogLevel() { return log_level; }
int SetLogLevel(int level)
{
    log_level = level;
    return level;
}
void LogDestory()
{
    fclose(log_file);
}
int LogInit(int level, const char *path)
{
    log_level = level;
    log_file = fopen(path, "at+");
    if (NULL == log_file)
    {
        return RET_ERR;
    }
    setvbuf(log_file, NULL, _IOLBF,
#ifndef __WIN32__
            0
#else
            512
#endif
    ); /* 行缓存*/
    debug_backtrace_init();
    return RET_OK;
}

int WriteLog(int v_level, int line,
             const char *func, const char *file,
             const char *format, ...)
{
    if (log_file == nullptr)
        return RET_ERR;
    if (v_level > LL_FATAL_ERROR || v_level < LL_DEBUG)
        return RET_ERR;
    if (log_level > v_level)
        return RET_ERR;

    /* ---时间-- */
    char log_time[LOG_CONTENT_LEN] = {0};
    time_t t = time(NULL);
    struct tm ptm;
#ifndef __WIN32__
    localtime_r(&t, &ptm);
#else
    localtime_s(&ptm, &t);
#endif
    snprintf(log_time,
             LOG_CONTENT_LEN - 1,
             "%4d-%02d-%02d %02d:%02d:%02d",
             ptm.tm_year + 1900,
             ptm.tm_mon + 1,
             ptm.tm_mday,
             ptm.tm_hour,
             ptm.tm_min,
             ptm.tm_sec);

    /* ---文件--行号---函数--- */
    char log_pos[LOG_CONTENT_LEN] = {0};
    const char *linuxpos = strrchr(file, '/');
    const char *winwpso = strrchr(file, '\\');
    snprintf(log_pos, LOG_CONTENT_LEN - 1,
             " [%s] [%s:%d] [%s] ",
             LogLevelStr[v_level],
             (NULL != linuxpos)
                 ? (linuxpos + 1)
                 : (NULL != winwpso)
                       ? (winwpso + 2)
                       : file,
             line, func);

    /* ---日志内容--- */
    char log_msg[LOG_MESSAGE_LEN] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    int nWrittenBytes = vsnprintf(log_msg, sizeof(log_msg), format, arg_ptr);
    if (nWrittenBytes < 0)
    {
        //perror("vsnprintf");
        return RET_ERR;
    }
    va_end(arg_ptr);

    /* ---完整日志拼接--- */
    nWrittenBytes = fprintf(log_file, "%s%s%s\n", log_time, log_pos, log_msg);
    if (bConsolePrint)
        printf("%s%s%s\n", log_time, log_pos, log_msg);
    return nWrittenBytes;
}

int LVOS_Printf(int fd, const char *format, ...)
{
    char buffer[1024] = {0};
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
};
static RspTable reptbl = {"LOG-INFO", tblbody, ARRAY_SIZE(tblbody)};
static TblBody tracetblbody[] = {
    {TRACE, 8},
};
static RspTable tracereptbl =
    {"LOG-TRACE", tracetblbody, ARRAY_SIZE(tracetblbody)};
class LogCmd : public CClibase, public objbase
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
        //    if (outmessage == nullptr)
        //       return -1;
        CAgrcList *list = new CAgrcList[1];
        list[0].addAgrc(LEVEL, LogLevelStr[GetLogLevel()]);
        //   outmessage->count = 1;
        //  outmessage->msg = list;
        cliMgr *objptr = (cliMgr *)g_objKernel->InterFace(MODELU_CLI);
        RspMsg *rspMessage = new RspMsg;
        rspMessage->count = 1;
        rspMessage->msg = list;
        rspMessage->cmd = CMD_GET_LOG_LEVEL;
        if (objptr != nullptr)
        {
            objptr->Report(rspMessage, MODELU_CLI, CMD_GET_LOG_LEVEL);
        }
        return 0;
    }
    int GetLogTreace(CAgrcList *message, RspMsg *outmessage,
                     int iModule, int iCmd)
    {
        if (outmessage == nullptr)
            return -1;
        CAgrcList *list = new CAgrcList[1];
        list[0].addAgrc(TRACE, GetMethod() ? ON : OFF);
        outmessage->count = 1;
        outmessage->msg = list;
        return 0;
    }
    int SetLogByLevel(CAgrcList *message, RspMsg *outmessage,
                      int iModule, int iCmd)
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
        PROCESS_BEGIN(iCmd)
        PROCESS_CALL(CMD_GET_LOG_LEVEL, GetLevelByLog)
        PROCESS_CALL(CMD_SET_LOG_LEVEL, SetLogByLevel)
        PROCESS_CALL(CMD_SET_LOG_TRACE, SetLogTreace)
        PROCESS_CALL(CMD_GET_LOG_TRACE, GetLogTreace)
        PROCESS_END()
    }
    void dump(int fd, Printfun callback)
    {
        (void)callback(fd, "log-level:%s(%d)\n",
                       LogLevelStr[GetLogLevel()],
                       GetLogLevel());
        (void)callback(fd,
                       "log-trace:%s(%d)\n",
                       GetMethod() ? ON : OFF,
                       GetMethod());
    }
    int Init()
    {
        Cnotify *obj =
            reinterpret_cast<Cnotify *>(g_objKernel->InterFace(MODELU_NOTIFY));
        if (obj)
            obj->RegReceiver(MODELU_LOG, new REGNOTIFY(this, "LogMgr"));
        return 0;
    }
    LogMgr()
    {
        LogInit(LL_WARNING);
    }
    ~LogMgr()
    {
        LogDestory();
    }
};

REG_TO_FRAMEWORK(TABLE_ONE, MODELU_KERNEL, LogMgr, MODELU_LOG)
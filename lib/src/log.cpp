#include "log.h"
#include "stacktrace.h"
#include "type.h"
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
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

#define LOG_MESSAGE_LEN 1024
#define MAX_BUFFER_MANAGE_SIZE (10)     //一个日志文件最大限制xM
#define MB_TO_BYTE(x) ((x)*1024 * 1024) //兆转字节
#define MAX_DATE_STR_LEN (16)
#define MAX_CMD_BUFFER_LEN (256 + 32)
#define MAX_FILE_PATH_LEN (128)
bool gIsCompressLog = false;
static FILE *log_file;
static int log_level = LL_INFO;
static bool bConsolePrint = false;
static char Debug_Logfile[MAX_FILE_PATH_LEN] = "message";
char LogLevelStr[][8] =
    {"DEBUG", "INFO", "WARN", "ERROR", "FATAL", "TRACE"};
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
static int _reOpen(const char *path)
{
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
    return RET_OK;
}
int LogInit(int level, const char *path)
{
    int len = 0;
    gIsCompressLog = false;
    log_level = level;
    _reOpen(path);
    debug_backtrace_init();
    len = snprintf(Debug_Logfile,
                   MAX_FILE_PATH_LEN,
                   "%s", path);
    Debug_Logfile[len - 1] = '\0';
    return RET_OK;
}
int CompressLogFilePack(void)
{
#ifndef _MSC_VER
    struct stat statbuff;
    struct tm now_time;
    time_t timer = time(NULL);
    char datetime[MAX_DATE_STR_LEN] = {0};
    char pCmdBuf[MAX_CMD_BUFFER_LEN] = {0};

    if (stat(Debug_Logfile, &statbuff) < 0)
    {
        return RET_ERR;
    }

    if (MB_TO_BYTE(MAX_BUFFER_MANAGE_SIZE) < statbuff.st_size)
    {
        gIsCompressLog = true;
        (void)fclose(log_file);
        log_file = NULL;

        //获取时间字符串
        strftime(datetime, MAX_DATE_STR_LEN,
                 "%Y%m%d%H%M%S",
                 localtime_r(&timer, &now_time));
        //移动文件重命名
        snprintf(pCmdBuf, MAX_CMD_BUFFER_LEN,
                 "mv %s %s_%s 2> /dev/null",
                 Debug_Logfile, Debug_Logfile, datetime);
        system(pCmdBuf);
        _reOpen(Debug_Logfile);
        gIsCompressLog = false;
    }
#endif
    return RET_OK;
}
int WriteLog(int v_level, int line,
             const char *func, const char *file,
             const char *format, ...)
{
    if (log_file == nullptr)
        return RET_ERR;
    if (log_level > v_level)
        return RET_ERR;

    /* ---时间-- */
    char log_msg[LOG_MESSAGE_LEN] = {0};
    time_t t = time(NULL);
    struct tm ptm;
    int nWrittenBytes = 0;
#ifndef __WIN32__
    localtime_r(&t, &ptm);
#else
    localtime_s(&ptm, &t);
#endif
    nWrittenBytes = snprintf(log_msg,
                             LOG_MESSAGE_LEN,
                             "%4d-%02d-%02d %02d:%02d:%02d",
                             ptm.tm_year + 1900,
                             ptm.tm_mon + 1,
                             ptm.tm_mday,
                             ptm.tm_hour,
                             ptm.tm_min,
                             ptm.tm_sec);

    /* ---文件--行号---函数--- */
    const char *linuxpos = strrchr(file, '/');
    const char *winwpos = strrchr(file, '\\');
    nWrittenBytes += snprintf(log_msg + nWrittenBytes,
                              LOG_MESSAGE_LEN - nWrittenBytes,
                              " [%s] [%s:%d] [%s] ",
                              LogLevelStr[v_level],
                              (NULL != linuxpos)
                                  ? (linuxpos + 1)
                                  : (NULL != winwpos)
                                        ? (winwpos + 2)
                                        : file,
                              line, func);

    /* ---日志内容--- */
    va_list arg_ptr;
    va_start(arg_ptr, format);
    nWrittenBytes += vsnprintf(log_msg + nWrittenBytes,
                               LOG_MESSAGE_LEN - nWrittenBytes,
                               format, arg_ptr);
    va_end(arg_ptr);
    if (log_msg[nWrittenBytes - 1] != '\n')
    {
        nWrittenBytes += snprintf(log_msg + nWrittenBytes,
                                  LOG_MESSAGE_LEN - nWrittenBytes,
                                  "%s", "\n");
    }
    while (gIsCompressLog)
    {
        /* 等待文件打开 */
    }

    /* ---完整日志拼接--- */
    fwrite(log_msg, 1, nWrittenBytes, log_file);
    if (bConsolePrint)
        printf("%s", log_msg);
    return nWrittenBytes;
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
        CAgrcList *list = new CAgrcList[1];
        list[0].addAgrc(LEVEL, LogLevelStr[GetLogLevel()]);
        outmessage->count = 1;
        outmessage->msg = list;
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
                CAgrcList *list = new CAgrcList[1];
                list[0].addAgrc(LEVEL, LogLevelStr[GetLogLevel()]);

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
    int CompressLogFile(CAgrcList *message, RspMsg *outmessage,
                        int iModule, int iCmd)
    {
        CompressLogFilePack();
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
        (void)callback(fd, "log-level:%s(%d)\r\n",
                       LogLevelStr[GetLogLevel()],
                       GetLogLevel());
        (void)callback(fd,
                       "log-trace:%s(%d)\r\n",
                       GetMethod() ? ON : OFF,
                       GetMethod());
        objbase::PrintEnd(fd, callback, 32);
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
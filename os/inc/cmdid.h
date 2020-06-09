#ifndef __CMDID_H__
#define __CMDID_H__
#include <type.h>
enum CMD_ID
{
    CMD_BASE_ID = 0,
    CMD_GET_LOG_LEVEL,
    CMD_SET_LOG_LEVEL,
    CMD_GET_LOG_TRACE,
    CMD_SET_LOG_TRACE,
    CMD_MSG_REPORT,
    CMD_MSG_TIMER,
    CMD_SYS_HELP,
    CMD_SYS_DUMP,
    CMD_SYS_EXIT,
    CMD_SYS_SHUT_DOWN,
    CMD_SYS_ERR_CODE,
    CMD_SET_LOG_COLOR,
    CMD_GET_LOG_COLOR,
    CMD_BASE_NUM,
};

enum MODELU_ID
{
    MODULE_ALL,
    MODELU_KERNEL,
    MODELU_LOG,
    MODELU_NOTIFY,
    MODELU_CLI,
    MODELU_TASK,
    MODELU_TELNET,
    MODELU_TIMER,
    MODULE_BASE_END,
};

enum ERR_CODE
{
    ERROR = -1,      //Operation failure
    SUCCESS,         //Operation is successful
    ERR_DISPATCH,    //Command distribution failed
    ERR_NOTIFY,      //Message publishing failed
    ERR_REPORT,      //Failed to report the message
    ERR_PARAM,       //Parameter processing error
    ERR_UNKOWN,      //Unregistered error
    ERR_NOT_SUPPORT, //Feature not supported
    ERR_MSG_PARAM,   //Message parameter is invalid
    ERR_MSG_FIELD,   //Invalid message field
};
typedef struct
{
    int id;
    const char *name;
    const char *desc;
} ERR_CODE_INFO;

IMPORT ERR_CODE_INFO gErrCode[]; //ÏµÍ³×¢²á´íÎóÂë
IMPORT int gErrCodeCnt;

#endif
#ifndef __CMDID_H__
#define __CMDID_H__
enum
{
    CMD_BASE_ID = 0,
    CMD_GET_LOG_LEVEL,
    CMD_SET_LOG_LEVEL,
    CMD_GET_LOG_TRACE,
    CMD_SET_LOG_TRACE,       
    CMD_BASE_NUM,
};
enum
{
    MODULE_BASE_ID = 0,
    MODELU_LOG,
    MODULE_BASE_END,
};
#endif
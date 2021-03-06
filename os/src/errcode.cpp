#include <cmdid.h>
ERR_CODE_INFO gErrCode[] =
    {
        {ERROR, "ERROR", "Operation failure"},
        {SUCCESS, "SUCCESS", "Operation is successful"},
        {ERR_DISPATCH, "ERR_DISPATCH", "Command distribution failed"},
        {ERR_NOTIFY, "ERR_NOTIFY", "Message publishing failed"},
        {ERR_REPORT, "ERR_REPORT", "Failed to report the message"},
        {ERR_PARAM, "ERR_PARAM", "Parameter processing error"},
        {ERR_UNKOWN, "ERR_UNKOWN", "Unregistered error"},
        {ERR_NOT_SUPPORT, "ERR_NOT_SUPPORT", "Feature not supported"},
        {ERR_MSG_PARAM, "ERR_MSG_PARAM", "Message parameter is invalid"},
        {ERR_MSG_FIELD, "ERR_MSG_FIELD", "Invalid message field"},
};
int gErrCodeCnt = sizeof(gErrCode) / sizeof(gErrCode[0]);
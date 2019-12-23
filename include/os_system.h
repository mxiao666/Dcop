/***********************************************************************************
 * 文 件 名   : os_system.h
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年4月1日
 * 文件描述   :  系统性的功能函数封装头文件
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   :
 * 修改日志   :
***********************************************************************************/


#ifndef __OS_SYSTEM_H__
#define __OS_SYSTEM_H__
#include "os_macro_define.h"
#include <sys/types.h>

#define STDOUT_FILENOED 2
#define PATH_NAME_MAX_LEN 256
#define OM_CMD_EXCE_TIME 5
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * 函 数 名  : OS_SafeSystem
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年4月1日
 * 函数功能  : 带超时机制的执行脚本命令
 * 输入参数  : char *pcCmd       待执行命令
               char *argv[]      执行命令时需要的参数
               u32 uiTimeOut     指定命令最长执行时间
               s32 *piScriptRet  命令执行退出结果
 * 输出参数  : 无
 * 返 回 值  :
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
s32 OS_SafeSystem(char *pcCmd, char *argv[], u32 uiTimeOut, s32 *piScriptRet);

/*****************************************************************************
 * 函 数 名  : OS_SafeSystemSub
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年6月16日
 * 函数功能  : 带超时机制的执行脚本命令，同时可以获知子进程的pid
 * 输入参数  : char *pcCmd        待执行命令
               char *argv[]       命令执行参数
               u32 uiTimeOut      命令执行超时时间
               s32 *piScriptRet   命令自行退出状态
               pid_t *ptChildPid  子线程id
 * 输出参数  : 无
 * 返 回 值  : 执行结果
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
s32 OS_SafeSystemSub(char *pcCmd, char *argv[], u32 uiTimeOut, s32 *piScriptRet, pid_t *ptChildPid);

/*****************************************************************************
 * 函 数 名  : OS_WaitChild
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年4月1日
 * 函数功能  : 等待子进程执行退出，带超时机制，同时可以获知脚本执行输出结果
 * 输入参数  : pid_t uiChildPid  子线程id
               int *piFd         文件描述符
               u32 uiTimeout     等待子线程最大执行时间
               s32 *iScriptRet   子线程退出状态
               char *pOutBuf     子线程输出缓存
               u64 uiOutBufLen   缓存大小
 * 输出参数  : 无
 * 返 回 值  : 执行结果
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
s32 OS_WaitChild(pid_t uiChildPid, int *piFd, u32 uiTimeout, s32 *iScriptRet, char *pOutBuf, u64 uiOutBufLen);

/*****************************************************************************
 * 函 数 名  : OS_CheckReadBuf
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年4月1日
 * 函数功能  : 通过句柄变化获知是否可以读数据
 * 输入参数  : s32 v_uiFd       文件描述符
               char *pOutBuf    输出缓存地址
               u32 uiOutBufLen  输出缓存的大小
 * 输出参数  : 无
 * 返 回 值  : 执行结果
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
s32 OS_CheckReadBuf(s32 v_uiFd, char *pOutBuf, u32 uiOutBufLen);

/*****************************************************************************
 * 函 数 名  : OS_Kill
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年6月16日
 * 函数功能  : kill指定的进程
 * 输入参数  : pid_t uiChildPid  需要杀死的进程id
 * 输出参数  : 无
 * 返 回 值  : 执行结果
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
s32 OS_Kill(pid_t uiChildPid);

/*****************************************************************************
 * 函 数 名  : OS_GetProcessStatus
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年4月1日
 * 函数功能  : 获取子进程退出的状态D
 * 输入参数  : pid_t uiPid     子线程pid
               char *v_Status  输出子线程执行状态
 * 输出参数  : 无
 * 返 回 值  : 执行结果
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
s32 OS_GetProcessStatus(pid_t uiPid, char *v_Status);

/*****************************************************************************
 * 函 数 名  : OS_GetExitStatus
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年4月1日
 * 函数功能  :  获取子进程退出的的状态码
 * 输入参数  : s32 iStatus       线程退出状态
               s32 *v_ScriptRet  输出状态
 * 输出参数  : 无
 * 返 回 值  : 执行结果
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
s32 OS_GetExitStatus(s32 iStatus, s32 *v_ScriptRet);
/*****************************************************************************
 * 函 数 名  : OS_GetSystemLlitm
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年4月1日
 * 函数功能  : 获取系统谁时间，毫秒级
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 秒级时间整数
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
u64 OS_GetSystemLlitm();

/*****************************************************************************
 * 函 数 名  : OS_ReadBufByCmd
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年4月1日
 * 函数功能  :  读取buffer中数据
 * 输入参数  : const char * pacCmd  待执行命令
               u32 uiTimeout        命令执行超时限制
               char *pBuffer        命令输出结果缓存地址
               u64 uiBufferLen      缓存地址大小限制
 * 输出参数  : 无
 * 返 回 值  : 执行结果
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
s32 OS_ReadBufByCmd(const char * pacCmd, u32 uiTimeout, char *pBuffer, u64 uiBufferLen);

/*****************************************************************************
 * 函 数 名  : OS_GetStrValueByCmd
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年4月1日
 * 函数功能  : 获取脚本执行后输出结果
 * 输入参数  : const char * pacCmd  待执行命令
               char *pBuffer        命令执行输出结果缓存地址
               u64 uiBufferLen      缓存地址大小限制
 * 输出参数  : 无
 * 返 回 值  :
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
s32 OS_GetStrValueByCmd(const char * pacCmd, char *pBuffer, u64 uiBufferLen);

#ifdef __cplusplus
}
#endif
#endif

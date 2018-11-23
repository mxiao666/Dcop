/***********************************************************************************
 * �� �� ��   : os_system.h
 * �� �� ��   : ¬����
 * ��������   : 2018��4��1��
 * �ļ�����   :  ϵͳ�ԵĹ��ܺ�����װͷ�ļ�
 * ��Ȩ˵��   : Copyright (c) 2008-2018   xx xx xx xx �������޹�˾
 * ��    ��   :
 * �޸���־   :
***********************************************************************************/


#ifndef __OS_SYSTEM_H__
#define __OS_SYSTEM_H__

#define STDOUT_FILENOED 2
#define PATH_NAME_MAX_LEN 256
#define OM_CMD_EXCE_TIME 5
#ifdef __cplusplus
extern "C" {
#endif
s32 OS_SafeSystem(char *pcCmd, char *argv[], u32 uiTimeOut, s32 *piScriptRet);
s32 OS_SafeSystemSub(char *pcCmd, char *argv[], u32 uiTimeOut, s32 *piScriptRet, pid_t *ptChildPid);
s32 OS_WaitChild(pid_t uiChildPid, int *piFd, u32 uiTimeout, s32 *iScriptRet, char *pOutBuf, u64 uiOutBufLen);
s32 OS_CheckReadBuf(s32 v_uiFd, char *pOutBuf, u32 uiOutBufLen);
s32 OS_Kill(pid_t uiChildPid);
s32 OS_GetProcessStatus(pid_t uiPid, char *v_Status);
s32 OS_GetExitStatus(s32 iStatus, s32 *v_ScriptRet);
u64 OS_GetSystemLlitm();
s32 OS_ReadBufByCmd(const char * pacCmd, u32 uiTimeout, char *pBuffer, u64 uiBufferLen);
s32 OS_GetStrValueByCmd(const char * pacCmd, char *pBuffer, u64 uiBufferLen);
#ifdef __cplusplus
}
#endif
#endif
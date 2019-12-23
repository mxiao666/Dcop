/***********************************************************************************
 * �� �� ��   : os_macro_define.h
 * �� �� ��   : ¬����
 * ��������   : 2018��11��23��
 * �ļ�����   : ���ͳ�������
 * ��Ȩ˵��   : Copyright (c) 2008-2018   xx xx xx xx �������޹�˾
 * ��    ��   : 
 * �޸���־   : 
***********************************************************************************/

#ifndef  _MACRO_DEFINE
#define  _MACRO_DEFINE

typedef  long int s32;
typedef  unsigned int u32;
typedef  long long int s64;
typedef  unsigned long long int u64;
typedef  unsigned char BYTE;


#define RET_OK 0
#define RET_ERR -1
#define RET_VOID

#define RET_EXCEPTIONAL 128//�쳣�˳�
#define RET_TIMEOUT     129//��ʱ�˳�
#define RET_INDIDE_ERR  130//�ڲ�����
#define RET_STAT_D      131//����D״̬

#define STAT_PATH_LEN 64
#define STAT_BUFFER_LEN 288
#define TV_USEC_VALUE 200
#define INVALUE_INAVLE  0xffffffffffffffffUL
#endif

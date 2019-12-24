/***********************************************************************************
 * �� �� ��   : os_stacktrace.c
 * �� �� ��   : ¬����
 * ��������   : 2018��11��23��
 * �ļ�����   : ��ӡ�������ö�ջ��Ϣ
 * ��Ȩ˵��   : Copyright (c) 2008-2018   xx xx xx xx �������޹�˾
 * ��    ��   : 
 * �޸���־   : 
***********************************************************************************/

#include <errno.h>
#include <unistd.h>
#include <ucontext.h>
#include <signal.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include "os_macro_define.h"
#include "os_log.h"

/*****************************************************************************
 * �� �� ��  : signals_trace
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ��Ҫ��׽�����ź�
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : static
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
static BYTE signals_trace[] =
{
    SIGILL,  /* Illegal instruction (ANSI).  */
    SIGABRT, /* Abort (ANSI).  */
    SIGBUS,  /* BUS error (4.2 BSD). (unaligned access) */
    SIGFPE,  /* Floating-point exception (ANSI).  */
    SIGSEGV, /* Segmentation violation (ANSI).  */
};

#define BACKTRACE_SIZE   64
/*****************************************************************************
 * �� �� ��  : debug_backtrace_dump
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ��ӡ��ջ��Ϣ
 * �������  : void   
 * �������  : ��
 * �� �� ֵ  : void
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
extern "C" void debug_backtrace_dump(void)
{
	int j, nptrs;
	void *buffer[BACKTRACE_SIZE];
	char **strings;
	
	nptrs = backtrace(buffer, BACKTRACE_SIZE);
	
	LOG_ERROR("stacktrace returned %d addresses", nptrs);
 
	strings = backtrace_symbols(buffer, nptrs);
	if (strings == NULL) {
		LOG_ERROR("backtrace_symbols");
		exit(EXIT_FAILURE);
	}
 
	for (j = 0; j < nptrs; j++)
		LOG_ERROR("\t[%02d] %s", j, strings[j]);
 
	free(strings);
}

/*****************************************************************************
 * �� �� ��  : debug_signal_handler
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : �źŲ�ص�����
 * �������  : int sig_num      �ź�
               siginfo_t *info  �źŴ���ṹ��
               void *ucontext   ����������
 * �������  : ��
 * �� �� ֵ  : void
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
extern "C" void debug_signal_handler(int sig_num, siginfo_t *info, void *ucontext)
{
	
#ifdef _DEBUG_ 	
	char buff[64] = {0x00};
	sprintf(buff,"cat /proc/%d/maps", getpid());		
	system((const char*) buff);
#endif	

	LOG_ERROR("=========>>>catch signal %s(%d) <<<=========", strsignal(sig_num), sig_num);
	LOG_ERROR("Dump stack start...");
	debug_backtrace_dump();
	LOG_ERROR("Dump stack end...");
#ifdef _DEBUG_ 
	signal(sig_num, SIG_DFL); /* �ָ��ź�Ĭ�ϴ��� */
	raise(sig_num);           /* ���·����ź� */
#else    
    exit(EXIT_FAILURE);    
#endif
}

/*****************************************************************************
 * �� �� ��  : debug_backtrace_init
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ע����Ҫ��׽���źŲ�
 * �������  : void   
 * �������  : ��
 * �� �� ֵ  : int
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
extern "C" int debug_backtrace_init(void)
{    
    int i;    
    struct sigaction sa;    
    (void)memset(&sa, 0, sizeof(struct sigaction));    
    sa.sa_sigaction = debug_signal_handler;    
    sa.sa_flags = SA_RESTART | SA_SIGINFO;    
    int ret = 0;    
    for (i = 0; i < (sizeof(signals_trace) / sizeof(signals_trace[0])); ++i) 
    {        
        if (sigaction(signals_trace[i], &sa, NULL) != 0) 
        {    
#if _DEBUG_	      
            fprintf(stderr, "Failed to set signal handler for %s(%d)!\n",                    
                strsignal(signals_trace[i]), signals_trace[i]); 
#else
            LOG_ERROR("Failed to set signal handler for %s(%d)!", strsignal(signals_trace[i]), signals_trace[i]);
#endif
            ret = -1;            
            break;        
        }    
    }   
    return ret;
}


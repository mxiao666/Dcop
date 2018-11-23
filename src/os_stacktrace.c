#include <errno.h>
#include <unistd.h>
#include <ucontext.h>
#include <signal.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include "os_macro_define.h"

static byte signals_trace[] =
{
    SIGILL,  /* Illegal instruction (ANSI).  */
    SIGABRT, /* Abort (ANSI).  */
    SIGBUS,  /* BUS error (4.2 BSD). (unaligned access) */
    SIGFPE,  /* Floating-point exception (ANSI).  */
    SIGSEGV, /* Segmentation violation (ANSI).  */
};

#define BACKTRACE_SIZE   64
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
 
extern "C" void debug_signal_handler(int sig_num, siginfo_t *info, void *ucontext)
{
	
#ifdef _DEBUG_ 	
	char buff[64] = {0x00};
	sprintf(buff,"cat /proc/%d/maps", getpid());		
	system((const char*) buff);
#endif	

	LOG_ERROR("=========>>>catch signal %d <<<=========", sig_num);	
	LOG_ERROR("Dump stack start...");
	debug_backtrace_dump();
	LOG_ERROR("Dump stack end...");
    
#ifdef _DEBUG_ 
	signal(sig_num, SIG_DFL); /* 恢复信号默认处理 */
	raise(sig_num);           /* 重新发送信号 */
#else    
    exit(EXIT_FAILURE);    
#endif
}

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


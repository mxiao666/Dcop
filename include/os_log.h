#ifndef   _MACRO_LogModule__
#define   _MACRO_LogModule__
#include <stdio.h>
#include <pthread.h>
#include "os_macro_define.h"
#include "os_frameworkmgr.h"

/* 每个线程的buffer size*/
#define   _LOG_BUFFSIZE  1024*1024*4
/* log 文件字符串路径最大长度*/
#define	  _LOG_PATH_LEN  256
/* 日志对应的模块名*/
#define   _LOG_MODULE_LEN 32

typedef  enum LogLevel {  
	LL_DEBUG,
	LL_TRACE,
	LL_NOTICE, 
	LL_WARNING , 
	LL_ERROR
}LogLevel;


/**
*	Log_Writer  日志类
*/
class Log_Writer: public IplugFrameWork{
	public:
		Log_Writer(){
			m_system_level = LL_NOTICE;
			//fp = stderr;
			m_fp = NULL;
			m_issync = false;
			m_isappend = true;
			m_filelocation[0] ='\0'; 
			pthread_mutex_init(&m_mutex, NULL);
		}
		~Log_Writer(){
			logclose();
		}
		int Init();
		bool loginit(LogLevel l, const  char *filelocation, bool append = true, bool issync = false);
		bool log(LogLevel l,char *logformat,...);
        inline LogLevel getLevel();
		bool logclose();
	private:
		inline const char* logLevelToString(LogLevel l);
		inline bool IsValidLevel(LogLevel l);
		int premakestr(char* m_buffer, LogLevel l);
		bool _write(char *_pbuffer, int len);
	private:
		enum LogLevel m_system_level;
		FILE* m_fp;
		bool m_issync;
		bool m_isappend;
		char m_filelocation[_LOG_PATH_LEN];
		pthread_mutex_t m_mutex;
		static __thread char m_buffer[_LOG_BUFFSIZE];
		//The __thread specifier may be applied to any global, file-scoped static, function-scoped static, 
		//or static data member of a class. It may not be applied to block-scoped automatic or non-static data member
		//in the log  scence,It's safe!!!!
		//一言以蔽之，此场景不用担心__thread带来资源leak,同时也不用担心多个Log_Writer会干扰，
		//因为一个线程同一时间只有一个Log_Writer在干活，干完之后m_buffer就reset了
		//所以即便一个线程用户多个Log_Write串行(因为一个线程内的运行态只有串行) 也是线程安全的！！！
};

extern Log_Writer INFO_W;

/**
 * LogLevel 日志级别
 * p_modulename 模块名 如mysql
 * p_logdir  日志输出目录
 * */
bool log_init(LogLevel l, const char* p_modulename, const char* p_logdir);
#endif

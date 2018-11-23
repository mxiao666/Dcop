#ifndef  _MACRO_DEFINE
#define  _MACRO_DEFINE
#include "os_log_macro_define.h"

typedef  long int s32;
typedef  unsigned int u32;
typedef  long long int s64;
typedef  unsigned long long int u64;
typedef  unsigned char byte;

#define RET_OK 0
#define RET_ERR -1
#define RET_VOID

#define RET_EXCEPTIONAL 128//异常退出
#define RET_TIMEOUT     129//超时退出
#define RET_INDIDE_ERR  130//内部错误
#define RET_STAT_D      131//进程D状态

#define STAT_PATH_LEN 64
#define STAT_BUFFER_LEN 288
#define TV_USEC_VALUE 200
#define INVALUE_INAVLE  0xffffffffffffffffUL
#endif

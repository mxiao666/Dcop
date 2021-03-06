/*****************************************************************************
 * 文 件 名   : os_macro_define.h
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月23日
 * 文件描述   : 类型常量定义
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
******************************************************************************/

#ifndef _MACRO_DEFINE
#define _MACRO_DEFINE

typedef long int s32;
typedef unsigned int u32;
typedef long long int s64;
typedef unsigned long long int u64;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned long       ULONG;
typedef unsigned short      USHORT;
typedef unsigned char       UCHAR;
typedef signed char         INT8;
typedef signed long          LONG;
typedef signed short        SHORT;
typedef int                 BOOL;
typedef int                 STATUS;
typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;

#define	IMPORT	extern
#define	LOCAL	static

#define RET_OK 0
#define RET_ERR -1
#define RET_VOID

#define RET_EXCEPTIONAL 128 //异常退出
#define RET_TIMEOUT 129     //超时退出
#define RET_INDIDE_ERR 130  //内部错误
#define RET_STAT_D 131      //进程D状态

#define STAT_PATH_LEN 64
#define STAT_BUFFER_LEN 288
#define TV_USEC_VALUE 200
#define INVALUE_INAVLE 0xffffffffffffffffUL
#endif

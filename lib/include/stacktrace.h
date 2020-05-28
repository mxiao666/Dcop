/******************************************************************************
 * 文 件 名   : os_stacktrace.h
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月23日
 * 文件描述   : 打印函数堆栈信息
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
*******************************************************************************/

#ifndef __OS_STACKTRACE_H__
#define __OS_STACKTRACE_H__

#ifdef __cplusplus
extern "C"
{
#endif
    /*************************************************************************
 * 函 数 名  : debug_backtrace_init
 * 负 责 人  : 21211
 * 创建日期  : 2018年11月23日
 * 函数功能  : 注册需要捕捉的信号槽
 * 输入参数  : void   
 * 输出参数  : 无
 * 返 回 值  : extern
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
    int debug_backtrace_init(void);

#ifdef __cplusplus
}
#endif
#endif

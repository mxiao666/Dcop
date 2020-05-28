/******************************************************************************
 * 文 件 名   : os_frameworkmgr.h
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月23日
 * 文件描述   : 框架自动注册调用
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
******************************************************************************/

#ifndef __OS_FRAMEWORKMGR__
#define __OS_FRAMEWORKMGR__
#include "objbase.h"
#include "cmdid.h"
/*****************************************************************************
 * 函 数 名  : FrameWork
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 注册链表结构体
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : typedef
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
typedef struct FrameWork
{
    int SuperId; // 目的注册类Id
    int id;      // 自身id
    void *fun;
    const char *ModuleName;
    FrameWork *Next;
} FrameWork;

/*****************************************************************************
 * 函 数 名  : FrameWorkMgr
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 链表注册管理类
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
enum
{
    TABLE_ONE,
    TABLE_TWO,
    TABLE_THREE,
    TABLE_NUM
};
class FrameWorkMgr
{
public:
    static FrameWork *m_Node[TABLE_NUM];
    static FrameWork *m_Current[TABLE_NUM];
    static int m_NodeCnt[TABLE_NUM];
    void RegInit();
    static FrameWorkMgr *getInstance();

private:
    FrameWorkMgr() {}
    FrameWorkMgr(FrameWorkMgr &) {}
    static FrameWorkMgr *m_Instance;
};

/*****************************************************************************
 * 函 数 名  : FrameWorkMgrCollector
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 注册收集类
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
class FrameWorkMgrCollector
{
public:
    FrameWorkMgrCollector(int index, FrameWork *F);

private:
    FrameWorkMgrCollector();
};

#define FRAMEWORK_INIT() FrameWorkMgr::getInstance()->RegInit()

#define REG_TO_FRAMEWORK(TableId, SuperId, Class, selfId) \
    static FrameWork g_FrameWork_##Class =                \
        {SuperId, selfId, new Class, #Class, NULL};       \
    static FrameWorkMgrCollector                          \
        g_FrameWorkMgrCollector_##Class(TableId, &g_FrameWork_##Class);
#endif

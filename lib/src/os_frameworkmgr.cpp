/***********************************************************************************
 * 文 件 名   : os_frameworkmgr.c
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月23日
 * 文件描述   : 自注册框架实现类
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#include "os_frameworkmgr.h"
#include "os_log.h"

/*****************************************************************************
 * 函 数 名  : FrameWorkMgr
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 注册管理类成员初始化
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
FrameWork* FrameWorkMgr::m_Node = NULL;
FrameWork* FrameWorkMgr::m_Current = NULL;
int FrameWorkMgr::m_NodeCnt = 0;

/*****************************************************************************
 * 函 数 名  : FrameWorkMgrCollector.FrameWorkMgrCollector
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 自动加入注册链表
 * 输入参数  : FrameWork* F  待注册类
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
FrameWorkMgrCollector::FrameWorkMgrCollector(FrameWork* F){
    if (NULL == FrameWorkMgr::m_Node)
    {
        FrameWorkMgr::m_Node = F;
        FrameWorkMgr::m_Current = F;
    }
    else
    {
        FrameWorkMgr::m_Current->Next = F;
        FrameWorkMgr::m_Current = F;
    }
    FrameWorkMgr::m_NodeCnt += 1;
};

/*****************************************************************************
 * 函 数 名  : FrameWorkMgr.RegInit
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 依次调用注册的类初始化
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : void
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void FrameWorkMgr::RegInit()
{
    FrameWork* frmgr= m_Node;
    for (int i = 0; (i< m_NodeCnt) && (NULL != frmgr); i++)
    {
        if(0 != frmgr->fun->Init())
        {
            LVOS_Log(LL_WARNING, "Init module %s fail.", frmgr->ModuleName);
        }
        frmgr = frmgr->Next;
    }
}

/*****************************************************************************
 * 函 数 名  : FrameWorkMgr
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 框架管理类实例初始化
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
FrameWorkMgr* FrameWorkMgr::m_Instance = NULL;

/*****************************************************************************
 * 函 数 名  : FrameWorkMgr.getInstance
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 框架注册管理类单实例获取
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : FrameWorkMgr*
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
FrameWorkMgr* FrameWorkMgr::getInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new FrameWorkMgr;
    }
    return m_Instance;
}

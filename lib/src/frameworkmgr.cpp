/***********************************************************************************
 * 文 件 名   : os_frameworkmgr.c
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月23日
 * 文件描述   : 自注册框架实现类
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/
#include "objKernel.h"
#include "frameworkmgr.h"

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
FrameWork *FrameWorkMgr::m_Node[TABLE_NUM];
FrameWork *FrameWorkMgr::m_Current[TABLE_NUM];
int FrameWorkMgr::m_NodeCnt[TABLE_NUM] = {0};

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
FrameWorkMgrCollector::FrameWorkMgrCollector(int index, FrameWork *F)
{
    if (NULL == FrameWorkMgr::m_Node[index])
    {
        FrameWorkMgr::m_Node[index] = F;
        FrameWorkMgr::m_Current[index] = F;
    }
    else
    {
        FrameWorkMgr::m_Current[index]->Next = F;
        FrameWorkMgr::m_Current[index] = F;
    }
    FrameWorkMgr::m_NodeCnt[index] += 1;
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
    for (size_t j = 0; j < TABLE_NUM; j++)
    {
        FrameWork *frmgr = m_Node[j];
        for (int i = 0; (i < m_NodeCnt[j]) && (NULL != frmgr); i++)
        {
            objbase *pObj = g_objKernel->InterFace(frmgr->SuperId);
            if (pObj != nullptr)
            {
                pObj->Reg(frmgr->ModuleName, frmgr->fun, frmgr->id);
            }
            frmgr = frmgr->Next;
        }
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
FrameWorkMgr *FrameWorkMgr::m_Instance = NULL;

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
FrameWorkMgr *FrameWorkMgr::getInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new FrameWorkMgr;
    }
    return m_Instance;
}

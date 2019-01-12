/***********************************************************************************
 * �� �� ��   : os_frameworkmgr.c
 * �� �� ��   : ¬����
 * ��������   : 2018��11��23��
 * �ļ�����   : ��ע����ʵ����
 * ��Ȩ˵��   : Copyright (c) 2008-2018   xx xx xx xx �������޹�˾
 * ��    ��   : 
 * �޸���־   : 
***********************************************************************************/

#include "os_frameworkmgr.h"
#include "os_log_macro_define.h"

/*****************************************************************************
 * �� �� ��  : FrameWorkMgr
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ע��������Ա��ʼ��
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
FrameWork* FrameWorkMgr::m_Node = NULL;
FrameWork* FrameWorkMgr::m_Current = NULL;
int FrameWorkMgr::m_NodeCnt = 0;

/*****************************************************************************
 * �� �� ��  : FrameWorkMgrCollector.FrameWorkMgrCollector
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : �Զ�����ע������
 * �������  : FrameWork* F  ��ע����
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

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
 * �� �� ��  : FrameWorkMgr.RegInit
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ���ε���ע������ʼ��
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : void
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
void FrameWorkMgr::RegInit()
{
    FrameWork* frmgr= m_Node;
    for (int i = 0; (i< m_NodeCnt) && (NULL !=frmgr); i++)
    {
        if(0 != 
#if _SW_VER_	
        (frmgr->fun())->Init()
#else
        frmgr->fun->Init()
#endif
        )
        {
            LVOS_Log(LL_WARNING, "Init module %s fail.", frmgr->ModuleName);
        }
        frmgr = frmgr->Next;
    }
}

/*****************************************************************************
 * �� �� ��  : FrameWorkMgr
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ��ܹ�����ʵ����ʼ��
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
FrameWorkMgr* FrameWorkMgr::m_Instance = NULL;

/*****************************************************************************
 * �� �� ��  : FrameWorkMgr.getInstance
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ���ע������൥ʵ����ȡ
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : FrameWorkMgr*
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
FrameWorkMgr* FrameWorkMgr::getInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new FrameWorkMgr;
    }
    return m_Instance;
}
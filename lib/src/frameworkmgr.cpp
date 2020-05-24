/***********************************************************************************
 * �� �� ��   : os_frameworkmgr.c
 * �� �� ��   : ¬����
 * ��������   : 2018��11��23��
 * �ļ�����   : ��ע����ʵ����
 * ��Ȩ˵��   : Copyright (c) 2008-2018   xx xx xx xx �������޹�˾
 * ��    ��   : 
 * �޸���־   : 
***********************************************************************************/
#include "objKernel.h"
#include "frameworkmgr.h"

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
FrameWork *FrameWorkMgr::m_Node[TABLE_NUM];
FrameWork *FrameWorkMgr::m_Current[TABLE_NUM];
int FrameWorkMgr::m_NodeCnt[TABLE_NUM] = {0};

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
FrameWorkMgr *FrameWorkMgr::m_Instance = NULL;

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
FrameWorkMgr *FrameWorkMgr::getInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new FrameWorkMgr;
    }
    return m_Instance;
}

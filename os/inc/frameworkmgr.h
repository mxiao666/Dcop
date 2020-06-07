/******************************************************************************
 * �� �� ��   : os_frameworkmgr.h
 * �� �� ��   : ¬����
 * ��������   : 2018��11��23��
 * �ļ�����   : ����Զ�ע�����
 * ��Ȩ˵��   : Copyright (c) 2008-2018   xx xx xx xx �������޹�˾
 * ��    ��   : 
 * �޸���־   : 
******************************************************************************/

#ifndef __OS_FRAMEWORKMGR__
#define __OS_FRAMEWORKMGR__
#include "objbase.h"
#include "cmdid.h"
/*****************************************************************************
 * �� �� ��  : FrameWork
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ע������ṹ��
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : typedef
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
typedef struct FrameWork
{
    int SuperId; // Ŀ��ע����Id
    int id;      // ����id
    void *fun;
    const char *ModuleName;
    FrameWork *Next;
} FrameWork;

/*****************************************************************************
 * �� �� ��  : FrameWorkMgr
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ����ע�������
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

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
 * �� �� ��  : FrameWorkMgrCollector
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ע���ռ���
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

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

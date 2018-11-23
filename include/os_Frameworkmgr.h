/***********************************************************************************
 * �� �� ��   : os_frameworkmgr.h
 * �� �� ��   : ¬����
 * ��������   : 2018��11��23��
 * �ļ�����   : ����Զ�ע�����
 * ��Ȩ˵��   : Copyright (c) 2008-2018   xx xx xx xx �������޹�˾
 * ��    ��   : 
 * �޸���־   : 
***********************************************************************************/

#ifndef   __OS_FRAMEWORKMGR__
#define   __OS_FRAMEWORKMGR__

/*****************************************************************************
 * �� �� ��  : IplugFrameWork
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : ���ඨ��
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
class IplugFrameWork{
public:
	IplugFrameWork(){ };
	virtual int Init() = 0;
};
#if _SW_VER_
    typedef IplugFrameWork* (*RegFun)();
#else
    typedef IplugFrameWork* RegFun;
#endif

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
typedef struct FrameWork{
	RegFun fun;
	const char* ModuleName;
	FrameWork* Next;
}FrameWork;

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
class FrameWorkMgr{
public:
	static  FrameWork* m_Node;
	static  FrameWork* m_Current;
	static int  m_NodeCnt;
	void RegInit();
	static FrameWorkMgr* getInstance();
private:
	FrameWorkMgr(){}
	FrameWorkMgr(FrameWorkMgr&){}
	FrameWorkMgr& opertor(FrameWorkMgr&){}
	static FrameWorkMgr* m_Instance;
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
class FrameWorkMgrCollector{
public:
	FrameWorkMgrCollector(FrameWork* F);
private:
	FrameWorkMgrCollector();

};

/*****************************************************************************
 * �� �� ��  : if REG_FUNCTION
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : �Զ�ע���
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
#if _SW_VER_
#define REG_FUNCTION(Class, Name) \
	static IplugFrameWork* g_IplugFrameWork_##Class(){ return new Class; } \
	static FrameWork g_FrameWork_##Class = { g_IplugFrameWork_##Class, Name, NULL }; \
	static FrameWorkMgrCollector g_FrameWorkMgrCollector_##Class(&g_FrameWork_##Class);
#else
#define REG_FUNCTION(Class, Name) \
	static IplugFrameWork* g_IplugFrameWork_##Class = new Class; \
	static FrameWork g_FrameWork_##Class = { g_IplugFrameWork_##Class, Name, NULL }; \
	static FrameWorkMgrCollector g_FrameWorkMgrCollector_##Class(&g_FrameWork_##Class);
#endif

#endif

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
typedef IplugFrameWork* RegFun;

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
	FrameWorkMgr& opertor(FrameWorkMgr&) = delete;
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
 * �� �� ��  : REG_FUNCTION_PLUS
 * �� �� ��  : ¬����
 * ��������  : 2018��11��23��
 * ��������  : C++�Զ�ע���
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
#define REG_FUNCTION_PLUS(Class, Name) \
	static FrameWork g_FrameWork_##Class = { new Class, Name, NULL }; \
	static FrameWorkMgrCollector g_FrameWorkMgrCollector_##Class(&g_FrameWork_##Class);

/*****************************************************************************
 * �� �� ��  : REG_FUNCTION
 * �� �� ��  : ¬����
 * ��������  : 2018��12��1��
 * ��������  : C�ķ�ʽ���ó�ʼ��
 * �������  : Class  ��������
               Func   �����õ���ͨ����
               Name   ������������
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
#define REG_FUNCTION(Func, Name) \
class IplugFrameWork##Func :public IplugFrameWork \
{  \
public:  \
	IplugFrameWork##Func(){ } \
	int Init(){ \
		return  Func(); \
	}  \
};  \
REG_FUNCTION_PLUS(IplugFrameWork##Func, Name);


#endif

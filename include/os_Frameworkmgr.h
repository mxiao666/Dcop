/***********************************************************************************
 * 文 件 名   : os_frameworkmgr.h
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月23日
 * 文件描述   : 框架自动注册调用
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef   __OS_FRAMEWORKMGR__
#define   __OS_FRAMEWORKMGR__

/*****************************************************************************
 * 函 数 名  : IplugFrameWork
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 基类定义
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

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
typedef struct FrameWork{
	RegFun fun;
	const char* ModuleName;
	FrameWork* Next;
}FrameWork;

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
class FrameWorkMgrCollector{
public:
	FrameWorkMgrCollector(FrameWork* F);
private:
	FrameWorkMgrCollector();

};

/*****************************************************************************
 * 函 数 名  : if REG_FUNCTION
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : 自动注册宏
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

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

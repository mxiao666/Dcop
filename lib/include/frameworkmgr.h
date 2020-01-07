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
typedef IplugFrameWork* RegFun;

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
	FrameWorkMgr& opertor(FrameWorkMgr&) = delete;
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
 * 函 数 名  : REG_FUNCTION_PLUS
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月23日
 * 函数功能  : C++自动注册宏
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
#define REG_FUNCTION_PLUS(Class, Name) \
	static FrameWork g_FrameWork_##Class = { new Class, Name, NULL }; \
	static FrameWorkMgrCollector g_FrameWorkMgrCollector_##Class(&g_FrameWork_##Class);

/*****************************************************************************
 * 函 数 名  : REG_FUNCTION
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年12月1日
 * 函数功能  : C的方式调用初始化
 * 输入参数  : Class  生成类名
               Func   被调用的普通函数
               Name   被调用者名字
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

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

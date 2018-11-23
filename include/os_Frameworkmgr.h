#ifndef   __OS_FRAMEWORKMGR__
#define   __OS_FRAMEWORKMGR__
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
typedef struct FrameWork{
	RegFun fun;
	const char* ModuleName;
	FrameWork* Next;
}FrameWork;

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
class FrameWorkMgrCollector{
public:
	FrameWorkMgrCollector(FrameWork* F);
private:
	FrameWorkMgrCollector();

};

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

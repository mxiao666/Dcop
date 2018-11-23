#include "os_frameworkmgr.h"
#include "os_log_macro_define.h"
FrameWork* FrameWorkMgr::m_Node = NULL;
FrameWork* FrameWorkMgr::m_Current = NULL;
int FrameWorkMgr::m_NodeCnt = 0;

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
    		OS_PLUG_LOG(LL_WARNING, "Init module %s fail.", frmgr->ModuleName);
		}
		frmgr = frmgr->Next;
	}
}
FrameWorkMgr* FrameWorkMgr::m_Instance = NULL;
FrameWorkMgr* FrameWorkMgr::getInstance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new FrameWorkMgr;
	}
	return m_Instance;
}

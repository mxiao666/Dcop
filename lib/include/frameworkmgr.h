/***********************************************************************************
 * 文 件 名   : os_frameworkmgr.h
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月23日
 * 文件描述   : 框架自动注册调用
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef __OS_FRAMEWORKMGR__
#define __OS_FRAMEWORKMGR__

#define REG_FRAMEWORK(NAME)                                          \
    typedef struct g_##NAME##_FrameWork                              \
    {                                                                \
        NAME *fun;                                                   \
        const char *ModuleName;                                      \
        g_##NAME##_FrameWork *Next;                                  \
    } g_##NAME##_FrameWork;                                          \
                                                                     \
    class g_##NAME##_FrameWorkMgr                                    \
    {                                                                \
    public:                                                          \
        static g_##NAME##_FrameWork *m_Node;                         \
        static g_##NAME##_FrameWork *m_Current;                      \
        static int m_NodeCnt;                                        \
                                                                     \
    private:                                                         \
        g_##NAME##_FrameWorkMgr(){};                                 \
    };                                                               \
    class g_##NAME##_FrameWorkMgrCollector                           \
    {                                                                \
    public:                                                          \
        g_##NAME##_FrameWorkMgrCollector(g_##NAME##_FrameWork *func) \
        {                                                            \
            if (NULL == g_##NAME##_FrameWorkMgr::m_Node)             \
            {                                                        \
                g_##NAME##_FrameWorkMgr::m_Node = func;              \
                g_##NAME##_FrameWorkMgr::m_Current = func;           \
            }                                                        \
            else                                                     \
            {                                                        \
                g_##NAME##_FrameWorkMgr::m_Current->Next = func;     \
                g_##NAME##_FrameWorkMgr::m_Current = func;           \
            }                                                        \
            g_##NAME##_FrameWorkMgr::m_NodeCnt += 1;                 \
        }                                                            \
                                                                     \
    private:                                                         \
        g_##NAME##_FrameWorkMgrCollector() = delete;                 \
    };
#define INIT_FRAMEWORK(NAME)                                         \
    g_##NAME##_FrameWork *g_##NAME##_FrameWorkMgr::m_Node = NULL;    \
    g_##NAME##_FrameWork *g_##NAME##_FrameWorkMgr::m_Current = NULL; \
    int g_##NAME##_FrameWorkMgr::m_NodeCnt = 0;
#define FRAMEWORK_BEGINE(NAME)                                                        \
    g_##NAME##_FrameWork *frmgr = g_##NAME##_FrameWorkMgr::m_Node;                    \
    for (int i = 0; (i < g_##NAME##_FrameWorkMgr::m_NodeCnt) && (NULL != frmgr); i++) \
    {

#define FRAMEWORK_END(NAME) \
    frmgr = frmgr->Next;    \
    }

#define FRAMEWORK_REG_FUNCTION(BaseClass, Class)                         \
    static g_##BaseClass##_FrameWork                                     \
        g_g_##BaseClass##_FrameWork_##Class = {new Class, #Class, NULL}; \
    static g_##BaseClass##_FrameWorkMgrCollector                         \
        g_##BaseClass##_FrameWorkMgrCollector_##Class(&g_g_##BaseClass##_FrameWork_##Class);

#endif

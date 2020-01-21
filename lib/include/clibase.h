#ifndef __OS_CLIBASE__
#define __OS_CLIBASE__
#include "cargclist.h"
#include "objbase.h"
#include "frameworkmgr.h"
#include "cnotify.h"
#include <condition_variable>
#include <queue>
typedef struct _TblBody
{
    const char *item;
    int fmrlen;
} TblBody;
typedef struct _ResTable
{
    const char *tblName;
    TblBody * tblBody;
    int count;
} ResTable;
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
class CClibase
{
public:
    CClibase() {}
    virtual ~CClibase() {}
    virtual void Help(Printfun func)
    {
        (void)func("plase man cmd.\n");
    }
    virtual int Set(CAgrcList *inPut, CAgrcList *outPut, bool *bOp) { return 0; };
    virtual int Get(CAgrcList *inPut, CAgrcList *outPut, bool *bOp) { return 0; };
    virtual int Add(CAgrcList *inPut, CAgrcList *outPut, bool *bOp) { return 0; };
    virtual int Init() { return 0; };
    virtual int ResponseTable(ResTable **tbl) { return 0; };
};

REG_FRAMEWORK(CClibase)
#define CMD_REG_FUNCTION(Class) \
    FRAMEWORK_REG_FUNCTION(CClibase, Class)

typedef struct _cmdObj
{
    int cmdModule;
    int cmdid;
    bool bSync;
    CClibase *objCli;
    _cmdObj(CClibase *obj, int cmdMod, int cmd, bool sync)
    {
        cmdModule = cmdMod;
        cmdid = cmd;
        objCli = obj;
        bSync = sync;
    }
    ~_cmdObj()
    {
        if (objCli != nullptr)
            delete objCli;
        objCli = nullptr;
    }
} cmdObj;
typedef struct _argcPool
{
    CAgrcList *msg;
    cmdObj *cli;
    _argcPool(CAgrcList *objmsg, cmdObj *objcli)
    {
        msg = objmsg;
        cli = objcli;
    }
    ~_argcPool()
    {
        if (msg)
            delete msg;
    }

} argcPool;
class cliMgr : public objbase
{
private:
    std::map<const char *, cmdObj *> m_cmdList;
    std::mutex m_reglock;
    Cnotify *m_Cnotify = nullptr;
    cmdObj *FindModule(const char *cmdName);
    const int CMD_OP_MAX = 3;
    const char *GET_STR = "get";
    const char *SET_STR = "set";
    const char *ADD_STR = "add";

    // 任务队列
    std::queue<argcPool *> tasks;
    // 同步
    std::mutex m_lock;
    // 条件阻塞
    std::condition_variable cv_task;
    // 是否关闭提交
    std::atomic<bool> stoped;
    //空闲线程数量
    std::atomic<int> idlThrNum;
    int Dispatch(CAgrcList *message, cmdObj *);

public:
    cliMgr();
    ~cliMgr();
    int Process();
    void dump(Printfun callback = printf);
    int RegCmd(const char *pzName, cmdObj *pobj);
    int Init();
    void AsyncProc();
};
#endif

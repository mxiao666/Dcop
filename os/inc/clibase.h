#ifndef __OS_CLIBASE__
#define __OS_CLIBASE__
#include "cargclist.h"
#include "objbase.h"
#include "frameworkmgr.h"
#include "cnotify.h"
typedef struct _TblBody
{
    const char *item;
    int fmrlen;
} TblBody;
typedef struct _ResTable
{
    const char *tblName;
    TblBody *tblBody;
    int count;
} RspTable;
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
class CClibase
{
private:
    int m_refCount = 0;

public:
    CClibase() {}
    virtual ~CClibase()
    {
        m_refCount = 0;
    }
    virtual void Help(int fd = 0, Printfun callback = LVOS_Printf)
    {
        (void)callback(fd, "plase man cmd.\r\n");
    }
    virtual int Set(CAgrcList *inPut, CAgrcList *outPut, bool *bOp)
    {
        return 0;
    };
    virtual int Get(CAgrcList *inPut, CAgrcList *outPut, bool *bOp)
    {
        return 0;
    };
    virtual int Add(CAgrcList *inPut, CAgrcList *outPut, bool *bOp)
    {
        return 0;
    };
    virtual int Init() { return 0; };
    int GetRefCount()
    {
        return m_refCount;
    }
    void AddRefCount()
    {
        ++m_refCount;
    }
    void DecRefCount()
    {
        --m_refCount;
    }
};

typedef struct _cmdObj
{
    int cmdModule;
    int cmdid;
    bool bSync;
    CClibase *objCli;
    RspTable *rspTable;
    _cmdObj(CClibase *obj, int cmdMod, int cmd,
            RspTable *tbl = nullptr, bool sync = true)
    {
        cmdModule = cmdMod;
        cmdid = cmd;
        objCli = obj;
        bSync = sync;
        rspTable = tbl;
    }
    ~_cmdObj()
    {
        if (objCli != nullptr)
        {
            objCli->DecRefCount();
            if (objCli->GetRefCount() == 0)
            {
                delete objCli;
            }
        }
        objCli = nullptr;
    }
} cmdObj;

typedef struct
{
    const char *name;
    int module;
    int cmd;
    RspTable *rsb;
    bool bSync;
} CMD_REG;

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
    int Dispatch(CAgrcList *message, cmdObj *, int fd = 0);
    int Report(RspMsg *outMessage, cmdObj *pcmdObj, int fd = 0);
    int Report(CAgrcList *message,
               RspMsg *outmessage,
               int iModule,
               int iCmd);
    int CliSys(int fd, cmdObj *pcmdObj,
               const char *cmd, const char *argc, int *isExit);

public:
    cliMgr();
    ~cliMgr();
    int Init();
    int Process();
    int Process(int fd, char *cmdbuffer, int len, int *isExit);
    void dump(int fd = 0, Printfun callback = LVOS_Printf);
    void Welcome(int fd = 0, Printfun callback = LVOS_Printf);
    int RegCmd(const char *pzName, cmdObj *pobj);
    static int Report(RspMsg *outMessage, int module, int cmd);
    virtual void Reg(const char *pzName, void *obj, int id);
    virtual int Process(CAgrcList *message,
                        RspMsg *outmessage, int iModule, int iCmd);
};
#define ARGC_DEFAULT "ARGC"
#endif

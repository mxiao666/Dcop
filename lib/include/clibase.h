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
    RspTable *rspTable;
    _cmdObj(CClibase *obj, int cmdMod, int cmd, RspTable *tbl = nullptr, bool sync = true)
    {
        cmdModule = cmdMod;
        cmdid = cmd;
        objCli = obj;
        bSync = sync;
        rspTable = tbl;
    }
    ~_cmdObj()
    {
        //if (objCli != nullptr)
        //    delete objCli;
        objCli = nullptr;
    }
} cmdObj;

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
    int Dispatch(CAgrcList *message, cmdObj *);

public:
    cliMgr();
    ~cliMgr();
    int Process();
    void dump(Printfun callback = printf);
    int RegCmd(const char *pzName, cmdObj *pobj);
    int Init();
    int Report(RspMsg *outMessage, int cmd);
};
#define ARGC_DEFAULT "ARGC"
#endif

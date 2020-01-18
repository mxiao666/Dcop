#ifndef __OS_CLIBASE__
#define __OS_CLIBASE__
#include "cargclist.h"
#include "objbase.h"
#include "frameworkmgr.h"
#include <condition_variable>
#include <future>
#include <functional>
typedef struct _ResTable
{
    const char *item;
    int fmrlen;
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

REG_FRAMEWORK(CClibase, CLI)
#define CMD_REG_FUNCTION(Class, Name) \
    FRAMEWORK_REG_FUNCTION(CLI, Class, Name)

typedef struct _cmdObj
{
    int cmdModule;
    int cmdid;
    CClibase *objCli;
    _cmdObj(CClibase *obj, int cmdMod, int cmd)
    {
        cmdModule = cmdMod;
        cmdid = cmd;
        objCli = obj;
    }
    ~_cmdObj()
    {
        if (objCli != nullptr)
            delete objCli;
        objCli = nullptr;
    }
} cmdObj;
class cliMgr : public objbase
{
private:
    std::map<const char *, cmdObj *> m_cmdList;
    std::mutex m_reglock;
    std::mutex m_excelock;
    cmdObj *FindModule(const char *cmdName);
    const int CMD_OP_MAX = 3;
    const char *GET_STR = "get";
    const char *SET_STR = "set";
    const char *ADD_STR = "add";

public:
    cliMgr() {}
    ~cliMgr();
    int Dispatch(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd);
    int Process();
    void dump(Printfun callback = printf);
    int RegCmd(const char *pzName, cmdObj *pobj);
    int Init();
};
#endif

#ifndef __OS_CLIBASE__
#define __OS_CLIBASE__
#include "cargclist.h"
#include "objbase.h"
#include "frameworkmgr.h"
#include <condition_variable>
#include <future>
#include <functional>
class CClibase
{
public:
    CClibase() {}
    virtual ~CClibase() {}
    virtual void Help(Printfun func)
    {
        (void)func("plase man cmd.");
    }
    virtual int Set(CAgrcList *inPut, CAgrcList *outPut, bool *bOp) { return 0; };
    virtual int Get(CAgrcList *inPut, CAgrcList *outPut, bool *bOp) { return 0; };
    virtual int Add(CAgrcList *inPut, CAgrcList *outPut, bool *bOp) { return 0; };
    virtual int Init() { return 0; };
};

REG_FRAMEWORK(CClibase, CLI)
#define CMD_REG_FUNCTION(Class, Name) \
    FRAMEWORK_REG_FUNCTION(CLI, Class, Name)

typedef struct _cmdObj
{
    int cmdModule;
    CClibase *objCli;
    _cmdObj(int cmd, CClibase *obj)
    {
        cmdModule = cmd;
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

public:
    cliMgr() {}
    ~cliMgr();
    int Dispatch();
    int Proc();
    void dump(Printfun callback = printf);
    int RegCmd(const char *pzName, cmdObj *pobj);
    int Init();
};
#endif

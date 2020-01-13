#ifndef __OS_CLIBASE__
#define __OS_CLIBASE__
#include "cargclist.h"
#include "objbase.h"

class CClibase
{
public:
    CClibase() {}
    virtual ~CClibase() {}
    void Help(Printfun func)
    {
        (void)func("plase man cmd.");
    }
    virtual int Set(CAgrcList &inPut, CAgrcList &outPut, bool &bOp) = 0;
    virtual int Get(CAgrcList &inPut, CAgrcList &outPut, bool &bOp) = 0;
    virtual int Add(CAgrcList &inPut, CAgrcList &outPut, bool &bOp) = 0;
};
class cliMgr : public objbase
{
private:
    std::map<int, CClibase *> m_cmdList;

public:
    cliMgr() {}
    ~cliMgr() {}
    int Dispatch();
    int Proc();
};
#endif

#ifndef __OS_CLIBASE__
#define __OS_CLIBASE__
#include "os_argclist.h"

using Printfun = void (*)(const char *format, ...);
class CClibase
{
public:
    CClibase() {}
    ~CClibase() {}
    void Help(Printfun func)
    {
        func("plase man cmd.");
    }
    virtual int Set(CAgrcList &inPut, CAgrcList &outPut, bool &bOp = false) = 0;
    virtual int Get(CAgrcList &inPut, CAgrcList &outPut, bool &bOp = false) = 0;
    virtual int Add(CAgrcList &inPut, CAgrcList &outPut, bool &bOp = false) = 0;
};
#endif

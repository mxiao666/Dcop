#include "clibase.h"
#include "frameworkmgr.h"
#include "objKernel.h"
int cliMgr::Dispatch()
{
    return 0;
}
namespace OS
{
bool equal(const char *s1, const char *s2)
{
    return
#ifndef __WIN32__
        strcasecmp
#else
        _stricmp
#endif
        (s1, s2) == 0;
}
}; // namespace OS
int cliMgr::Proc()
{

    const int ArSize = 128;
    char name[ArSize] = {0};
    bool isExit = false;
    while (!isExit)
    {
        std::cout << "$:";
        std::cin.getline(name, ArSize);
        const char *pos = strchr(name, ':');
        CStream cmd;
        CStream argc;
        if (pos != nullptr)
            cmd.Append((BYTE *)name, pos - name);
        else
            cmd.Append((BYTE *)name, strlen(name));
        if (pos != nullptr && ++pos != nullptr)
            argc.Append((BYTE *)pos, strlen(pos));
        if (OS::equal((char *)cmd.GetBuff(), "quit"))
        {
            isExit = true;
            continue;
        }
        if (OS::equal((char *)cmd.GetBuff(), "dump"))
        {
            objbase *obj = g_objKernel->InterFace((char *)argc.GetBuff());
            if (obj)
                obj->dump();
            else
                g_objKernel->dump();
            continue;
        }
        if (OS::equal((char *)cmd.GetBuff(), "man"))
        {
            continue;
        }
    }
    return 0;
}
REG_FUNCTION_PLUS(cliMgr, "cliMgr")
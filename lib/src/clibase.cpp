#include "clibase.h"
#include "log.h"
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
void cliMgr::dump(Printfun callback)
{
    objbase::PrintHead(callback, "cliMgr", 66);
    (void)callback("%-16s %-16s %-32s\n", "id", "objPtr", "cmd");
    for (auto &iter : m_cmdList)
        (void)callback("%#-16x %#-16x %-32s\n", iter.second->cmdModule, iter.second->objCli, iter.first);
    (void)callback("Tatol: %d\n", m_cmdList.size());
}
cliMgr::~cliMgr()
{
    for (auto &iter : m_cmdList)
        if (iter.second != nullptr)
        {
            delete iter.second;
            iter.second = nullptr;
        }
}
int cliMgr::RegCmd(const char *pzName, cmdObj *pobj)
{
    std::unique_lock<std::mutex> lock{m_reglock};
    for (auto &iter : m_cmdList)
        if (OS::equal(pzName, iter.first))
            return -1;
    m_cmdList[pzName] = pobj;
    return 0;
}
int cliMgr::Init()
{
    FRAMEWORK_BEGINE(CLI)
    if (0 != frmgr->fun->Init())
        LVOS_Log(LL_WARNING, "Init CLI module %s fail.", frmgr->ModuleName);
    FRAMEWORK_END(CLI)
    return 0;
}
INIT_FRAMEWORK(CLI)

REG_FUNCTION_PLUS(cliMgr, "cliMgr");

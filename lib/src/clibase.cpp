#include "clibase.h"
#include "log.h"
#include "objKernel.h"
#include "cnotify.h"
int cliMgr::Dispatch(CAgrcList *message, CAgrcList *outmessage, int iModule, int iCmd)
{
    std::unique_lock<std::mutex> lock{m_excelock};
    Cnotify *obj = reinterpret_cast<Cnotify *>(g_objKernel->InterFace("Cnotify"));
    if (obj)
        return obj->notify(message, outmessage, iModule, iCmd);
    return -1;
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
bool equal(const char *s1, const char *s2, int max)
{
    return (strncmp(s1, s2, max) == 0);
}
}; // namespace OS
int cliMgr::Process()
{

    const int ArSize = 128;
    char name[ArSize] = {0};
    bool isExit = false;
    while (!isExit)
    {
        std::cout << "$:";
        std::cin.getline(name, ArSize);
        if (strlen(name) == 0)
            continue;
        const char *pos = strchr(name, ':');
        CStream cmd;
        CStream argc;
        CAgrcList inMessage;
        CAgrcList outMessage;
        CAgrcList cliMessage;
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
            objbase *pobjBase = g_objKernel->InterFace((char *)argc.GetBuff());
            if (pobjBase)
                pobjBase->dump();
            else
                g_objKernel->dump();
            continue;
        }

        cmdObj *pcmdObj = nullptr;
        if (OS::equal((char *)cmd.GetBuff(), "man"))
        {
            pcmdObj = FindModule((char *)argc.GetBuff());
            if (pcmdObj)
                pcmdObj->objCli->Help(printf);
            else
                dump();
            continue;
        }
        pcmdObj = FindModule((char *)cmd.GetBuff());
        int iRet = -1;
        bool cliOp = false;
        if (pcmdObj != nullptr)
        {
            if (OS::equal((char *)cmd.GetBuff(), GET_STR, CMD_OP_MAX))
            {
                iRet = pcmdObj->objCli->Get(&inMessage, &cliMessage, &cliOp);
            }
            else if (OS::equal((char *)cmd.GetBuff(), SET_STR, CMD_OP_MAX))
            {
                iRet = pcmdObj->objCli->Get(&inMessage, &cliMessage, &cliOp);
            }
            else if (OS::equal((char *)cmd.GetBuff(), ADD_STR, CMD_OP_MAX))
            {
                iRet = pcmdObj->objCli->Get(&inMessage, &cliMessage, &cliOp);
            }
            else
            {
            }
            if (iRet != 0)
            {
                printf("Cmd param is process error=%d\n", iRet);
                continue;
            }
        }
        else
        {
            printf("Cmd is unregister.\n");
            continue;
        }

        iRet = Dispatch((cliOp == false) ? &inMessage : &cliMessage, &outMessage,
                        pcmdObj->cmdModule, pcmdObj->cmdid);
        if (iRet != 0)
        {
            printf("Dispatch is process error:%#16x\n", iRet);
        }
        else
        {
            ResTable *ptbl = nullptr;
            int num = pcmdObj->objCli->ResponseTable(&ptbl);
            if (ptbl == nullptr)
                continue;
            CAgrcList outinfo;
            for (int i = 0; i < num; i++)
            {
                printf("%-*s", ptbl[i].fmrlen, ptbl[i].item);
                CStream framtlen;
                framtlen << (WORD)ptbl[i].fmrlen;
                outinfo.addAgrc(ptbl[i].item, (const char *)framtlen.GetBuff());
            }
            printf("\n");
            for (int i = 0; i < outMessage.GetCount(); i++)
            {
                for (int i = 0; i < num; i++)
                {
                    printf("%-*s", ptbl[i].fmrlen, outMessage.GetAgrc(ptbl[i].item)->GetBuff());
                    printf("\n");
                }
            }
            printf("Command response %d line record\n", outMessage.GetCount());
        }
    }
    return 0;
}
cmdObj *cliMgr::FindModule(const char *cmdName)
{
    for (auto &iter : m_cmdList)
        if (OS::equal(cmdName, iter.first))
            return iter.second;
    return nullptr;
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

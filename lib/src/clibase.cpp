#include "clibase.h"
#include "log.h"
#include "objKernel.h"

int cliMgr::Dispatch(CAgrcList *inMessage, cmdObj *pcmdObj)
{

    RspMsg outMessage;
    int iRet = 0;
    if (pcmdObj->bSync)
    {
        if (pcmdObj->cmdModule != 0)
        {
            iRet = m_Cnotify->Notify(inMessage, &outMessage, pcmdObj->cmdModule, pcmdObj->cmdid);
            if (iRet == 0)
                iRet = Report(&outMessage, pcmdObj->cmdid);
        }
        else
        {
            m_Cnotify->SendToAll(inMessage, &outMessage, pcmdObj->cmdModule, pcmdObj->cmdid);
        }

    }
    else
    {
        iRet = m_Cnotify->AsyncNotify(inMessage, pcmdObj->cmdModule, pcmdObj->cmdid);
    }
    return iRet;
}
int cliMgr::Report(RspMsg *outMessage, int cmd)
{

    if ((outMessage == nullptr) ||
        (outMessage->count == 0) ||
        (outMessage->msg == nullptr))
        return -1;
    // 查找命令模块
    bool isfind = false;
    cmdObj *pcmdObj = nullptr;
    for(auto & iter : m_cmdList)
    {
        if(iter.second != nullptr)
        {
            if(iter.second->cmdid == cmd)
            {
                isfind = true;
                pcmdObj = iter.second;
            }
        }
    }
    if(!isfind)
        return -1;
    ResTable *ptbl = nullptr;
    pcmdObj->objCli->ResponseTable(&ptbl);
    if (ptbl == nullptr ||
        ptbl->count == 0 ||
        ptbl->tblBody == nullptr)
        return -1;
    CStream outinfo;
    int totallen = 0;
    for (int i = 0; i < ptbl->count; i++)
    {
        const int maxlen = 16;
        char buf[maxlen] = {0};
        int len = snprintf(buf, maxlen, "%-*s", ptbl->tblBody[i].fmrlen, ptbl->tblBody[i].item);
        outinfo.Append((const BYTE *)buf, len);
        totallen += ptbl->tblBody[i].fmrlen;
    }
    printf("\n%*s\n%s\n", totallen / 2, ptbl->tblName, outinfo.GetBuff());
    for (int i = 0; i < outMessage->count; i++)
    {
        for (int j = 0; j < ptbl->count; j++)
        {
            printf("%-*s",
                   ptbl->tblBody[j].fmrlen,
                   outMessage->msg[i].GetAgrc(ptbl->tblBody[j].item)->GetBuff());
        }
        printf("\n");
    }
    printf("Command response %d line record\n", outMessage->count);
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
        char *pos = strchr(name, ':');
        CStream cmd;
        if (pos != nullptr)
            cmd.Append((BYTE *)name, pos - name);
        else
            cmd.Append((BYTE *)name, strlen(name));
        CStream argc;
        CAgrcList inMessage;
        if (pos != nullptr && ++pos != nullptr)
        {
            argc.Append((BYTE *)pos, strlen(pos));
            // 提取参数列表
            char delims[] = ",";
            char *result = NULL;
            char *argcpos = NULL;
            result = strtok(pos, delims);
            while(result != NULL)
            {
                argcpos = strstr(result, "=");
                if (argcpos != nullptr)
                {
                    *argcpos++ = '\0';
                    if (strlen(argcpos) > 0)
                        inMessage.addAgrc(result, argcpos);
                }
                result = strtok(NULL, delims);
            }
        }
        if (OS::equal((char *)cmd.GetBuff(), "q") ||
            OS::equal((char *)cmd.GetBuff(), "quit") ||
            OS::equal((char *)cmd.GetBuff(), "e") ||
            OS::equal((char *)cmd.GetBuff(), "exit"))
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
        CAgrcList cliMessage;
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

        iRet = Dispatch((cliOp == false) ? &inMessage : &cliMessage, pcmdObj);
        if (iRet != 0)
        {
            printf("Dispatch is process error:%#16x\n", iRet);
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
cliMgr::cliMgr()
{
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
    m_Cnotify = reinterpret_cast<Cnotify *>(g_objKernel->Query("Cnotify"));
    FRAMEWORK_BEGINE(CClibase)
    if (0 != frmgr->fun->Init())
        LVOS_Log(LL_WARNING, "Init CLI module %s fail.", frmgr->ModuleName);
    FRAMEWORK_END(CClibase)
    // 注册cli循环处理
    g_objKernel->Init([]()->void
    {
        cliMgr *obj = reinterpret_cast<cliMgr*>(g_objKernel->InterFace("cliMgr"));
        if (obj != nullptr)
            obj->Process();
    });
    return 0;
}
INIT_FRAMEWORK(CClibase)

REG_FUNCTION_PLUS(cliMgr);

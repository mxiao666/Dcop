#include "clibase.h"
#include "log.h"
#include "objKernel.h"
#include "objTask.h"
int cliMgr::Dispatch(CAgrcList *inMessage, cmdObj *pcmdObj)
{

    RspMsg outMessage;
    if (pcmdObj->cmdModule == 0)
    {
        m_Cnotify->SendToAll(inMessage, &outMessage, pcmdObj->cmdModule, pcmdObj->cmdid);
        return 0;
    }
    int iRet = m_Cnotify->Notify(inMessage, &outMessage, pcmdObj->cmdModule, pcmdObj->cmdid);
    if (iRet != 0)
    {
        printf("Dispatch is process error:%#16x\n", iRet);
        return iRet;
    }
    if (outMessage.count == 0 || outMessage.msg == nullptr)
        return iRet;

    ResTable *ptbl = nullptr;
    pcmdObj->objCli->ResponseTable(&ptbl);
    if (ptbl == nullptr || ptbl->count == 0 || ptbl->tblBody == nullptr)
        return iRet;
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
    for (int i = 0; i < outMessage.count; i++)
    {
        for (int j = 0; j < ptbl->count; j++)
        {
            printf("%-*s",
                   ptbl->tblBody[j].fmrlen,
                   outMessage.msg[i].GetAgrc(ptbl->tblBody[j].item)->GetBuff());
        }
        printf("\n");
    }
    printf("Command response %d line record\n", outMessage.count);
    return iRet;
}
void cliMgr::AsyncProc()
{
    while (!this->stoped)
    {
        argcPool *message = nullptr;
        {
            std::unique_lock<std::mutex> lock{this->m_lock};

            this->cv_task.wait(lock,
                               [this] {
                                   return this->stoped.load() || !this->tasks.empty();
                               });
            if (this->stoped && this->tasks.empty())
                return;
            message = this->tasks.front(); // 取一个 task
            this->tasks.pop();
        }
        idlThrNum--;
        (void)Dispatch(message->msg, message->cli);
        idlThrNum++;
    }
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
        CAgrcList *inMessage = new CAgrcList;
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
                iRet = pcmdObj->objCli->Get(inMessage, &cliMessage, &cliOp);
            }
            else if (OS::equal((char *)cmd.GetBuff(), SET_STR, CMD_OP_MAX))
            {
                iRet = pcmdObj->objCli->Get(inMessage, &cliMessage, &cliOp);
            }
            else if (OS::equal((char *)cmd.GetBuff(), ADD_STR, CMD_OP_MAX))
            {
                iRet = pcmdObj->objCli->Get(inMessage, &cliMessage, &cliOp);
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
        if (pcmdObj)
        {
            argcPool *pool = new argcPool(inMessage, pcmdObj);
            std::lock_guard<std::mutex> lock{m_lock};
            tasks.emplace(pool);
            cv_task.notify_one(); // 唤醒一个线程执行
        }
        else
        {
            iRet = Dispatch((cliOp == false) ? inMessage : &cliMessage, pcmdObj);
            delete inMessage;
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
    stoped.store(true);
    cv_task.notify_all(); // 唤醒所有线程执行
}
cliMgr::cliMgr()
{
    stoped = false;
    idlThrNum = 10;
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
    for (int i = 0; i < idlThrNum; i++)
    {
        const int ibufmaxlen = 12;
        char buf[ibufmaxlen] = {0};
        snprintf(buf, ibufmaxlen - 1, "clitask%d", i);
        objPara *pObjPara = new objPara(this);
        CREATE_OBJTASK(buf, [](objPara *pobjPara) {
            cliMgr *obj = reinterpret_cast<cliMgr *>(pobjPara->GetPara());
            if (obj)
                obj->AsyncProc();
            return (void *)0;
        },
                       pObjPara);
    }

    return 0;
}
INIT_FRAMEWORK(CClibase)

REG_FUNCTION_PLUS(cliMgr);

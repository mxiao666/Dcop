#ifndef __WIN32__
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <netinet/in.h>
#endif
#include <fcntl.h>
#include "clibase.h"
#include "log.h"
#include "objKernel.h"
#include "template.h"
#define MAX_BUFFER_LEN 256
enum EXIT_TYPE
{
    EXIT_INVALID,
    USER_EXIT,
    SYS_EXIT
};
#ifndef __WIN32__
class telnet : public objbase
{
private:
    int m_port;
    int m_listenFd;
    EXIT_TYPE m_isExit;
    int m_epfd;
    cliMgr *m_cliMgr;
    std::map<int, int> m_servList;
    std::mutex m_synclock;
#define MAX_EVENTS 100
public:
    telnet(int port = 2323)
    {
        m_port = port;
        m_listenFd = 0;
        m_isExit = EXIT_INVALID;
        m_epfd = 0;
        m_servList.clear();
    }
    ~telnet()
    {
        close(m_listenFd);
        close(m_epfd);
        m_isExit = EXIT_INVALID;
        for (auto &iter : m_servList)
        {
            close(iter.first);
        }
    }
    virtual int Init()
    {
        struct sockaddr_in addr;
        int addrlen;

        /* 创建 socket */
        m_listenFd = socket(PF_INET, SOCK_STREAM, 0);
        if (m_listenFd < 0)
        {
            LOG_FATAL("create socket error");
            return RET_ERR;
        }
        fcntl(m_listenFd, F_SETFL, O_NONBLOCK); // 设置非阻塞方式
        /* 设置端口快速重用 */
        addrlen = 1;
        setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &addrlen,
                   sizeof(addrlen));

        addr.sin_family = AF_INET;
        addr.sin_port = htons(m_port);
        addr.sin_addr.s_addr = INADDR_ANY;
        addrlen = sizeof(struct sockaddr_in);
        /* 绑定地址、端口等信息 */

        if (bind(m_listenFd, (struct sockaddr *)&addr, addrlen) < 0)
        {
            LOG_FATAL("bind socket error");
            return RET_ERR;
        }

        /* 开启临听 */
        if (listen(m_listenFd, 5) < 0)
        {
            LOG_FATAL("listen socket error");
            return RET_ERR;
        }
        struct epoll_event ev;
        //生成用于处理accept的 epoll专用的文件描述符
        m_epfd = epoll_create(MAX_EVENTS);
        //设置与要处理的事件相关的文件描述符
        ev.data.fd = m_listenFd;
        //设置要处理的事件类型
        ev.events = EPOLLIN | EPOLLET;
        //注册epoll事件
        if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listenFd, &ev) < 0)
        {
            LOG_FATAL("worker epoll_ctl error = %s.", strerror(errno));
            return RET_ERR;
        }
        m_cliMgr =
            reinterpret_cast<cliMgr *>(g_objKernel->Query(MODELU_CLI));
        g_objKernel->Init([]() -> void {
            telnet *obj =
                reinterpret_cast<telnet *>(g_objKernel->InterFace(MODELU_TELNET));
            if (obj != nullptr)
                obj->Process();
        });
        return RET_OK;
    }
    int Process()
    {
        int isExit;
        int len;
        struct epoll_event ev, events[MAX_EVENTS];
        while (!m_isExit)
        {
            // 等待epoll事件的发生
            int nfds = epoll_wait(m_epfd, events, MAX_EVENTS, -1);
            // 处理所发生的所有事件
            for (int i = 0; i < nfds; ++i)
            {
                if (events[i].data.fd == m_listenFd)
                {
                    socklen_t clilen;
                    struct sockaddr_in clientaddr;

                    int sockfd = accept(m_listenFd,
                                        (sockaddr *)&clientaddr,
                                        &clilen);
                    if (sockfd < 0)
                    {
                        continue;
                    }

                    // 设置非阻塞
                    if (fcntl(sockfd,
                              F_SETFL,
                              fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1)
                    {
                        continue;
                    }
                    //设置用于读操作的文件描述符
                    ev.data.fd = sockfd;
                    //设置用于注测的读操作事件
                    ev.events = EPOLLIN | EPOLLET;
                    //注册ev
                    epoll_ctl(m_epfd, EPOLL_CTL_ADD, sockfd, &ev);
                    m_cliMgr->Welcome(sockfd);
                    {
                        std::unique_lock<std::mutex> lock{m_synclock};
                        m_servList[sockfd] = sockfd;
                    }
                }
                else if (events[i].events & EPOLLIN)
                {
                    int sockfd = events[i].data.fd;
                    if (sockfd < 0)
                    {
                        continue;
                    }

                    char buf[MAX_BUFFER_LEN] = {0};
                    // 开始处理每个新连接上的数据收发
                    bzero(buf, sizeof(buf));
                    len = read(sockfd, buf, MAX_BUFFER_LEN - 1);

                    if (len <= 0)
                    {
                        close(sockfd);
                        events[i].data.fd = -1;
                        {
                            std::unique_lock<std::mutex> lock{m_synclock};
                            m_servList.erase(sockfd);
                        }
                        //删除ev
                        ev.data.fd = sockfd;
                        epoll_ctl(m_epfd, EPOLL_CTL_DEL, sockfd, &ev);
                        LOG_ERROR("worker read data error = %s.",
                                  strerror(errno));
                    }
                    else
                    {
                        char *pos = strrchr(buf, '\r');
                        isExit = EXIT_INVALID;
                        buf[len - 1] = '\0';
                        if (pos != nullptr)
                        {
                            *pos = '\0';
                        }
                        m_cliMgr->Process(sockfd, buf,
                                          strlen(buf), &isExit);
                        switch (isExit)
                        {
                        case USER_EXIT:
                            close(sockfd);
                            events[i].data.fd = -1;
                            break;
                        case SYS_EXIT:
                            return RET_OK;
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
        return RET_OK;
    }
    void GetServerList(std::map<int, int> &list)
    {
        std::unique_lock<std::mutex> lock{m_synclock};
        list = m_servList;
    }
};
REG_TO_FRAMEWORK(TABLE_ONE, MODELU_KERNEL, telnet, MODELU_TELNET)
#endif

class SysCli : public CClibase
{
private:
public:
    virtual int Init()
    {
        LOCAL CMD_OBJ syscmd[] =
            {
                {"help", "help:dump", MODELU_CLI, CMD_SYS_HELP, this, nullptr, true},
                {"dump", "dump:climgr", MODELU_CLI, CMD_SYS_DUMP, this, nullptr, true},
                {"exit", "exits the current user", MODELU_CLI, CMD_SYS_EXIT, this, nullptr, true},
                {"sys exit", "shutdown system", MODELU_CLI, CMD_SYS_SHUT_DOWN, this, nullptr, true},
                {"get-err-list",
                 "Get all the error code information registered by the system",
                 MODELU_CLI, CMD_SYS_ERR_CODE, this, nullptr, true},
            };

        cliMgr *cli =
            reinterpret_cast<cliMgr *>(g_objKernel->InterFace(MODELU_CLI));
        if (cli)
        {

            cli->RegCmd(syscmd, ARRAY_SIZE(syscmd));
        }

        return 0;
    }
    void Help(int fd = 0, Printfun callback = LVOS_Printf)
    {
        (callback)(fd, "\r\nplease:\r\n");
        (callback)(fd, "\tdump:climgr => Print the climgr module information\r\n");
        (callback)(fd, "\thelp:dump => Print dump command information\r\n");
        (callback)(fd, "\texit =>exits the current user\r\n");
        (callback)(fd, "\tsys exit => shutdown system\r\n");
    }
};
REG_TO_FRAMEWORK(TABLE_TWO, MODELU_CLI, SysCli, MODELU_CLI)

int cliMgr::Dispatch(CAgrcList *inMessage, CMD_OBJ *pcmdObj, int fd)
{

    RspMsg outMessage;
    int iRet = 0;
    if (pcmdObj->bSync)
    {
        iRet = m_Cnotify->Notify(inMessage,
                                 &outMessage,
                                 pcmdObj->cmdModule,
                                 pcmdObj->cmdId);
        if (iRet == 0 && outMessage.count != 0)
        {
            iRet = Report(&outMessage, pcmdObj, fd);
        }
    }
    else
    {
        if (pcmdObj->cmdModule == MODULE_ALL)
        {
            m_Cnotify->NotifyA(inMessage,
                               &outMessage,
                               pcmdObj->cmdModule,
                               pcmdObj->cmdId);
        }
        else
        {
            iRet = m_Cnotify->AsyncNotify(inMessage,
                                          pcmdObj->cmdModule,
                                          pcmdObj->cmdId);
        }
    }
    return iRet;
}
int cliMgr::Report(CAgrcList *message,
                   RspMsg *outmessage,
                   int iModule,
                   int iCmd)
{
    if (message != nullptr)
    {
        std::map<int, int> list;
#ifndef __WIN32__
        telnet *obj =
            reinterpret_cast<telnet *>(g_objKernel->InterFace(MODELU_TELNET));
        if (obj != nullptr)
            obj->GetServerList(list);
#else
        list[0] = 0;
#endif
        // 查找命令模块
        bool isfind = false;
        CMD_OBJ *pcmdObj = nullptr;
        for (auto &iter : m_cmdList)
        {
            if (iter.second != nullptr)
            {
                if (iter.second->cmdId == outmessage->cmd)
                {
                    isfind = true;
                    pcmdObj = iter.second;
                    break;
                }
            }
        }
        if (isfind)
        {
            for (auto &iter : list)
            {
                Report(outmessage, pcmdObj, iter.first);
            }
        }

        delete outmessage;
    }

    return RET_OK;
}
int cliMgr::Process(CAgrcList *message,
                    RspMsg *outmessage, int iModule, int iCmd)
{
    PROCESS_BEGIN(iCmd)
    PROCESS_CALL(CMD_MSG_REPORT, Report)
    PROCESS_END()
}

int cliMgr::Report(RspMsg *outMessage, int module, int cmd)
{
    if (g_objKernel != nullptr)
    {
        Cnotify *objptr = (Cnotify *)g_objKernel->InterFace(MODELU_NOTIFY);
        if (objptr != nullptr)
        {
            objptr->AsyncNotify(nullptr,
                                MODELU_CLI,
                                CMD_MSG_REPORT,
                                outMessage);
        }
    }
    return RET_ERR;
}
int cliMgr::Report(RspMsg *outMessage, CMD_OBJ *pcmdObj, int fd)
{

    if ((outMessage == nullptr) ||
        (outMessage->count == 0) ||
        (outMessage->msg == nullptr))
    {
        return RET_ERR;
    }

    RspTable *ptbl = pcmdObj->rspTable;
    if (ptbl == nullptr ||
        ptbl->count == 0 ||
        ptbl->tblBody == nullptr)
    {
        return RET_ERR;
    }

    CStream _outinfo;
    CStream outinfo;
    int totallen = 0;
    const int maxlen = 128;
    char buf[maxlen] = {0};
    int len = 0;
    for (int i = 0; i < ptbl->count; i++)
    {
        len = snprintf(buf,
                       maxlen,
                       "%-*s",
                       ptbl->tblBody[i].fmrlen,
                       ptbl->tblBody[i].item);
        _outinfo.Append((const BYTE *)buf, len);
        totallen += ptbl->tblBody[i].fmrlen;
    }
    len = snprintf(buf,
                   maxlen,
                   "\r\n%*s\r\n%s\r\n",
                   totallen / 2,
                   ptbl->tblName,
                   _outinfo.GetBuff());
    outinfo.Append((const BYTE *)buf, len);
    for (int i = 0; i < outMessage->count; i++)
    {
        for (int j = 0; j < ptbl->count; j++)
        {
            len = snprintf(buf, maxlen, "%-*s",
                           ptbl->tblBody[j].fmrlen,
                           outMessage->msg[i].GetAgrc(
                                                 ptbl->tblBody[j].item)
                               ->GetBuff());
            outinfo.Append((const BYTE *)buf, len);
        }
        len = snprintf(buf, maxlen, "%s", "\r\n");
        outinfo.Append((const BYTE *)buf, len);
    }
    len = snprintf(buf,
                   maxlen,
                   "Tatol: %d record\r\n",
                   outMessage->count);
    outinfo.Append((const BYTE *)buf, len);
    (void)LVOS_Printf(fd, (char *)outinfo.GetBuff());
    return RET_OK;
}
void cliMgr::Welcome(int fd, Printfun callback)
{
#ifndef __WIN32__
    g_objKernel->Welcome(fd);
#endif
    callback(fd, "\tWelcome!");
    callback(fd, "\r\nhim:#", 6);
}
int cliMgr::Process(int fd, char *cmdbuffer, int len, int *isExit)
{
    int iRet = -1;
    bool cliOp = false;
    CMD_OBJ *pcmdObj = nullptr;
    CStream cmd;
    CStream argc;
    CAgrcList inMessage;
    CAgrcList cliMessage;
    ERR_CODE_INFO *err;
    char *pos = nullptr;
    if (cmdbuffer == nullptr ||
        len <= 0 ||
        len > MAX_BUFFER_LEN ||
        *cmdbuffer == '\0')
    {
        iRet = RET_ERR;
        goto _end;
    }
    pos = strchr(cmdbuffer, ':');
    if (pos != nullptr)
        cmd.Append((BYTE *)cmdbuffer, pos - cmdbuffer);
    else
        cmd.Append((BYTE *)cmdbuffer, strlen(cmdbuffer));
    if (pos != nullptr && ++pos != nullptr)
    {
        argc.Append((BYTE *)pos, strlen(pos));
        // 提取参数列表
        char delims[] = ",";
        char *result = NULL;
        char *argcpos = NULL;
        result = strtok(pos, delims);
        while (result != NULL)
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
    if (inMessage.GetCount() == 0 && pos != nullptr && *pos != '\0')
    {
        inMessage.addAgrc(ARGC_DEFAULT, pos);
    }

    pcmdObj = FindModule((char *)cmd.GetBuff());

    iRet = CliSys(fd, pcmdObj, (char *)argc.GetBuff(), isExit);

    if (iRet == RET_OK)
    {
        return RET_OK;
    }

    if (pcmdObj != nullptr)
    {
        if (OS::equal((char *)cmd.GetBuff(), GET_STR, CMD_OP_MAX))
        {
            iRet = pcmdObj->objCli->Get(&inMessage, &cliMessage, &cliOp);
        }
        else if (OS::equal((char *)cmd.GetBuff(), SET_STR, CMD_OP_MAX))
        {
            iRet = pcmdObj->objCli->Get(&inMessage, &cliMessage, &cliOp);
            LOG_TRACE("cmd:%s", cmdbuffer);
        }
        else if (OS::equal((char *)cmd.GetBuff(), ADD_STR, CMD_OP_MAX))
        {
            iRet = pcmdObj->objCli->Get(&inMessage, &cliMessage, &cliOp);
            LOG_TRACE("cmd:%s", cmdbuffer);
        }
        else
        {
        }
        if (iRet != 0)
        {
            err = g_objKernel->ErrGet(iRet);
            LVOS_Printf(fd, "Param process error:%s(%#X)",
                        err->name, err->id);
            iRet = RET_ERR;
            goto _end;
        }
    }
    else
    {
        LVOS_Printf(fd, "Cmd is unregister.\r\n");
        iRet = RET_ERR;
        goto _end;
    }

    iRet = Dispatch((cliOp == false) ? &inMessage : &cliMessage, pcmdObj, fd);
    if (iRet != 0)
    {
        err = g_objKernel->ErrGet(iRet);
        LOG_WARN("Dispatch is process error:%s(%#X)", err->name, err->id);
        LVOS_Printf(fd, "%s(%#X):%s\r\n", err->name, err->id, err->desc);
    }
_end:
    LVOS_Printf(fd, "\r\nhim:#", 6);
    return iRet;
}

int cliMgr::Process()
{
    int isExit = false;
    Welcome();
    while (!isExit)
    {
        char name[MAX_BUFFER_LEN] = {0};
        isExit = false;
        std::cin.getline(name, MAX_BUFFER_LEN);
        Process(0, name, strlen(name), &isExit);
    }
    return 0;
}
CMD_OBJ *cliMgr::FindModule(const char *cmdName)
{
    for (auto &iter : m_cmdList)
    {
        if (OS::equal(cmdName, iter.second->name))
            return iter.second;
    }
    return nullptr;
}
void cliMgr::dump(int fd, Printfun callback)
{
    objbase::PrintHead(fd, callback, "cliMgr", 66);
    (void)callback(fd, "%-8s %-8s %-16s %-32s\r\n", "cmdId", "objId", "objPtr", "cmd");
    objbase::PrintEnd(fd, callback, 66);
    for (auto &iter : m_cmdList)
        (void)callback(fd,
                       "%#-8x %#-8x %#-16x %-32s\r\n",
                       iter.second->cmdId,
                       iter.second->cmdModule,
                       iter.second->objCli,
                       iter.second->name);
    objbase::PrintEnd(fd, callback, 66);
    (void)callback(fd, "Tatol: %d\r\n", m_cmdList.size());
}
cliMgr::~cliMgr()
{
    for (auto &iter : m_cmdList)
        if (iter.second != nullptr)
        {
            CClibase *objCli = iter.second->objCli;
            if (objCli != nullptr)
            {
                objCli->DecRefCount();
                if (objCli->GetRefCount() == 0)
                {
                    delete objCli;
                }
            }
            objCli = nullptr;
            iter.second = nullptr;
        }
    m_cmdList.clear();
}
cliMgr::cliMgr()
{
}
/*
    注册命令到cli管理模块
*/
int cliMgr::RegCmd(CMD_OBJ *list, int count)
{
    int index = 0;

    std::unique_lock<std::mutex> lock{m_reglock};
    for (index = 0; index < count; index++)
    {
        auto iter = m_cmdList.find(list[index].cmdId);
        if (iter != m_cmdList.end())
        {
            continue;
        }
        m_cmdList[list[index].cmdId] = &list[index];
        list[index].objCli->AddRefCount();
    }

    return (index + 1); //返回注册成功的数量
}
int cliMgr::Init()
{
    m_Cnotify = reinterpret_cast<Cnotify *>(g_objKernel->Query(MODELU_NOTIFY));
    m_Cnotify->RegReceiver(MODELU_CLI, new REGNOTIFY(this, "cliMgr"));
#ifdef __WIN32__
    g_objKernel->Init([]() -> void {
        cliMgr *obj =
            reinterpret_cast<cliMgr *>(g_objKernel->InterFace(MODELU_CLI));
        if (obj != nullptr)
            obj->Process();
    });
#endif
    return 0;
}
void cliMgr::Reg(const char *pzName, void *obj, int id)
{
    ((CClibase *)obj)->Init();
}

int cliMgr::CliSys(int fd, CMD_OBJ *pcmdObj,
                   const char *argc, int *isExit)
{
    int iRet = RET_ERR;

    if (pcmdObj != nullptr)
    {
        switch (pcmdObj->cmdId)
        {
        case CMD_SYS_HELP:
        {
            if (!strlen(argc))
            {
                g_objKernel->Query(MODELU_CLI)->dump(fd);
                iRet = RET_OK;
                break;
            }
            CMD_OBJ *pobj = FindModule(argc);
            if (pobj != nullptr)
            {
                LVOS_Printf(fd, pobj->help);
                LVOS_Printf(fd, "\r\n");
                iRet = RET_OK;
            }
            else
            {
                iRet = RET_ERR;
            }

            break;
        }

        case CMD_SYS_DUMP:
        {
            objbase *pobjBase = g_objKernel->InterFace(argc);
            if (pobjBase)
                pobjBase->dump(fd);
            else
                g_objKernel->dump(fd);
            iRet = RET_OK;
            break;
        }

        case CMD_SYS_EXIT:

        {
            *isExit = USER_EXIT;
            iRet = RET_OK;
            break;
        }

        case CMD_SYS_SHUT_DOWN:
        {
            *isExit = SYS_EXIT;
            iRet = RET_OK;
            break;
        }

        case CMD_SYS_ERR_CODE:
        {
            std::map<int, ERR_CODE_INFO *> list;
            g_objKernel->ErrGet(list);
            objbase::PrintHead(fd, LVOS_Printf, "ERR-CODE-TABLE", 128);
            LVOS_Printf(fd, "%-16s %-32s %-16s\r\n", "errId", "Name", "Description");
            objbase::PrintEnd(fd, LVOS_Printf, 128);
            for (auto &iter : list)
                (void)LVOS_Printf(fd,
                                  "%#-16X %#-32s %-128s\r\n",
                                  iter.second->id,
                                  iter.second->name,
                                  iter.second->desc);
            objbase::PrintEnd(fd, LVOS_Printf, 128);
            (void)LVOS_Printf(fd, "Tatol: %d\r\n", list.size());
            iRet = RET_OK;
            break;
        }

        default:
        {
            break;
        }
        }
    }
    return iRet;
}
REG_TO_FRAMEWORK(TABLE_ONE, MODELU_KERNEL, cliMgr, MODELU_CLI)

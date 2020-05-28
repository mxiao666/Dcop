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
#ifndef __WIN32__
class telnet : public objbase
{
private:
    int m_port;
    int m_listenFd;
    bool m_isExit;
    int m_epfd;
    cliMgr *m_cliMgr;
    std::map<int, int> m_servList;
    std::mutex m_synclock;
#define MAX_BUFFER_LEN 1024
#define MAX_EVENTS 1000
public:
    telnet(int port = 2323)
    {
        m_port = port;
        m_listenFd = 0;
        m_isExit = false;
        m_epfd = 0;
        m_servList.clear();
    }
    ~telnet()
    {
        close(m_listenFd);
        close(m_epfd);
        m_isExit = true;
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
        bool isExit;
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
                    len = read(sockfd, buf, MAX_BUFFER_LEN);

                    if (len < 0)
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
                        isExit = false;
                        char *pos = strrchr(buf, '\r');
                        if (pos != nullptr)
                        {
                            *pos = '\0';
                        }
                        m_cliMgr->Process(sockfd, buf,
                                          strlen(buf), isExit);
                        if (isExit)
                        {
                            close(sockfd);
                            events[i].data.fd = -1;
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
int cliMgr::Dispatch(CAgrcList *inMessage, cmdObj *pcmdObj, int fd)
{

    RspMsg outMessage;
    int iRet = 0;
    if (pcmdObj->bSync)
    {
        if (pcmdObj->cmdModule != 0)
        {
            iRet = m_Cnotify->Notify(inMessage,
                                     &outMessage,
                                     pcmdObj->cmdModule,
                                     pcmdObj->cmdid);
            if (iRet == 0 && outMessage.count != 0)
            {
                iRet = Report(&outMessage, pcmdObj, fd);
            }
        }
        else
        {
            m_Cnotify->SendToAll(inMessage,
                                 &outMessage,
                                 pcmdObj->cmdModule,
                                 pcmdObj->cmdid);
        }
    }
    else
    {
        iRet = m_Cnotify->AsyncNotify(inMessage,
                                      pcmdObj->cmdModule,
                                      pcmdObj->cmdid);
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
        cmdObj *pcmdObj = nullptr;
        for (auto &iter : m_cmdList)
        {
            if (iter.second != nullptr)
            {
                if (iter.second->cmdid == outmessage->cmd)
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
    return m_Cnotify->AsyncNotify(nullptr,
                                  MODELU_CLI,
                                  CMD_MSG_REPORT,
                                  outMessage);
}
int cliMgr::Report(RspMsg *outMessage, cmdObj *pcmdObj, int fd)
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
                   "\n%*s\n%s\n",
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
        len = snprintf(buf, maxlen, "%s", "\n");
        outinfo.Append((const BYTE *)buf, len);
    }
    len = snprintf(buf,
                   maxlen,
                   "Command response %d line record\n",
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
    callback(fd, "\nhim:#", 6);
}
int cliMgr::Process(int fd, char *cmdbuffer, int len, bool &isExit)
{
    int iRet = -1;
    bool cliOp = false;
    cmdObj *pcmdObj = nullptr;
    CStream cmd;
    CStream argc;
    CAgrcList inMessage;
    CAgrcList cliMessage;
    char *pos = nullptr;
    if (cmdbuffer == nullptr || len <= 0 || *cmdbuffer == '\0')
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
    if (OS::equal((char *)cmd.GetBuff(), "q") ||
        OS::equal((char *)cmd.GetBuff(), "quit") ||
        OS::equal((char *)cmd.GetBuff(), "e") ||
        OS::equal((char *)cmd.GetBuff(), "exit"))
    {
        isExit = true;
        iRet = RET_OK;
        goto _end;
    }

    if (OS::equal((char *)cmd.GetBuff(), "dump"))
    {
        objbase *pobjBase = g_objKernel->InterFace((char *)argc.GetBuff());
        if (pobjBase)
            pobjBase->dump(fd);
        else
            g_objKernel->dump(fd);
        iRet = RET_OK;
        goto _end;
    }

    if (OS::equal((char *)cmd.GetBuff(), "man") ||
        OS::equal((char *)cmd.GetBuff(), "help"))
    {
        pcmdObj = FindModule((char *)argc.GetBuff());
        if (pcmdObj)
            pcmdObj->objCli->Help(fd);
        else
            dump(fd);
        iRet = RET_OK;
        goto _end;
    }

    pcmdObj = FindModule((char *)cmd.GetBuff());
    if (pcmdObj != nullptr)
    {
        if (OS::equal((char *)cmd.GetBuff(), GET_STR, CMD_OP_MAX))
        {
            iRet = pcmdObj->objCli->Get(&inMessage, &cliMessage, &cliOp);
        }
        else if (OS::equal((char *)cmd.GetBuff(), SET_STR, CMD_OP_MAX))
        {
            iRet = pcmdObj->objCli->Get(&inMessage, &cliMessage, &cliOp);
            LOG_INFO("cmd:%s", cmdbuffer);
        }
        else if (OS::equal((char *)cmd.GetBuff(), ADD_STR, CMD_OP_MAX))
        {
            iRet = pcmdObj->objCli->Get(&inMessage, &cliMessage, &cliOp);
            LOG_INFO("cmd:%s", cmdbuffer);
        }
        else
        {
        }
        if (iRet != 0)
        {
            LVOS_Printf(fd, "Cmd param is process error=%d\n", iRet);
            iRet = RET_ERR;
            goto _end;
        }
    }
    else
    {
        LVOS_Printf(fd, "Cmd is unregister.\n");
        iRet = RET_ERR;
        goto _end;
    }

    iRet = Dispatch((cliOp == false) ? &inMessage : &cliMessage, pcmdObj, fd);
    if (iRet != 0)
    {
        LOG_WARN("Dispatch is process error:%#016x", iRet);
        LVOS_Printf(fd, "Dispatch is process error:%#016x\n", iRet);
    }
_end:
    LVOS_Printf(fd, "\nhim:#", 6);
    return iRet;
}

int cliMgr::Process()
{

    const int ArSize = 128;
    bool isExit = false;
    Welcome();
    while (!isExit)
    {
        char name[ArSize] = {0};
        isExit = false;
        std::cin.getline(name, ArSize);
        Process(0, name, strlen(name), isExit);
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
void cliMgr::dump(int fd, Printfun callback)
{
    objbase::PrintHead(fd, callback, "cliMgr", 66);
    (void)callback(fd, "%-16s %-16s %-32s\n", "ModuleId", "objPtr", "cmd");
    for (auto &iter : m_cmdList)
        (void)callback(fd,
                       "%#-16x %#-16x %-32s\n",
                       iter.second->cmdModule,
                       iter.second->objCli,
                       iter.first);
    (void)callback(fd, "Tatol: %d\n", m_cmdList.size());
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
    m_Cnotify = reinterpret_cast<Cnotify *>(g_objKernel->Query(MODELU_NOTIFY));
    m_Cnotify->RegReceiver(MODELU_CLI, this);
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
REG_TO_FRAMEWORK(TABLE_ONE, MODELU_KERNEL, cliMgr, MODELU_CLI)
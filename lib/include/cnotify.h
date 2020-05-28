#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <vector>
#include <string>
#include <map>
#include "objbase.h"
#include <queue>

typedef struct _AsyncArgc
{
    CAgrcList *msg;
    int iModule;
    int iCmd;
    void *m_ptr;
    _AsyncArgc(CAgrcList *objmsg, int module, int cmd, void *parm = nullptr)
    {
        msg = new CAgrcList(objmsg);
        iModule = module;
        iCmd = cmd;
        m_ptr = parm;
    }
    ~_AsyncArgc()
    {
        if (msg)
            delete msg;
    }

} AsyncArgc;

class Cnotify : public objbase
{
public:
    Cnotify();
    virtual ~Cnotify();
    void UnRegReceiver(int iModule);
    void SendToAll(CAgrcList *message,
                   RspMsg *outmessage, int iModule, int iCmd);
    void RegReceiver(int iModule, objbase *pRecv);
    int Notify(CAgrcList *message,
               RspMsg *outmessage, int iModule, int iCmd);
    void dump(int fd = 0, Printfun callback = LVOS_Printf);
    void AsyncProc();
    int Init();
    int AsyncNotify(CAgrcList *message, int iModule,
                    int iCmd, void *parm = nullptr);

private:
    std::map<int, objbase *> observerList;
    std::mutex m_reglock;
    // 任务队列
    std::queue<AsyncArgc *> tasks;
    // 同步
    std::mutex m_lock;
    // 条件阻塞
    std::condition_variable cv_task;
    // 是否关闭提交
    std::atomic<bool> stoped;
    //空闲线程数量
    std::atomic<int> idlThrNum;
};

#endif // NOTIFIER_H

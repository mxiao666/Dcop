#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <vector>
#include <string>
#include <map>
#include "objbase.h"
#include <queue>
#include "objKernel.h"
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
typedef struct regNotify
{
#define REG_NAME_MAX_LEN 16
    objbase *obj;
    char name[REG_NAME_MAX_LEN];
    regNotify(objbase *_obj, const char *_name)
    {
        this->obj = _obj;
        (void)snprintf(this->name, REG_NAME_MAX_LEN, "%s", _name);
    }
    ~regNotify()
    {
        this->obj = nullptr;
    }
} REGNOTIFY;
class Cnotify : public objbase
{
public:
    Cnotify();
    virtual ~Cnotify();
    void UnRegReceiver(int iModule);
    void NotifyA(CAgrcList *message,
                 RspMsg *outmessage, int iModule, int iCmd, bool sync = false);
    void RegReceiver(int iModule, REGNOTIFY *pRecv);
    int Notify(CAgrcList *message,
               RspMsg *outmessage, int iModule, int iCmd);
    void dump(int fd = 0, Printfun callback = LVOS_Printf);
    void AsyncProc();
    int Init();
    int AsyncNotify(CAgrcList *message, int iModule,
                    int iCmd, void *parm = nullptr);

private:
    std::map<int, REGNOTIFY *> observerList;
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

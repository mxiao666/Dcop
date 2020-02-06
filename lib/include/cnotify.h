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
    _AsyncArgc(CAgrcList *objmsg, int module, int cmd)
    {
        msg = new CAgrcList(objmsg);
        iModule = module;
        iCmd = cmd;        
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
    void SendToAll(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd);
    void RegReceiver(int iModule, objbase *pRecv);
    int Notify(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd);
    void dump(Printfun callback);
    void AsyncProc();
    int Init();
    int AsyncNotify(CAgrcList *message,int iModule, int iCmd);

private:
    std::map<int, objbase *> observerList;
    std::mutex m_reglock;
    // �������
    std::queue<AsyncArgc*> tasks;
    // ͬ��
    std::mutex m_lock;
    // ��������
    std::condition_variable cv_task;
    // �Ƿ�ر��ύ
    std::atomic<bool> stoped;
    //�����߳�����
    std::atomic<int> idlThrNum;    
};

#endif // NOTIFIER_H

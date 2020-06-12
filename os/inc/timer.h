#ifndef __TIMERTASK_H__
#define __TIMERTASK_H__

/* ��ʱ��������� */
#include "objbase.h"
#define ull unsigned long long
#define __INF__ 1e18

class Timer
{
public:
    enum TimerType
    {
        ONCE,
        CIRCLE
    };
    TimerType ttype;          //�������ͣ�һ�λ���ѭ��
    void *arg;                //�����������
    ull itvl;                 //ʱ��Ƭ����(ms)
    ull expires;              //����ʱ��
    void *(*run)(void *args); //�ص�����
    bool stop;
    Timer(void *(*run)(void *args), void *arg, ull itvl, Timer::TimerType ttype); //��ʼ��
    void Start();
    void Stop();
    void OnTimer(); //��ʣ��ʱ��
};
class TimerManager : public objbase
{
private:
    std::mutex m_reglock;
    std::map<const char *, Timer *> heap; //��
    bool stop;
    int sysShutdown(CAgrcList *message, RspMsg *outmessage,
                    int iModule, int iCmd);

public:
    TimerManager();
    ~TimerManager();                               //��������
    void DetectTimers();                           //����Ƿ�ʱ
    void AddTimer(const char *name, Timer *timer); //��������һ����ʱ��
    void RemoveTimer(Timer *timer);                //�Ƴ���ʱ��
    int Init();
    void dump(int fd = 0, Printfun callback = LVOS_Printf);
    static void *TimerMsg(void *args);
    static ull GetCurrentMillisecs(); //�õ���ǰʱ��(ms)
    int Process(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd);
};

#endif
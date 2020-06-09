#ifndef __TIMERTASK_H__
#define __TIMERTASK_H__

/* 定时器任务基类 */
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
    TimerType ttype;          //任务类型，一次还是循环
    void *arg;                //会调函数参数
    ull itvl;                 //时间片长度(ms)
    ull expires;              //到期时间
    void *(*run)(void *args); //回调函数
    bool stop;
    Timer(void *(*run)(void *args), void *arg, ull itvl, Timer::TimerType ttype); //初始化
    void Start();
    void Stop();
    void OnTimer(); //无剩余时间
};
class TimerManager : public objbase
{
private:
    std::mutex m_reglock;
    std::map<const char *, Timer *> heap; //堆
    bool stop;
    int sysShutdown(CAgrcList *message, RspMsg *outmessage,
                    int iModule, int iCmd);

public:
    TimerManager();
    ~TimerManager();                               //析构函数
    void DetectTimers();                           //检测是否超时
    void AddTimer(const char *name, Timer *timer); //向堆中添加一个定时器
    void RemoveTimer(Timer *timer);                //移除定时器
    int Init();
    void dump(int fd = 0, Printfun callback = LVOS_Printf);
    static void *TimerMsg(void *args);
    static ull GetCurrentMillisecs(); //得到当前时间(ms)
    int Process(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd);
};

#endif
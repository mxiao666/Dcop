#ifdef _MSC_VER
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif
#include <thread>
#include <chrono>
#include <vector>
#include "timer.h"
#include "objKernel.h"
#include "objTask.h"
#include "cnotify.h"
Timer::Timer(void *(*run)(void *args), void *arg, ull itvl, Timer::TimerType ttype)
{
    this->itvl = itvl;
    this->run = run;
    this->arg = arg;
    this->ttype = ttype;
    this->expires = 0;
    this->stop = false;
}
void Timer::Start()
{
    this->stop = false;
}
void Timer::Stop()
{
    this->stop = true;
}
void Timer::OnTimer()
{
    if (stop)
        return;
    expires = (expires + 1000) % itvl;
    if (expires == 0)
        this->run(this->arg);
}
TimerManager::TimerManager()
{
    heap.clear();
    stop = false;
}
void *TimerManager::TimerMsg(void *args)
{
    Cnotify *obj =
        reinterpret_cast<Cnotify *>(args);
    if (obj)
        obj->NotifyA(nullptr, nullptr, 0, CMD_MSG_TIMER);
    return (void *)0;
}
TimerManager::~TimerManager()
{
    std::unique_lock<std::mutex> lock{m_reglock};
    for (auto &iter : heap)
        delete iter.second;
    heap.clear();
}
int TimerManager::Init()
{
    Cnotify *obj =
        reinterpret_cast<Cnotify *>(g_objKernel->InterFace(MODELU_NOTIFY));
    if (obj)
        obj->RegReceiver(MODELU_TIMER, new REGNOTIFY(this, "TimerManager"));
    AddTimer("TimerManager",
             new Timer(TimerManager::TimerMsg,
                       g_objKernel->InterFace(MODELU_NOTIFY),
                       1000,
                       Timer::CIRCLE));
    objPara *pObjPara = new objPara(this);
    CREATE_OBJTASK(
        "TimerManager", [](objPara *pobjPara) {
            TimerManager *obj =
                reinterpret_cast<TimerManager *>(pobjPara->GetPara());
            if (obj)
                obj->DetectTimers();
            return (void *)0;
        },
        pObjPara);
    return 0;
}
void TimerManager::AddTimer(const char *name, Timer *timer)
{
    std::unique_lock<std::mutex> lock{m_reglock};
    heap[name] = timer;
}

void TimerManager::RemoveTimer(Timer *timer)
{
    std::unique_lock<std::mutex> lock{m_reglock};
    for (auto iter = heap.begin(); iter != heap.end(); iter++)
    {
        if (iter->second == timer)
        {
            heap.erase(iter);
            return;
        }
    }
}

void TimerManager::DetectTimers()
{
    while (!stop)
    {
        {
            std::unique_lock<std::mutex> lock{m_reglock};
            for (auto iter = heap.begin(); iter != heap.end(); iter++)
            {
                iter->second->OnTimer();
            }
        _delete:
            for (auto iter = heap.begin(); iter != heap.end(); iter++)
            {
                if (iter->second->ttype == Timer::ONCE)
                {
                    heap.erase(iter);
                    goto _delete;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

ull TimerManager::GetCurrentMillisecs()
{
#ifdef _MSC_VER
    _timeb timebuffer;
    _ftime(&timebuffer);
    ull ret = timebuffer.time;
    ret = ret * 1000 + timebuffer.millitm;
    return ret;
#else
    timeval tv;
    ::gettimeofday(&tv, 0);
    ull ret = tv.tv_sec;
    return ret * 1000 + tv.tv_usec / 1000;
#endif
}
void TimerManager::dump(int fd, Printfun callback)
{
    objbase::PrintHead(fd, callback, "TimerManager", 32);
    (void)callback(fd, "%-16s %-16s\r\n", "objName", "obPtr");
    objbase::PrintEnd(fd, callback, 32);
    for (auto &iter : heap)
        (void)callback(fd, "%-16s %#-16X\r\n",
                       iter.first,
                       iter.second);

    objbase::PrintEnd(fd, callback, 32);
    (void)callback(fd, "Tatol: %d\r\n", heap.size());
}
int TimerManager::Process(CAgrcList *message, RspMsg *outmessage, int iModule, int iCmd)
{
    PROCESS_BEGIN(iCmd)
    PROCESS_CALL(CMD_SYS_SHUT_DOWN, sysShutdown)
    PROCESS_END()
}
int TimerManager::sysShutdown(CAgrcList *message, RspMsg *outmessage,
                              int iModule, int iCmd)
{
    stop = true;
    return SUCCESS;
}
REG_TO_FRAMEWORK(TABLE_ONE, MODELU_KERNEL, TimerManager, MODELU_TIMER)
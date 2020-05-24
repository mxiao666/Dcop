#ifndef __OBJTASK_H__
#define __OBJTASK_H__
#include <iostream>
#include <map>
#include <mutex>
#include <string.h>
#include <objbase.h>
#include "macro_define.h"
class objPara
{
public:
    objPara(void *obj) { m_para = obj; }
    ~objPara() {}
    void *GetPara() { return m_para; }

private:
    void *m_para;
};
typedef void *(*FunEntry)(objPara *pobjPara);
#define TASK_NAME_LEN 32
class objTask
{

public:
    objTask(const char *pzName, FunEntry func, objPara *obj);
    inline const char *Name() { return name; }
    inline u64 GetId() { return tId; }
    inline void SetId(u64 id) { tId = id; }
    inline objPara *Para() { return m_objPara; }
    inline void Run() { m_func(m_objPara); }
    ~objTask();

private:
    objTask() = delete;
    objTask(objTask &) = delete;
    u64 tId = 0;
    char name[TASK_NAME_LEN] = {0};
    FunEntry m_func;
    objPara *m_objPara;
};

class objTaskMgr : public objbase
{

public:
    static objTaskMgr *GetInstance();
    int addObj(objTask *obj);
    void delObj(u64 id);
    ~objTaskMgr();
    objTaskMgr();
    void dump(int fd = 0, Printfun callback = LVOS_Printf);

private:
    objTaskMgr(objTaskMgr &) = delete;
    const objTaskMgr &operator=(const objTaskMgr &) = delete;
    std::map<u64, objTask *> m_objlist;
    std::mutex m_lock;
};

#define CREATE_OBJTASK(objTaskName, func, pObjPara) \
    objTaskEntry(objTaskName, func, pObjPara, __FILE__, __LINE__)

objTask *objTaskEntry(const char *objTaskName,
                      FunEntry func,
                      objPara *pObjPara,
                      const char *file,
                      int len);

#endif

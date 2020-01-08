#ifndef __OBJTASK_H__
#define __OBJTASK_H__
#include <string.h>
class objPara{
public:
    objPara(void * obj){ m_para = obj;}
    ~objPara(){}
    void* GetPara(){return m_para;}
private:
    void * m_para;
};
typedef void*(*FunEntry)(objPara* pobjPara);
#define TASK_NAME_LEN 32
class objTask{

public:
    objTask(const char* pzName,FunEntry func,objPara** obj)
    {
        if(pzName != nullptr)
            strncpy(name, pzName,TASK_NAME_LEN - 1);
        name[TASK_NAME_LEN - 1] = '\0';
        m_func = func;
        m_objPara = obj;
    }  
    inline const char* Name() { return name;}
    inline int  GetId() {return tId;}
    inline void SetId(int id) { tId = id;}
    inline objPara* Para(){return *m_objPara;}
    inline void  Run(){ m_func(*m_objPara);}
    ~objTask(){ 
        if(*m_objPara != nullptr) delete *m_objPara;
        *m_objPara = nullptr;
    }
private:
    objTask() = delete;
    objTask(objTask&) = delete;
    int tId = 0;
    char name[TASK_NAME_LEN] = {0};
    FunEntry m_func;
    objPara** m_objPara;
};

#endif

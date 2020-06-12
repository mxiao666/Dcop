#ifndef __OS_CLIBASE__
#define __OS_CLIBASE__
#include "cargclist.h"
#include "objbase.h"
#include "frameworkmgr.h"
#include "cnotify.h"

typedef struct
{
    const char *item;
    int fmrlen;
} TblBody;
typedef struct
{
    const char *tblName;
    TblBody *tblBody;
    int count;
} RspTable;
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
class Clibase
{
private:
    int m_refCount = 0;

public:
    Clibase() {}
    virtual ~Clibase()
    {
        m_refCount = 0;
    }
    virtual int Set(CAgrcList *inPut, CAgrcList *outPut, bool *bOp)
    {
        return 0;
    };
    virtual int Get(CAgrcList *inPut, CAgrcList *outPut, bool *bOp)
    {
        return 0;
    };
    virtual int Add(CAgrcList *inPut, CAgrcList *outPut, bool *bOp)
    {
        return 0;
    };
    virtual int Init() { return 0; };
    int GetRefCount()
    {
        return m_refCount;
    }
    void AddRefCount()
    {
        ++m_refCount;
    }
    void DecRefCount()
    {
        --m_refCount;
    }
};
typedef struct
{
    const char *name;   //�������ַ���
    const char *help;   //����İ�����Ϣ
    int cmdModule;      // ��������ģ���id
    int cmdId;          // �����id
    Clibase *objCli;   // ���������Ҫ�Լ�������Σ��������
    RspTable *rspTable; //������Ҫ��Ӧ���ʱ�����ڱ����Ϣ
    bool bSync;         // ����Ϊͬ�� true
} CMD_OBJ;
class cliMgr : public objbase
{
private:
    std::map<int, CMD_OBJ *> m_cmdList;
    std::mutex m_reglock;
    Cnotify *m_Cnotify = nullptr;
    CMD_OBJ *FindModule(const char *cmdName);
    const int CMD_OP_MAX = 3;
    const char *GET_STR = "get";
    const char *SET_STR = "set";
    const char *ADD_STR = "add";
    int Dispatch(CAgrcList *message, CMD_OBJ *, int fd = 0);
    int Report(RspMsg *outMessage, CMD_OBJ *pcmdObj, int fd = 0);
    int Report(CAgrcList *message,
               RspMsg *outmessage,
               int iModule,
               int iCmd);
    int CliSys(int fd, CMD_OBJ *pcmdObj,
               const char *argc, int *isExit);

public:
    cliMgr();
    ~cliMgr();
    int Init();
    int Process();
    int Process(int fd, char *cmdbuffer, int len, int *isExit);
    void dump(int fd = 0, Printfun callback = LVOS_Printf);
    void Welcome(int fd = 0, Printfun callback = LVOS_Printf);
    int RegCmd(CMD_OBJ *list, int count);
    static int Report(RspMsg *outMessage, int module, int cmd);
    virtual void Reg(const char *pzName, void *obj, int id);
    virtual int Process(CAgrcList *message,
                        RspMsg *outmessage, int iModule, int iCmd);
};
#define ARGC_DEFAULT "ARGC"
#endif

#ifndef __OS_CSTREAM_H__
#define __OS_CSTREAM_H__
#include "type.h"
#define Stackm_len 64

class CStream
{
public:
    CStream();
    ~CStream();
    CStream(BYTE *buf, DWORD len);
    CStream(const BYTE *buf);
    BYTE *GetBuff();
    char *c_str() { return (char *)m_cbuf; }
    inline DWORD length()
    {
        return m_curLen;
    }
    inline void SetPos(DWORD len)
    {
        m_pos = len < m_len ? len : m_len;
    }
    inline DWORD GetPos()
    {
        return m_pos;
    }
    inline void SetCount(DWORD count)
    {
        m_count = count;
    }
    inline DWORD GetCount()
    {
        return m_count;
    }
    CStream &operator<<(BYTE t);
    CStream &operator<<(WORD t);
    CStream &operator<<(DWORD t);
    CStream &operator<<(CStream &t);

    CStream &operator>>(BYTE &t);
    CStream &operator>>(WORD &t);
    CStream &operator>>(DWORD &t);
    CStream &operator>>(CStream &t);

    CStream &Append(const BYTE *t, DWORD len);
    CStream &Extract(const BYTE *buf, DWORD len, DWORD begin = 0);
    void Clear();
    WORD to_WORD();
    DWORD to_DWORD();

private:
    CStream(const CStream &);
    CStream &operator=(const CStream &);
    bool GetMoreCapacity(DWORD len);
    void Init();
    void _PubInit();

private:
    BYTE m_buf[Stackm_len]; //缺省值放在栈上
    BYTE *m_cbuf;
    enum BufType
    {
        USER_BUFFER,
        FREAM_BUFFER,
        MALLOC_BUFFER
    };
    BYTE m_bufType; //根据此值决定释放空间
    DWORD m_len;
    DWORD m_curLen;
    DWORD m_pos;
    DWORD m_count;
    bool m_isOk; // 流的状态是否正常，上一步操作是否成功
};

#endif
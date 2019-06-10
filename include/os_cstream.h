#ifndef __OS_CSTREAM_H__
#define __OS_CSTREAM_H__
using BYTE = unsigned char;
using DWORD = unsigned int;
using WORD = unsigned short;
#define Stackm_len  64

class CStream
{
public:
    CStream();
    ~CStream();
    CStream(BYTE *buf, DWORD len);
    BYTE *GetBuff();
    inline DWORD  length(){
        return m_curLen;
    }
    inline void SetPos(DWORD len){
        m_pos = len < m_len ? len : m_len;
    }
    CStream& operator <<(BYTE t);
    CStream& operator <<(WORD t);
    CStream& operator <<(DWORD t);
    CStream& operator <<(CStream &t);

    CStream& operator >>(BYTE &t);
    CStream& operator >>(WORD &t);
    CStream& operator >>(DWORD &t);
    CStream& operator >>(CStream &t);

    CStream& Append(const BYTE *t, DWORD len);
    CStream& Extract(const BYTE *buf, DWORD len, DWORD begin = 0);
    void Clear();
private:                                   
    CStream(const CStream &);
    CStream &operator=(const CStream &);
    bool GetMoreCapacity(DWORD len);
    void Init();
private:
    BYTE m_buf[Stackm_len]; //缺省值放在栈上
    BYTE *m_cbuf;
    enum BufType
    {
        USER_BUFFER,
        FREAM_BUFFER,
        MALLOC_BUFFER
    };
    BYTE m_bufType;//根据此值决定释放空间
    DWORD m_len;
    DWORD m_curLen;
    DWORD m_pos;
};

#endif
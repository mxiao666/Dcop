#include "os_cstream.h"
#include <iostream>
#ifndef nullptr
#define nullptr 0
#endif

#if _LINUX_
#include <string.h>
#define    memcpy_s(dest, mlen, src, len)   memcpy(dest, src, len)
#endif

CStream::CStream()
{
    m_bufType = FREAMm_bufFER;
    m_cbuf = m_buf;
    m_len = Stackm_len;
    m_curLen = 0;
    m_buf[0] = '\0';
}

CStream::CStream(BYTE *buf,DWORD len)
{
    m_bufType = USERm_bufFER;
    m_cbuf = buf;
    m_len = len;
    m_curLen = 0;
    m_buf[0] = '\0';
}

CStream::~CStream(){
    if (MACLLOCm_bufFER == m_bufType){
        delete[] m_cbuf;
    }
}

CStream& CStream::operator << (BYTE t){
    if (!GetMoreCapacity(sizeof(t))){
        return *this;
    }
    m_cbuf[m_pos] = t; //添入缓冲区
    m_curLen += sizeof(t);
    return *this;
}
CStream& CStream::operator << (WORD t){
    if (!GetMoreCapacity(sizeof(t))){
        return *this;
    }

    for (size_t i = 0; i < sizeof(t); i++)
    {
        m_cbuf[m_curLen + i] = (t >> (8 - i * 8));
    }
    m_curLen += sizeof(t);//移动当前索引
    return *this;
}
CStream& CStream::operator << (DWORD t){
    if (!GetMoreCapacity(sizeof(t))){
        return *this;
    }
    
    for (size_t i = 0; i < sizeof(t); i++)
    {
        m_cbuf[m_curLen + i] = (t >> (24 - i * 8));
    }    
    m_curLen += sizeof(t);//移动当前索引
    return *this;
}
CStream& CStream::operator >> (BYTE &t){
    t = m_cbuf[m_pos]; //添入缓冲区
    m_pos += sizeof(t);     //移动当前索引
    return *this;
}
CStream& CStream::operator >> (DWORD &t){
    t = m_cbuf[m_pos] << 24 | m_cbuf[m_pos + 1] << 16 | m_cbuf[m_pos + 2] << 8 | m_cbuf[m_pos + 3];
    m_pos += sizeof(t);     //移动当前索引
    return *this;
}
CStream& CStream::operator >> (WORD &t){
    t = m_cbuf[m_pos] << 8 | m_cbuf[m_pos + 1];
    m_pos += sizeof(t);     //移动当前索引
    return *this;
}
bool CStream::GetMoreCapacity(DWORD len)
{
    if (USERm_bufFER == m_bufType){
        return false;
    }
    DWORD nSizeNew = m_curLen + len;
    if (nSizeNew < m_len){
        return true;
    }
    while (nSizeNew > m_len){
        m_len *= 2;
    }
    BYTE *bytesMore = new BYTE[m_len]; //两倍
    if (bytesMore == nullptr){
        return false;
    }

    (void)memcpy_s(bytesMore, m_len, m_cbuf, m_curLen);
    (void)memset(bytesMore + m_curLen, 0, m_len - m_curLen);

    if (MACLLOCm_bufFER == m_bufType){
        delete[] m_cbuf;
    }
    m_cbuf = bytesMore;
    return true;
}
BYTE *CStream::GetBuff(){
    return m_cbuf;
}
CStream& CStream::Append(const BYTE *buf, DWORD len){
    if (buf == nullptr || !GetMoreCapacity(len)){
        return *this;
    }
    (void)memcpy_s(m_cbuf + m_curLen, m_len - m_curLen, buf, len);
    m_curLen += len;
    return *this;
}

CStream& CStream::Extract(const BYTE *buf, DWORD len, DWORD begin){
    if (buf == nullptr || begin > m_curLen){
        return *this;
    }
    DWORD ff = m_curLen - begin;
    DWORD nlen = m_curLen - begin > len ? len : m_curLen - begin;
    (void)memcpy_s((BYTE*)buf, len, m_cbuf + begin, nlen);
    m_pos += m_curLen - m_pos;
    return *this;
}

CStream& CStream::operator >> (CStream &t){

    t.Append(this->GetBuff(), this->length());
    return (*this);
}
CStream& CStream::operator << (CStream &t){

    return this->Append(t.GetBuff(), t.length());
}
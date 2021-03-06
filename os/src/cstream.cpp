#include "cstream.h"
#include <iostream>
#include <string.h>

#ifndef nullptr
#define nullptr 0
#endif

#ifndef __WIN_API__
#define memcpy_s(dest, mlen, src, len) memcpy(dest, src, len)
#endif

CStream::CStream()
{
    Init();
}
void CStream::Init()
{

    m_bufType = FREAM_BUFFER;
    m_cbuf = m_buf;
    m_len = Stackm_len;
    _PubInit();
}
void CStream::_PubInit()
{
    m_curLen = 0;
    m_buf[0] = '\0';
    m_pos = 0;
    m_count = 0;
    m_isOk = true;
}
CStream::CStream(BYTE *buf, DWORD len)
{
    m_bufType = USER_BUFFER;
    m_cbuf = buf;
    m_len = len;
    _PubInit();
}
CStream::CStream(const BYTE *buf)
{
    Init();
    Append(buf, strlen((const char *)buf));
}
CStream::~CStream()
{
    if (MALLOC_BUFFER == m_bufType)
    {
        delete[] m_cbuf;
    }
}

CStream &CStream::operator<<(BYTE t)
{
    if (!GetMoreCapacity(sizeof(t)))
    {
        return *this;
    }
    m_cbuf[m_pos] = t; //添入缓冲区
    m_curLen += sizeof(t);
    return *this;
}
CStream &CStream::operator<<(WORD t)
{
    if (!GetMoreCapacity(sizeof(t)))
    {
        return *this;
    }

    for (size_t i = 0; i < sizeof(t); i++)
    {
        m_cbuf[m_curLen + i] = (t >> (8 - i * 8));
    }
    m_curLen += sizeof(t); //移动当前索引
    return *this;
}
CStream &CStream::operator<<(DWORD t)
{
    if (!GetMoreCapacity(sizeof(t)))
    {
        return *this;
    }

    for (size_t i = 0; i < sizeof(t); i++)
    {
        m_cbuf[m_curLen + i] = (BYTE)(t >> (24 - i * 8));
    }
    m_curLen += sizeof(t); //移动当前索引
    return *this;
}
CStream &CStream::operator>>(BYTE &t)
{
    t = m_cbuf[m_pos];  //添入缓冲区
    m_pos += sizeof(t); //移动当前索引
    return *this;
}
CStream &CStream::operator>>(DWORD &t)
{
    t = m_cbuf[m_pos] << 24 |
        m_cbuf[m_pos + 1] << 16 |
        m_cbuf[m_pos + 2] << 8 |
        m_cbuf[m_pos + 3];
    m_pos += sizeof(t); //移动当前索引
    return *this;
}
CStream &CStream::operator>>(WORD &t)
{
    t = m_cbuf[m_pos] << 8 | m_cbuf[m_pos + 1];
    m_pos += sizeof(t); //移动当前索引
    return *this;
}
bool CStream::GetMoreCapacity(DWORD len)
{
    if (USER_BUFFER == m_bufType)
    {
        return false;
    }
    DWORD nSizeNew = m_curLen + len;
    if (nSizeNew < m_len)
    {
        return true;
    }
    while (nSizeNew >= m_len)
    {
        m_len *= 2;
    }
    BYTE *bytesMore = new BYTE[m_len]; //两倍
    if (bytesMore == nullptr)
    {
        m_isOk = false;
        return false;
    }

    (void)memcpy_s(bytesMore, m_len, m_cbuf, m_curLen);
    (void)memset(bytesMore + m_curLen, 0, m_len - m_curLen);

    if (MALLOC_BUFFER == m_bufType)
    {
        delete[] m_cbuf;
    }
    m_bufType = MALLOC_BUFFER;
    m_cbuf = bytesMore;
    return true;
}
BYTE *CStream::GetBuff()
{
    return m_cbuf;
}
CStream &CStream::Append(const BYTE *buf, DWORD len)
{
    if (buf == nullptr || !GetMoreCapacity(len))
    {
        m_isOk = false;
        return *this;
    }
    (void)memcpy_s(m_cbuf + m_curLen, m_len - m_curLen, buf, len);
    m_curLen += len;
    m_cbuf[m_curLen] = '\0';
    return *this;
}

CStream &CStream::Extract(const BYTE *buf, DWORD len, DWORD begin)
{
    if (buf == nullptr || begin > m_curLen)
    {
        m_isOk = false;
        return *this;
    }

    DWORD nlen = m_curLen - begin > len ? len : m_curLen - begin;
    (void)memcpy_s((BYTE *)buf, len, m_cbuf + begin, nlen);
    m_pos += m_curLen - m_pos;
    return *this;
}

CStream &CStream::operator>>(CStream &t)
{

    t.Append(this->GetBuff(), this->length());
    return (*this);
}
CStream &CStream::operator<<(CStream &t)
{

    return this->Append(t.GetBuff(), t.length());
}
WORD CStream::to_WORD()
{
    return (WORD)to_DWORD();
}
DWORD CStream::to_DWORD()
{
    char *pos;
    return strtoul(c_str(), &pos, 10);
}
void CStream::Clear()
{

    if (USER_BUFFER == m_bufType)
    {
        m_curLen = 0;
        m_pos = 0;
    }
    else
    {
        if (MALLOC_BUFFER == m_bufType)
        {
            delete[] m_cbuf;
        }
        Init();
    }
    (void)memset(m_cbuf, 0, m_len);
}

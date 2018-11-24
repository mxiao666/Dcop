/***********************************************************************************
 * 文 件 名   : os_streanhelper.c
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月24日
 * 文件描述   : 内存数据查看辅助类
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#include "os_streamhelper.h"

/*****************************************************************************
 * 函 数 名  : CStreamCStream
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月24日
 * 函数功能  : 字节流构造函数
 * 输入参数  : BYTE *byInput  字节流
               const T& Len   长度
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
//template<class T>
CStream::CStream(BYTE *byInput, const int/*T*/& Len){
        m_ByStream = byInput;
        m_length   = Len;
    }

/*****************************************************************************
 * 函 数 名  : ByteToString
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月24日
 * 函数功能  : 字节流转字符串
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : const
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
const std::string CStream::ByteToString()
{
    BYTE byArray[4] = { 0 };
    m_Stream.clear();
    for (BYTE i = 0; i < m_length; ++i)
    {
        (void)snprintf((char*)byArray, 4, "%02x ", m_ByStream[i]);//linux
        (void)m_Stream.append((char*)byArray);
    }
    return m_Stream;
}


/*****************************************************************************
 * 函 数 名  : CStream.Print
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月24日
 * 函数功能  : 打印字节流为十六进制
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : void
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void CStream::Print()
{
    ByteToString();
    const BYTE iOutLen = 78; // 必须为3的倍数
    for (int iLen = 0; iLen < m_Stream.length(); iLen += iOutLen)
    {
        LVOS_Log(LL_NOTICE,"%s", m_Stream.substr(iLen, iOutLen).c_str());
    }
}



/***********************************************************************************
 * �� �� ��   : os_streamhelper.h
 * �� �� ��   : ¬����
 * ��������   : 2018��11��24��
 * �ļ�����   : ���߸�����
 * ��Ȩ˵��   : Copyright (c) 2008-2018   xx xx xx xx �������޹�˾
 * ��    ��   : 
 * �޸���־   : 
***********************************************************************************/

#ifndef __OS_STREAMHELPER_H__
#define __OS_STREAMHELPER_H__
#include "iostream"
#include "stdio.h"
#include <sstream>
#include "macro_define.h"

/*****************************************************************************
 * �� �� ��  : ��ӡ�ڴ�����
 * �� �� ��  : ¬����
 * ��������  : 2018��11��24��
 * ��������  : 
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
//template<class T>
class CStreamPrint
{
public:
    CStreamPrint(BYTE *byInput, const int /*T*/ &Len);
    const std::string ByteToString();
    void Print();

private:
    BYTE *m_ByStream;
    int m_length;
    std::string m_Stream;
};

/*****************************************************************************
 * �� �� ��  : MConvertsion
 * �� �� ��  : ¬����
 * ��������  : 2018��11��24��
 * ��������  : ����c++��ԭ�����U����ת��T��ô�ͻ�ִ��Test��T�����������
               ��ô�������;���__int64���������ִ��ʡ�Ժ��Ǹ�
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
#ifndef __WIN32__
template <class T, class U>
class MConvertsion
{
private:
    //Ŀ�ı�֤sizeof �ó���С��һ��
    static __int64_t Test(T);
    static u_int Test(...);
    static U genU();

public:
    enum
    {
        value = (sizeof(Test(genU())) == sizeof(__int64_t))
    };
};

/*****************************************************************************
 * �� �� ��  : MCopyValue
 * �� �� ��  : ¬����
 * ��������  : 2018��11��24��
 * ��������  : ����value��ֵ���ж����������Ƿ�һ�£�ѡȡ���ڵ�ת������
 * �������  : ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
template <bool>
struct MCopyValue
{
    template <class T, class U>
    static void inline apply(T &val1, const U &val2)
    {
        val1 = val2;
    }
};
template <>
struct MCopyValue<false>
{
    template <class T, class U>
    static void inline apply(T &val1, const U &val2)
    {
        std::stringstream os;
        os << val2;
        os >> val1;
    }
    template <class U>
    static void inline apply(std::string &str, const U &val2)
    {
        std::stringstream os;
        os << val2;
        str = os.str();
    }
};

/*****************************************************************************
 * �� �� ��  : convert
 * �� �� ��  : ¬����
 * ��������  : 2018��11��24��
 * ��������  : �������������ͼ�ת��
 * �������  : L& val    ת��������
               R& right  ��ת������
 * �������  : ��
 * �� �� ֵ  : void
 * ���ù�ϵ  :
 * ��    ��  :

*****************************************************************************/
template <class L, class R>
void inline convert(L &val, R &right)
{
    MCopyValue<MConvertsion<L, R>::value>::apply(val, right);
}
#endif
#endif

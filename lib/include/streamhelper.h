/***********************************************************************************
 * 文 件 名   : os_streamhelper.h
 * 负 责 人   : 卢美宏
 * 创建日期   : 2018年11月24日
 * 文件描述   : 工具辅助类
 * 版权说明   : Copyright (c) 2008-2018   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef __OS_STREAMHELPER_H__
#define __OS_STREAMHELPER_H__
#include "iostream"
#include "stdio.h"
#include <sstream>
#include "macro_define.h"

/*****************************************************************************
 * 函 数 名  : 打印内存数据
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月24日
 * 函数功能  : 
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

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
 * 函 数 名  : MConvertsion
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月24日
 * 函数功能  : 按照c++的原则，如果U可以转换T那么就会执行Test（T）这个函数，
               那么返回类型就是__int64，否则就是执行省略号那个
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
#ifndef __WIN32__
template <class T, class U>
class MConvertsion
{
private:
    //目的保证sizeof 得出大小不一致
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
 * 函 数 名  : MCopyValue
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月24日
 * 函数功能  : 根据value的值来判断数据类型是否一致，选取对于的转换函数
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

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
 * 函 数 名  : convert
 * 负 责 人  : 卢美宏
 * 创建日期  : 2018年11月24日
 * 函数功能  : 自适配数据类型间转换
 * 输入参数  : L& val    转换后数据
               R& right  待转换数据
 * 输出参数  : 无
 * 返 回 值  : void
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
template <class L, class R>
void inline convert(L &val, R &right)
{
    MCopyValue<MConvertsion<L, R>::value>::apply(val, right);
}
#endif
#endif

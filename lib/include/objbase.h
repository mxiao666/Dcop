#ifndef __OBJBASE_H__
#define __OBJBASE_H__

class objbase
{
private:
    /* data */
public:
    objbase() {}
    ~objbase() {}
    void dump() {}
    int a = 1;
};
namespace OS
{
template <class T>
inline typename T::iterator find(const char *pzName, T &list)
{
    for (typename T::iterator iter = list.begin();
         iter != list.end(); iter++)
    {
        if (
#ifndef __WIN32__
            strcasecmp
#else
            _stricmp
#endif
            (pzName, iter->first) == 0)
        {
            return iter;
        }
    }
    return list.end();
}
}; // namespace OS

#endif

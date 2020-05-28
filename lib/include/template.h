#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

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

    bool equal(const char *s1, const char *s2);
    bool equal(const char *s1, const char *s2, int max);
}; // namespace OS
#endif
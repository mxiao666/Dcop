#include "string.h"
namespace OS
{
bool equal(const char *s1, const char *s2)
{
    return (
#ifndef __WIN32__
        strcasecmp
#else
        _stricmp
#endif
        (s1, s2) == 0);
}
bool equal(const char *s1, const char *s2, int max)
{
    return (strncmp(s1, s2, max) == 0);
}
}; // namespace OS
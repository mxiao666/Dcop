#include "objbase.h"
void objbase::dump(int fd, Printfun callback)
{
    (void)callback(fd, "Query is NULL.\r\n");
}
void objbase::PrintHead(int fd, Printfun callback,
                        const char *tableName, int tatol, char split)
{
    (void)callback(fd, "\r\n%*s\r\n", tatol / 2 + 1, tableName);
    PrintEnd(fd, callback, tatol, split);
}
void objbase::PrintEnd(int fd, Printfun callback, int tatol, char split)
{
    char _split[128] = {0};
    tatol = (tatol < 128) ? tatol : 127;
    memset(_split, split, tatol);
    _split[(tatol < 127) ? tatol : 127] = '\0';
    (void)callback(fd, _split);
    (void)callback(fd, "\r\n");
}
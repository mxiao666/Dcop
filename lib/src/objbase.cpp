#include "objbase.h"
void objbase::dump(int fd, Printfun callback)
{
    (void)callback(fd, "Query is NULL.\n");
}
void objbase::PrintHead(int fd, Printfun callback,
                        const char *tableName, int tatol, char split)
{
    (void)callback(fd, "\n%*s\n", tatol / 2 + 1, tableName);
    //PrintEnd(callback, tatol, split);
}
void objbase::PrintEnd(int fd, Printfun callback, int tatol, char split)
{
    for (int i = 0; i < tatol; i++)
        (void)callback(fd, "%c", split);
    (void)callback(fd, "\n");
}
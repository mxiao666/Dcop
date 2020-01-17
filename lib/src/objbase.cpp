#include "objbase.h"
void objbase::dump(Printfun callback)
{
    (void)callback("Query is NULL.\n");
}
void objbase::PrintHead(Printfun callback, const char *tableName, int tatol, char split)
{
    (void)callback("\n%*s\n", tatol / 2 + 1, tableName);
    PrintEnd(callback, tatol, split);
}
void objbase::PrintEnd(Printfun callback, int tatol, char split)
{
    for (int i = 0; i < tatol; i++)
        (void)callback("%c", split);
    (void)callback("\n");
}
INIT_FRAMEWORK(objbase)

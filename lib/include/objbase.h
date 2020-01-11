#ifndef __OBJBASE_H__
#define __OBJBASE_H__
class objbase
{
private:
    /* data */
public:
    objbase(int a) { this->a = a; }
    ~objbase() {}
    void dump() {}
    int a = 0;
};

#endif

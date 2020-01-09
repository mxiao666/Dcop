#include <string.h>
#include "objKernel.h"

objKernel::objKernel()
{
	printf(
			"  __      __    __\n"
			" |  |    |  |  |__|   __________\n"
			" |  |____|  |   __   |  __  __  |\n"
			" |   ____   |  |  |  |  ||  ||  |\n"
			" |  |    |  |  |  |  |  ||  ||  |\n"
			" |__|    |__|  |__|  |__||__||__|\n"
			"\n"
			" [Herobrine (Alpha)             ]\n"
			" [Update Date: %-17.17s]\n"
			" [Update Time: %-17.17s]\n",
			__DATE__, __TIME__);
}
objKernel::~objKernel()
{
    for (auto& iter : m_objList)
    {
        ObjModule* pObj = iter.second;
        if(pObj != nullptr) delete pObj;
        pObj = nullptr;
    }
}
objbase* objKernel::InterFace()
{
    return nullptr;
}
objbase* objKernel::Query(const char* pzName)
{
     auto iter = find(pzName);
     if (iter != m_objList.end())
         return iter->second->obj;
    return nullptr;    
}
void objKernel::Release(const char* pzName)
{
    auto iter = find(pzName);
    if((iter != m_objList.end()) && (--iter->second->refCount <= 0))
    {
        delete iter->second;
        iter->second = nullptr;
        m_objList.erase(iter);
    }
}
void objKernel::Entry()
{
    m_objList["test1"] = new ObjModule(new objbase(100));
    m_objList["test2"] = new ObjModule(new objbase(205));
}
std::map<const char*, ObjModule*>::iterator objKernel::find(const char* pzName)
{
    for(std::map<const char*, ObjModule*>::iterator iter = m_objList.begin();
        iter != m_objList.end(); iter++)
    {
        if (strcasecmp(pzName, iter->first) == 0)
        {
            return iter;
        }
    }
    return m_objList.end();
}


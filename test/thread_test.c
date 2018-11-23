#include "os_frameworkmgr.h"
#include "iostream"
using namespace std;
class Test :public IplugFrameWork
{
public:
	Test(){
	}
	int Init(){ 
	printf("Hello world!\n"); 
	return 0;
	}
};
REG_FUNCTION(Test, "classTest");

class Test1 :public IplugFrameWork
{
public:
	Test1()
	{

	}
	int Init(){
	printf("Hello world!12312\n");
    char *tmp = NULL;    *tmp = 0;    printf("xxx=%s\n", tmp);    free(tmp);    free(tmp);
	return 0;
	}
};
REG_FUNCTION(Test1, "classTest1");
int main()
{
	FrameWorkMgr::getInstance()->RegInit();

	return 0;
}
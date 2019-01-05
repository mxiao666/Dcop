#include "os_frameworkmgr.h"
#include "os_system.h"
#include "os_macro_define.h"
#include "iostream"
#include "os_streamhelper.h"
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
REG_FUNCTION_PLUS(Test, "classTest");

class Test1 :public IplugFrameWork
{
public:
	Test1(){
	}
	int Init(){
    	printf("Hello world! Test1\n");
    	return 0;
	}
    static void backtrace()
    {
        char *tmp = NULL;    
        *tmp = 0;    
        printf("xxx=%s\n", tmp);    
        free(tmp);    
        free(tmp);
    }	
};
REG_FUNCTION_PLUS(Test1, "classTest1");


int MyInit(){
	printf("Hello world! MyInit\n");
	return 0;
}
REG_FUNCTION(IplugFrameWorkMyInit, MyInit, "MyInit111");

int main()
{
    Logger::GetInstance()->Start();

    //需要初始化的模块
	FrameWorkMgr::getInstance()->RegInit();
	
	//打印内存数据
	BYTE arr[] = "1234aaaaaaaaaaaakwqld1290dqwkl12222knwslqwns21siqwls12s9xqwk21ls1209sa";
	CStream(arr, sizeof(arr)).Print();
	
	//转换器
	std::string str;
	std::string str1 = "123";
	long b = 123;
	convert(str, str1);
	std::cout << str.c_str() << std::endl;
    LVOS_Log(LL_NOTICE, "str = %s",str.c_str());
	long a = 10;
	convert(a, str1);
	std::cout << a << std::endl;
	LVOS_Log(LL_NOTICE, "a = %ld",a);
	s32 iret;
	s32 it = OS_SafeSystem("ls",NULL,5,&iret);
	LVOS_Log(LL_NOTICE, "OS_SafeSystem %d,%d",it,iret);
	char strcmd[100]= {0};
	it = OS_GetStrValueByCmd("pwd",strcmd,100);
	LVOS_Log(LL_NOTICE, "OS_GetStrValueByCmd %d,%s",it,strcmd);
	//段错误信息捕获
    Test1::backtrace();

	return 0;    
}

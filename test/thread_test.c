#include "os_frameworkmgr.h"

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
REG_FUNCTION(Test, "classTest");

class Test1 :public IplugFrameWork
{
public:
	Test1(){
	}
	int Init(){
    	printf("Hello world!12312\n");
//	    char *tmp = NULL;    
//        *tmp = 0;    
//        printf("xxx=%s\n", tmp);    
//        free(tmp);    
//        free(tmp);
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
REG_FUNCTION(Test1, "classTest1");

int main()
{
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

	//段错误信息捕获
    Test1::backtrace();

	return 0;    
}

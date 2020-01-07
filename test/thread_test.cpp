#include <iostream>
#include <vector>
#include "getmessage.h"
#include "receiver.h"
 #include "cnotify.h"
using namespace std;
 
int main()
{
    cout << "Hello World!" << endl;
 
   /////////////////////
   Cnotify notifier1;
   getMessage receiver1;
   getMessage receiver2;
 
   notifier1.regReceiver(1,&receiver1);
   notifier1.regReceiver(2,&receiver2);
    CAgrcList aa;
    aa.addAgrc("A","here it is");
   //notifier1.sendToAllRecv("here it is");
    notifier1.sendToAllRecv(&aa,nullptr,1,2);
    notifier1.notify(&aa,nullptr,2,33);
   ////////////////////////
 
 
    getchar();
    return 0;
}

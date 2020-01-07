#include "receiver.h"
 
receiver::receiver()
{
 
}
 
receiver::~receiver()
{
 
}
 
void receiver::recvMessage(std::string message)
{
    printf("Receiver get message: %s \n",message);
    std::cout<<message<<"\n";
}


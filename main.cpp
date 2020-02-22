#include "webserver.h"
int main()
{
    threadpool* pool = new threadpool(3);
    
	webserver* mywebsvr = webserver::createwebsvr(8080, pool);
	if(mywebsvr->init() == false)
		return 0;
	mywebsvr->start();    
	return 0; 
}

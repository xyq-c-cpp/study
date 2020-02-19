#include "webserver.h"
int main()
{
    
	webserver* mywebsvr = webserver::createwebsvr(8080);
	if(mywebsvr->init() == false)
		return 0;
	mywebsvr->start();    
	return 0; 
}

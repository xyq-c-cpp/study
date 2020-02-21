#ifndef  _WEBSERVER_H_
#define  _WEBSERVER_H_

#include <string>
#include <vector>
#include "requestmsg.h"
#include "threadpool.h"
#include <sys/epoll.h>
//#include <WinSock2.h>

class webserver
{
private:
	int Port;
	int LSocket;
    int EpollFd;
    
    threadpool *Pool;
	
	webserver(int _Port = 80, threadpool* _Pool = nullptr);
	~webserver();
   
    
    bool doevent(struct epoll_event* _epollevents, int eventsnum);   
	bool domessage(requestmsg* _req , int _dofd);
    bool connection();
    
public:
	static webserver *createwebsvr(int _Port, threadpool* _Pool);
	bool init();
	void start();
   
};

#endif // ! _WEBSERVER_H_


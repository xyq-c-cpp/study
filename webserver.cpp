#include "webserver.h"
#include "filetype.h"
#include "common.h"
#include "requestmsg.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <signal.h>

#include <iostream>
#include <string>
#include <thread>

const int MAX_LISTEN_SIZE = 1024;
const int MAX_EVENTS = MAX_LISTEN_SIZE + 1;

struct epoll_event* myevents = new epoll_event[MAX_EVENTS];


webserver::webserver(int _Port, threadpool* _Pool)
{
    
	Port = _Port;
    Pool = _Pool;
}
webserver::~webserver()
{
    close(LSocket);
    close(EpollFd);
    if(Pool)
        delete Pool;
}
webserver *webserver::createwebsvr(int _Port, threadpool* _Pool)
{   
    int port = _Port;
    
    if(_Port > 65536 || _Port < 1024)
    {
        print("the PORT is unvaliable , it will be set default num 8080 .");
        port = 8080;
    }
    if(!_Pool){
        print("threadpool* is unvaliable....");
        exit(0);
    }
	static webserver* websvr = new webserver(port, _Pool);
	return websvr;
}
bool webserver::init()
{    
	LSocket = socket(AF_INET,SOCK_STREAM,0);
    
	if (LSocket == -1)
	{
		print("error message socket() fail ,websvr will be exit !");
		return false;
	}
    //using the same port with the last time.
    int opt = 1;
    if(setsockopt(LSocket, SOL_SOCKET,  SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        return false;
 	
	struct sockaddr_in LSocket_addr;
	memset(&LSocket_addr,0,sizeof(LSocket_addr));
	LSocket_addr.sin_family = AF_INET;
	LSocket_addr.sin_port = htons((unsigned int)Port);
	LSocket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//LSocket_addr.sin_addr.s_addr = inet_addr("192.168.43.176");	

	int result = bind(LSocket,(struct sockaddr*)&LSocket_addr,sizeof(LSocket_addr));

	if (result == -1)
	{
		print("error message bind to LSocket fail , websvr will be exit !");
		return false;
	}
    
    
	if ( listen(LSocket,MAX_LISTEN_SIZE) != 0)
	{
		print("error message listen to LSocket fail , websvr will be exit !");
        close(LSocket);
		return false;
	}
    
    if(setnoblock(this->LSocket) == false)
    {
        return false;
    }
    
	EpollFd = epoll_create(MAX_LISTEN_SIZE+1);
    if(EpollFd == -1)
        return false;
    print("epoll_create called success............ ");
    
    requestmsg* req = new requestmsg(LSocket);
    __uint32_t events = EPOLLIN | EPOLLET;
    
    if(_epoll_ctl(this->EpollFd, EPOLL_CTL_ADD , static_cast<void*>(req), this->LSocket, events) == false)
    {
        return false;
    }      
    
	print("webserve init success ,LSocket ...",LSocket," EpollFd ...",EpollFd);    
    return true;
}
bool webserver::connection()
{
    struct sockaddr_in client_sock;    
	socklen_t client_len = 0;   
    
    memset(&client_sock, 0, sizeof(struct sockaddr_in));
    while(1)
    {
        int accept_fd = accept(LSocket,(struct sockaddr*)&client_sock,&client_len);
        if (accept_fd <= 0)
        {
            print("error message accept client connection fail , the return of accept_fd .......", accept_fd);
            break;
        }
        if(setnoblock(accept_fd) == false)
        {
            print("error message : accept_fd set noblock fail ! accept_fd : ", accept_fd);
            close(accept_fd);
            break;
        }
           
        requestmsg* req = new requestmsg(accept_fd);
        __uint32_t reqevent = EPOLLIN | EPOLLET | EPOLLONESHOT;
        
        if( _epoll_ctl(this->EpollFd, EPOLL_CTL_ADD ,static_cast<void*>(req), accept_fd, reqevent) == false )
        {          
            close(accept_fd);
            break;
        }
        print("connection success and add to epoll !     accept_fd : ",accept_fd);                     
    }
    return true;   
}
bool webserver::doevent(struct epoll_event* _epollevents, int eventsnum)
{
    requestmsg* req = nullptr;
    for(int i = 0;i < eventsnum ;++i)
    {
        req = static_cast<requestmsg*>(_epollevents[i].data.ptr);
        if(req->getfd() == LSocket)
        {
            auto ret = connection();
            if (ret == false)
               print("error message accept connection fail .continue .........");
        }
        else 
        {
            if ((_epollevents[i].events & EPOLLERR) || (_epollevents[i].events & EPOLLHUP)
                || (!(_epollevents[i].events & EPOLLIN)))
            {   
                auto events = _epollevents[i].events;
                print("error message the opposite fd  had shutdown the connection ......", events);
                close(req->getfd());                
                delete req;
                continue;
            }           
            //int __fd = req->getfd();    
            //int ret = req->handlequest(EpollFd);
            Pool->push(requestmsg::handle, EpollFd, req);
            
            /* if(ret == false )
               print("domessage fail , client socket ....... ",__fd);    */        
        }
    }
    return true;
}
void webserver::start()
{	
	while (1)
	{
        int waitnum = epoll_wait(EpollFd ,myevents ,MAX_EVENTS , -1);
        if(waitnum  <= 0)
        {
            continue;
        }
        print("the num of events to deal is : ",waitnum);        
        int ret = doevent(myevents, waitnum);
                
        print("doevent status is ..........  ",ret);        
	}	 	        
}







#include "common.h"

#include <sys/epoll.h>
#include <fcntl.h>
#include <iostream>

void print(void)
{
    std::cout<<std::endl;
}

bool setnoblock(int _Fd)
{
    int flags = fcntl(_Fd, F_GETFL, 0);
    if(flags == -1)
    {
        print("error message set noblocksocket1 fail ,connection fail !");
        return false;
    }
    flags |= O_NONBLOCK;
    if(fcntl(_Fd, F_SETFL, flags) == -1)
    {
        print("error message set noblocksocket2 fail ,connection fail !");
        return false;
    }
    print("set noblock successs , fd : ", _Fd);
    return true;
}

bool _epoll_ctl(int _epollfd, int _op, void* req, int _clientfd, __int32_t events)
{
    struct epoll_event event;
    event.data.ptr = req;
    event.events = events;
    if(epoll_ctl(_epollfd, _op, _clientfd, &event) < 0)
    {
        print("error message : epoll_ctl fail ! _epollfd : ",_epollfd ," clientfd : ",_clientfd);
        return false;
    }
    print("epoll_ctl successs , _epollfd : ", _epollfd, "clientfd : ", _clientfd);
    return true;
}
ssize_t _read(int _fd, char* _buffer, size_t _num)
{
    size_t left = _num;
    ssize_t sum = 0 , nowread = 0;
    while(left > 0)
    {
        nowread = read(_fd, _buffer+sum, left);
        //nowread == 0 mean that the data have been read totally.
        //nowread < 0 ,and errno ==EINTR or EAGAIN mean that operation can be execute again.
        if(nowread < 0)
        {
            if(errno == EINTR)
            {
                nowread = 0;
                continue;
            }               
            else if(errno == EAGAIN)
                return sum;
            else return -1;
        }
        else if(nowread == 0)
            break;
        sum+=nowread;
        left-=nowread;
    }
    return sum;
}
ssize_t _write(int _fd, char* _buffer,size_t _num)
{
    size_t left = _num;
    ssize_t sum = 0 , nowrite = 0;
    while(left > 0)
    {
        nowrite = write(_fd, _buffer+sum, left);
        //nowrite ==0 ,can not write something mean that some mistakes happened.
        //nowrite < 0 ,if errno == EINTR or EAGAIN mean that operation can execute again.
        if(nowrite < 0)
        {
            if(errno == EINTR || errno == EAGAIN)
                continue;
            else return -1;
        }
                                    
        left-=nowrite;
        sum+=nowrite;       
    }
    return sum;
       
}


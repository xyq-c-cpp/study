#ifndef _COMMON_H_
#define _COMMON_H_

//question 1:the function template declare and defined have to put together 
//otherwise,link will be fail.

//question 2:multiple definition  of function ,so the template function declare and definition put together. 
//          in front of the other function ,we can add extern to aviod this problem.      
//          because the template function put here, and the other function also put at here also must do this
//          to aviod this problem .

#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*
template<typename T>
void ERROR(T t)
{
	std::cout<<t<<std::endl;
}

template<typename T,typename O>
void PRINT(T t,O o)
{
	std::cout<<t<<o<<std::endl;
}
*/

// this function must be put in front of the template as the end fundtion.
extern void print(void); 

template<class M,class ...N>
void print(M& m,N& ...n)
{
	std::cout<<m;
	print(n...);
}


extern bool setnoblock(int _Fd);


extern bool _epoll_ctl(int _epollfd, int _op, void* req, int _clientfd, __int32_t events);


extern ssize_t _read(int _fd, char* _buffer, size_t _num);

extern ssize_t _write(int _fd, char* _buffer,size_t _num);



#endif

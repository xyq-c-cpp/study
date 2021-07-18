/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the defination of web server class, a single class.
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#ifndef  _WEBSERVER_H_
#define  _WEBSERVER_H_

#include <string>
#include <vector>
#include <sys/epoll.h>
#include <requestmsg.h>
#include <threadpool.h>

class webserver
{
 public:
  static webserver *createwebsvr(int _Port, threadpool* _Pool);
  bool init();
  void start();

 private:
  webserver(int _Port = 80, threadpool* _Pool = nullptr);
  ~webserver();
  bool doevent(struct epoll_event* _epollevents, int eventsnum);
  bool domessage(requestmsg* _req , int _dofd);
  bool connection();

  int Port;
  int LSocket;
  int EpollFd;
  threadpool *Pool;
};

#endif // ! _WEBSERVER_H_


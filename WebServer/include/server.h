/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the defination of web server class, a single class.
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#ifndef  _SERVER_H_
#define  _SERVER_H_

#include <string>
#include <vector>
#include <sys/epoll.h>

#include <message.h>
#include <threadpool.h>
#include <common.h>

/*
 * a single class of the server
 */
class Server
{
 public:
  static webserver *CreateServer(uint32_t port, threadpool* pool);
  int32_t Init(void);
  int32_t Start(void);

 private:
  Server(uint32_t port = 80, threadpool *pool = nullptr);
  ~Server();
  bool EventProc(struct epoll_event* events, uint32_t num);
  bool MessageProc(Message     *req, uint32_t fd);
  bool Connection();

  uint32_t port_;
  uint32_t listen_fd_;
  uint32_t epoll_fd_;
  ThreadPool *pool_;
};

#endif /* _SERVER_H_ */


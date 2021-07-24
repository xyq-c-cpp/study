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

class ThreadPool;
class Epoller;
class TimerQueue;
class Connector;

/*
 * a single class of the server
 */
class Server
{
 public:
  static Server *CreateServer(uint32_t port, int thread_nr, int listen_cnt);
  int32_t Start(void);
  void Insert(std::pair<int, Channal *> &channal);
  void Erase(int fd);

 private:
  Server(int port, int thread_nr, int listen_cnt);
  ~Server() = default;

  bool EventProc(struct epoll_event* events, uint32_t num);
  bool MessageProc(Message     *req, uint32_t fd);

  const int port_;
  Epoller *epoller_;
  ThreadPool *pool_;
  TimerQueue *timer_queue_;
  Connector *connector_;
  std::unordered_map<int, Channal *> channal_map_;
};

#endif /* _SERVER_H_ */


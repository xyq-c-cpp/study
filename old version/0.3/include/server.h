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

#include <common.h>

/*
 * a single class of the server
 */
class Server {
 public:
  static Server *CreateServer(unsigned int port, int thread_nr, int listen_cnt);
  void Start(void);
  void Init(void);
  void TaskInQueue(Task callback);
  Epoller *GetEpoller();

 private:
  Server(int port, int thread_nr, int listen_cnt);
  ~Server() = default;

  const int port_;
  Epoller *epoller_;
  ThreadPool *pool_;
  TimerQueue *timer_queue_;
  std::shared_ptr<Connector> connector_;
};

#endif /* _SERVER_H_ */


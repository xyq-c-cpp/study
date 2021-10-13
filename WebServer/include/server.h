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

#include <Common.h>

/*
 * a single class of the server
 */
class Server {
public:
  explicit Server(int port, std::shared_ptr<EventLoop> mainLoop, int threadNum = 3);
  ~Server() = default;
  void start();

private:
  int newListenFd();
  int acceptConnection();
  int handleError();

  const int port_;
  std::shared_ptr<EventLoop> mainLoop_;
  std::shared_ptr<EventLoopThreadPool> pool_;
  std::shared_ptr<Channal> acceptor_;
};

#endif /* _SERVER_H_ */


/*
 * author: 657023321@qq.com
 * date: 2021-07-18
 * function: the class of epoller, wraping epoll
 */

#ifndef _EPOLLER_H_
#define _EPOLLER_H_

#include <Common.h>

class Epoller {
public:
  explicit Epoller(int maxFdNum = 65536);
  ~Epoller();
  bool epollAdd(std::shared_ptr<Channal> channal, int msec = 0);
  bool epollMod(int fd, uint32_t event, int msec = 0);
  bool epollDel(int fd);
  void epollWait(int msec = 0);
  void handleExpired();

private:
  bool epollCtl(int op, int fd, uint32_t event);

  const int maxFdNum_;
  int fd_;
  std::unique_ptr<struct epoll_event> eventArray_;
  std::shared_ptr<TimerManager> timerManager_;
  std::map<int, std::shared_ptr<Channal>> fd2channal_;
};

#endif

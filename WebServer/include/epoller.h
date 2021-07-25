/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of epoller, wraping epoll
 */

#ifndef _EPOLLER_H_
#define _EPOLLER_H_

#include <common.h>

 class Callback {
 public:
  std::function<int(Epoller *)> read_callback_;
  std::function<int(Epoller *)> write_callback_;

  Callback() = default;
  ~Callback() = default;

  Callback(std::function<int(Epoller *)> callback1, 
          std::function<int(Epoller *)> callback2)
    : read_callback_(std::move(callback1)),
      write_callback_(std::move(callback2)) {

  }
      
  Callback(const Callback &another) {
    read_callback_ = another.read_callback_;
    write_callback_ = another.write_callback_;
  }

  Callback& operator = (const Callback &another) {
    read_callback_ = another.read_callback_;
    write_callback_ = another.write_callback_;
    return *this;
  }
  
  void SetReadCallback(std::function<int(Epoller *)> callbck) {
    read_callback_ = std::move(callbck);
  }
  
  void SetWriteCallback(std::function<int(Epoller *)> callback) {
    write_callback_ = std::move(callback);
  }
};

class Epoller {
 public:
  static Epoller *CreateEpoller(unsigned int event_num, Server *server);
  int AddReadEvent(int fd, std::function<int(Epoller *)> callback);
  int AddWriteEvent(int fd, std::function<int(Epoller *)> callback);
  int AddReadWriteEvent(int fd, std::function<int(Epoller *)> read_cb, 
    std::function<int(Epoller *)> write_cb);
  void EpollWait(int timeout);
  void DelFd(int fd);

  ~Epoller();

 private:
  Epoller(unsigned int event_num, Server *server);
  int EpollCtl(int flag, int fd, int event, void *arg);

  const unsigned int event_num_;
  const int fd_;
  Server *server_;
  std::vector<struct epoll_event> event_arr_;
  std::unordered_map<int, Callback> event_callbck_;
  std::unordered_map<int, unsigned int> fd_event_;
  struct epoll_event event_;
};

#endif


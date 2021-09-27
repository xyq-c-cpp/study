/*
 * author: 657023321@qq.com
 * date: 2021-07-18
 * function: the class of epoller, wraping epoll
 */

#ifndef _EPOLLER_H_
#define _EPOLLER_H_

#include <common.h>

class Callback {
 public:
  Callback() = default;
  ~Callback() = default;

  Callback(EventCb callback1, EventCb callback2)
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

  void SetReadCallback(EventCb cb) {
    read_callback_ = std::move(cb);
  }

  void SetWriteCallback(EventCb cb) {
    write_callback_ = std::move(cb);
  }

  int RunReadCallback(void) {
    return read_callback_();
  }

  int RunWriteCallback(void) {
    return write_callback_();
  }

 private:
  EventCb read_callback_;
  EventCb write_callback_;
};

class Epoller {
 public:
  static Epoller *CreateEpoller(unsigned int event_num);
  int AddReadEvent(int fd, EventCb callback, int event);
  int AddWriteEvent(int fd, EventCb callback, int event);
  int AddReadWriteEvent(int fd, EventCb read_cb, EventCb write_cb, int event);
  void EpollWait(int timeout);
  void DelFd(int fd);
  void SetOwn(Server *server) {
    server_ = server;
    assert(server_ != nullptr);
  }
  int GetFdNum() {
      return fd_num_;
  }
  int GetEpollFd(void) {return fd_;}

  ~Epoller();

 private:
  Epoller(unsigned int event_num);
  int EpollCtl(int op, int fd, int event, void *arg);

  const unsigned int event_num_;
  const int fd_;
  Server *server_;
  int fd_num_;
  std::vector<struct epoll_event> event_arr_;
  std::unordered_map<int, Callback> event_callbck_;
  std::unordered_map<int, unsigned int> fd_event_;
};

#endif


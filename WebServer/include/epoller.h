/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of epoller, wraping epoll
 */

#ifndef _EPOLLER_H_
#define _EPOLLER_H_

typedef struct {
 public:
  std::function<int()> read_callback_;
  std::function<int()> write_callback_;

  Callback(std::function<int()> &callback1, std::function<int()> &callback2)
    : read_callback_(std::move(callback1)),
      write_callback_(std::move(callback2)){

  }
}Callback;

class Epoller {
 public:
  static Epoller *CreateEpoller(uint32_t event_num);
  int AddReadEvent(int fd, std::function<int()> &callback);
  int AddWriteEvent(int fd, std::function<int()> &callback);
  int AddReadWriteEvent(int fd, std::function<int()> &read_cb, 
    std::function<int> &write_cb);
  void EpollWait(int timeout);
  int DelFd(int fd);

  ~Epoller();

 private:
  Epoller(uint32_t event_num, int listen_fd);
  int EpollCtl(int flag, int fd, int32_t event, void *arg);

  const uint32_t event_num_;
  const int fd_;
  std::vector<struct epoll_event> event_arr_;
  std::unordered_map<int, Callback> event_callbck_;
  std::unordered_map<int, uint32_t> fd_event_;
  struct epoll_event event_;
};

#endif


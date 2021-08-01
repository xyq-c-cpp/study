/*
 * author: 657023321@qq.com
 * date: 2021-07-18
 * function: the class of epoller, wraping epoll
 */

#include <epoller.h>
#include <server.h>

Epoller::Epoller(unsigned int event_num) 
    : event_num_(event_num), 
      fd_(epoll_create(event_num_)),
      server_(nullptr),
      event_arr_(std::vector<struct epoll_event>(event_num_)) {
  assert(fd_ >= 0);
}

Epoller *Epoller::CreateEpoller(unsigned int event_num) {
  static Epoller *tmp = nullptr;

  if (!tmp) {
    tmp = new Epoller(event_num);
    assert(tmp != nullptr);
  }

  return tmp;
}

void Epoller::DelFd(int fd) {
  unsigned int event = EPOLLIN | EPOLLOUT;
  EpollCtl(EPOLL_CTL_DEL, fd, event, NULL);
  fd_event_.erase(fd);
  event_callbck_.erase(fd);
}

Epoller::~Epoller() {
  close(fd_);
}

int Epoller::EpollCtl(int op, int fd, int event, void *arg) {
  struct epoll_event ev;

  ev.data.ptr = arg;
  ev.data.fd = fd;
  ev.events = event;

  return epoll_ctl(fd_, op, fd, &ev);
}

int Epoller::AddReadEvent(int fd, EventCb callback, int event) {
  if (fd_event_.find(fd) == fd_event_.end()) {
    fd_event_.insert(std::make_pair(fd, event));
    event_callbck_.insert(std::make_pair(fd, Callback(std::move(callback), 
      EventCb())));

    return EpollCtl(EPOLL_CTL_ADD, fd, event, NULL);
  } else {
    fd_event_[fd] = event;
    event_callbck_[fd].SetReadCallback(std::move(callback));
 
    return EpollCtl(EPOLL_CTL_MOD, fd, event, NULL);
  }
}

int Epoller::AddWriteEvent(int fd, EventCb callback, int event) {
  if (fd_event_.find(fd) == fd_event_.end()) {
    fd_event_.insert(std::make_pair(fd, event));
    event_callbck_.insert(std::make_pair(fd, Callback(std::move(callback), 
      EventCb())));

    return EpollCtl(EPOLL_CTL_ADD, fd, event, NULL);
  } else {
    fd_event_[fd] = event;
    event_callbck_[fd].SetWriteCallback(std::move(callback));

    return EpollCtl(EPOLL_CTL_MOD, fd, event, NULL);
  }
}

int Epoller::AddReadWriteEvent(int fd, EventCb read_cb, EventCb write_cb, 
      int event) {
  if (fd_event_.find(fd) == fd_event_.end()) {
    LOG_DEBUG("The first time to add read-write event, fd %d", fd);

    fd_event_.insert(std::make_pair(fd, event));
    event_callbck_.insert(std::make_pair(fd, Callback(std::move(read_cb), 
      std::move(write_cb))));

    return EpollCtl(EPOLL_CTL_ADD, fd, event, NULL);
  } else {
    fd_event_[fd] = event;
    event_callbck_[fd].SetReadCallback(std::move(read_cb));
    event_callbck_[fd].SetWriteCallback(std::move(write_cb));
 
    return EpollCtl(EPOLL_CTL_MOD, fd, event, NULL);
  }
}

void Epoller::EpollWait(int timeout) {
  int ret;
  int callback_ret;

  ret = epoll_wait(fd_, &event_arr_[0], static_cast<int>(event_num_), timeout);
  if (ret <= 0) {
    LOG_WARN("epoll_wait ret <= 0, No events to process...");
    return;
  }

  LOG_DEBUG("epoll_wait -------------> ret %d", ret);

  for (int i = 0; i < ret; ++i) {
    LOG_DEBUG("process fd %d, event %d, i %d", event_arr_[i].data.fd, 
      event_arr_[i].events, i);

    if (event_arr_[i].events & EPOLLIN) {
      callback_ret = event_callbck_[event_arr_[i].data.fd].RunReadCallback();

      LOG_DEBUG("fd read callbck ret %d, fd %d, event %d", callback_ret, \
        event_arr_[i].data.fd, event_arr_[i].events);
    } else if (event_arr_[i].events & EPOLLOUT) {
      callback_ret = event_callbck_[event_arr_[i].data.fd].RunWriteCallback();
 
      LOG_DEBUG("fd write callbck ret %d, fd %d, event %d", callback_ret, \
        event_arr_[i].data.fd, event_arr_[i].events);
    } else {
      LOG_DEBUG("No expected event %d, fd %d, close it", event_arr_[i].events, \
        event_arr_[i].data.fd);

      DelFd(event_arr_[i].data.fd);
      server_->EraseChannal(event_arr_[i].data.fd);
      (void)close(event_arr_[i].data.fd);
    }
  }
}


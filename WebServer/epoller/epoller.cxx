/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of epoller, wraping epoll
 */

#include <epoller.h>

Epoller::Epoller(uint32_t event_num) 
    : event_num_(event_num), 
      fd_(epoll_create(event_num_)),
      event_arr_(std::vector<struct epoll_event>(event_num_)) {
  static_assert(fd_ >= 0, "fd_ should >= 0, epoll_create failed");
}

Epoller::CreateEpoller(uint32_t event_num) {
  static Epoller *tmp = nullptr;

  if (!tmp) {
    tmp = new Epoller(event_num);
    static_assert(tmp != nullptr, "new Epoller failed");
  }

  return tmp;
}

void Epoller::DelFd(int fd) {
  uint32_t event = EPOLLIN | EPOLLOUT;
  EpollCtl(EPOLL_CTL_DEL, fd, evnet, NULL);
  fd_event_.erase(fd);
  event_callbck_.erase(fd);
}

Epoller::~Epoller() {
  close(fd_);
}

int Epoller::EpollCtl(int flag, int fd, int32_t event, void *arg) {
  event_.data.ptr = arg;
  event_.events = event;

  return epoll_ctl(fd_, flag, fd, &event_);
}

int Epoller::AddReadEvent(int fd, std::function<int()> &callback) {
  uint32_t event = EPOLLIN | EPOLLET | EPOLLONESHOT;

  if (fd_event_.find(fd) == fd_event_.end()) {
    fd_event_.insert(std::make_pair(fd, event));
    event_callbck_.insert(std::make_pair(fd, Callback(std::move(callback), 
      std::function<int()>())));
    goto end;
  } else {
    event = fd_event_[fd];
    if (event & EPOLLIN) { /* it should be the second time to call */
      goto end;
    }
    event |= EPOLLIN;
    fd_event_[fd] = event;
    event_callbck_[fd].read_callback_ = std::move(callback);
    goto end;
  }

end:
  return EpollCtl(EPOLL_CTL_MOD, fd, event, NULL);
}

int Epoller::AddWriteEvent(int fd, std::function<int()> &callback) {
  uint32_t event = EPOLLOUT | EPOLLET | EPOLLONESHOT;

  if (fd_event_.find(fd) == fd_event_.end()) {
    fd_event_.insert(std::make_pair(fd, event));
    event_callbck_.insert(std::make_pair(fd, Callback(std::move(callback), 
      std::function<int()>())));
    goto end;
  } else {
    event = fd_event_[fd];
    if (event & EPOLLOUT) {
      goto end;
    }
    event |= EPOLLOUT;
    fd_event_[fd] = event;
    event_callbck_[fd].write_callback_ = std::move(callback);
  }

end:
  return EpollCtl(EPOLL_CTL_MOD, fd, event, NULL);
}

int Epoller::AddReadWriteEvent(int fd, std::function<int()> &read_cb, 
    std::function<int> &write_cb) {
  uint32_t event = EPOLLOUT | EPOLLOUT | EPOLLET | EPOLLONESHOT;

  if (fd_event_.find(fd) == fd_event_.end()) {
    LOG_DEBUG("The first time to add read-write event, fd %d", fd);
    fd_event_.insert(std::make_pair(fd, event));
    event_callbck_.insert(std::make_pair(fd, Callback(std::move(read_cb), 
      std::move(write_cb))));
    goto end;
  } else {
    event = fd_event_[fd];
    if (event & EPOLLIN && event & EPOLLOUT) {
      goto end;
    }
    event |= EPOLLIN | EPOLLOUT;
    fd_event_[fd] = event;
    event_callbck_[fd].read_callback_ = std::move(read_cb);
    event_callbck_[fd].write_callback_ = std::move(write_cb);
  }

end:
  return EpollCtl(EPOLL_CTL_ADD, fd, event, NULL);
}

void Epoller::EpollWait(int timeout) {
  int ret;
  int callback_ret;

  ret = epoll_wait(fd_, &event_arr_[0], static_cast<int>(event_num_), timeout);
  if (ret <= 0) {
    LOG_WARN("epoll_wait ret <= 0, maybe invalid events");
    return;
  }

  for (int i = 0; i < ret; ++i) {
    if (event_arr_[i].events & EPOLLIN) {
      callback_ret = event_callbck_[event_arr_[i].data.fd].read_callback_();
      LOG_DEBUG("fd read callbck ret %d, fd %d", callback_ret, \
        event_arr_[i].data.fd);
    } else if (event_arr_[i].events & EPOLLOUT) {
      callback_ret = event_callbck_[event_arr_[i].data.fd].write_callback_();
      LOG_DEBUG("fd write callbck ret %d, fd %d", callback_ret, \
        event_arr_[i].data.fd);
    } else {
      LOG_DEBUG("not subscribe event %d, fd %d", event_arr_[i].events, \
        event_arr_[i].data.fd);
    }
  }
}

